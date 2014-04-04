#include "d3d_common.h"
#include "d3d_device.h"

HANDLE g_WaitingForVideoEvent = nullptr;
int g_WindowWidth = 0;
int g_WindowHeight = 0;
IUnknown* g_Window = nullptr;

void D3DWin8_InitDeferral()
{
    g_WaitingForVideoEvent = CreateEventEx( nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS );
}

void D3DWin8_CleanupDeferral()
{
    SAFE_RELEASE(g_Window);
    CloseHandle( g_WaitingForVideoEvent );
}

void D3DWin8_Init()
{
    // Wait for video init before we do anything
    WaitForSingleObjectEx( g_WaitingForVideoEvent, INFINITE, FALSE );

    assert(g_Window);

    // Now go ahead :)
    QD3D11Device* device = InitDevice();

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

    IDXGIFactory2* dxgiFactory = nullptr;
    HRESULT hr = QD3D::GetDxgiFactory( device, &dxgiFactory );
    if ( FAILED( hr ) )
    {
        ri.Error( ERR_FATAL, "Failed to get DXGI Factory: 0x%08x.\n", hr );
        return;
    }

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

    InitSwapChain( swapChain );

    SAFE_RELEASE( swapChain );
    SAFE_RELEASE( device );
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
}

void D3DWin8_NotifyWindowResize( int logicalSizeX, int logicalSizeY )
{
    assert( g_Window );

    g_WindowWidth = logicalSizeX;
    g_WindowHeight = logicalSizeY;

    // Allow the video driver to proceed now we have the right information
    if ( DeviceStarted() )
    {
        // @pjb: todo: need to resize
        assert(0);
    }
}
