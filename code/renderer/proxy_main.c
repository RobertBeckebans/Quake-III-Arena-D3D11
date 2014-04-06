#include "tr_local.h"
#include "tr_layer.h"
#include "proxy_main.h"

// D3D headers
#include "../d3d11/d3d_driver.h"
#include "../d3d11/d3d_wnd.h"

// GL headers
#include "gl_common.h"

typedef struct graphicsApiLayer_s {
    void            (* Shutdown)( void );
    void            (* UnbindResources)( void );
    size_t          (* LastError)( void );
    void            (* ReadPixels)( int x, int y, int width, int height, imageFormat_t requestedFmt, void* dest );
    void            (* ReadDepth)( int x, int y, int width, int height, float* dest );
    void            (* ReadStencil)( int x, int y, int width, int height, byte* dest );
    void            (* CreateImage)( const image_t* image, const byte *pic, qboolean isLightmap );
    void            (* DeleteImage)( const image_t* image );
    void            (* UpdateCinematic)( const image_t* image, const byte* pic, int cols, int rows, qboolean dirty );
    void            (* DrawImage)( const image_t* image, const float* coords, const float* texcoords, const float* color );
    imageFormat_t   (* GetImageFormat)( const image_t* image );
    void            (* SetGamma)( unsigned char red[256], unsigned char green[256], unsigned char blue[256] );
    int             (* GetFrameImageMemoryUsage)( void );
    void            (* GraphicsInfo)( void );
    void            (* Clear)( unsigned long bits, const float* clearCol, unsigned long stencil, float depth );
    void            (* SetProjectionMatrix)( const float* projMatrix );
    void            (* GetProjectionMatrix)( float* projMatrix );
    void            (* SetModelViewMatrix)( const float* modelViewMatrix );
    void            (* GetModelViewMatrix)( float* modelViewMatrix );
    void            (* SetViewport)( int left, int top, int width, int height );
    void            (* Flush)( void );
    void            (* SetState)( unsigned long stateMask ); // Use GLS_* flags in tr_state.h
    void            (* ResetState2D)( void );
    void            (* ResetState3D)( void );
    void            (* SetPortalRendering)( qboolean enabled, const float* flipMatrix, const float* plane );
    void            (* SetDepthRange)( float minRange, float maxRange );
    void            (* SetDrawBuffer)( int buffer );
    void            (* EndFrame)( void );
    void            (* MakeCurrent)( qboolean current );
    void            (* ShadowSilhouette)( const float* edges, int edgeCount );
    void            (* ShadowFinish)( void );
    void            (* DrawSkyBox)( const skyboxDrawInfo_t* skybox, const float* eye_origin, const float* colorTint );
    void            (* DrawBeam)( const image_t* image, const float* color, const vec3_t startPoints[], const vec3_t endPoints[], int segs );
    void            (* DrawStageGeneric)( const shaderCommands_t *input );
    void            (* DrawStageVertexLitTexture)( const shaderCommands_t *input );
    void            (* DrawStageLightmappedMultitexture)( const shaderCommands_t *input );
    void            (* DebugDrawAxis)( void );
    void            (* DebugDrawNormals)( const shaderCommands_t *input );
    void            (* DebugDrawTris)( const shaderCommands_t *input );
    void            (* DebugSetOverdrawMeasureEnabled)( qboolean enabled );
    void            (* DebugSetTextureMode)( const char* mode );
    void            (* DebugDrawPolygon)( int color, int numPoints, const float* points );
} graphicsApiLayer_t;

static graphicsApiLayer_t glDriver;
static graphicsApiLayer_t d3dDriver;
static cvar_t* proxy_driverPriority = NULL;

void PROXY_Shutdown( void )
{
    d3dDriver.Shutdown();
    glDriver.Shutdown();
}

// Why? If the D3D driver fails, we can't call regular Shutdown because glDriver will still be null
void PROXY_ShutdownOne( void )
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

void PROXY_UpdateCinematic( const image_t* image, const byte* pic, int cols, int rows, qboolean dirty )
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

void PROXY_DebugDrawTris( const shaderCommands_t *input )
{
    d3dDriver.DebugDrawTris( input );
    glDriver.DebugDrawTris( input );
}

void PROXY_DebugDrawNormals( const shaderCommands_t *input )
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

static void ReconcileVideoConfigs( const vdconfig_t* d3dConfig, const vdconfig_t* glConfig, vdconfig_t* outConfig )
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

void ShadowGfxApiBindings( graphicsApiLayer_t* layer )
{
    layer->Shutdown = GFX_Shutdown;
    layer->UnbindResources = GFX_UnbindResources;
    layer->LastError = GFX_LastError;
    layer->ReadPixels = GFX_ReadPixels;
    layer->ReadDepth = GFX_ReadDepth;
    layer->ReadStencil = GFX_ReadStencil;
    layer->CreateImage = GFX_CreateImage;
    layer->DeleteImage = GFX_DeleteImage;
    layer->UpdateCinematic = GFX_UpdateCinematic;
    layer->DrawImage = GFX_DrawImage;
    layer->GetImageFormat = GFX_GetImageFormat;
    layer->SetGamma = GFX_SetGamma;
    layer->GetFrameImageMemoryUsage = GFX_GetFrameImageMemoryUsage;
    layer->GraphicsInfo = GFX_GraphicsInfo;
    layer->Clear = GFX_Clear;
    layer->SetProjectionMatrix = GFX_SetProjectionMatrix;
    layer->GetProjectionMatrix = GFX_GetProjectionMatrix;
    layer->SetModelViewMatrix = GFX_SetModelViewMatrix;
    layer->GetModelViewMatrix = GFX_GetModelViewMatrix;
    layer->SetViewport = GFX_SetViewport;
    layer->Flush = GFX_Flush;
    layer->SetState = GFX_SetState;
    layer->ResetState2D = GFX_ResetState2D;
    layer->ResetState3D = GFX_ResetState3D;
    layer->SetPortalRendering = GFX_SetPortalRendering;
    layer->SetDepthRange = GFX_SetDepthRange;
    layer->SetDrawBuffer = GFX_SetDrawBuffer;
    layer->EndFrame = GFX_EndFrame;
    layer->MakeCurrent = GFX_MakeCurrent;
    layer->ShadowSilhouette = GFX_ShadowSilhouette;
    layer->ShadowFinish = GFX_ShadowFinish;
    layer->DrawSkyBox = GFX_DrawSkyBox;
    layer->DrawBeam = GFX_DrawBeam;
    layer->DrawStageGeneric = GFX_DrawStageGeneric;
    layer->DrawStageVertexLitTexture = GFX_DrawStageVertexLitTexture;
    layer->DrawStageLightmappedMultitexture = GFX_DrawStageLightmappedMultitexture;
    layer->DebugDrawAxis = GFX_DebugDrawAxis;
    layer->DebugDrawTris = GFX_DebugDrawTris;
    layer->DebugDrawNormals = GFX_DebugDrawNormals;
    layer->DebugSetOverdrawMeasureEnabled = GFX_DebugSetOverdrawMeasureEnabled;
    layer->DebugSetTextureMode = GFX_DebugSetTextureMode;
    layer->DebugDrawPolygon = GFX_DebugDrawPolygon;
}

void SetProxyBindings( void )
{
    GFX_Shutdown = PROXY_ShutdownOne;
    GFX_UnbindResources = PROXY_UnbindResources;
    GFX_LastError = PROXY_LastError;
    GFX_ReadPixels = PROXY_ReadPixels;
    GFX_ReadDepth = PROXY_ReadDepth;
    GFX_ReadStencil = PROXY_ReadStencil;
    GFX_CreateImage = PROXY_CreateImage;
    GFX_DeleteImage = PROXY_DeleteImage;
    GFX_UpdateCinematic = PROXY_UpdateCinematic;
    GFX_DrawImage = PROXY_DrawImage;
    GFX_GetImageFormat = PROXY_GetImageFormat;
    GFX_SetGamma = PROXY_SetGamma;
    GFX_GetFrameImageMemoryUsage = PROXY_SumOfUsedImages;
    GFX_GraphicsInfo = PROXY_GfxInfo;
    GFX_Clear = PROXY_Clear;
    GFX_SetProjectionMatrix = PROXY_SetProjection;
    GFX_GetProjectionMatrix = PROXY_GetProjection;
    GFX_SetModelViewMatrix = PROXY_SetModelView;
    GFX_GetModelViewMatrix = PROXY_GetModelView;
    GFX_SetViewport = PROXY_SetViewport;
    GFX_Flush = PROXY_Flush;
    GFX_SetState = PROXY_SetState;
    GFX_ResetState2D = PROXY_ResetState2D;
    GFX_ResetState3D = PROXY_ResetState3D;
    GFX_SetPortalRendering = PROXY_SetPortalRendering;
    GFX_SetDepthRange = PROXY_SetDepthRange;
    GFX_SetDrawBuffer = PROXY_SetDrawBuffer;
    GFX_EndFrame = PROXY_EndFrame;
    GFX_MakeCurrent = PROXY_MakeCurrent;
    GFX_ShadowSilhouette = PROXY_ShadowSilhouette;
    GFX_ShadowFinish = PROXY_ShadowFinish;
    GFX_DrawSkyBox = PROXY_DrawSkyBox;
    GFX_DrawBeam = PROXY_DrawBeam;
    GFX_DrawStageGeneric = PROXY_DrawStageGeneric;
    GFX_DrawStageVertexLitTexture = PROXY_DrawStageVertexLitTexture;
    GFX_DrawStageLightmappedMultitexture = PROXY_DrawStageLightmappedMultitexture;
    GFX_DebugDrawAxis = PROXY_DebugDrawAxis;
    GFX_DebugDrawTris = PROXY_DebugDrawTris;
    GFX_DebugDrawNormals = PROXY_DebugDrawNormals;
    GFX_DebugSetOverdrawMeasureEnabled = PROXY_DebugSetOverdrawMeasureEnabled;
    GFX_DebugSetTextureMode = PROXY_DebugSetTextureMode;
    GFX_DebugDrawPolygon = PROXY_DebugDrawPolygon;
}

/*
    @pjb: Validates the driver is completely full of pointers
*/
void ValidateDriverLayerMemory( size_t* memory, size_t size )
{
    size_t i;
    for ( i = 0; i < size / sizeof(size_t); ++i ) {
        if ( memory[i] == 0 )
            Com_Error( ERR_FATAL, "Video API layer not set up correctly: missing binding in slot %d.\n", i );
    }
}

/*
    @pjb: Expose the validation API to other areas of the game
*/
void ValidateGraphicsLayer( graphicsApiLayer_t* layer )
{
    ValidateDriverLayerMemory( (size_t*) layer, sizeof( graphicsApiLayer_t ) );
}

void PROXY_DriverInit( void )
{
    vdconfig_t old_vdConfig, gl_vdConfig, d3d_vdConfig;

    // Get the configurable driver priority
    proxy_driverPriority = Cvar_Get( "proxy_driverPriority", "0", CVAR_ARCHIVE );

    // If using the proxy driver we cannot use fullscreen
    Cvar_Set( "r_fullscreen", "0" );

    // Ignore hardware gamma (D3D does it another way)
    Cvar_Set( "r_ignorehwgamma", "1" );

    // Backup the vdConfig because the drivers will make changes to this (unfortunately)
    old_vdConfig = vdConfig;

    // Init the D3D driver and back up the vdConfig
    D3DDrv_DriverInit();
    ShadowGfxApiBindings( &d3dDriver );
    ValidateGraphicsLayer( &d3dDriver );
    d3d_vdConfig = vdConfig;

    // Init the GL driver and back up the vdConfig
    GLRB_DriverInit();
    ShadowGfxApiBindings( &glDriver );
    ValidateGraphicsLayer( &glDriver );
    gl_vdConfig = vdConfig;

    // Now bind our abstracted functions
    SetProxyBindings();

    // Let's arrange these windows shall we?
    PositionOpenGLWindowRightOfD3D();

    // Restore the old config and reconcile the new configs
    vdConfig = old_vdConfig;
    ReconcileVideoConfigs( &d3d_vdConfig, &gl_vdConfig, &vdConfig );
}


