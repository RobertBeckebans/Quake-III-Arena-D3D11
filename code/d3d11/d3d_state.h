#pragma once

// @pjb: stores the D3D state. The game is set up like a state machine so we'll be doing the same.
struct d3dState_t {
    // Init-time state
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    D3D_FEATURE_LEVEL featureLevel;

    // Run-time state
    float modelViewMatrix[16];
    float projectionMatrix[16];
    unsigned long stateMask; // combination of GLS_* flags

};

//----------------------------------------------------------------------------
// Imports from d3d_device.cpp
//----------------------------------------------------------------------------
extern HRESULT g_hrLastError;
extern d3dState_t g_State;

//----------------------------------------------------------------------------
// Imports from d3d_wnd.cpp
//----------------------------------------------------------------------------
extern ID3D11Device* g_pDevice;
extern ID3D11DeviceContext* g_pImmediateContext;
extern IDXGISwapChain* g_pSwapChain;

//----------------------------------------------------------------------------
// Driver entry points
//----------------------------------------------------------------------------

void D3DDrv_Shutdown( void );
void D3DDrv_UnbindResources( void );
size_t D3DDrv_LastError( void );
void D3DDrv_ReadPixels( int x, int y, int width, int height, imageFormat_t requestedFmt, void* dest );
void D3DDrv_ReadDepth( int x, int y, int width, int height, float* dest );
void D3DDrv_ReadStencil( int x, int y, int width, int height, byte* dest );
void D3DDrv_DrawImage( const image_t* image, const float* coords, const float* texcoords, const float* color );
void D3DDrv_SetGamma( unsigned char red[256], unsigned char green[256], unsigned char blue[256] );
void D3DDrv_GfxInfo( void );
void D3DDrv_Clear( unsigned long bits, const float* clearCol, unsigned long stencil, float depth );
void D3DDrv_SetProjection( const float* projMatrix );
void D3DDrv_GetProjection( float* projMatrix );
void D3DDrv_SetModelView( const float* modelViewMatrix );
void D3DDrv_GetModelView( float* modelViewMatrix );
void D3DDrv_SetViewport( int left, int top, int width, int height );
void D3DDrv_Flush( void );
void D3DDrv_SetState( unsigned long stateMask );
void D3DDrv_ResetState2D( void );
void D3DDrv_ResetState3D( void );
void D3DDrv_SetPortalRendering( qboolean enabled, const float* flipMatrix, const float* plane );
void D3DDrv_SetDepthRange( float minRange, float maxRange );
void D3DDrv_SetDrawBuffer( int buffer );
void D3DDrv_EndFrame( void );
void D3DDrv_MakeCurrent( qboolean current );
void D3DDrv_ShadowSilhouette( const float* edges, int edgeCount );
void D3DDrv_ShadowFinish( void );
void D3DDrv_DrawSkyBox( const skyboxDrawInfo_t* skybox, const float* eye_origin, const float* colorTint );
void D3DDrv_DrawBeam( const image_t* image, const float* color, const vec3_t startPoints[], const vec3_t endPoints[], int segs );
void D3DDrv_DrawStageGeneric( const shaderCommands_t *input );
void D3DDrv_DrawStageVertexLitTexture( const shaderCommands_t *input );
void D3DDrv_DrawStageLightmappedMultitexture( const shaderCommands_t *input );
void D3DDrv_DebugDrawAxis( void );
void D3DDrv_DebugDrawTris( shaderCommands_t *input );
void D3DDrv_DebugDrawNormals( shaderCommands_t *input );
void D3DDrv_DebugSetOverdrawMeasureEnabled( qboolean enabled );
void D3DDrv_DebugSetTextureMode( const char* mode );
void D3DDrv_DebugDrawPolygon( int color, int numPoints, const float* points );