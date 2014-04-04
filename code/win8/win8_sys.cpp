extern "C" {
#   include "../client/client.h"
#   include "../qcommon/qcommon.h"
#   include "../win32/win_shared.h"
}

#include <ppl.h>
#include <ppltasks.h>
#include <assert.h>

#include "win8_app.h"

//============================================
// Win8 stuff

static double g_Freq = 0;

void Win8_InitTimer()
{
    LARGE_INTEGER i;
    QueryPerformanceFrequency( &i );
    g_Freq = 1000.0 / i.QuadPart;
}

int Win8_GetTime()
{
    LARGE_INTEGER i;
    QueryPerformanceCounter( &i );
    return (int)( i.QuadPart * g_Freq );
}

size_t Win8_CopyString( Platform::String^ str, char* dst, size_t dstLen )
{
    size_t numConverted = 0;
    errno_t err = wcstombs_s(
        &numConverted,
        dst, dstLen,
        str->Data(), dstLen);
    return numConverted > 0 ? numConverted-1 : 0;
}


Platform::String^ Win8_CopyString( const char* src )
{
    size_t len = strlen( src );

    wchar_t* wide = new wchar_t[len + 1];
    if ( !wide )
        throw; // I've actually no idea how I want to handle this.

    size_t numConverted = 0;
    mbstowcs_s(
        &numConverted,
        wide, len+1,
        src, len );

    Platform::String^ dst = ref new Platform::String( wide );

    delete [] wide;
    
    return dst;
}

size_t Win8_MultiByteToWide( 
    const char* src, 
    wchar_t* dst,
    size_t dstLen )
{
    size_t numConverted = 0;
    mbstowcs_s(
        &numConverted,
        dst, dstLen,
        src, strlen( src ) );

    return numConverted > 0 ? numConverted-1 : 0;
}

void Win8_SetCommandLine( Platform::Array<Platform::String^>^ args )
{
    // Set the command line string
    size_t offset = 0;
    for ( int i = 1; i < args->Length; ++i )
    {
        if ( offset + 2 + args[i]->Length() >= MAX_STRING_CHARS )
            break;

        sys_cmdline[offset++] = '\"';
        offset += Win8_CopyString( args[i], sys_cmdline + offset, MAX_STRING_CHARS - offset - 2 );
        sys_cmdline[offset++] = '\"';

        if ( offset < MAX_STRING_CHARS - 1 )
        {
            sys_cmdline[offset++] = ' ';
            sys_cmdline[offset] = 0;
        }
    }
}

void Win8_Throw( HRESULT hr, Platform::String^ str )
{
    if ( str == nullptr )
        throw ref new Platform::Exception( hr );
    else
        throw ref new Platform::Exception( hr, str );
}

//============================================
// Quake APIs

WIN8_EXPORT char *Sys_GetCurrentUser( void )
{
	static char s_userName[1024];
	unsigned long size = sizeof( s_userName );

    strcpy( s_userName, "player" );

    try
    {
        auto t = concurrency::create_task( 
            Windows::System::UserProfile::UserInformation::GetDisplayNameAsync() );
    
        Win8_CopyString( t.get(), s_userName, sizeof( s_userName ) );
    }
    catch ( Platform::Exception^ ex )
    {
        OutputDebugStringW( ex->Message->Data() );
    }

	if ( !s_userName[0] )
	{
		strcpy( s_userName, "player" );
	}

	return s_userName;
}

/*
================
Sys_Milliseconds
================
*/
int			sys_timeBase;
WIN8_EXPORT void  Sys_InitTimer (void)
{
	static qboolean	initialized = qfalse;

	if (!initialized) {
        Win8_InitTimer();
		sys_timeBase = Win8_GetTime();
		initialized = qtrue;
	}
}

WIN8_EXPORT int Sys_Milliseconds (void)
{
	return Win8_GetTime() - sys_timeBase;
}

// @pjb I hate this
int g_LastFrameEventTime = 0;
void Sys_SetFrameTime( int time )
{
    g_LastFrameEventTime = time;
}
WIN8_EXPORT int Sys_FrameTime (void)
{
    return g_LastFrameEventTime;
}

/*
==================
Sys_LowPhysicalMemory()
==================
*/

WIN8_EXPORT qboolean Sys_LowPhysicalMemory() {
	// @pjb: TODO: I don't know what the equivalent is here
    return qfalse;
}

/*
==================
Sys_ShowConsole()
==================
*/
WIN8_EXPORT void Sys_ShowConsole( int visLevel, qboolean quitOnClose )
{
    (void)( visLevel );
    (void)( quitOnClose );

    if (quitOnClose)
        Win8_PostQuitMessage();
}

/*
=============
Sys_Error

Show the early console as an error dialog
=============
*/
WIN8_EXPORT void QDECL Sys_Error( const char *error, ... ) {
	va_list		argptr;
	char		text[4096];

	va_start (argptr, error);
	vsprintf (text, error, argptr);
	va_end (argptr);

    OutputDebugStringA( text );

    Platform::String^ textObj = Win8_CopyString( text );
    Win8_Throw( E_FAIL, textObj );
}

/*
==============
Sys_Quit
==============
*/
WIN8_EXPORT void Sys_Quit( void ) {
    Win8_PostQuitMessage();
}

/*
==============
Sys_Print
==============
*/
WIN8_EXPORT void Sys_Print( const char *msg ) {
	OutputDebugStringA( msg );
}

/*
==============
Sys_Cwd

Win8: return the installation directory (read-only)
==============
*/
WIN8_EXPORT char *Sys_Cwd( void ) {
	static char cwd[MAX_OSPATH];

	Windows::ApplicationModel::Package^ pkg = Windows::ApplicationModel::Package::Current;
    Windows::Storage::StorageFolder^ installDir = pkg->InstalledLocation;

    Win8_CopyString( installDir->Path, cwd, sizeof(cwd) );

	return cwd;
}

/*
==============
Sys_UserDir

Win8: return the writable directory
==============
*/
WIN8_EXPORT char *Sys_UserDir( void ) {
	static char cwd[MAX_OSPATH];

	auto localFolder = Windows::Storage::ApplicationData::Current->LocalFolder;

    Win8_CopyString( localFolder->Path, cwd, sizeof(cwd) );

	return cwd;
}

/*
================
Sys_GetClipboardData

================
*/
WIN8_EXPORT char *Sys_GetClipboardData( void ) {

    static char clipBuf[4096];

    auto data = Windows::ApplicationModel::DataTransfer::Clipboard::GetContent();
    if ( data->Contains( Windows::ApplicationModel::DataTransfer::StandardDataFormats::Text ) )
    {
        auto t = concurrency::create_task( data->GetTextAsync() );

        Win8_CopyString( t.get(), clipBuf, sizeof( clipBuf ) );
        return clipBuf;
    }
    else
    {
        return NULL;
    }
}


/*
========================================================================

LOAD/UNLOAD DLL

========================================================================
*/


/*
=================
Sys_UnloadDll

=================
*/
WIN8_EXPORT void Sys_UnloadDll( void *dllHandle ) {
	if ( !dllHandle ) {
		return;
	}
	if ( !FreeLibrary( (HMODULE) dllHandle ) ) {
		Com_Error (ERR_FATAL, "Sys_UnloadDll FreeLibrary failed");
	}
}

/*
=================
Sys_LoadDll

Used to load a development dll instead of a virtual machine

TTimo: added some verbosity in debug
=================
*/
WIN8_EXPORT extern char		*FS_BuildOSPath( const char *base, const char *game, const char *qpath );

// fqpath param added 7/20/02 by T.Ray - Sys_LoadDll is only called in vm.c at this time
// fqpath will be empty if dll not loaded, otherwise will hold fully qualified path of dll module loaded
// fqpath buffersize must be at least MAX_QPATH+1 bytes long
WIN8_EXPORT void * QDECL Sys_LoadDll( const char *name, char *fqpath , int (QDECL **entryPoint)(size_t, ...),
				  int (QDECL *systemcalls)(size_t, ...) ) {
	static int	lastWarning = 0;
	HINSTANCE	libHandle;
	void	(QDECL *dllEntry)( int (QDECL *syscallptr)(size_t, ...) );
	char	filename[MAX_QPATH];
    wchar_t wfilename[MAX_QPATH];

	*fqpath = 0 ;		// added 7/20/02 by T.Ray

#ifdef _M_X64
	Com_sprintf( filename, sizeof( filename ), "%sx64.dll", name );
#elif defined(_ARM_)
	Com_sprintf( filename, sizeof( filename ), "%sarm.dll", name );
#else
    Com_sprintf(filename, sizeof(filename), "%sx86.dll", name);
#endif

    Win8_MultiByteToWide( filename, wfilename, _countof( wfilename ) );
    
    // There's a lot of logic here in the original game (dealing with overloaded
    // paths etc), but WinRT simplifies that for us. It can only ever be in the root 
    // folder.
	libHandle = LoadPackagedLibrary( wfilename, 0 );
    if (libHandle) {
        Com_Printf("LoadPackagedLibrary '%s' ok\n", filename);
    }
    else {
        Com_Printf("LoadPackagedLibrary '%s' failed\n", filename);
        return NULL;
    }

	dllEntry = ( void (QDECL *)( int (QDECL *)( size_t, ... ) ) )GetProcAddress( libHandle, "dllEntry" ); 
	*entryPoint = (int (QDECL *)(size_t,...))GetProcAddress( libHandle, "vmMain" );
	if ( !*entryPoint || !dllEntry ) {
		FreeLibrary( libHandle );
		return NULL;
	}
	dllEntry( systemcalls );

	if ( libHandle ) Q_strncpyz ( fqpath , filename , MAX_QPATH ) ;		// added 7/20/02 by T.Ray
	return libHandle;
}

/*
================
Sys_GetEvent

================
*/
WIN8_EXPORT sysEvent_t Sys_GetEvent( void ) {
	sysEvent_t	ev;
	msg_t		netmsg;
	netadr_t	adr;

	// return if we have data
	if ( eventHead > eventTail ) {
		eventTail++;
		return eventQue[ ( eventTail - 1 ) & MASK_QUED_EVENTS ];
	}

	// check for network packets
	MSG_Init( &netmsg, sys_packetReceived, sizeof( sys_packetReceived ) );
	if ( Sys_GetPacket ( &adr, &netmsg ) ) {
		netadr_t		*buf;
		int				len;

		// copy out to a seperate buffer for qeueing
		// the readcount stepahead is for SOCKS support
		len = sizeof( netadr_t ) + netmsg.cursize - netmsg.readcount;
		buf = ( netadr_t* ) Z_Malloc( len );
		*buf = adr;
		memcpy( buf+1, &netmsg.data[netmsg.readcount], netmsg.cursize - netmsg.readcount );
		Sys_QueEvent( 0, SE_PACKET, 0, 0, len, buf );
	}

	// return if we have data
	if ( eventHead > eventTail ) {
		eventTail++;
		return eventQue[ ( eventTail - 1 ) & MASK_QUED_EVENTS ];
	}

	// create an empty event to return

	memset( &ev, 0, sizeof( ev ) );
	ev.evTime = Sys_Milliseconds();

	return ev;
}

/*
================
Sys_Init

Called after the common systems (cvars, files, etc)
are initialized
================
*/
WIN8_EXPORT void Sys_Init( void ) {

	Cmd_AddCommand ("in_restart", Sys_In_Restart_f);
	Cmd_AddCommand ("net_restart", Sys_Net_Restart_f);

	Cvar_Set( "arch", "win8" );

    Sys_DetectCPU();

	Cvar_Set( "username", Sys_GetCurrentUser() );

	IN_Init();		// FIXME: not in dedicated?
}





