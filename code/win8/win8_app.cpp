#include <ppl.h>
#include <ppltasks.h>
#include <assert.h>
#include "win8_app.h"


using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace concurrency;

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

	// @pjb: init game, but not graphics yet
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

    // @pjb: todo: initialize renderer here
	//m_renderer->Initialize(CoreWindow::GetForCurrentThread());
}

void Quake3Win8App::Load(Platform::String^ entryPoint)
{
    // @pjb: todo: ???
}

void Quake3Win8App::Run()
{
	while (!m_windowClosed)
	{
		if (m_windowVisible)
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
			
            // @pjb: todo: do a frame
		}
		else
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
		}
	}
}

void Quake3Win8App::Uninitialize()
{
    // @pjb: todo
}

void Quake3Win8App::OnWindowSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args)
{
    // @pjb: todo: window size changed. Vid restart?
}

void Quake3Win8App::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
	m_windowVisible = args->Visible;

    // @pjb: todo: lose the input 
}

void Quake3Win8App::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
	m_windowClosed = true;

    // @pjb: todo: lose the input... or should we die?
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

    // @pjb: todo: rebind input?
}

void Quake3Win8App::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
	// Save app state asynchronously after requesting a deferral. Holding a deferral
	// indicates that the application is busy performing suspending operations. Be
	// aware that a deferral may not be held indefinitely. After about five seconds,
	// the app will be forced to exit.
	SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();

	create_task([this, deferral]()
	{
		// @pjb: todo: kill the game

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
	auto q3ApplicationSource = ref new Quake3Win8ApplicationSource();
	CoreApplication::Run(q3ApplicationSource);
	return 0;
}
