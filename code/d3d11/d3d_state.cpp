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
// Set the culling mode depending on whether it's a mirror or not
//----------------------------------------------------------------------------
void SetCullMode( int cullType )
{
    int maskBits = ( backEnd.viewParms.isMirror << 3 ) | cullType;

	if ( cullType == CT_TWO_SIDED ) 
	{
        g_pImmediateContext->RSSetState( g_DrawState.rasterStates.cullNone );
	} 
	else 
	{
		if ( cullType == CT_BACK_SIDED )
		{
			if ( backEnd.viewParms.isMirror )
			{
                g_pImmediateContext->RSSetState( g_DrawState.rasterStates.cullFront );
			}
			else
			{
                g_pImmediateContext->RSSetState( g_DrawState.rasterStates.cullBack );
			}
		}
		else
		{
			if ( backEnd.viewParms.isMirror )
			{
                g_pImmediateContext->RSSetState( g_DrawState.rasterStates.cullBack );
			}
			else
			{
                g_pImmediateContext->RSSetState( g_DrawState.rasterStates.cullFront );
			}
		}
	}
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

    // Set up default state
    Com_Memcpy( g_RunState.constants.modelViewMatrix, s_identityMatrix, sizeof(float) * 16 );
    Com_Memcpy( g_RunState.constants.projectionMatrix, s_identityMatrix, sizeof(float) * 16 );
    g_RunState.constants.depthRange[0] = 0;
    g_RunState.constants.depthRange[1] = 1;
    g_RunState.stateMask = GLS_DEFAULT;
    g_RunState.dirtyConstants = qtrue;
    
    // Create D3D objects
    DestroyBuffers();
    CreateBuffers();
    InitImages();
    InitShaders();

    InitQuadRenderData( &g_DrawState.quadRenderData );
    InitViewRenderData( &g_DrawState.viewRenderData );
    InitGenericStageRenderData( &g_DrawState.genericStage );
    InitTessBuffers( &g_DrawState.tessBufs );
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
    DestroyTessBuffers( &g_DrawState.tessBufs );
    DestroyGenericStageRenderData( &g_DrawState.genericStage );
    DestroyQuadRenderData( &g_DrawState.quadRenderData );
    DestroyViewRenderData( &g_DrawState.viewRenderData );
    DestroyShaders();
    DestroyImages();
    DestroyBuffers();

    Com_Memset( &g_RunState, 0, sizeof( g_RunState ) );
    Com_Memset( &g_DrawState, 0, sizeof( g_DrawState ) );
}

