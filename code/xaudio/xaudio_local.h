#pragma once

//--------------------------------------------------------------------------------------
// Callback structure
//--------------------------------------------------------------------------------------
struct StreamingVoiceContext : public IXAudio2VoiceCallback
{
            STDMETHOD_( void, OnVoiceProcessingPassStart )( UINT32 )
            {
            }
            STDMETHOD_( void, OnVoiceProcessingPassEnd )()
            {
            }
            STDMETHOD_( void, OnStreamEnd )()
            {
            }
            STDMETHOD_( void, OnBufferStart )( void* )
            {
            }
            STDMETHOD_( void, OnBufferEnd )( void* )
            {
                //SetEvent( hBufferEndEvent );
                ReleaseSemaphore( hBufferEndEvent, 1, nullptr );
            }
            STDMETHOD_( void, OnLoopEnd )( void* )
            {
            }
            STDMETHOD_( void, OnVoiceError )( void*, HRESULT )
            {
            }

    HANDLE hBufferEndEvent;
            int BuffersInUse;

            StreamingVoiceContext( int maxCount ) 
                //: hBufferEndEvent( CreateEvent( NULL, FALSE, FALSE, NULL ) )
                : hBufferEndEvent( CreateSemaphore( nullptr, maxCount, maxCount, nullptr ) )
            {
            }
    virtual ~StreamingVoiceContext()
    {
        CloseHandle( hBufferEndEvent );
    }
};

