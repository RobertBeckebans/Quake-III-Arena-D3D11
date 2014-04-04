#pragma once

enum NET_MSG
{
    NET_MSG_CLIENT_CONNECT
};

extern Win8::MessageQueue g_NetMsgQueue;

/*
    Async Events
*/

void NET_OnConnectionReceivedAsync( 
    Windows::Networking::Sockets::StreamSocketListener^ sender, 
    Windows::Networking::Sockets::StreamSocketListenerConnectionReceivedEventArgs^ args );

/*
    Thread-safe Quake functions
*/
extern "C" int Sys_Milliseconds( void );
