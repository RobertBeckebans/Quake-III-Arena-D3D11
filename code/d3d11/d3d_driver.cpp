// D3D headers
#include "d3d_common.h"
#include "d3d_driver.h"
#include "d3d_wnd.h"
#include "d3d_state.h"
#include "d3d_image.h"
#include "d3d_shaders.h"

// @pjb: this is just here to deliberately fuck the build if driver is used in here
#define driver #driver_disallowed

//----------------------------------------------------------------------------
// Local data
//----------------------------------------------------------------------------
HRESULT g_hrLastError = S_OK;

//----------------------------------------------------------------------------
//
// DRIVER INTERFACE
//
//----------------------------------------------------------------------------
void D3DDrv_Shutdown( void )
{
    DestroyDrawState();
    D3DWnd_Shutdown();
}

void D3DDrv_UnbindResources( void )
{

}

size_t D3DDrv_LastError( void )
{
    return (size_t) g_hrLastError;
}

void D3DDrv_ReadPixels( int x, int y, int width, int height, imageFormat_t requestedFmt, void* dest )
{
    
}

void D3DDrv_ReadDepth( int x, int y, int width, int height, float* dest )
{

}

void D3DDrv_ReadStencil( int x, int y, int width, int height, byte* dest )
{

}

void D3DDrv_DrawImage( const image_t* image, const float* coords, const float* texcoords, const float* color )
{
    DrawQuad(
        &g_DrawState.quadRenderData,
        GetImageRenderInfo( image ),
        coords,
        texcoords,
        color );
}

void D3DDrv_SetGamma( unsigned char red[256], unsigned char green[256], unsigned char blue[256] )
{
    // @pjb: todo?
    if ( vdConfig.deviceSupportsGamma )
    {
        ri.Error( ERR_FATAL, "D3D11 hardware gamma ramp not implemented." );
    }
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

        g_pImmediateContext->ClearRenderTargetView( g_State.backBufferView, clearCol );
    }

    if ( bits & ( CLEAR_DEPTH | CLEAR_STENCIL ) )
    {
        DWORD clearBits = 0;
        if ( bits & CLEAR_DEPTH ) { clearBits |= D3D11_CLEAR_DEPTH; }
        if ( bits & CLEAR_STENCIL ) { clearBits |= D3D11_CLEAR_STENCIL; }
        g_pImmediateContext->ClearDepthStencilView( g_State.depthBufferView, clearBits, depth, (UINT8) stencil );
    }
}

void D3DDrv_SetProjection( const float* projMatrix )
{
    memcpy( g_RunState.projectionMatrix, projMatrix, sizeof(float) * 16 );

    d3dViewConstantBuffer_t* cb = QD3D::MapDynamicBuffer<d3dViewConstantBuffer_t>( g_pImmediateContext, g_DrawState.viewRenderData.constantBuffer );
    memcpy( cb->projectionMatrix, projMatrix, sizeof(float) * 16 );
    g_pImmediateContext->Unmap( g_DrawState.viewRenderData.constantBuffer, 0 );
}

void D3DDrv_GetProjection( float* projMatrix )
{
    memcpy( projMatrix, g_RunState.projectionMatrix, sizeof(float) * 16 );
}

void D3DDrv_SetModelView( const float* modelViewMatrix )
{
    memcpy( g_RunState.modelViewMatrix, modelViewMatrix, sizeof(float) * 16 );

    d3dViewConstantBuffer_t* cb = QD3D::MapDynamicBuffer<d3dViewConstantBuffer_t>( g_pImmediateContext, g_DrawState.viewRenderData.constantBuffer );
    memcpy( cb->modelViewMatrix, modelViewMatrix, sizeof(float) * 16 );
    g_pImmediateContext->Unmap( g_DrawState.viewRenderData.constantBuffer, 0 );
}

void D3DDrv_GetModelView( float* modelViewMatrix )
{
    memcpy( modelViewMatrix, g_RunState.modelViewMatrix, sizeof(float) * 16 );
}

void D3DDrv_SetViewport( int left, int top, int width, int height )
{
    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = left;
    viewport.TopLeftY = top;
    viewport.Width = width;
    viewport.Height = height;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 0;
    g_pImmediateContext->RSSetViewports( 1, &viewport );
}

void D3DDrv_Flush( void )
{
    g_pImmediateContext->Flush();
}

void D3DDrv_SetState( unsigned long stateMask )
{
    g_RunState.stateMask = stateMask;
}

void D3DDrv_ResetState2D( void )
{
    D3DDrv_SetModelView( s_identityMatrix );
    D3DDrv_SetState( GLS_DEPTHTEST_DISABLE |
			  GLS_SRCBLEND_SRC_ALPHA |
			  GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA );

    // @pjb: todo: GL_Cull( CT_TWO_SIDED );

    D3DDrv_SetPortalRendering( qfalse, NULL, NULL );
}

void D3DDrv_ResetState3D( void )
{
    D3DDrv_SetModelView( s_identityMatrix );
    D3DDrv_SetState( GLS_DEFAULT );
}

void D3DDrv_SetPortalRendering( qboolean enabled, const float* flipMatrix, const float* plane )
{

}

void D3DDrv_SetDepthRange( float minRange, float maxRange )
{

}

void D3DDrv_SetDrawBuffer( int buffer )
{

}

void D3DDrv_EndFrame( void )
{
    switch (vdConfig.displayFrequency)
    {
    case 60: g_pSwapChain->Present( 1, 0 ); break; 
    case 30: g_pSwapChain->Present( 2, 0 ); break;
    default: g_pSwapChain->Present( 0, 0 ); break; 
    }
}

void D3DDrv_MakeCurrent( qboolean current )
{
    
}

void D3DDrv_ShadowSilhouette( const float* edges, int edgeCount )
{

}

void D3DDrv_ShadowFinish( void )
{

}

void D3DDrv_DrawSkyBox( const skyboxDrawInfo_t* skybox, const float* eye_origin, const float* colorTint )
{

}

void D3DDrv_DrawBeam( const image_t* image, const float* color, const vec3_t startPoints[], const vec3_t endPoints[], int segs )
{

}

void D3DDrv_DrawStageGeneric( const shaderCommands_t *input )
{

}

void D3DDrv_DrawStageVertexLitTexture( const shaderCommands_t *input )
{

}

void D3DDrv_DrawStageLightmappedMultitexture( const shaderCommands_t *input )
{

}

void D3DDrv_DebugDrawAxis( void )
{

}

void D3DDrv_DebugDrawTris( shaderCommands_t *input )
{

}

void D3DDrv_DebugDrawNormals( shaderCommands_t *input )
{

}

void D3DDrv_DebugSetOverdrawMeasureEnabled( qboolean enabled )
{

}

void D3DDrv_DebugSetTextureMode( const char* mode )
{

}

void D3DDrv_DebugDrawPolygon( int color, int numPoints, const float* points )
{

}

void SetupVideoConfig()
{
    // Set up a bunch of default state
    const char* d3dVersionStr = "Direct3D 11";
    switch ( g_State.featureLevel )
    {
    case D3D_FEATURE_LEVEL_9_1 : d3dVersionStr = "v9.1 (Compatibility)"; break;
    case D3D_FEATURE_LEVEL_9_2 : d3dVersionStr = "v9.2 (Compatibility)"; break;
    case D3D_FEATURE_LEVEL_9_3 : d3dVersionStr = "v9.3 (Compatibility)"; break;
    case D3D_FEATURE_LEVEL_10_0: d3dVersionStr = "v10.0 (Compatibility)"; break;
    case D3D_FEATURE_LEVEL_10_1: d3dVersionStr = "v10.1 (Compatibility)"; break;
    case D3D_FEATURE_LEVEL_11_0: d3dVersionStr = "v11.0"; break;
    case D3D_FEATURE_LEVEL_11_1: d3dVersionStr = "v11.1"; break;
    }
    Q_strncpyz( vdConfig.renderer_string, "Direct3D 11", sizeof( vdConfig.renderer_string ) );
    Q_strncpyz( vdConfig.version_string, d3dVersionStr, sizeof( vdConfig.version_string ) );
    Q_strncpyz( vdConfig.vendor_string, "Microsoft Corporation", sizeof( vdConfig.vendor_string ) );

    D3D11_DEPTH_STENCIL_VIEW_DESC depthBufferViewDesc;
    g_State.depthBufferView->GetDesc( &depthBufferViewDesc );

    DWORD colorDepth = 0, depthDepth = 0, stencilDepth = 0;
    if ( FAILED( QD3D::GetBitDepthForFormat( g_State.swapChainDesc.BufferDesc.Format, &colorDepth ) ) )
        ri.Error( ERR_FATAL, "Bad bit depth supplied for color channel (%x)\n", g_State.swapChainDesc.BufferDesc.Format );

    if ( FAILED( QD3D::GetBitDepthForDepthStencilFormat( depthBufferViewDesc.Format, &depthDepth, &stencilDepth ) ) )
        ri.Error( ERR_FATAL, "Bad bit depth supplied for depth-stencil (%x)\n", depthBufferViewDesc.Format );

    vdConfig.maxTextureSize = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
    vdConfig.maxActiveTextures = D3D11_REQ_SAMPLER_OBJECT_COUNT_PER_DEVICE;
    vdConfig.colorBits = (int) colorDepth;
    vdConfig.depthBits = (int) depthDepth;
    vdConfig.stencilBits = (int) stencilDepth;

    vdConfig.driverType = GLDRV_ICD;
    vdConfig.hardwareType = GLHW_GENERIC;
    vdConfig.deviceSupportsGamma = qfalse; // @pjb: todo?
    vdConfig.textureCompression = TC_NONE; // @pjb: todo: d3d texture compression
    vdConfig.textureEnvAddAvailable = qtrue;
    vdConfig.stereoEnabled = qfalse; // @pjb: todo: d3d stereo support

	DEVMODE dm;
    memset( &dm, 0, sizeof( dm ) );
	dm.dmSize = sizeof( dm );
	if ( EnumDisplaySettings( NULL, ENUM_CURRENT_SETTINGS, &dm ) )
	{
		vdConfig.displayFrequency = dm.dmDisplayFrequency;
	}
    
    // We expect vidWidth, vidHeight and windowAspect to all be set by now
}

D3D_PUBLIC void D3DDrv_DriverInit( graphicsApiLayer_t* layer )
{
    layer->Shutdown = D3DDrv_Shutdown;
    layer->UnbindResources = D3DDrv_UnbindResources;
    layer->LastError = D3DDrv_LastError;
    layer->ReadPixels = D3DDrv_ReadPixels;
    layer->ReadDepth = D3DDrv_ReadDepth;
    layer->ReadStencil = D3DDrv_ReadStencil;
    layer->CreateImage = D3DDrv_CreateImage;
    layer->DeleteImage = D3DDrv_DeleteImage;
    layer->UpdateCinematic = D3DDrv_UpdateCinematic;
    layer->DrawImage = D3DDrv_DrawImage;
    layer->GetImageFormat = D3DDrv_GetImageFormat;
    layer->SetGamma = D3DDrv_SetGamma;
    layer->GetFrameImageMemoryUsage = D3DDrv_SumOfUsedImages;
    layer->GraphicsInfo = D3DDrv_GfxInfo;
    layer->Clear = D3DDrv_Clear;
    layer->SetProjectionMatrix = D3DDrv_SetProjection;
    layer->GetProjectionMatrix = D3DDrv_GetProjection;
    layer->SetModelViewMatrix = D3DDrv_SetModelView;
    layer->GetModelViewMatrix = D3DDrv_GetModelView;
    layer->SetViewport = D3DDrv_SetViewport;
    layer->Flush = D3DDrv_Flush;
    layer->SetState = D3DDrv_SetState;
    layer->ResetState2D = D3DDrv_ResetState2D;
    layer->ResetState3D = D3DDrv_ResetState3D;
    layer->SetPortalRendering = D3DDrv_SetPortalRendering;
    layer->SetDepthRange = D3DDrv_SetDepthRange;
    layer->SetDrawBuffer = D3DDrv_SetDrawBuffer;
    layer->EndFrame = D3DDrv_EndFrame;
    layer->MakeCurrent = D3DDrv_MakeCurrent;
    layer->ShadowSilhouette = D3DDrv_ShadowSilhouette;
    layer->ShadowFinish = D3DDrv_ShadowFinish;
    layer->DrawSkyBox = D3DDrv_DrawSkyBox;
    layer->DrawBeam = D3DDrv_DrawBeam;
    layer->DrawStageGeneric = D3DDrv_DrawStageGeneric;
    layer->DrawStageVertexLitTexture = D3DDrv_DrawStageVertexLitTexture;
    layer->DrawStageLightmappedMultitexture = D3DDrv_DrawStageLightmappedMultitexture;
    layer->DebugDrawAxis = D3DDrv_DebugDrawAxis;
    layer->DebugDrawTris = D3DDrv_DebugDrawTris;
    layer->DebugDrawNormals = D3DDrv_DebugDrawNormals;
    layer->DebugSetOverdrawMeasureEnabled = D3DDrv_DebugSetOverdrawMeasureEnabled;
    layer->DebugSetTextureMode = D3DDrv_DebugSetTextureMode;
    layer->DebugDrawPolygon = D3DDrv_DebugDrawPolygon;

    Com_Memset( &g_State, 0, sizeof( g_State ) );

    // This, weirdly, can be called multiple times. Catch that if that's the case.
    if ( g_pDevice == nullptr )
    {
        D3DWnd_Init();
    }

    InitDrawState();
    
    // Set up vdConfig global
    SetupVideoConfig();
}

