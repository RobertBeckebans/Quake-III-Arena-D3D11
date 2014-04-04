#include "d3d_common.h"
#include "d3d_views.h"

#include "../renderer/tr_local.h"
#include "../qcommon/qcommon.h"
#include "../win32/resource.h"
#include "../win32/win_local.h"

#define	WINDOW_CLASS_NAME	"Quake 3: Arena"

HWND g_hWnd = nullptr;
ID3D11Device* g_pDevice = nullptr;
ID3D11DeviceContext* g_pImmediateContext = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;
D3D11_TEXTURE2D_DESC g_BackBufferDesc;
ID3D11RenderTargetView* g_pBackBufferView = nullptr;
ID3D11DepthStencilView* g_pDepthBufferView = nullptr;

// @pjb: todo: replace these with defaults from CVars
static const DXGI_FORMAT DEPTH_TEXTURE_FORMAT = DXGI_FORMAT_R24G8_TYPELESS;
static const DXGI_FORMAT DEPTH_DEPTH_VIEW_FORMAT = DXGI_FORMAT_D24_UNORM_S8_UINT;
static const DXGI_FORMAT DEPTH_SHADER_VIEW_FORMAT = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

void CreateBuffers();
void DestroyBuffers();

//----------------------------------------------------------------------------
// WndProc: Intercepts window events before passing them on to the game.
//----------------------------------------------------------------------------
static LONG WINAPI Direct3DWndProc (
    HWND    hWnd,
    UINT    uMsg,
    WPARAM  wParam,
    LPARAM  lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
        // If our window was closed, lose our cached handle
		if (hWnd == g_hWnd) {
            g_hWnd = NULL;
        }
		
        // We want to pass this message on to the MainWndProc
        break;
	case WM_SIZE:

        // @pjb: todo: recreate swapchain?
        // @pjb: actually fuck that. disable window sizing.

        break;

    default:
        break;
    }

    return MainWndProc( hWnd, uMsg, wParam, lParam );
}

//----------------------------------------------------------------------------
// Register the window class.
//----------------------------------------------------------------------------
static BOOL RegisterWindowClass()
{
    WNDCLASS wc;

    ZeroMemory(&wc, sizeof(wc));
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC) Direct3DWndProc;
    wc.cbClsExtra = 0;
    wc.hInstance = g_wv.hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = WINDOW_CLASS_NAME;

    return ::RegisterClass( &wc ) != 0;
}

//----------------------------------------------------------------------------
// Creates a window to render our game in.
//----------------------------------------------------------------------------
static HWND CreateGameWindow( int x, int y, int width, int height )
{
    const UINT exStyle = WS_EX_APPWINDOW; // To enable sizing: | WS_EX_WINDOWEDGE;
    const UINT style = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    RECT rect = { x, y, width, height };
    AdjustWindowRectEx(&rect, style, FALSE, exStyle);

    return CreateWindowEx(
        exStyle, 
        WINDOW_CLASS_NAME,
        "Quake 3: Arena",
        style, 
        rect.left, 
        rect.top, 
        rect.right - rect.left, 
        rect.bottom - rect.top, 
        NULL, 
        NULL, 
        g_wv.hInstance, 
        NULL );
}

//----------------------------------------------------------------------------
// Creates a window, initializes the driver and sets up Direct3D state.
//----------------------------------------------------------------------------
D3D_PUBLIC void D3DDrv_Init( void )
{
	ri.Printf( PRINT_ALL, "Initializing D3D11 subsystem\n" );

    if ( !RegisterWindowClass() )
    {
        ri.Error( ERR_FATAL, "Failed to register D3D11 window class.\n" );
        return;
    }

    cvar_t* vid_xpos = ri.Cvar_Get ("vid_xpos", "", 0);
    cvar_t* vid_ypos = ri.Cvar_Get ("vid_ypos", "", 0);
    
    g_hWnd = CreateGameWindow( 
        vid_xpos->integer, 
        vid_ypos->integer, 
        vdConfig.vidWidth, 
        vdConfig.vidHeight );
    if ( !g_hWnd )
    {
        ri.Error( ERR_FATAL, "Failed to create Direct3D 11 window.\n" );
        return;
    }

	D3D_FEATURE_LEVEL featureLevel; 
	HRESULT hr = QD3D::CreateDefaultDevice(
		D3D_DRIVER_TYPE_HARDWARE, 
		&g_pDevice, 
		&g_pImmediateContext, 
		&featureLevel);
    if (FAILED(hr) || !g_pDevice || !g_pImmediateContext)
	{
        ri.Error( ERR_FATAL, "Failed to create Direct3D 11 device.\n" );
        return;
	}

    ri.Printf( PRINT_ALL, "... feature level %d\n", featureLevel );

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    // @pjb: todo: do these based on cvars (or if not set, pick the best one)
    QD3D::GetBestQualitySwapChainDesc(g_hWnd, g_pDevice, &swapChainDesc);

    if (FAILED(QD3D::CreateSwapChain(g_pDevice, &swapChainDesc, &g_pSwapChain)))
    {
        // @pjb: todo: if swapchain desc is too fancy, fall back
        ri.Error( ERR_FATAL, "Failed to create Direct3D 11 swapchain.\n" );
        return;
    }
    
    CreateBuffers();
}

//----------------------------------------------------------------------------
// Cleans up and stops D3D, and closes the window.
//----------------------------------------------------------------------------
D3D_PUBLIC void D3DDrv_Shutdown( void )
{
    DestroyBuffers();

    SAFE_RELEASE(g_pSwapChain);
    SAFE_RELEASE(g_pImmediateContext);
    SAFE_RELEASE(g_pDevice);

    g_hWnd = NULL;
}

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

