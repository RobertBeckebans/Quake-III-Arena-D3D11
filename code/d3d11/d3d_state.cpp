#include "d3d_common.h"
#include "d3d_wnd.h"
#include "d3d_state.h"
#include "d3d_image.h"
#include "d3d_shaders.h"
#include "d3d_drawdata.h"

//----------------------------------------------------------------------------
// Locals
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Globals
//----------------------------------------------------------------------------
d3dRunState_t g_RunState;
d3dDrawState_t g_DrawState;

//----------------------------------------------------------------------------
// Constants
//----------------------------------------------------------------------------
// @pjb: todo: replace these with defaults from CVars
static const DXGI_FORMAT DEPTH_TEXTURE_FORMAT = DXGI_FORMAT_R24G8_TYPELESS;
static const DXGI_FORMAT DEPTH_DEPTH_VIEW_FORMAT = DXGI_FORMAT_D24_UNORM_S8_UINT;
static const DXGI_FORMAT DEPTH_SHADER_VIEW_FORMAT = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

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
    g_BufferState.backBufferView = QD3D::CreateBackBufferView(g_pSwapChain, g_pDevice, &g_BufferState.backBufferDesc);
    ASSERT(g_BufferState.backBufferView);

    g_BufferState.depthBufferView = QD3D::CreateDepthBufferView(
        g_pDevice,
        g_BufferState.backBufferDesc.Width,
        g_BufferState.backBufferDesc.Height,
        // @pjb: todo: make these dependent on Cvars
        DEPTH_TEXTURE_FORMAT,
        DEPTH_DEPTH_VIEW_FORMAT,
        g_BufferState.backBufferDesc.SampleDesc.Count,
        g_BufferState.backBufferDesc.SampleDesc.Quality,
        D3D11_BIND_SHADER_RESOURCE);
    ASSERT(g_BufferState.depthBufferView);
}

//----------------------------------------------------------------------------
// Release references to the back buffer and depth
//----------------------------------------------------------------------------
void DestroyBuffers()
{
    SAFE_RELEASE(g_BufferState.backBufferView);
    SAFE_RELEASE(g_BufferState.depthBufferView);
}

//----------------------------------------------------------------------------
//
// ENTRY POINTS
//
//----------------------------------------------------------------------------

void InitDrawState()
{
    // Don't memset g_BufferState here.

    Com_Memset( &g_RunState, 0, sizeof( g_RunState ) );
    Com_Memset( &g_DrawState, 0, sizeof( g_DrawState ) );

    DestroyBuffers();
    CreateBuffers();
    InitImages();
    InitShaders();

    InitQuadRenderData( &g_DrawState.quadRenderData );
    InitViewRenderData( &g_DrawState.viewRenderData );
    InitRasterStates( &g_DrawState.rasterStates );
    InitDepthStates( &g_DrawState.depthStates );
    InitBlendStates( &g_DrawState.blendStates );

    // Set up some default state
    float blendFactor[4] = {0, 0, 0, 0};
    g_pImmediateContext->OMSetDepthStencilState( g_DrawState.depthStates.none, 0 );
    g_pImmediateContext->RSSetState( g_DrawState.rasterStates.cullNone );
    g_pImmediateContext->OMSetBlendState( g_DrawState.blendStates.opaque, blendFactor, ~0U );
    g_pImmediateContext->OMSetRenderTargets( 1, &g_BufferState.backBufferView, g_BufferState.depthBufferView );
    D3DDrv_SetViewport( 0, 0, g_BufferState.backBufferDesc.Width, g_BufferState.backBufferDesc.Height );

    // Clear the targets
    FLOAT clearCol[4] = { 0, 0, 0, 0 };
    g_pImmediateContext->ClearRenderTargetView( g_BufferState.backBufferView, clearCol );
    g_pImmediateContext->ClearDepthStencilView( g_BufferState.depthBufferView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0 );
}

void DestroyDrawState()
{
    DestroyRasterStates( &g_DrawState.rasterStates );
    DestroyDepthStates( &g_DrawState.depthStates );
    DestroyBlendStates( &g_DrawState.blendStates );
    DestroyQuadRenderData( &g_DrawState.quadRenderData );
    DestroyViewRenderData( &g_DrawState.viewRenderData );
    DestroyShaders();
    DestroyImages();
    DestroyBuffers();

    Com_Memset( &g_RunState, 0, sizeof( g_RunState ) );
    Com_Memset( &g_DrawState, 0, sizeof( g_DrawState ) );
}

void UpdateDirtyTransform()
{
    d3dViewConstantBuffer_t* cb = QD3D::MapDynamicBuffer<d3dViewConstantBuffer_t>( g_pImmediateContext, g_DrawState.viewRenderData.constantBuffer );
    memcpy( cb->projectionMatrix, g_RunState.projectionMatrix, sizeof(float) * 16 );
    memcpy( cb->modelViewMatrix, g_RunState.modelViewMatrix, sizeof(float) * 16 );
    g_pImmediateContext->Unmap( g_DrawState.viewRenderData.constantBuffer, 0 );
    g_RunState.dirtyTransform = qfalse;
}

// @pjb: forceinline because I don't want to put the 'if' inside UpdateDirtyTransform
__forceinline void EnsureCleanTransform()
{
    if ( g_RunState.dirtyTransform )
        UpdateDirtyTransform();
}

void DrawQuad( 
    const d3dQuadRenderData_t* qrd,
    const d3dImage_t* image,
    const float* coords,
    const float* texcoords,
    const float* color )
{
    EnsureCleanTransform();

    //
    // Update the constant buffer
    //
    d3dQuadRenderConstantBuffer_t* cb = QD3D::MapDynamicBuffer<d3dQuadRenderConstantBuffer_t>( 
        g_pImmediateContext, 
        qrd->constantBuffer );

    if ( color ) {
        memcpy( cb->color, color, sizeof(float) * 3 );
        cb->color[3] = 1;
    } else {
        cb->color[0] = 1; cb->color[1] = 1; cb->color[2] = 1; cb->color[3] = 1; 
    }

    g_pImmediateContext->Unmap( qrd->constantBuffer, 0 );

    //
    // Update the vertex buffer
    //
    d3dQuadRenderVertex_t* vb = QD3D::MapDynamicBuffer<d3dQuadRenderVertex_t>( 
        g_pImmediateContext, 
        qrd->vertexBuffer );

    vb[0].texcoords[0] = texcoords[0]; vb[0].texcoords[1] = texcoords[1];
    vb[1].texcoords[0] = texcoords[2]; vb[1].texcoords[1] = texcoords[1];
    vb[2].texcoords[0] = texcoords[2]; vb[2].texcoords[1] = texcoords[3];
    vb[3].texcoords[0] = texcoords[0]; vb[3].texcoords[1] = texcoords[3];

    vb[0].coords[0] = coords[0]; vb[0].coords[1] = coords[1];
    vb[1].coords[0] = coords[2]; vb[1].coords[1] = coords[1];
    vb[2].coords[0] = coords[2]; vb[2].coords[1] = coords[3];
    vb[3].coords[0] = coords[0]; vb[3].coords[1] = coords[3];

    g_pImmediateContext->Unmap( qrd->vertexBuffer, 0 );

    //
    // Draw
    //
    UINT stride = sizeof(float) * 4;
    UINT offset = 0;

    g_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    g_pImmediateContext->IASetVertexBuffers( 0, 1, &qrd->vertexBuffer, &stride, &offset );
    g_pImmediateContext->IASetInputLayout( qrd->inputLayout );
    g_pImmediateContext->IASetIndexBuffer( qrd->indexBuffer, DXGI_FORMAT_R16_UINT, 0 );
    g_pImmediateContext->VSSetShader( qrd->vertexShader, nullptr, 0 );
    g_pImmediateContext->VSSetConstantBuffers( 0, 1, &g_DrawState.viewRenderData.constantBuffer );
    g_pImmediateContext->PSSetShader( qrd->pixelShader, nullptr, 0 );
    g_pImmediateContext->PSSetSamplers( 0, 1, &image->pSampler );
    g_pImmediateContext->PSSetShaderResources( 0, 1, &image->pSRV );
    g_pImmediateContext->PSSetConstantBuffers( 0, 1, &g_DrawState.quadRenderData.constantBuffer );
    g_pImmediateContext->DrawIndexed( 6, 0, 0 );
}

