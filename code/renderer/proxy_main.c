#include "tr_local.h"
#include "tr_layer.h"
#include "proxy_main.h"

// D3D headers
#include "../d3d11/d3d_driver.h"
#include "../d3d11/d3d_wnd.h"

// GL headers
#include "gl_common.h"
#include "gl_image.h"

// @pjb: this is just here to deliberately fuck the build if driver is used in here
#define driver #driver_disallowed

static graphicsApiLayer_t openglDriver;
static graphicsApiLayer_t d3dDriver;
static cvar_t* proxy_driverPriority = NULL;

void PROXY_Shutdown( void )
{
    d3dDriver.Shutdown();
    openglDriver.Shutdown();
}

void PROXY_UnbindResources( void )
{
    d3dDriver.UnbindResources();
    openglDriver.UnbindResources();
}

size_t PROXY_LastError( void )
{
    size_t glError = openglDriver.LastError();
    size_t d3dError = d3dDriver.LastError();

    // @pjb: may want to change this but for now...
    if ( proxy_driverPriority->integer == 0 )
        return d3dError;
    else
        return glError;
}

void PROXY_ReadPixels( int x, int y, int width, int height, imageFormat_t requestedFmt, void* dest )
{
    //@pjb: todo: decide which one of these takes preference
    if (proxy_driverPriority->integer == 0)
        d3dDriver.ReadPixels( x, y, width, height, requestedFmt, dest );
    else
        openglDriver.ReadPixels( x, y, width, height, requestedFmt, dest );
}

void PROXY_CreateImage( const image_t* image, const byte *pic, qboolean isLightmap )
{
    d3dDriver.CreateImage( image, pic, isLightmap );
    openglDriver.CreateImage( image, pic, isLightmap );
}

void PROXY_DeleteImage( const image_t* image )
{
    d3dDriver.DeleteImage( image );
    openglDriver.DeleteImage( image );
}

imageFormat_t PROXY_GetImageFormat( const image_t* image )
{
    imageFormat_t d3dFmt = d3dDriver.GetImageFormat( image );
    imageFormat_t glFmt = openglDriver.GetImageFormat( image );

    // @pjb: todo: enable this when relevant
    //assert( d3dFmt == glFmt );
    //return d3dFmt;

    return glFmt;
}

void PROXY_SetGamma( unsigned char red[256], unsigned char green[256], unsigned char blue[256] )
{
    d3dDriver.SetGamma( red, green, blue );
    openglDriver.SetGamma( red, green, blue );
}

int PROXY_SumOfUsedImages( void )
{
    return d3dDriver.GetFrameImageMemoryUsage() + openglDriver.GetFrameImageMemoryUsage(); 
}

void PROXY_GfxInfo( void )
{
	ri.Printf( PRINT_ALL, "----- OpenGL -----\n" );
    openglDriver.GraphicsInfo( );
    // @pjb: todo
	ri.Printf( PRINT_ALL, "----- PROXY -----\n" );
    ri.Printf( PRINT_ALL, "Using proxied driver: all commands issued to OpenGL and D3D11.\n" );
}

void PROXY_Clear( unsigned long bits, const float* clearCol, unsigned long stencil, float depth )
{
    d3dDriver.Clear( bits, clearCol, stencil, depth );
    openglDriver.Clear( bits, clearCol, stencil, depth );
}

void PROXY_SetProjection( const float* projMatrix )
{
    d3dDriver.SetProjection( projMatrix );
    openglDriver.SetProjection( projMatrix );
}

void PROXY_SetViewport( int left, int top, int width, int height )
{
    d3dDriver.SetViewport( left, top, width, height );
    openglDriver.SetViewport( left, top, width, height );
}

void PROXY_Flush( void )
{
    d3dDriver.Flush();
    openglDriver.Flush();
}

void PROXY_SetState( unsigned long stateMask )
{
    d3dDriver.SetState( stateMask );
    openglDriver.SetState( stateMask );
}

void PROXY_PositionOpenGLWindowRightOfD3D( void )
{
    HWND hD3DWnd, hGLWnd;
    RECT d3dR, glR;

    hD3DWnd = D3DWnd_GetWindowHandle();
    hGLWnd = GLimp_GetWindowHandle();

    GetWindowRect( hD3DWnd, &d3dR );
    GetWindowRect( hGLWnd, &glR );

    if ( !MoveWindow( hGLWnd, d3dR.right, d3dR.top, glR.right - glR.left, glR.bottom - glR.top, TRUE ) )
    {
        ri.Printf( PRINT_WARNING, "Failed to move GL window: %d, %d, %d, %d\n",
            d3dR.left, d3dR.top, glR.right - glR.left, glR.bottom - glR.top );
    }
}

void PROXY_DriverInit( graphicsApiLayer_t* layer )
{
    layer->Shutdown = PROXY_Shutdown;
    layer->UnbindResources = PROXY_UnbindResources;
    layer->LastError = PROXY_LastError;
    layer->ReadPixels = PROXY_ReadPixels;
    layer->CreateImage = PROXY_CreateImage;
    layer->DeleteImage = PROXY_DeleteImage;
    layer->GetImageFormat = PROXY_GetImageFormat;
    layer->SetGamma = PROXY_SetGamma;
    layer->GetFrameImageMemoryUsage = PROXY_SumOfUsedImages;
    layer->GraphicsInfo = PROXY_GfxInfo;
    layer->Clear = PROXY_Clear;
    layer->SetProjection = PROXY_SetProjection;
    layer->SetViewport = PROXY_SetViewport;
    layer->Flush = PROXY_Flush;
    layer->SetState = PROXY_SetState;

    // Get the configurable driver priority
    proxy_driverPriority = Cvar_Get( "proxy_driverPriority", "0", CVAR_ARCHIVE );

    // If using the proxy driver we cannot use fullscreen
    Cvar_Set( "r_fullscreen", "0" );

    D3DDrv_DriverInit( &d3dDriver );

    // @pjb: todo: remove this. Used to determine what state is changed by GL.
    vdconfig_t vdConfigCuriosityHack = vdConfig;
    GLRB_DriverInit( &openglDriver );

    (void)( vdConfigCuriosityHack );

    R_ValidateGraphicsLayer( &d3dDriver );
    R_ValidateGraphicsLayer( &openglDriver );

    // Let's arrange these windows shall we?
    PROXY_PositionOpenGLWindowRightOfD3D();
            
    // Copy the resource strings to the vgConfig
    Q_strncpyz( vdConfig.renderer_string, "PROXY DRIVER", sizeof( vdConfig.renderer_string ) );
    Q_strncpyz( vdConfig.vendor_string, "@pjb", sizeof( vdConfig.vendor_string ) );
    Q_strncpyz( vdConfig.version_string, "1.0", sizeof( vdConfig.version_string ) );
    
    // todo: arbitrate conflicting vdConfig settings

}

