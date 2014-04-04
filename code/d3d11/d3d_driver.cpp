// D3D headers
#include "d3d_common.h"
#include "d3d_driver.h"
#include "d3d_wnd.h"
#include "d3d_views.h"

// @pjb: this is just here to deliberately fuck the build if driver is used in here
#define driver #driver_disallowed

//----------------------------------------------------------------------------
// Imports from d3d_wnd.cpp
//----------------------------------------------------------------------------
extern ID3D11Device* g_pDevice;
extern ID3D11DeviceContext* g_pImmediateContext;
extern IDXGISwapChain* g_pSwapChain;

//----------------------------------------------------------------------------
// Local data
//----------------------------------------------------------------------------
D3D11_TEXTURE2D_DESC g_BackBufferDesc;
ID3D11RenderTargetView* g_pBackBufferView = nullptr;
ID3D11DepthStencilView* g_pDepthBufferView = nullptr;

//----------------------------------------------------------------------------
// Constants
//----------------------------------------------------------------------------
// @pjb: todo: replace these with defaults from CVars
static const DXGI_FORMAT DEPTH_TEXTURE_FORMAT = DXGI_FORMAT_R24G8_TYPELESS;
static const DXGI_FORMAT DEPTH_DEPTH_VIEW_FORMAT = DXGI_FORMAT_D24_UNORM_S8_UINT;
static const DXGI_FORMAT DEPTH_SHADER_VIEW_FORMAT = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

//----------------------------------------------------------------------------
// Forward declarations
//----------------------------------------------------------------------------
void CreateBuffers();
void DestroyBuffers();

//----------------------------------------------------------------------------
//
// DRIVER INTERFACE
//
//----------------------------------------------------------------------------
void D3DDrv_Shutdown( void )
{
    DestroyBuffers();
    D3DWnd_Shutdown();
}

void D3DDrv_UnbindResources( void )
{
   
}

size_t D3DDrv_LastError( void )
{
    return 0;
}

void D3DDrv_ReadPixels( int x, int y, int width, int height, imageFormat_t requestedFmt, void* dest )
{
    
}

void D3DDrv_CreateImage( const image_t* image, const byte *pic, qboolean isLightmap )
{
    
}

void D3DDrv_DeleteImage( const image_t* image )
{
    
}

imageFormat_t D3DDrv_GetImageFormat( const image_t* image )
{
    return IMAGEFORMAT_UNKNOWN;
}

void D3DDrv_SetGamma( unsigned char red[256], unsigned char green[256], unsigned char blue[256] )
{
    
}

int D3DDrv_SumOfUsedImages( void )
{
    return 0;
}

void D3DDrv_GfxInfo( void )
{
	ri.Printf( PRINT_ALL, "----- Direct3D 11 -----\n" );
    
}

void D3DDrv_Clear( unsigned long bits, const float* clearCol, unsigned long stencil, float depth )
{
    if ( bits & CLEAR_COLOR )
    {
        static float defaultCol[] = { 0, 0, 0, 0 };
        if ( !clearCol ) { clearCol = defaultCol; }

        g_pImmediateContext->ClearRenderTargetView( g_pBackBufferView, clearCol );
    }

    if ( bits & ( CLEAR_DEPTH | CLEAR_STENCIL ) )
    {
        DWORD clearBits = 0;
        if ( bits & CLEAR_DEPTH ) { clearBits |= D3D11_CLEAR_DEPTH; }
        if ( bits & CLEAR_STENCIL ) { clearBits |= D3D11_CLEAR_STENCIL; }
        g_pImmediateContext->ClearDepthStencilView( g_pDepthBufferView, clearBits, depth, (UINT8) stencil );
    }
}

void D3DDrv_SetProjection( const float* projMatrix )
{
    
}

void D3DDrv_SetViewport( int left, int top, int width, int height )
{
    
}

void D3DDrv_Flush( void )
{
    
}

void D3DDrv_SetState( unsigned long stateMask )
{
    
}

void D3DDrv_ResetState2D( void )
{

}

void D3DDrv_ResetState3D( void )
{

}

D3D_PUBLIC void D3DDrv_DriverInit( graphicsApiLayer_t* layer )
{
    layer->Shutdown = D3DDrv_Shutdown;
    layer->UnbindResources = D3DDrv_UnbindResources;
    layer->LastError = D3DDrv_LastError;
    layer->ReadPixels = D3DDrv_ReadPixels;
    layer->CreateImage = D3DDrv_CreateImage;
    layer->DeleteImage = D3DDrv_DeleteImage;
    layer->GetImageFormat = D3DDrv_GetImageFormat;
    layer->SetGamma = D3DDrv_SetGamma;
    layer->GetFrameImageMemoryUsage = D3DDrv_SumOfUsedImages;
    layer->GraphicsInfo = D3DDrv_GfxInfo;
    layer->Clear = D3DDrv_Clear;
    layer->SetProjection = D3DDrv_SetProjection;
    layer->SetViewport = D3DDrv_SetViewport;
    layer->Flush = D3DDrv_Flush;
    layer->SetState = D3DDrv_SetState;
    layer->ResetState2D = D3DDrv_ResetState2D;
    layer->ResetState3D = D3DDrv_ResetState3D;

    // This, weirdly, can be called multiple times. Catch that if that's the case.
    if ( g_pDevice == nullptr )
    {
        D3DWnd_Init();
    }
    
    DestroyBuffers();
    CreateBuffers();
}

//----------------------------------------------------------------------------
//
// LOCAL FUNCTIONS
//
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// Get the back buffer and depth/stencil buffer.
//----------------------------------------------------------------------------
void CreateBuffers()
{
    g_pBackBufferView = QD3D::CreateBackBufferView(g_pSwapChain, g_pDevice, &g_BackBufferDesc);
    ASSERT(g_pBackBufferView);

    g_pDepthBufferView = QD3D::CreateDepthBufferView(
        g_pDevice,
        g_BackBufferDesc.Width,
        g_BackBufferDesc.Height,
        DEPTH_TEXTURE_FORMAT,
        DEPTH_DEPTH_VIEW_FORMAT,
        g_BackBufferDesc.SampleDesc.Count,
        g_BackBufferDesc.SampleDesc.Quality,
        D3D11_BIND_SHADER_RESOURCE);
    ASSERT(g_pDepthBufferView);
}

//----------------------------------------------------------------------------
// Release references to the back buffer and depth
//----------------------------------------------------------------------------
void DestroyBuffers()
{
    SAFE_RELEASE(g_pBackBufferView);
    SAFE_RELEASE(g_pDepthBufferView);
}

