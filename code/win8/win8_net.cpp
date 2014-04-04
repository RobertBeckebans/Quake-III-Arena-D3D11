extern "C" {
#   include "../game/q_shared.h"
#   include "../qcommon/qcommon.h"
#   include "../win32/win_shared.h"
}

#include <ppl.h>
#include <ppltasks.h>
#include <assert.h>

#include "win8_msgq.h"
#include "win8_app.h"

using namespace Windows::Networking;
using namespace Windows::Networking::Connectivity;
using namespace Windows::Networking::Sockets;

static qboolean networkingEnabled = qfalse;

static cvar_t	*net_noudp;


/*
=============
Sys_StringToAdr

idnewt
192.246.40.70
=============
*/
WIN8_EXPORT qboolean Sys_StringToAdr( const char *s, netadr_t *a ) {
    // @pjb: todo
    return qfalse;
}

/*
==================
Sys_GetPacket

Never called by the game logic, just the system event queing
==================
*/
WIN8_EXPORT qboolean Sys_GetPacket( netadr_t *net_from, msg_t *net_message ) {
    // @pjb: todo
    return qfalse;
}

/*
==================
Sys_SendPacket
==================
*/
WIN8_EXPORT void Sys_SendPacket( int length, const void *data, netadr_t to ) {
    // @pjb: todo
}

/*
==================
Sys_IsLANAddress

LAN clients will have their rate var ignored
==================
*/
WIN8_EXPORT qboolean Sys_IsLANAddress( netadr_t adr ) {
    // @pjb: todo
    return qfalse;
}

/*
==================
Sys_ShowIP
==================
*/
WIN8_EXPORT void Sys_ShowIP(void) {
    // @pjb: todo
}

/*
=====================
NET_StartListeningOnAddressAndPort
=====================
*/
qboolean NET_StartListeningOnAddressAndPort( const char* ip, int port )
{
    // @pjb: todo
}

/*
=====================
NET_GetLocalAddress
=====================
*/
void NET_GetLocalAddress( void ) {
    // @pjb: todo
}

/*
====================
NET_StartListening
====================
*/
void NET_StartListening( void ) {

	cvar_t* ip = Cvar_Get( "net_ip", "localhost", CVAR_LATCH );
	int port = Cvar_Get( "net_port", va( "%i", PORT_SERVER ), CVAR_LATCH )->integer;

	// automatically scan for a valid port, so multiple
	// dedicated servers can be started without requiring
	// a different net_port for each one
	for( int i = 0 ; i < 10 ; i++ ) {
		if ( NET_StartListeningOnAddressAndPort( ip->string, port + i ) ) {
			Cvar_SetValue( "net_port", port + i );
			NET_GetLocalAddress();
			return;
		}
	}
	Com_Printf( "WARNING: Couldn't allocate IP port\n");
}
/*
====================
NET_StopListening
====================
*/
void NET_StopListening( void ) {
    // @pjb: todo
}

/*
====================
NET_GetCvars
====================
*/
static qboolean NET_GetCvars( void ) {
	qboolean	modified;

	modified = qfalse;

	if( net_noudp && net_noudp->modified ) {
		modified = qtrue;
	}
	net_noudp = Cvar_Get( "net_noudp", "0", CVAR_LATCH | CVAR_ARCHIVE );

	return modified;
}

/*
====================
NET_Config
====================
*/
void NET_Config( qboolean enableNetworking ) {

	qboolean	modified;
	qboolean	stop;
	qboolean	start;

	// get any latched changes to cvars
	modified = NET_GetCvars();

	if( net_noudp->integer ) {
		enableNetworking = qfalse;
	}

	// if enable state is the same and no cvars were modified, we have nothing to do
	if( enableNetworking == networkingEnabled && !modified ) {
		return;
	}

	if( enableNetworking == networkingEnabled ) {
		if( enableNetworking ) {
			stop = qtrue;
			start = qtrue;
		}
		else {
			stop = qfalse;
			start = qfalse;
		}
	}
	else {
		if( enableNetworking ) {
			stop = qfalse;
			start = qtrue;
		}
		else {
			stop = qtrue;
			start = qfalse;
		}
		networkingEnabled = enableNetworking;
	}

	if( stop ) {
		NET_StopListening();
	}

	if( start ) {
		NET_StartListening();
	}
}


/*
====================
NET_Init
====================
*/
WIN8_EXPORT void NET_Init( void ) {

    // @pjb: todo

    // Print the local addresses and adapters
	Com_Printf( "Init WinRT Sockets\n" );

    char hostnameTmp[256];
    char adapterIdTmp[256];
    for each ( HostName^ localHostInfo in NetworkInformation::GetHostNames() )
    {
        if ( localHostInfo->IPInformation != nullptr )
        {
            Win8_CopyString( localHostInfo->DisplayName, hostnameTmp, sizeof( hostnameTmp ) );
            Win8_CopyString( localHostInfo->IPInformation->NetworkAdapter->NetworkAdapterId->ToString(), adapterIdTmp, sizeof( adapterIdTmp ) );

            Com_Printf( "... Address: %s Adapter: %s\n", hostnameTmp, adapterIdTmp );
        }
    }

    // Configure the addresses
	NET_Config( qtrue );
}

/*
====================
NET_Shutdown
====================
*/
WIN8_EXPORT void NET_Shutdown( void ) {

    // @pjb: todo

	NET_Config( qfalse );
}

/*
====================
NET_Sleep

sleeps msec or until net socket is ready
====================
*/
WIN8_EXPORT void NET_Sleep( int msec ) {
    // @pjb: nothing to do here.
}

/*
====================
NET_Restart_f
====================
*/
WIN8_EXPORT void NET_Restart( void ) {
	NET_Config( networkingEnabled );
}
