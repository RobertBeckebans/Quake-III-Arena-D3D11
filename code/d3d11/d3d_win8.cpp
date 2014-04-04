#include "d3d_common.h"
#include "d3d_device.h"

HANDLE g_WaitingForVideoEvent = nullptr;
int g_WindowWidth = 0;
int g_WindowHeight = 0;

void D3DWin8_InitDeferral()
{
    g_WaitingForVideoEvent = CreateEventEx( nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, SYNCHRONIZE );
}

void D3DWin8_Init()
{
    // Wait for video init before we do anything
    WaitForSingleObjectEx( g_WaitingForVideoEvent, INFINITE, FALSE );

    // Now go ahead :)
    ID3D11Device2* device = InitDevice();

    // Prepare the swap chain
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
    ZeroMemory( &swapChainDesc, sizeof(swapChainDesc) );
    swapChainDesc.Width = (UINT) g_WindowWidth;
    swapChainDesc.Height = (UINT) g_WindowHeight;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // All Windows Store apps must use this SwapEffect.

    GetSwapChainDescFromConfig( &swapChainDesc );



    SAFE_RELEASE( device );
}

void D3DWin8_Shutdown()
{
    DestroyDevice();
    DestroySwapChain();
}

void D3DWin8_NotifyWindowResize( int logicalSizeX, int logicalSizeY )
{
    g_WindowWidth = logicalSizeX;
    g_WindowHeight = logicalSizeY;
    SetEvent( g_WaitingForVideoEvent );

    // Allow the video driver to proceed now we have the right information
    if ( DeviceStarted() )
    {
        // @pjb: todo: need to resize
        assert(0);
    }
}
