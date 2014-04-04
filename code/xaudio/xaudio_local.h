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
            STDMETHOD_( void, OnBufferStart )( void* handle )
            {
                if ( handle )
                {
                    BuffersInUse++;
                }
            }
            STDMETHOD_( void, OnBufferEnd )( void* handle )
            {

                //SetEvent( hBufferEndEvent );
                HANDLE hSem = (HANDLE) handle;
                if ( hSem )
                {
                    ReleaseSemaphore( hSem, 1, nullptr );
                    BuffersInUse--;
                }
            }
            STDMETHOD_( void, OnLoopEnd )( void* )
            {
            }
            STDMETHOD_( void, OnVoiceError )( void*, HRESULT )
            {
            }

    //HANDLE hBufferEndEvent;
            int BuffersInUse;

            StreamingVoiceContext() : BuffersInUse(0) // : hBufferEndEvent( CreateEvent( NULL, FALSE, FALSE, NULL ) )
            {
            }
    virtual ~StreamingVoiceContext()
    {
        //CloseHandle( hBufferEndEvent );
    }
};

