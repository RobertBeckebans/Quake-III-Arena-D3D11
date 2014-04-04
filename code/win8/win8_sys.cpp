extern "C" {
#   include "../client/client.h"
#   include "../qcommon/qcommon.h"
#   include "../win32/win_shared.h"
}

#include <ppl.h>
#include <ppltasks.h>
#include <assert.h>

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

void Win8_CopyString( Platform::String^ str, char* dst, size_t dstLen )
{
    size_t numConverted = 0;
    errno_t err = wcstombs_s(
        &numConverted,
        dst, dstLen,
        str->Data(), dstLen);
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
        wide, len,
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

    return numConverted;
}

//============================================
// Quake APIs

char *Sys_GetCurrentUser( void )
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
    catch ( ... )
    {
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
void  Sys_InitTimer (void)
{
	static qboolean	initialized = qfalse;

	if (!initialized) {
        Win8_InitTimer();
		sys_timeBase = Win8_GetTime();
		initialized = qtrue;
	}
}

int Sys_Milliseconds (void)
{
	return Win8_GetTime() - sys_timeBase;
}

/*
=============
Sys_Error

Show the early console as an error dialog
=============
*/
void QDECL Sys_Error( const char *error, ... ) {
	va_list		argptr;
	char		text[4096];

	va_start (argptr, error);
	vsprintf (text, error, argptr);
	va_end (argptr);

    OutputDebugStringA( text );

    Platform::String^ textObj = Win8_CopyString( text );

    // Throw up a dialog box!
    Windows::UI::Popups::MessageDialog^ dlg = ref new Windows::UI::Popups::MessageDialog(textObj);
    auto asyncOp = concurrency::create_task( dlg->ShowAsync() );
    asyncOp.wait();

	IN_Shutdown();

    // TODO: Probably a better way to handle this?
	exit (1);
}

/*
==============
Sys_Quit
==============
*/
void Sys_Quit( void ) {

    IN_Shutdown();
	
	exit (0);
}

/*
==============
Sys_Print
==============
*/
void Sys_Print( const char *msg ) {
	OutputDebugStringA( msg );
}

/*
==============
Sys_Cwd

Win8: return the installation directory (read-only)
==============
*/
char *Sys_Cwd( void ) {
	static char cwd[MAX_OSPATH];

	Windows::ApplicationModel::Package^ pkg = Windows::ApplicationModel::Package::Current;
    Windows::Storage::StorageFolder^ installDir = pkg->InstalledLocation;

    Win8_CopyString( installDir->Path, cwd, sizeof(cwd) );

	return cwd;
}

/*
================
Sys_GetClipboardData

================
*/
char *Sys_GetClipboardData( void ) {

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
void Sys_UnloadDll( void *dllHandle ) {
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
extern char		*FS_BuildOSPath( const char *base, const char *game, const char *qpath );

// fqpath param added 7/20/02 by T.Ray - Sys_LoadDll is only called in vm.c at this time
// fqpath will be empty if dll not loaded, otherwise will hold fully qualified path of dll module loaded
// fqpath buffersize must be at least MAX_QPATH+1 bytes long
void * QDECL Sys_LoadDll( const char *name, char *fqpath , int (QDECL **entryPoint)(size_t, ...),
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
sysEvent_t Sys_GetEvent( void ) {
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
#define OSR2_BUILD_NUMBER 1111
#define WIN98_BUILD_NUMBER 1998

void Sys_Init( void ) {
	int cpuid;

	Cmd_AddCommand ("in_restart", Sys_In_Restart_f);
	Cmd_AddCommand ("net_restart", Sys_Net_Restart_f);

	Cvar_Set( "arch", "win8" );

	//
	// figure out our CPU
	//
	Cvar_Get( "sys_cpustring", "detect", 0 );
	if ( !Q_stricmp( Cvar_VariableString( "sys_cpustring"), "detect" ) )
	{
		Com_Printf( "...detecting CPU, found " );

		cpuid = Sys_GetProcessorId();

		switch ( cpuid )
		{
		case CPUID_GENERIC:
			Cvar_Set( "sys_cpustring", "generic" );
			break;
		case CPUID_INTEL_UNSUPPORTED:
			Cvar_Set( "sys_cpustring", "x86 (pre-Pentium)" );
			break;
		case CPUID_INTEL_PENTIUM:
			Cvar_Set( "sys_cpustring", "x86 (P5/PPro, non-MMX)" );
			break;
		case CPUID_INTEL_MMX:
			Cvar_Set( "sys_cpustring", "x86 (P5/Pentium2, MMX)" );
			break;
		case CPUID_INTEL_KATMAI:
			Cvar_Set( "sys_cpustring", "Intel Pentium III" );
			break;
		case CPUID_AMD_3DNOW:
			Cvar_Set( "sys_cpustring", "AMD w/ 3DNow!" );
			break;
		case CPUID_AXP:
			Cvar_Set( "sys_cpustring", "Alpha AXP" );
			break;
		default:
			Com_Error( ERR_FATAL, "Unknown cpu type %d\n", cpuid );
			break;
		}
	}
	else
	{
		Com_Printf( "...forcing CPU type to " );
		if ( !Q_stricmp( Cvar_VariableString( "sys_cpustring" ), "generic" ) )
		{
			cpuid = CPUID_GENERIC;
		}
		else if ( !Q_stricmp( Cvar_VariableString( "sys_cpustring" ), "x87" ) )
		{
			cpuid = CPUID_INTEL_PENTIUM;
		}
		else if ( !Q_stricmp( Cvar_VariableString( "sys_cpustring" ), "mmx" ) )
		{
			cpuid = CPUID_INTEL_MMX;
		}
		else if ( !Q_stricmp( Cvar_VariableString( "sys_cpustring" ), "3dnow" ) )
		{
			cpuid = CPUID_AMD_3DNOW;
		}
		else if ( !Q_stricmp( Cvar_VariableString( "sys_cpustring" ), "PentiumIII" ) )
		{
			cpuid = CPUID_INTEL_KATMAI;
		}
		else if ( !Q_stricmp( Cvar_VariableString( "sys_cpustring" ), "axp" ) )
		{
			cpuid = CPUID_AXP;
		}
		else
		{
			Com_Printf( "WARNING: unknown sys_cpustring '%s'\n", Cvar_VariableString( "sys_cpustring" ) );
			cpuid = CPUID_GENERIC;
		}
	}
	Cvar_SetValue( "sys_cpuid", cpuid );
	Com_Printf( "%s\n", Cvar_VariableString( "sys_cpustring" ) );

	Cvar_Set( "username", Sys_GetCurrentUser() );

	IN_Init();		// FIXME: not in dedicated?
}
/*
//=======================================================================

int	totalMsec, countMsec;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	char		cwd[MAX_OSPATH];
	int			startTime, endTime;

    // should never get a previous instance in Win32
    if ( hPrevInstance ) {
        return 0;
	}

	Q_strncpyz( sys_cmdline, lpCmdLine, sizeof( sys_cmdline ) );

	// no abort/retry/fail errors
	SetErrorMode( SEM_FAILCRITICALERRORS );

	// get the initial time base
	Sys_InitTimer();
    Sys_InitStreamThread();

	Com_Init( sys_cmdline );
	NET_Init();

	Com_Printf( "Working directory: %s\n", Sys_GetCwd() );

    // main game loop
	while( 1 ) {
		// if not running as a game client, sleep a bit
		if ( g_wv.isMinimized || ( com_dedicated && com_dedicated->integer ) ) {
			Sleep( 5 );
		}

		// set low precision every frame, because some system calls
		// reset it arbitrarily
//		_controlfp( _PC_24, _MCW_PC );
//    _controlfp( -1, _MCW_EM  ); // no exceptions, even if some crappy
                                // syscall turns them back on!

		startTime = Sys_Milliseconds();

		// make sure mouse and joystick are only called once a frame
		IN_Frame();

		// run the game
		Com_Frame();

		endTime = Sys_Milliseconds();
		totalMsec += endTime - startTime;
		countMsec++;
	}

	// never gets here
}




*/





