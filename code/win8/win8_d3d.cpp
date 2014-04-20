#include "d3d_common.h"
#include "d3d_device.h"

HANDLE g_WaitingForVideoEvent = nullptr;
HANDLE g_WaitingForVideoFinishedEvent = nullptr;
int g_WindowWidth = 0;
int g_WindowHeight = 0;
IUnknown* g_Window = nullptr;

void D3DWin8_InitDeferral()
{
    g_WaitingForVideoEvent = CreateEventEx( nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS );
    g_WaitingForVideoFinishedEvent = CreateEventEx( nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS );
}

void D3DWin8_CleanupDeferral()
{
    SAFE_RELEASE(g_Window);
    CloseHandle( g_WaitingForVideoEvent );
    CloseHandle( g_WaitingForVideoFinishedEvent );
}

void D3DWin8_Init()
{
    // Wait for video init before we do anything
    WaitForSingleObjectEx( g_WaitingForVideoEvent, INFINITE, FALSE );

    assert(g_Window);

    // Now go ahead :)
    QD3D11Device* device = InitDevice();

    IDXGIFactory2* dxgiFactory = nullptr;
    HRESULT hr = QD3D::GetDxgiFactory( device, &dxgiFactory );
    if ( FAILED( hr ) )
    {
        ri.Error( ERR_FATAL, "Failed to get DXGI Factory: 0x%08x.\n", hr );
        return;
    }

    // Prepare the swap chain
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
    ZeroMemory( &swapChainDesc, sizeof(swapChainDesc) );

    GetSwapChainDescFromConfig( &swapChainDesc );

    // Set up win8 params. Force disable multisampling.
    swapChainDesc.Width = (UINT) g_WindowWidth;
    swapChainDesc.Height = (UINT) g_WindowHeight;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // All Windows Store apps must use this SwapEffect.
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    IDXGISwapChain1* swapChain = nullptr;
    hr = dxgiFactory->CreateSwapChainForCoreWindow(
        device, 
        g_Window,
        &swapChainDesc, 
        nullptr, 
        &swapChain);

    if (FAILED(hr))
    {
        ri.Error( ERR_FATAL, "Failed to create Direct3D 11 swapchain: 0x%08x.\n", hr );
        return;
    }

    QDXGIDevice* dxgiDevice = nullptr;
    hr = QD3D::GetDxgiDevice( device, &dxgiDevice );
    if ( SUCCEEDED( hr ) )
    {
		// Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
		// ensures that the application will only render after each VSync, minimizing power consumption.
        dxgiDevice->SetMaximumFrameLatency(1);
    }

    InitSwapChain( swapChain );

    SAFE_RELEASE( swapChain );
    SAFE_RELEASE( device );

    // release the main thread
    SetEvent( g_WaitingForVideoFinishedEvent );
}

void D3DWin8_Shutdown()
{
    DestroyDevice();
    DestroySwapChain();
}

void D3DWin8_NotifyNewWindow( IUnknown* window, int logicalSizeX, int logicalSizeY )
{
    // Re-enable the deferral of the video init
    ResetEvent( g_WaitingForVideoEvent );
    ResetEvent( g_WaitingForVideoFinishedEvent );

    // Restart the video if we're already running
    if ( g_Window != nullptr )
    {
        Cbuf_AddText( "vid_restart\n" );
        SAFE_RELEASE( g_Window );
    }

    // Swap out the window information
    g_Window = window;
    g_WindowWidth = logicalSizeX;
    g_WindowHeight = logicalSizeY;

    // Allow the game to run again
    SetEvent( g_WaitingForVideoEvent );

    // Wait for video to bring up before we allow the main thread to continue
    WaitForSingleObjectEx( g_WaitingForVideoFinishedEvent, INFINITE, FALSE );
}

void D3DWin8_NotifyWindowResize( int logicalSizeX, int logicalSizeY )
{
    assert( g_Window );

    g_WindowWidth = logicalSizeX;
    g_WindowHeight = logicalSizeY;

    // Allow the video driver to proceed now we have the right information
    if ( DeviceStarted() )
    {
        Cbuf_AddText( "vid_restart\n" );
    }
}
