extern "C" {
#   include "../game/q_shared.h"
#   include "../qcommon/qcommon.h"
#   include "../win32/win_shared.h"
}

#include <ppl.h>
#include <ppltasks.h>
#include <assert.h>

static cvar_t	*net_noudp;
static cvar_t	*net_noipx;

static cvar_t	*net_socksEnabled;
static cvar_t	*net_socksServer;
static cvar_t	*net_socksPort;
static cvar_t	*net_socksUsername;
static cvar_t	*net_socksPassword;

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
====================
NET_Init
====================
*/
WIN8_EXPORT void NET_Init( void ) {
    // @pjb: todo
}

/*
====================
NET_Shutdown
====================
*/
WIN8_EXPORT void NET_Shutdown( void ) {
    // @pjb: todo
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
    // @pjb: todo
}
