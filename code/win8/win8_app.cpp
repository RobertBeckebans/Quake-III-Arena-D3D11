#include <ppl.h>
#include <ppltasks.h>
#include <assert.h>
#include "win8_app.h"
#include "win8_msgq.h"
#include "../d3d11/d3d_win8.h"

extern "C" {
#   include "../client/client.h"
#   include "../qcommon/qcommon.h"
#   include "../win32/win_shared.h"
}

using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace concurrency;

bool Win8_DisplayException( Platform::Exception^ ex )
{
    // Throw up a dialog box!
    try
    {
        Windows::UI::Popups::MessageDialog^ dlg = ref new Windows::UI::Popups::MessageDialog(ex->Message);
        concurrency::create_task( dlg->ShowAsync() );

        return true;
    }
    catch ( Platform::AccessDeniedException^ )
    {
        return false;
    }
    catch ( Platform::Exception^ ex )
    {
        OutputDebugStringW( ex->Message->Data() );
        return false;
    }
    catch ( ... )
    {
        return false;
    }
}

namespace Q3Win8
{
    void HandleMessage( const MSG* msg )
    {
        switch (msg->Message)
        {
        case MSG_VIDEO_CHANGE:
            D3DWin8_NotifyWindowResize( (int) msg->Param0, (int) msg->Param1 );
            break;
        default:
            // Wat
            assert(0);
        }
    }

    void GameThreadLoop()
    {
        MSG msg;

        for ( ; ; )
	    {
            // Wait for a new frame
            // @pjb: todo

            // Process any messages from the queue
            while ( PopMessage( &msg ) )
            {
                if ( msg.Message == MSG_QUIT )
                    return;

                HandleMessage( &msg );
            }

		    // make sure mouse and joystick are only called once a frame
		    IN_Frame();

		    // run the game
		    Com_Frame();
	    };
    }

    int GameThread( void* )
    {
        try
        {
            // Force the graphics layer to wait for window bringup
            D3DWin8_InitDeferral();

            // Init quake stuff
	        Sys_InitTimer();
            Sys_InitStreamThread();

	        Com_Init( sys_cmdline );
	        NET_Init();

	        Com_Printf( "Working directory: %s\n", Sys_Cwd() );

            // Loop until done
            GameThreadLoop();
        }
        catch ( Platform::Exception^ ex )
        {
            Win8_DisplayException( ex );
        }
        catch ( ... )
        {
        }

        Com_Quit_f();

        IN_Shutdown();
        NET_Shutdown();

        return 0;
    }
}

Quake3Win8App::Quake3Win8App() :
	m_windowClosed(false),
	m_windowVisible(true)
{
}

void Quake3Win8App::Initialize(CoreApplicationView^ applicationView)
{
	applicationView->Activated +=
        ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &Quake3Win8App::OnActivated);

	CoreApplication::Suspending +=
        ref new EventHandler<SuspendingEventArgs^>(this, &Quake3Win8App::OnSuspending);

	CoreApplication::Resuming +=
        ref new EventHandler<Platform::Object^>(this, &Quake3Win8App::OnResuming);
}

void Quake3Win8App::SetWindow(CoreWindow^ window)
{
	window->SizeChanged += 
        ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &Quake3Win8App::OnWindowSizeChanged);

	window->VisibilityChanged +=
		ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &Quake3Win8App::OnVisibilityChanged);

	window->Closed += 
        ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &Quake3Win8App::OnWindowClosed);

	window->PointerCursor = ref new CoreCursor(CoreCursorType::Arrow, 0);

	window->PointerPressed +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &Quake3Win8App::OnPointerPressed);

	window->PointerMoved +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &Quake3Win8App::OnPointerMoved);

	// Disable all pointer visual feedback for better performance when touching.
	auto pointerVisualizationSettings = Windows::UI::Input::PointerVisualizationSettings::GetForCurrentView();
	pointerVisualizationSettings->IsContactFeedbackEnabled = false; 
	pointerVisualizationSettings->IsBarrelButtonFeedbackEnabled = false;
	m_logicalSize = Windows::Foundation::Size(window->Bounds.Width, window->Bounds.Height);

    Q3Win8::MSG msg;
    ZeroMemory( &msg, sizeof(msg) );
    msg.Message = Q3Win8::MSG_VIDEO_CHANGE;
    msg.Param0 = m_logicalSize.Width;
    msg.Param1 = m_logicalSize.Height;
    Q3Win8::PostMessage( &msg );
}

void Quake3Win8App::Load(Platform::String^ entryPoint)
{
    m_gameThread = Concurrency::create_task( [] () -> int
    {
        return Q3Win8::GameThread( nullptr );
    } );
}

void Quake3Win8App::Run()
{
	while ( !m_gameThread.is_done() )
	{
		if (m_windowVisible)
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
			
		    // @pjb: todo: Notify the game that there's a new frame
		}
		else
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
		}
	}
}

void Quake3Win8App::Uninitialize()
{
}

void Quake3Win8App::OnWindowSizeChanged(CoreWindow^ window, WindowSizeChangedEventArgs^ args)
{
	m_logicalSize = Windows::Foundation::Size(window->Bounds.Width, window->Bounds.Height);
    
    Q3Win8::MSG msg;
    ZeroMemory( &msg, sizeof(msg) );
    msg.Message = Q3Win8::MSG_VIDEO_CHANGE;
    msg.Param0 = m_logicalSize.Width;
    msg.Param1 = m_logicalSize.Height;
    Q3Win8::PostMessage( &msg );
}

void Quake3Win8App::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
	m_windowVisible = args->Visible;
}

void Quake3Win8App::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
	m_windowClosed = true;

    Q3Win8::MSG msg;
    ZeroMemory( &msg, sizeof(msg) );
    msg.Message = Q3Win8::MSG_QUIT;
    Q3Win8::PostMessage( &msg );
}

void Quake3Win8App::OnPointerPressed(CoreWindow^ sender, PointerEventArgs^ args)
{
	// @pjb: todo: click event?
}

void Quake3Win8App::OnPointerMoved(CoreWindow^ sender, PointerEventArgs^ args)
{
	// @pjb: todo: emit mouse moved event
}

void Quake3Win8App::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
	CoreWindow::GetForCurrentThread()->Activate();
}

void Quake3Win8App::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
	// Save app state asynchronously after requesting a deferral. Holding a deferral
	// indicates that the application is busy performing suspending operations. Be
	// aware that a deferral may not be held indefinitely. After about five seconds,
	// the app will be forced to exit.
	SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();

    // Quickly quit!
    Q3Win8::MSG msg;
    ZeroMemory( &msg, sizeof(msg) );
    msg.Message = Q3Win8::MSG_QUIT;
    Q3Win8::PostMessage( &msg );

	create_task([this, deferral]()
	{
        // Wait for game to shut down
        m_gameThread.wait();

		deferral->Complete();
	});
}
 
void Quake3Win8App::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{
	// Restore any data or state that was unloaded on suspend. By default, data
	// and state are persisted when resuming from suspend. Note that this event
	// does not occur if the app was previously terminated.

    // @pjb: todo: do we need to handle this or will simply loading the game cover it?
}

IFrameworkView^ Quake3Win8ApplicationSource::CreateView()
{
    return ref new Quake3Win8App();
}

/*
==================
main

==================
*/
[Platform::MTAThread]
int main( Platform::Array<Platform::String^>^ args )
{
    Win8_SetCommandLine( args );
    
    auto q3ApplicationSource = ref new Quake3Win8ApplicationSource();

    try
    {
        CoreApplication::Run(q3ApplicationSource);
    }
    catch ( Platform::Exception^ ex )
    {
    }

	return 0;
}
