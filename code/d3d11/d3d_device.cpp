#include "d3d_common.h"
#include "d3d_device.h"
#include "d3d_state.h"

QD3D11Device* g_pDevice = nullptr;
ID3D11DeviceContext* g_pImmediateContext = nullptr;
IDXGISwapChain1* g_pSwapChain = nullptr;

bool DeviceStarted()
{
    return g_pDevice != nullptr;
}

QD3D11Device* InitDevice()
{
    Com_Memset( &g_BufferState, 0, sizeof( g_BufferState ) );

    g_BufferState.featureLevel = D3D_FEATURE_LEVEL_11_0; 
	HRESULT hr = QD3D::CreateDefaultDevice(
		D3D_DRIVER_TYPE_HARDWARE, 
		&g_pDevice, 
		&g_pImmediateContext, 
		&g_BufferState.featureLevel);
    if (FAILED(hr) || !g_pDevice || !g_pImmediateContext)
	{
        ri.Error( ERR_FATAL, "Failed to create Direct3D 11 device: 0x%08x.\n", hr );
        return nullptr;
	}

    ri.Printf( PRINT_ALL, "... feature level %d\n", g_BufferState.featureLevel );

    g_pDevice->AddRef();
    return g_pDevice;
}

void InitSwapChain( IDXGISwapChain1* swapChain )
{
    swapChain->AddRef();
    g_pSwapChain = swapChain;
    g_pSwapChain->GetDesc1( &g_BufferState.swapChainDesc );
}

void DestroyDevice()
{
    SAFE_RELEASE(g_pImmediateContext);
    SAFE_RELEASE(g_pDevice);
}

void DestroySwapChain()
{
    SAFE_RELEASE(g_pSwapChain);
    Com_Memset( &g_BufferState, 0, sizeof( g_BufferState ) );
}

void GetSwapChainDescFromConfig( DXGI_SWAP_CHAIN_DESC1* scDesc )
{
    // @pjb: TODO: get based off config
    QD3D::GetBestQualitySwapChainDesc( g_pDevice, scDesc );
}



