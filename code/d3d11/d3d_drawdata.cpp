#include "d3d_common.h"
#include "d3d_state.h"
#include "d3d_drawdata.h"
#include "d3d_shaders.h"
#include "d3d_image.h"

void InitQuadRenderData( d3dQuadRenderData_t* qrd ) 
{
    Com_Memset( qrd, 0, sizeof( d3dQuadRenderData_t ) );

    ID3DBlob* vsByteCode = nullptr;

    qrd->vertexShader = CompileVertexShader( "fsq_vs", &vsByteCode );
    qrd->pixelShader = CompilePixelShader( "fsq_ps" );

    D3D11_INPUT_ELEMENT_DESC elements[] = { 
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    HRESULT hr = g_pDevice->CreateInputLayout(
        elements,
        _countof( elements ),
        vsByteCode->GetBufferPointer(),
        vsByteCode->GetBufferSize(),
        &qrd->inputLayout );
    if ( FAILED( hr ) ) {
        ri.Error( ERR_FATAL, "Failed to create input layout: 0x%08X", hr );
    }

    SAFE_RELEASE( vsByteCode );

    static const USHORT indices[] = 
    {
	    0, 2, 1,
	    1, 2, 3
    };

    qrd->indexBuffer = QD3D::CreateImmutableBuffer( 
        g_pDevice, 
        D3D11_BIND_INDEX_BUFFER, 
        indices,
        sizeof( indices ) );
    if ( !qrd->indexBuffer ) {
        ri.Error( ERR_FATAL, "Could not create FSQ index buffer.\n" );
    }

    qrd->vertexBuffer = QD3D::CreateDynamicBuffer<d3dQuadRenderVertex_t>(
        g_pDevice, 
        D3D11_BIND_VERTEX_BUFFER,
        4);
    if ( !qrd->vertexBuffer ) {
        ri.Error( ERR_FATAL, "Could not create FSQ vertex buffer.\n" );
    }

    qrd->constantBuffer = QD3D::CreateDynamicBuffer<d3dQuadRenderConstantBuffer_t>( 
        g_pDevice, 
        D3D11_BIND_CONSTANT_BUFFER );
    if ( !qrd->constantBuffer ) {
        ri.Error( ERR_FATAL, "Could not create FSQ constant buffer.\n" );
    }
}

void DestroyQuadRenderData( d3dQuadRenderData_t* qrd )
{
    SAFE_RELEASE( qrd->inputLayout );
    SAFE_RELEASE( qrd->vertexShader );
    SAFE_RELEASE( qrd->pixelShader );
    SAFE_RELEASE( qrd->vertexBuffer );
    SAFE_RELEASE( qrd->indexBuffer );
    SAFE_RELEASE( qrd->constantBuffer );

    Com_Memset( qrd, 0, sizeof( d3dQuadRenderData_t ) );
}

void InitViewRenderData( d3dViewRenderData_t* vrd )
{
    Com_Memset( vrd, 0, sizeof( d3dViewRenderData_t ) );

    vrd->constantBuffer = QD3D::CreateDynamicBuffer<d3dViewConstantBuffer_t>( 
        g_pDevice, 
        D3D11_BIND_CONSTANT_BUFFER );
    if ( !vrd->constantBuffer ) {
        ri.Error( ERR_FATAL, "Could not create view constant buffer.\n" );
    }
}

void DestroyViewRenderData( d3dViewRenderData_t* vrd )
{
    SAFE_RELEASE( vrd->constantBuffer );

    Com_Memset( vrd, 0, sizeof( d3dViewRenderData_t ) );
}

