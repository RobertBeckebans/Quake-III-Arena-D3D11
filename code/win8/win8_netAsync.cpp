/*
    @pjb: DO NOT INCLUDE ANY QUAKE HEADERS IN THIS FILE.

    DO NOT USE ANY QUAKE FUNCTIONS.
    (Sys_Milliseconds is an exception.)

    This entire file is full of async calls and MUST interop with the game
    through the message queue.
*/

#include <ppl.h>
#include <ppltasks.h>
#include <assert.h>

#include "win8_msgq.h"
#include "win8_net.h"

using namespace Windows::Foundation;
using namespace Windows::Networking;
using namespace Windows::Networking::Connectivity;
using namespace Windows::Networking::Sockets;

/*
====================
NET_OnConnectionReceived
====================
*/
void NET_OnConnectionReceivedAsync( 
    StreamSocketListener^ sender, 
    StreamSocketListenerConnectionReceivedEventArgs^ args )
{
    // @pjb: Todo

    Win8::MSG msg;
    INIT_MSG( msg );
    msg.Message = NET_MSG_CLIENT_CONNECT;
    
    g_NetMsgQueue.Post( &msg );
}

