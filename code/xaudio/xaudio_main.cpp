extern "C" {
#   include "../client/snd_local.h"
}

#include <xaudio2.h>
#include "xaudio_public.h"
#include "xaudio_local.h"

#define STREAMING_BUFFER_SIZE 0x10000

//#define LOOPING_BUFFERS

static IXAudio2* g_pXAudio2 = NULL;
static IXAudio2MasteringVoice* g_pMasterVoice = NULL;
static IXAudio2SourceVoice* g_pSourceVoice = NULL;

#define STAGED_BUFFERS 1

#if STAGED_BUFFERS
#define STREAMING_BUFFER_COUNT 3
static BYTE g_StreamingBuffers[STREAMING_BUFFER_COUNT][STREAMING_BUFFER_SIZE];
static int g_StreamingBufferIndex = 0;
static StreamingVoiceContext g_Context;
#else
static BYTE g_StreamingBuffer[STREAMING_BUFFER_SIZE];
#endif

/*
==================
XAudio_Shutdown
==================
*/
void XAudio_Shutdown( void ) 
{
    if ( g_pSourceVoice ) 
    {
        g_pSourceVoice->Stop();
        g_pSourceVoice->DestroyVoice();
    }

    if ( g_pMasterVoice )
    {
        g_pMasterVoice->DestroyVoice();
    }

    if ( g_pXAudio2 )
    {
        g_pXAudio2->Release();
    }

    g_pSourceVoice = NULL;
    g_pMasterVoice = NULL;
    g_pXAudio2 = NULL;

    Com_Printf( "XAudio2 shut down.\n" );
}

/*
==================
XAudio_Init

Initialize direct sound
Returns false if failed
==================
*/
qboolean XAudio_Init(void) 
{
    Com_Printf( "XAudio2 initializing:\n" );

    CoInitializeEx( NULL, COINIT_MULTITHREADED );

    HRESULT hr = XAudio2Create( &g_pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR );
    if ( FAILED( hr ) ) 
    {
        Com_Printf( "... Warning: XAudio2 failed to initialize.\n" );
        goto fail;
    }

    cvar_t* xaudio_debugLevel = Cvar_Get( "xaudio_debugLevel", "0", CVAR_TEMP | CVAR_LATCH );

    if ( xaudio_debugLevel->integer > 0 )
    {
        XAUDIO2_DEBUG_CONFIGURATION dbgCfg;
        ZeroMemory( &dbgCfg, sizeof( dbgCfg ) );
        dbgCfg.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
        
        if ( xaudio_debugLevel->integer > 1 )
            dbgCfg.TraceMask |= XAUDIO2_LOG_INFO;
        if ( xaudio_debugLevel->integer > 2 )
            dbgCfg.TraceMask |= XAUDIO2_LOG_DETAIL;
        if ( xaudio_debugLevel->integer > 3 )
            dbgCfg.TraceMask |= XAUDIO2_LOG_STREAMING;

        if ( IsDebuggerPresent() ) 
        {
            dbgCfg.BreakMask = XAUDIO2_LOG_ERRORS;
        }

        g_pXAudio2->SetDebugConfiguration( &dbgCfg );
    }

    hr = g_pXAudio2->CreateMasteringVoice( &g_pMasterVoice );
    if ( FAILED( hr ) )
    {
        Com_Printf( "... Warning: XAudio2 failed to initialize master voice.\n" );
        goto fail;
    }
    
    // Set up the DMA structure
    dma.channels = 2;
    dma.samplebits = 16;
    dma.speed = 22050;
    Com_Printf( "... Configuring: channels(%d) samplebits(%d) speed(%d).\n", 
        dma.channels, dma.samplebits, dma.speed);

    // Set up the wave format
    WAVEFORMATEX format;
	memset (&format, 0, sizeof(format));
	format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = dma.channels;
    format.wBitsPerSample = dma.samplebits;
    format.nSamplesPerSec = dma.speed;
    format.nBlockAlign = format.nChannels * format.wBitsPerSample / 8;
    format.cbSize = 0;
    format.nAvgBytesPerSec = format.nSamplesPerSec*format.nBlockAlign; 

    // Create the source voice
#if STAGED_BUFFERS
    hr = g_pXAudio2->CreateSourceVoice( &g_pSourceVoice, &format, 0, 2.0f, &g_Context );
#else
    hr = g_pXAudio2->CreateSourceVoice( &g_pSourceVoice, &format );
#endif
    if ( FAILED( hr ) )
    {
        Com_Printf( "... Warning: XAudio2 failed to initialize source voice.\n" );
        goto fail;
    }

    // Start the voice (but there's no data yet)
    g_pSourceVoice->Start( 0, 0 );

    
	dma.channels = format.nChannels;
	dma.samplebits = format.wBitsPerSample;
	dma.speed = format.nSamplesPerSec;
	dma.samples = STREAMING_BUFFER_SIZE / (dma.samplebits/8);
	dma.submission_chunk = 1;
	dma.buffer = NULL;

#if STAGED_BUFFERS
    dma.manybuffered = qtrue;
    g_StreamingBufferIndex = 0;
    
    for ( int i = 0; i < STREAMING_BUFFER_COUNT; ++i )
    {
        Com_Memset( g_StreamingBuffers[i], 0, dma.samples * dma.samplebits/8 );
    }
#else
    dma.manybuffered = qfalse;
    Com_Memset( g_StreamingBuffer, 0, dma.samples * dma.samplebits/8 );
#endif

    Com_Printf( "... OK.\n" );
    return qtrue;

fail:

    Com_Printf( "... Init Failed: 0x%08X\n", hr );

    XAudio_Shutdown();
    return qfalse;
}

/*
==============
XAudio_GetDMAPos

return the current sample position (in mono samples read)
inside the recirculating dma buffer, so the mixing code will know
how many sample are required to fill it up.
===============
*/
static int lastSample = 0;
int XAudio_GetDMAPos( void ) 
{
    return lastSample;
    /*
    XAUDIO2_VOICE_STATE state;
    g_pSourceVoice->GetState( &state );
    return state.SamplesPlayed % dma.samples;
    */
}

/*
==============
XAudio_BeginPainting

Makes sure dma.buffer is valid
===============
*/

void XAudio_BeginPainting( void ) 
{
#if STAGED_BUFFERS
    XAUDIO2_VOICE_STATE state;

    // Wait for a buffer to become available
    while (true)
    {
        g_pSourceVoice->GetState( &state );
        if ( state.BuffersQueued < STREAMING_BUFFER_COUNT )
            break;

        WaitForSingleObject( g_Context.hBufferEndEvent, INFINITE );
    }

    // Pass back the pointer to this buffer
    dma.buffer = g_StreamingBuffers[g_StreamingBufferIndex];

    // Switch to the next buffer
    g_StreamingBufferIndex = (g_StreamingBufferIndex + 1) % STREAMING_BUFFER_COUNT;
#else
    // TODO: where?
    int offset = (lastSample) & (dma.samples-1);
    dma.buffer = &g_StreamingBuffer[offset * dma.samplebits / 8];
#endif
}

/*
==============
XAudio_Submit

Send sound to device if buffer isn't really the dma buffer
Also unlocks the dsound buffer
===============
*/
void XAudio_Submit( int offset, int length ) 
{
    //Com_Printf( "Offset %d length %d\n", offset, length );

    XAUDIO2_BUFFER xbuf;
    ZeroMemory( &xbuf, sizeof( xbuf ) );

    xbuf.AudioBytes = length * ( dma.samplebits / 8 );

#if STAGED_BUFFERS
    xbuf.pAudioData = dma.buffer;
#else
    offset = offset & (dma.samples - 1);
    UINT32 offsetBytes = 2 * offset * ( dma.samplebits / 8 );
    xbuf.pAudioData = dma.buffer + offsetBytes;
#endif

    // lastSample = offset + length;
    lastSample += length;
    
    g_pSourceVoice->SubmitSourceBuffer( &xbuf );
}

/*
=================
XAudio_Activate

When we change windows we need to do this
=================
*/
void XAudio_Activate( void ) 
{

    // Nothing required here.
}
