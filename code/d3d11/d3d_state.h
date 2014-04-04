#pragma once

#include "d3d_common.h"

// @pjb: back-end representation of an image_t
struct d3dImage_t
{
    ID3D11Texture2D* pTexture;
    ID3D11ShaderResourceView* pSRV;
    ID3D11SamplerState* pSampler;
    DXGI_FORMAT internalFormat;
    int width;
    int height;
    int frameUsed;
    qboolean dynamic;
};

struct d3dViewRenderData_t
{
    ID3D11Buffer* constantBuffer;
};

struct d3dQuadRenderData_t
{
    // Shaders
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;

    // Vertex buffers
    ID3D11InputLayout* inputLayout;
    ID3D11Buffer* indexBuffer;
    ID3D11Buffer* vertexBuffer;

    // Constant buffers
    ID3D11Buffer* constantBuffer;
};

// @pjb: stores the D3D state
struct d3dState_t {
    D3D11_TEXTURE2D_DESC backBufferDesc;
    ID3D11RenderTargetView* backBufferView;
    ID3D11DepthStencilView* depthBufferView;
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    D3D_FEATURE_LEVEL featureLevel;
};

// @pjb: stores common raster states
struct d3dRasterStates_t
{
    ID3D11RasterizerState* cullNone;
};

// @pjb: stores draw info like samplers and buffers
struct d3dDrawState_t
{
    d3dQuadRenderData_t quadRenderData;
    d3dViewRenderData_t viewRenderData;
    d3dRasterStates_t rasterStates;
};

// @pjb: stores the run-time game state. The game is set up like a state machine so we'll be doing the same.
struct d3dRunState_t {
    float modelViewMatrix[16];
    float projectionMatrix[16];
    unsigned long stateMask; // combination of GLS_* flags
};

//----------------------------------------------------------------------------
// Dynamic buffer layouts
//----------------------------------------------------------------------------
struct d3dQuadRenderVertex_t
{
    float coords[2];    
    float texcoords[2];
};

struct d3dQuadRenderConstantBuffer_t 
{
    float color[4];
};

struct d3dViewConstantBuffer_t
{
    float projectionMatrix[16];
    float modelViewMatrix[16];
};

//----------------------------------------------------------------------------
// Imports from d3d_device.cpp
//----------------------------------------------------------------------------
extern HRESULT g_hrLastError;
extern d3dState_t g_State;
extern d3dRunState_t g_RunState;
extern d3dDrawState_t g_DrawState;

//----------------------------------------------------------------------------
// Imports from d3d_wnd.cpp
//----------------------------------------------------------------------------
extern ID3D11Device* g_pDevice;
extern ID3D11DeviceContext* g_pImmediateContext;
extern IDXGISwapChain* g_pSwapChain;

//----------------------------------------------------------------------------
// Internal APIs
//----------------------------------------------------------------------------

void InitDrawState();
void DestroyDrawState();

void DrawQuad( 
    const d3dQuadRenderData_t* qrd, 
    const d3dImage_t* texture, 
    const float* coords, 
    const float* texcoords, 
    const float* color );


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

