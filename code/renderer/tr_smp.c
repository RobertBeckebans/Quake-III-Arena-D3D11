/*
===========================================================

SMP acceleration

===========================================================
*/

// @pjb: needed to rip this out of GLimp

#include "tr_local.h"
#include "tr_layer.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

HANDLE	renderCommandsEvent;
HANDLE	renderCompletedEvent;
HANDLE	renderActiveEvent;

static	void	*smpData;

static void RenderThreadWrapper( void (* function)( void ) ) {
	function();

	// unbind the context before we die
    graphicsDriver.MakeCurrent( qfalse );
}

/*
=======================
GLimp_SpawnRenderThread
=======================
*/
HANDLE	renderThreadHandle;
int		renderThreadId;
qboolean RSMP_SpawnRenderThread( void (*function)( void ) ) {

	renderCommandsEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	renderCompletedEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	renderActiveEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

	renderThreadHandle = CreateThread(
	   NULL,	// LPSECURITY_ATTRIBUTES lpsa,
	   0,		// DWORD cbStack,
	   (LPTHREAD_START_ROUTINE)RenderThreadWrapper,	// LPTHREAD_START_ROUTINE lpStartAddr,
	   function,			// LPVOID lpvThreadParm,
	   0,			//   DWORD fdwCreate,
	   &renderThreadId );

	if ( !renderThreadHandle ) {
		return qfalse;
	}

	return qtrue;
}

void* RSMP_RendererSleep( void ) {
	void	*data;

    graphicsDriver.MakeCurrent( qfalse );

	// after this, the front end can exit GLimp_FrontEndSleep
	SetEvent( renderCompletedEvent );

	ResetEvent( renderActiveEvent );

	WaitForSingleObject( renderCommandsEvent, INFINITE );

    graphicsDriver.MakeCurrent( qtrue );

	ResetEvent( renderCompletedEvent );
	ResetEvent( renderCommandsEvent );

	data = smpData;

	// after this, the main thread can exit GLimp_WakeRenderer
	SetEvent( renderActiveEvent );

	return data;
}


void RSMP_FrontEndSleep( void ) {
	WaitForSingleObject( renderCompletedEvent, INFINITE );

    graphicsDriver.MakeCurrent( qtrue );
}


void RSMP_WakeRenderer( void *data ) {
	smpData = data;

    graphicsDriver.MakeCurrent( qfalse );

	// after this, the renderer can continue through GLimp_RendererSleep
	SetEvent( renderCommandsEvent );

	WaitForSingleObject( renderActiveEvent, INFINITE );
}

