#pragma once

#include "d3d_common.h"

//----------------------------------------------------------------------------
// Consstants
//----------------------------------------------------------------------------

enum 
{
    BLENDSTATE_SRC_COUNT = 9,
    BLENDSTATE_DST_COUNT = 8,
    CULLMODE_COUNT = 3
};

enum 
{
    DEPTHSTATE_FLAG_TEST = 1,
    DEPTHSTATE_FLAG_MASK = 2,
    DEPTHSTATE_FLAG_EQUAL = 4, // as opposed to the default, LEq.
    DEPTHSTATE_COUNT = 8
};

enum 
{
    RASTERIZERSTATE_FLAG_POLY_OFFSET = 1,
    RASTERIZERSTATE_FLAG_POLY_OUTLINE = 2,
    RASTERIZERSTATE_COUNT = 4
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

// @pjb: todo: consider splitting this up if it's a perf issue
// reuploading the whole thing each time
struct d3dViewVSConstantBuffer_t
{
    float projectionMatrix[16];
    float modelViewMatrix[16];
    float depthRange[2];
    float __padding[2];
};

struct d3dViewPSConstantBuffer_t
{
    float clipPlane[4];
};

//----------------------------------------------------------------------------
// Internal structures
//----------------------------------------------------------------------------
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
    ID3D11Buffer* vsConstantBuffer;
    ID3D11Buffer* psConstantBuffer;
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

// @pjb: for the generic stage rendering
struct d3dGenericStageRenderData_t 
{
    // Single-texture
    ID3D11VertexShader* vertexShaderST;
    ID3D11PixelShader* pixelShaderST;
    ID3D11InputLayout* inputLayoutST;

    // Multi-texture
    ID3D11VertexShader* vertexShaderMT;
    ID3D11PixelShader* pixelShaderMT;
    ID3D11InputLayout* inputLayoutMT;
};

// @pjb: represents the GPU caches for stageVars_t
struct d3dTessStageBuffers_t {
    ID3D11Buffer* texCoords[NUM_TEXTURE_BUNDLES];
    ID3D11Buffer* colors;
};

// @pjb: represents the GPU caches for shaderCommands_t
struct d3dTessBuffers_t {
    ID3D11Buffer* indexes;
    ID3D11Buffer* xyz;
    d3dTessStageBuffers_t stages[MAX_SHADER_STAGES];
};

// @pjb: stores the D3D state and only changes every WndInit
struct d3dBackBufferState_t {
    D3D11_TEXTURE2D_DESC backBufferDesc;
    ID3D11RenderTargetView* backBufferView;
    ID3D11DepthStencilView* depthBufferView;
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    D3D_FEATURE_LEVEL featureLevel;
};

// @pjb: stores common raster states
struct d3dRasterStates_t
{
    ID3D11RasterizerState* states[CULLMODE_COUNT][RASTERIZERSTATE_COUNT];
};

// @pjb: stores common depth states
struct d3dDepthStates_t
{
    ID3D11DepthStencilState* states[DEPTHSTATE_COUNT];
};

// @pjb: stores common blend states
struct d3dBlendStates_t
{
    ID3D11BlendState* opaque;
    ID3D11BlendState* states[BLENDSTATE_SRC_COUNT][BLENDSTATE_DST_COUNT];
};

// @pjb: stores draw info like samplers and buffers
struct d3dDrawState_t
{
    d3dQuadRenderData_t quadRenderData;
    d3dViewRenderData_t viewRenderData;
    
    d3dTessBuffers_t tessBufs;
    d3dGenericStageRenderData_t genericStage;

    d3dRasterStates_t rasterStates;
    d3dDepthStates_t depthStates;
    d3dBlendStates_t blendStates;
};

// @pjb: stores the run-time game state. The game is set up like a state machine so we'll be doing the same.
struct d3dRunState_t {
    d3dViewVSConstantBuffer_t vsConstants;
    d3dViewPSConstantBuffer_t psConstants;
    unsigned long stateMask; // combination of GLS_* flags
    int cullMode; // CT_ flag
    unsigned long depthStateMask;
    qboolean vsDirtyConstants;
    qboolean psDirtyConstants;
};

//----------------------------------------------------------------------------
// Imports from d3d_device.cpp
//----------------------------------------------------------------------------
extern HRESULT g_hrLastError;
extern d3dBackBufferState_t g_BufferState;
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

// cullmode = CT_ flags
void CommitRasterizerState( int cullMode, qboolean polyOffset, qboolean outline );

ID3D11RasterizerState* GetRasterizerState( D3D11_CULL_MODE cullmode, unsigned long mask );
ID3D11DepthStencilState* GetDepthState( unsigned long mask ); // DEPTHSTATE_FLAG_ enum
ID3D11BlendState* GetBlendState( int src, int dst );
D3D11_BLEND GetSrcBlendConstant( int qConstant );
D3D11_BLEND GetDestBlendConstant( int qConstant );
D3D11_BLEND GetSrcBlendAlphaConstant( int qConstant );
D3D11_BLEND GetDestBlendAlphaConstant( int qConstant );

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
void D3DDrv_BeginTessellate( const shaderCommands_t* input );
void D3DDrv_EndTessellate( const shaderCommands_t* input );
void D3DDrv_DebugDrawAxis( void );
void D3DDrv_DebugDrawTris( const shaderCommands_t *input );
void D3DDrv_DebugDrawNormals( const shaderCommands_t *input );
void D3DDrv_DebugSetOverdrawMeasureEnabled( qboolean enabled );
void D3DDrv_DebugSetTextureMode( const char* mode );
void D3DDrv_DebugDrawPolygon( int color, int numPoints, const float* points );

