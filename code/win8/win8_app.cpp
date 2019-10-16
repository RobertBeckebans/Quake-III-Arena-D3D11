#include <ppl.h>
#include <ppltasks.h>
#include <assert.h>
#include <agile.h>

#include "win8_app.h"
#include "win8_msgq.h"
#include "win8_msgs.h"
#include "win8_utils.h"
#include "win8_d3d.h"

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
using namespace Windows::Devices::Input;
using namespace concurrency;

#define NUM_MOUSE_BUTTONS 3

void Sys_SetFrameTime( int time );

int ConvertDipsToPixels(float dips)
{
#ifdef _WIN32_WINNT_WINBLUE
	static const float dipsPerInch = 96.0f;
	return (int) floor(dips * DisplayInformation::GetForCurrentView()->LogicalDpi / dipsPerInch + 0.5f); // Round to nearest integer.
#else
	static const float dipsPerInch = 96.0f;
	return (int) floor(dips * DisplayProperties::LogicalDpi / dipsPerInch + 0.5f); // Round to nearest integer.
#endif
}

namespace Win8
{
    MessageQueue g_gameMsgs;
    MessageQueue g_sysMsgs;

    void HandleMessage( const MSG* msg )
    {
        switch (msg->Message)
        {
        case GAME_MSG_VIDEO_CHANGE:
            D3DWin8_NotifyWindowResize( (int) msg->Param0, (int) msg->Param1 );
            break;
        case GAME_MSG_MOUSE_MOVE:
        	Sys_QueEvent( 0, SE_MOUSE, (int) msg->Param0, (int) msg->Param1, 0, NULL );
            break;
        case GAME_MSG_MOUSE_WHEEL:
            {
                int delta = (int) msg->Param0;
                if ( delta > 0 )
                {
				    Sys_QueEvent( SYS_EVENT_FRAME_TIME, SE_KEY, K_MWHEELUP, qtrue, 0, NULL );
				    Sys_QueEvent( SYS_EVENT_FRAME_TIME, SE_KEY, K_MWHEELUP, qfalse, 0, NULL );
                }
                else
                {
				    Sys_QueEvent( SYS_EVENT_FRAME_TIME, SE_KEY, K_MWHEELDOWN, qtrue, 0, NULL );
				    Sys_QueEvent( SYS_EVENT_FRAME_TIME, SE_KEY, K_MWHEELDOWN, qfalse, 0, NULL );
                }
            }
            break;
        case GAME_MSG_MOUSE_DOWN: 
            for ( size_t i = 0; i < NUM_MOUSE_BUTTONS; ++i )
            {
                // Bit is set if the mouse button was pressed
                if ( msg->Param0 & (1LL << i) )
		        	Sys_QueEvent( SYS_EVENT_FRAME_TIME, SE_KEY, K_MOUSE1 + (int) i, qtrue, 0, NULL );
            }
            break;
        case GAME_MSG_MOUSE_UP: 
            for ( size_t i = 0; i < NUM_MOUSE_BUTTONS; ++i )
            {
                // Bit is set if the mouse button was released
                if ( msg->Param0 & (1LL << i) )
		        	Sys_QueEvent( SYS_EVENT_FRAME_TIME, SE_KEY, K_MOUSE1 + (int) i, qfalse, 0, NULL );
            }
            break;
        case GAME_MSG_KEY_CHAR:
		    Sys_QueEvent( SYS_EVENT_FRAME_TIME, SE_CHAR, (int) msg->Param0, 0, 0, NULL );
            break;
        case GAME_MSG_KEY_DOWN:
    		Sys_QueEvent( SYS_EVENT_FRAME_TIME, SE_KEY, Sys_MapKey( (int) msg->Param1 ), qtrue, 0, NULL );
            break;
        case GAME_MSG_KEY_UP:
    		Sys_QueEvent( SYS_EVENT_FRAME_TIME, SE_KEY, Sys_MapKey( (int) msg->Param1 ), qfalse, 0, NULL );
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
            while ( g_gameMsgs.Pop( &msg ) )
            {
                if ( msg.Message == GAME_MSG_QUIT )
                    return;

                Sys_SetFrameTime( (int) msg.TimeStamp );

                HandleMessage( &msg );
            }

		    // make sure mouse and joystick are only called once a frame
		    IN_Frame();

		    // run the game
		    Com_Frame();
	    };
    }

    void PostGameReadyMsg()
    {
        MSG msg;
        ZeroMemory( &msg, sizeof( msg ) );
        msg.Message = SYS_MSG_GAME_READY;
        msg.TimeStamp = Sys_Milliseconds();
        g_sysMsgs.Post( &msg );
    }

    void GameThread()
    {
        try
        {
            // Init quake stuff
	        Sys_InitTimer();
            Sys_InitStreamThread();

	        Com_Init( sys_cmdline );
	        NET_Init();
            IN_Init();

	        Com_Printf( "Working directory: %s\n", Sys_Cwd() );

            PostGameReadyMsg();

            // Loop until done
            GameThreadLoop();
        }
        catch ( Platform::Exception^ ex )
        {
            Win8::MSG msg;
            ZeroMemory( &msg, sizeof(msg) );
            msg.Message = SYS_MSG_EXCEPTION;
            msg.Param0 = (size_t) Win8::GetPointer( ex );
            msg.TimeStamp = Sys_Milliseconds();
            g_sysMsgs.Post( &msg );
        }
        catch ( ... )
        {
        }

        Com_Quit_f();

        IN_Shutdown();
        NET_Shutdown();

        D3DWin8_CleanupDeferral();
    }
}

void Win8::PostQuitMessage()
{
    Win8::MSG msg;
    ZeroMemory( &msg, sizeof(msg) );
    msg.Message = GAME_MSG_QUIT;
    msg.TimeStamp = Sys_Milliseconds();
    g_gameMsgs.Post(&msg);
}

Win8::Quake3App::Quake3App() :
	m_windowClosed(false),
	m_windowVisible(true),
    m_currentMsgDlg(nullptr),
    m_gameIsDone(false)
{
}

void Win8::Quake3App::Initialize(CoreApplicationView^ applicationView)
{
    // @pjb: Todo: on Blue this is way better: poll m_gameThread.is_done()
    m_gameThread = Concurrency::create_task( [] ()
    {
        Win8::GameThread();
    } ).then( [this] () 
    {
        m_gameIsDone = true;
    } );

	applicationView->Activated +=
        ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &Quake3App::OnActivated);

	CoreApplication::Suspending +=
        ref new EventHandler<SuspendingEventArgs^>(this, &Quake3App::OnSuspending);

	CoreApplication::Resuming +=
        ref new EventHandler<Platform::Object^>(this, &Quake3App::OnResuming);
}

void Win8::Quake3App::SetWindow(CoreWindow^ window)
{
	window->SizeChanged += 
        ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &Quake3App::OnWindowSizeChanged);

	window->VisibilityChanged +=
		ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &Quake3App::OnVisibilityChanged);

	window->Closed += 
        ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &Quake3App::OnWindowClosed);

	window->PointerPressed +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &Quake3App::OnPointerPressed);

	window->PointerReleased +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &Quake3App::OnPointerReleased);

    window->PointerWheelChanged +=
        ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &Quake3App::OnPointerWheelChanged);

	window->CharacterReceived +=
		ref new TypedEventHandler<CoreWindow^, CharacterReceivedEventArgs^>(this, &Quake3App::OnCharacterReceived);

	window->KeyDown +=
		ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &Quake3App::OnKeyDown);

	window->KeyUp +=
		ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &Quake3App::OnKeyUp);

	window->PointerCursor = nullptr;

	// Disable all pointer visual feedback for better performance when touching.
	auto pointerVisualizationSettings = Windows::UI::Input::PointerVisualizationSettings::GetForCurrentView();
	pointerVisualizationSettings->IsContactFeedbackEnabled = false; 
	pointerVisualizationSettings->IsBarrelButtonFeedbackEnabled = false;
	m_logicalSize = Windows::Foundation::Size(window->Bounds.Width, window->Bounds.Height);

    IUnknown* windowPtr = Win8::GetPointer( window );
    D3DWin8_NotifyNewWindow( 
        windowPtr, 
        ConvertDipsToPixels( m_logicalSize.Width ), 
        ConvertDipsToPixels( m_logicalSize.Height ) );
}

void Win8::Quake3App::Load(Platform::String^ entryPoint)
{
}

void Win8::Quake3App::HandleExceptionMessage( const MSG* msg )
{
    assert( msg->Message == SYS_MSG_EXCEPTION );
    m_currentMsgDlg = Win8::DisplayException( Win8::GetType<Platform::Exception>( (IUnknown*) msg->Param0 ) );
}

void Win8::Quake3App::WaitForGameReady()
{
    Win8::MSG msg;

    // Get new messages
    while ( !g_sysMsgs.Pop( &msg ) ) {}

    HandleMessageFromGame( &msg );
}

void Win8::Quake3App::HandleMessagesFromGame()
{
    Win8::MSG msg;

    // Get new messages
    while ( g_sysMsgs.Pop( &msg ) )
    {
        HandleMessageFromGame( &msg );
    }
}

void Win8::Quake3App::HandleMessageFromGame( const MSG* msg )
{
    switch ( msg->Message )
    {
    case SYS_MSG_EXCEPTION:
        HandleExceptionMessage( msg );
        break;
    case SYS_MSG_GAME_READY:
        // Yay, game is ready to go.
        break;
    default:
        assert(0);
        break;
    }
}

void Win8::Quake3App::Run()
{
    // Stall until the game is ready
    // WaitForGameReady(); // @pjb: commented out because it's not really a problem

	while ( !m_gameIsDone ) // @pjb: Todo: on Blue this is way better: m_gameThread.is_done()
	{
		if (m_windowVisible)
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
			
            HandleMessagesFromGame();
		}
		else
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
		}
	}

    // Consume any dangling messagse
    HandleMessagesFromGame();

    // If there's a dialog waiting to be dismissed, poll
    if ( m_currentMsgDlg != nullptr )
    {
        std::atomic<bool> dlgDismissed = false;
        Concurrency::create_task( m_currentMsgDlg ).then([&dlgDismissed] ( Windows::UI::Popups::IUICommand^ ) 
        {
            dlgDismissed = true;
        });

        // @pjb: Todo: on Blue this is way better: task.is_done()
        while ( !dlgDismissed )
        {
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
        }
    }
}

void Win8::Quake3App::Uninitialize()
{
    ReleaseMouse();
}

void Win8::Quake3App::OnWindowSizeChanged(CoreWindow^ window, WindowSizeChangedEventArgs^ args)
{
	m_logicalSize = Windows::Foundation::Size(window->Bounds.Width, window->Bounds.Height);
    
    Win8::MSG msg;
    ZeroMemory( &msg, sizeof(msg) );
    msg.Message = GAME_MSG_VIDEO_CHANGE;
    msg.Param0 = ConvertDipsToPixels( m_logicalSize.Width );
    msg.Param1 = ConvertDipsToPixels( m_logicalSize.Height );
    msg.TimeStamp = Sys_Milliseconds();
    g_gameMsgs.Post( &msg );
}

void Win8::Quake3App::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
	m_windowVisible = args->Visible;
}

void Win8::Quake3App::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
	m_windowClosed = true;

    Win8::MSG msg;
    ZeroMemory( &msg, sizeof(msg) );
    msg.Message = GAME_MSG_QUIT;
    msg.TimeStamp = Sys_Milliseconds();
    g_gameMsgs.Post( &msg );
}

// Pack the keycode information in the Win32 way (given here:
// http://msdn.microsoft.com/en-us/library/windows/desktop/ms646280(v=vs.85).aspx)
static size_t PackKeyStatus( Windows::UI::Core::CorePhysicalKeyStatus status, bool keyDown )
{
    size_t packed;
    packed  = status.RepeatCount & 0xFFFF;
    packed |= (status.ScanCode & 0xFF) << 16;
    packed |= (status.IsExtendedKey & 1) << 24;
    packed |= (status.WasKeyDown & 1) << 30;
    packed |= ((keyDown & 1) ^ 1) << 31;
    return packed;
}

static size_t PackMouseButtons( Windows::UI::Input::PointerPointProperties^ properties )
{
    // @pjb: todo: support for more buttons?
    size_t packed = 0;
    packed |= (properties->IsLeftButtonPressed & 1);
    packed |= (properties->IsRightButtonPressed & 1) << 1;
    packed |= (properties->IsMiddleButtonPressed & 1) << 2;
    return packed;
}

void Win8::Quake3App::OnPointerPressed(CoreWindow^ sender, PointerEventArgs^ args)
{
    Win8::MSG msg;
    ZeroMemory( &msg, sizeof(msg) );
    msg.Message = GAME_MSG_MOUSE_DOWN;
    msg.Param0 = PackMouseButtons( args->CurrentPoint->Properties );
    msg.TimeStamp = Sys_Milliseconds();
    g_gameMsgs.Post( &msg );
}

void Win8::Quake3App::OnPointerReleased(CoreWindow^ sender, PointerEventArgs^ args)
{
    Win8::MSG msg;
    ZeroMemory( &msg, sizeof(msg) );
    msg.Message = GAME_MSG_MOUSE_UP;
    msg.Param0 = ~PackMouseButtons( args->CurrentPoint->Properties );
    msg.TimeStamp = Sys_Milliseconds();
    g_gameMsgs.Post( &msg );
}

void Win8::Quake3App::OnPointerMoved(MouseDevice^ mouse, MouseEventArgs^ args)
{
    Win8::MSG msg;
    ZeroMemory( &msg, sizeof(msg) );
    msg.Message = GAME_MSG_MOUSE_MOVE;
    msg.Param0 = args->MouseDelta.X;
    msg.Param1 = args->MouseDelta.Y;
    msg.TimeStamp = Sys_Milliseconds();
    g_gameMsgs.Post( &msg );
}

void Win8::Quake3App::OnPointerWheelChanged(CoreWindow^ sender, PointerEventArgs^ args)
{
    Win8::MSG msg;
    ZeroMemory( &msg, sizeof(msg) );
    msg.Message = GAME_MSG_MOUSE_WHEEL;
    msg.Param0 = args->CurrentPoint->Properties->MouseWheelDelta;
    msg.TimeStamp = Sys_Milliseconds();
    g_gameMsgs.Post( &msg );
}

void Win8::Quake3App::OnCharacterReceived(CoreWindow^ sender, CharacterReceivedEventArgs^ args)
{
    Win8::MSG msg;
    ZeroMemory( &msg, sizeof(msg) );
    msg.Message = GAME_MSG_KEY_CHAR;
    msg.Param0 = (size_t) args->KeyCode;
    msg.Param1 = PackKeyStatus( args->KeyStatus, true );
    msg.TimeStamp = Sys_Milliseconds();
    g_gameMsgs.Post( &msg );
}

void Win8::Quake3App::OnKeyDown(CoreWindow^ sender, KeyEventArgs^ args)
{
    Win8::MSG msg;
    ZeroMemory( &msg, sizeof(msg) );
    msg.Message = GAME_MSG_KEY_DOWN;
    msg.Param0 = (size_t) args->VirtualKey;
    msg.Param1 = PackKeyStatus( args->KeyStatus, true );
    msg.TimeStamp = Sys_Milliseconds();
    g_gameMsgs.Post( &msg );
}

void Win8::Quake3App::OnKeyUp(CoreWindow^ sender, KeyEventArgs^ args)
{
    Win8::MSG msg;
    ZeroMemory( &msg, sizeof(msg) );
    msg.Message = GAME_MSG_KEY_UP;
    msg.Param0 = (size_t) args->VirtualKey;
    msg.Param1 = PackKeyStatus( args->KeyStatus, false );
    msg.TimeStamp = Sys_Milliseconds();
    g_gameMsgs.Post( &msg );
}

void Win8::Quake3App::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
    auto window = CoreWindow::GetForCurrentThread();
    window->Activate();

    CaptureMouse();
}

void Win8::Quake3App::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
	// Save app state asynchronously after requesting a deferral. Holding a deferral
	// indicates that the application is busy performing suspending operations. Be
	// aware that a deferral may not be held indefinitely. After about five seconds,
	// the app will be forced to exit.
	SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();

    // Quickly quit!
    Win8::MSG msg;
    ZeroMemory( &msg, sizeof(msg) );
    msg.Message = GAME_MSG_QUIT;
    msg.TimeStamp = Sys_Milliseconds();
    g_gameMsgs.Post( &msg );

    ReleaseMouse();

	create_task([this, deferral]()
	{
        // Wait for game to shut down
        m_gameThread.wait();

		deferral->Complete();
	});
}
 
void Win8::Quake3App::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{
	// Restore any data or state that was unloaded on suspend. By default, data
	// and state are persisted when resuming from suspend. Note that this event
	// does not occur if the app was previously terminated.

    // @pjb: todo: do we need to handle this or will simply loading the game cover it?
}

void Win8::Quake3App::CaptureMouse()
{
	m_mouseCaptureHandle = MouseDevice::GetForCurrentView()->MouseMoved +=
		ref new TypedEventHandler<MouseDevice^, MouseEventArgs^>(this, &Quake3App::OnPointerMoved);
}

void Win8::Quake3App::ReleaseMouse()
{
	MouseDevice::GetForCurrentView()->MouseMoved -= m_mouseCaptureHandle;
}

IFrameworkView^ Win8::Quake3ApplicationSource::CreateView()
{
    return ref new Quake3App();
}

/*
==================
main

==================
*/
[Platform::MTAThread]
int main( Platform::Array<Platform::String^>^ args )
{
    Win8::SetCommandLine( args );

    // Force the graphics layer to wait for window bringup
    D3DWin8_InitDeferral();
    
    auto q3ApplicationSource = ref new Win8::Quake3ApplicationSource();

    try
    {
        CoreApplication::Run(q3ApplicationSource);
    }
    catch ( Platform::Exception^ ex )
    {
    }

	return 0;
}
