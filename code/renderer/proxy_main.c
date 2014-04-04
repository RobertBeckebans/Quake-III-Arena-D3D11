#include "tr_local.h"
#include "tr_layer.h"
#include "proxy_main.h"

// D3D headers
#include "../d3d11/d3d_driver.h"
#include "../d3d11/d3d_wnd.h"

// GL headers
#include "gl_common.h"

// @pjb: this is just here to deliberately fuck the build if driver is used in here
#define driver #driver_disallowed

static graphicsApiLayer_t glDriver;
static graphicsApiLayer_t d3dDriver;
static cvar_t* proxy_driverPriority = NULL;

void PROXY_Shutdown( void )
{
    d3dDriver.Shutdown();
    glDriver.Shutdown();
}

void PROXY_UnbindResources( void )
{
    d3dDriver.UnbindResources();
    glDriver.UnbindResources();
}

size_t PROXY_LastError( void )
{
    size_t glError = glDriver.LastError();
    size_t d3dError = d3dDriver.LastError();

    // @pjb: may want to change this but for now...
    if ( proxy_driverPriority->integer == 0 )
        return d3dError;
    else
        return glError;
}

void PROXY_ReadPixels( int x, int y, int width, int height, imageFormat_t requestedFmt, void* dest )
{
    if (proxy_driverPriority->integer == 0)
        d3dDriver.ReadPixels( x, y, width, height, requestedFmt, dest );
    else
        glDriver.ReadPixels( x, y, width, height, requestedFmt, dest );
}

void PROXY_ReadDepth( int x, int y, int width, int height, float* dest )
{
    if (proxy_driverPriority->integer == 0)
        d3dDriver.ReadDepth( x, y, width, height, dest );
    else
        glDriver.ReadDepth( x, y, width, height, dest );
}

void PROXY_ReadStencil( int x, int y, int width, int height, byte* dest )
{
    if (proxy_driverPriority->integer == 0)
        d3dDriver.ReadStencil( x, y, width, height, dest );
    else
        glDriver.ReadStencil( x, y, width, height, dest );
}

void PROXY_CreateImage( const image_t* image, const byte *pic, qboolean isLightmap )
{
    d3dDriver.CreateImage( image, pic, isLightmap );
    glDriver.CreateImage( image, pic, isLightmap );
}

void PROXY_DeleteImage( const image_t* image )
{
    d3dDriver.DeleteImage( image );
    glDriver.DeleteImage( image );
}

void PROXY_UpdateCinematic( image_t* image, const byte* pic, int cols, int rows, qboolean dirty )
{
    d3dDriver.UpdateCinematic( image, pic, cols, rows, dirty );
    glDriver.UpdateCinematic( image, pic, cols, rows, dirty );
}

void PROXY_DrawImage( const image_t* image, const float* coords, const float* texcoords, const float* color )
{
    d3dDriver.DrawImage( image, coords, texcoords, color );
    glDriver.DrawImage( image, coords, texcoords, color );
}

imageFormat_t PROXY_GetImageFormat( const image_t* image )
{
    imageFormat_t d3dFmt = d3dDriver.GetImageFormat( image );
    imageFormat_t glFmt = glDriver.GetImageFormat( image );

    // @pjb: todo: enable this when relevant
    //assert( d3dFmt == glFmt );
    //return d3dFmt;

    return glFmt;
}

void PROXY_SetGamma( unsigned char red[256], unsigned char green[256], unsigned char blue[256] )
{
    d3dDriver.SetGamma( red, green, blue );
    glDriver.SetGamma( red, green, blue );
}

int PROXY_SumOfUsedImages( void )
{
    return d3dDriver.GetFrameImageMemoryUsage() + glDriver.GetFrameImageMemoryUsage(); 
}

void PROXY_GfxInfo( void )
{
	ri.Printf( PRINT_ALL, "----- OpenGL -----\n" );
    glDriver.GraphicsInfo( );
    // @pjb: todo
	ri.Printf( PRINT_ALL, "----- PROXY -----\n" );
    ri.Printf( PRINT_ALL, "Using proxied driver: all commands issued to OpenGL and D3D11.\n" );
}

void PROXY_Clear( unsigned long bits, const float* clearCol, unsigned long stencil, float depth )
{
    d3dDriver.Clear( bits, clearCol, stencil, depth );
    glDriver.Clear( bits, clearCol, stencil, depth );
}

void PROXY_SetProjection( const float* projMatrix )
{
    d3dDriver.SetProjectionMatrix( projMatrix );
    glDriver.SetProjectionMatrix( projMatrix );
}

void PROXY_GetProjection( float* projMatrix )
{
    if (proxy_driverPriority->integer == 0)
        d3dDriver.GetProjectionMatrix( projMatrix );
    else
        glDriver.GetProjectionMatrix( projMatrix );
}

void PROXY_SetModelView( const float* modelViewMatrix )
{
    d3dDriver.SetModelViewMatrix( modelViewMatrix );
    glDriver.SetModelViewMatrix( modelViewMatrix );
}

void PROXY_GetModelView( float* modelViewMatrix )
{
    if (proxy_driverPriority->integer == 0)
        d3dDriver.GetModelViewMatrix( modelViewMatrix );
    else
        glDriver.GetModelViewMatrix( modelViewMatrix );
}

void PROXY_SetViewport( int left, int top, int width, int height )
{
    d3dDriver.SetViewport( left, top, width, height );
    glDriver.SetViewport( left, top, width, height );
}

void PROXY_Flush( void )
{
    d3dDriver.Flush();
    glDriver.Flush();
}

void PROXY_SetState( unsigned long stateMask )
{
    d3dDriver.SetState( stateMask );
    glDriver.SetState( stateMask );
}

void PROXY_ResetState2D( void )
{
    d3dDriver.ResetState2D();
    glDriver.ResetState2D();
}

void PROXY_ResetState3D( void )
{
    d3dDriver.ResetState3D();
    glDriver.ResetState3D();
}

void PROXY_SetPortalRendering( qboolean enabled, const float* flipMatrix, const float* plane )
{
    d3dDriver.SetPortalRendering( enabled, flipMatrix, plane );
    glDriver.SetPortalRendering( enabled, flipMatrix, plane );
}

void PROXY_SetDepthRange( float minRange, float maxRange )
{
    d3dDriver.SetDepthRange( minRange, maxRange );
    glDriver.SetDepthRange( minRange, maxRange );
}

void PROXY_SetDrawBuffer( int buffer )
{
    d3dDriver.SetDrawBuffer( buffer );
    glDriver.SetDrawBuffer( buffer );
}

void PROXY_EndFrame( void )
{
    d3dDriver.EndFrame();
    glDriver.EndFrame();
}

void PROXY_MakeCurrent( qboolean current )
{
    d3dDriver.MakeCurrent( current );
    glDriver.MakeCurrent( current );
}

void PROXY_ShadowSilhouette( const float* edges, int edgeCount )
{
    d3dDriver.ShadowSilhouette( edges, edgeCount );
    glDriver.ShadowSilhouette( edges, edgeCount );
}

void PROXY_ShadowFinish( void )
{
    d3dDriver.ShadowFinish();
    glDriver.ShadowFinish();
}

void PROXY_DrawSkyBox( const skyboxDrawInfo_t* skybox, const float* eye_origin, const float* colorTint )
{
    d3dDriver.DrawSkyBox( skybox, eye_origin, colorTint );
    glDriver.DrawSkyBox( skybox, eye_origin, colorTint );
}

void PROXY_DrawBeam( const image_t* image, const float* color, const vec3_t startPoints[], const vec3_t endPoints[], int segs )
{
    d3dDriver.DrawBeam( image, color, startPoints, endPoints, segs );
    glDriver.DrawBeam( image, color, startPoints, endPoints, segs );
}

void PROXY_DrawStageGeneric( const shaderCommands_t *input )
{
    d3dDriver.DrawStageGeneric( input );
    glDriver.DrawStageGeneric( input );
}

void PROXY_DrawStageVertexLitTexture( const shaderCommands_t *input )
{
    d3dDriver.DrawStageVertexLitTexture( input );
    glDriver.DrawStageVertexLitTexture( input );
}

void PROXY_DrawStageLightmappedMultitexture( const shaderCommands_t *input )
{
    d3dDriver.DrawStageLightmappedMultitexture( input );
    glDriver.DrawStageLightmappedMultitexture( input );
}

void PROXY_DebugDrawAxis( void )
{
    d3dDriver.DebugDrawAxis();
    glDriver.DebugDrawAxis();
}

void PROXY_DebugDrawTris( shaderCommands_t *input )
{
    d3dDriver.DebugDrawTris( input );
    glDriver.DebugDrawTris( input );
}

void PROXY_DebugDrawNormals( shaderCommands_t *input )
{
    d3dDriver.DebugDrawNormals( input );
    glDriver.DebugDrawNormals( input );
}

void PROXY_DebugSetOverdrawMeasureEnabled( qboolean enabled )
{
    d3dDriver.DebugSetOverdrawMeasureEnabled( enabled );
    glDriver.DebugSetOverdrawMeasureEnabled( enabled );
}

void PROXY_DebugSetTextureMode( const char* mode )
{
    d3dDriver.DebugSetTextureMode( mode );
    glDriver.DebugSetTextureMode( mode );
}

void PROXY_DebugDrawPolygon( int color, int numPoints, const float* points )
{
    d3dDriver.DebugDrawPolygon( color, numPoints, points );
    glDriver.DebugDrawPolygon( color, numPoints, points );
}



static void PositionOpenGLWindowRightOfD3D( void )
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

static void ReconcileVideoConfigs( const vdconfig_t* glConfig, const vdconfig_t* d3dConfig, vdconfig_t* outConfig )
{
    // Copy the resource strings to the vgConfig
    Q_strncpyz( outConfig->renderer_string, "PROXY DRIVER", sizeof( outConfig->renderer_string ) );
    Q_strncpyz( outConfig->vendor_string, "@pjb", sizeof( outConfig->vendor_string ) );
    Q_strncpyz( outConfig->version_string, "1.0", sizeof( outConfig->version_string ) );

    if ( glConfig->deviceSupportsGamma != d3dConfig->deviceSupportsGamma ) {
        ri.Error( ERR_FATAL, "Conflicting gamma support: D3D %s, GL %s\n", 
            d3dConfig->deviceSupportsGamma ? "yes" : "no",
            glConfig->deviceSupportsGamma ? "yes" : "no" );
    }
    
    if ( glConfig->stereoEnabled != d3dConfig->stereoEnabled ) {
        ri.Error( ERR_FATAL, "Conflicting stereo support: D3D %s, GL %s\n", 
            d3dConfig->stereoEnabled ? "yes" : "no",
            glConfig->stereoEnabled ? "yes" : "no" );
    }
    
    if ( glConfig->isFullscreen != d3dConfig->isFullscreen ) {
        ri.Error( ERR_FATAL, "Conflicting fullscreen support: D3D %s, GL %s\n", 
            d3dConfig->isFullscreen ? "yes" : "no",
            glConfig->isFullscreen ? "yes" : "no" );
    }
    
    if ( glConfig->textureEnvAddAvailable != d3dConfig->textureEnvAddAvailable ) {
        ri.Error( ERR_FATAL, "Conflicting texenv add support: D3D %s, GL %s\n", 
            d3dConfig->textureEnvAddAvailable ? "yes" : "no",
            glConfig->textureEnvAddAvailable ? "yes" : "no" );
    }
    
    if ( glConfig->displayFrequency != d3dConfig->displayFrequency ) {
        ri.Error( ERR_FATAL, "Conflicting displayFrequency: D3D %d, GL %d\n", 
            d3dConfig->displayFrequency,
            glConfig->displayFrequency );
    }
    
    if ( glConfig->hardwareType != GLHW_GENERIC || d3dConfig->hardwareType != GLHW_GENERIC ) {
        ri.Error( ERR_FATAL, "Proxy driver requires GENERIC hardware type: D3D %d, GL %d\n", 
            d3dConfig->hardwareType,
            glConfig->hardwareType );
    }

    if ( glConfig->vidWidth != d3dConfig->vidWidth ||
         glConfig->vidHeight != d3dConfig->vidHeight ) {
        ri.Error( ERR_FATAL, "Conflicting video resolution: D3D %dx%d, GL %dx%d\n", 
            d3dConfig->vidWidth, d3dConfig->vidHeight,
            glConfig->vidWidth, glConfig->vidHeight );
    }
    
    outConfig->maxTextureSize = min( d3dConfig->maxTextureSize, glConfig->maxTextureSize );
    outConfig->colorBits = min( d3dConfig->colorBits, glConfig->colorBits );
    outConfig->depthBits = min( d3dConfig->depthBits, glConfig->depthBits );
    outConfig->stencilBits = min( d3dConfig->stencilBits, glConfig->stencilBits );
}

void PROXY_DriverInit( graphicsApiLayer_t* layer )
{
    vdconfig_t old_vdConfig, gl_vdConfig, d3d_vdConfig;

    layer->Shutdown = PROXY_Shutdown;
    layer->UnbindResources = PROXY_UnbindResources;
    layer->LastError = PROXY_LastError;
    layer->ReadPixels = PROXY_ReadPixels;
    layer->ReadDepth = PROXY_ReadDepth;
    layer->ReadStencil = PROXY_ReadStencil;
    layer->CreateImage = PROXY_CreateImage;
    layer->DeleteImage = PROXY_DeleteImage;
    layer->UpdateCinematic = PROXY_UpdateCinematic;
    layer->DrawImage = PROXY_DrawImage;
    layer->GetImageFormat = PROXY_GetImageFormat;
    layer->SetGamma = PROXY_SetGamma;
    layer->GetFrameImageMemoryUsage = PROXY_SumOfUsedImages;
    layer->GraphicsInfo = PROXY_GfxInfo;
    layer->Clear = PROXY_Clear;
    layer->SetProjectionMatrix = PROXY_SetProjection;
    layer->GetProjectionMatrix = PROXY_GetProjection;
    layer->SetModelViewMatrix = PROXY_SetModelView;
    layer->GetModelViewMatrix = PROXY_GetModelView;
    layer->SetViewport = PROXY_SetViewport;
    layer->Flush = PROXY_Flush;
    layer->SetState = PROXY_SetState;
    layer->ResetState2D = PROXY_ResetState2D;
    layer->ResetState3D = PROXY_ResetState3D;
    layer->SetPortalRendering = PROXY_SetPortalRendering;
    layer->SetDepthRange = PROXY_SetDepthRange;
    layer->SetDrawBuffer = PROXY_SetDrawBuffer;
    layer->EndFrame = PROXY_EndFrame;
    layer->MakeCurrent = PROXY_MakeCurrent;
    layer->ShadowSilhouette = PROXY_ShadowSilhouette;
    layer->ShadowFinish = PROXY_ShadowFinish;
    layer->DrawSkyBox = PROXY_DrawSkyBox;
    layer->DrawBeam = PROXY_DrawBeam;
    layer->DrawStageGeneric = PROXY_DrawStageGeneric;
    layer->DrawStageVertexLitTexture = PROXY_DrawStageVertexLitTexture;
    layer->DrawStageLightmappedMultitexture = PROXY_DrawStageLightmappedMultitexture;
    layer->DebugDrawAxis = PROXY_DebugDrawAxis;
    layer->DebugDrawTris = PROXY_DebugDrawTris;
    layer->DebugDrawNormals = PROXY_DebugDrawNormals;
    layer->DebugSetOverdrawMeasureEnabled = PROXY_DebugSetOverdrawMeasureEnabled;
    layer->DebugSetTextureMode = PROXY_DebugSetTextureMode;
    layer->DebugDrawPolygon = PROXY_DebugDrawPolygon;

    // Get the configurable driver priority
    proxy_driverPriority = Cvar_Get( "proxy_driverPriority", "0", CVAR_ARCHIVE );

    // If using the proxy driver we cannot use fullscreen
    Cvar_Set( "r_fullscreen", "0" );

    // Backup the vdConfig because the drivers will make changes to this (unfortunately)
    old_vdConfig = vdConfig;

    // Init the D3D driver and back up the vdConfig
    D3DDrv_DriverInit( &d3dDriver );
    d3d_vdConfig = vdConfig;

    // Init the GL driver and back up the vdConfig
    GLRB_DriverInit( &glDriver );
    gl_vdConfig = vdConfig;

    R_ValidateGraphicsLayer( &d3dDriver );
    R_ValidateGraphicsLayer( &glDriver );

    // Let's arrange these windows shall we?
    PositionOpenGLWindowRightOfD3D();

    // Restore the old config and reconcile the new configs
    vdConfig = old_vdConfig;
    ReconcileVideoConfigs( &d3d_vdConfig, &gl_vdConfig, &vdConfig );
}

