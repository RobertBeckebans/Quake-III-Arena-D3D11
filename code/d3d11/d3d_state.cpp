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
d3dState_t g_State;
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
    g_State.backBufferView = QD3D::CreateBackBufferView(g_pSwapChain, g_pDevice, &g_State.backBufferDesc);
    ASSERT(g_State.backBufferView);

    g_State.depthBufferView = QD3D::CreateDepthBufferView(
        g_pDevice,
        g_State.backBufferDesc.Width,
        g_State.backBufferDesc.Height,
        // @pjb: todo: make these dependent on Cvars
        DEPTH_TEXTURE_FORMAT,
        DEPTH_DEPTH_VIEW_FORMAT,
        g_State.backBufferDesc.SampleDesc.Count,
        g_State.backBufferDesc.SampleDesc.Quality,
        D3D11_BIND_SHADER_RESOURCE);
    ASSERT(g_State.depthBufferView);
}

//----------------------------------------------------------------------------
// Release references to the back buffer and depth
//----------------------------------------------------------------------------
void DestroyBuffers()
{
    SAFE_RELEASE(g_State.backBufferView);
    SAFE_RELEASE(g_State.depthBufferView);
}

//----------------------------------------------------------------------------
//
// ENTRY POINTS
//
//----------------------------------------------------------------------------

void InitDrawState()
{
    // Don't memset g_State here.

    Com_Memset( &g_RunState, 0, sizeof( g_RunState ) );
    Com_Memset( &g_DrawState, 0, sizeof( g_DrawState ) );

    DestroyBuffers();
    CreateBuffers();
    InitImages();
    InitShaders();

    InitQuadRenderData( &g_DrawState.quadRenderData );
    InitViewRenderData( &g_DrawState.viewRenderData );
    InitRasterStates( &g_DrawState.rasterStates );

    // Set up some default state
    g_pImmediateContext->RSSetState( g_DrawState.rasterStates.cullNone );
    g_pImmediateContext->OMSetRenderTargets( 1, &g_State.backBufferView, g_State.depthBufferView );
}

void DestroyDrawState()
{
    DestroyRasterStates( &g_DrawState.rasterStates );
    DestroyQuadRenderData( &g_DrawState.quadRenderData );
    DestroyViewRenderData( &g_DrawState.viewRenderData );
    DestroyShaders();
    DestroyImages();
    DestroyBuffers();

    Com_Memset( &g_RunState, 0, sizeof( g_RunState ) );
    Com_Memset( &g_DrawState, 0, sizeof( g_DrawState ) );
}

void DrawQuad( 
    const d3dQuadRenderData_t* qrd,
    const d3dImage_t* image,
    const float* coords,
    const float* texcoords,
    const float* color )
{
    //
    // Update the constant buffer
    //
    d3dQuadRenderConstantBuffer_t* cb = QD3D::MapDynamicBuffer<d3dQuadRenderConstantBuffer_t>( 
        g_pImmediateContext, 
        qrd->constantBuffer );

    memcpy( cb->color, color, sizeof(float) * 3 );
    cb->color[3] = 1;

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
    UINT stride = sizeof(float) * 2;
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

