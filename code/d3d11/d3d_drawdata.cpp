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
	    1, 2, 0,
	    2, 3, 0
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

void InitRasterStates( d3dRasterStates_t* rs )
{
    Com_Memset( rs, 0, sizeof( d3dRasterStates_t ) );

    D3D11_RASTERIZER_DESC rd;
    ZeroMemory( &rd, sizeof( rd ) );
    rd.FillMode = D3D11_FILL_SOLID;
    rd.FrontCounterClockwise = TRUE;

    // 
    // No culling
    //
    rd.CullMode = D3D11_CULL_NONE;
    g_pDevice->CreateRasterizerState( &rd, &rs->cullNone );

    // 
    // Backface culling
    //
    ZeroMemory( &rd, sizeof( rd ) );
    rd.CullMode = D3D11_CULL_BACK;
    g_pDevice->CreateRasterizerState( &rd, &rs->cullBack );

    // 
    // Frontface culling
    //
    rd.CullMode = D3D11_CULL_FRONT;
    g_pDevice->CreateRasterizerState( &rd, &rs->cullFront );
}

void DestroyRasterStates( d3dRasterStates_t* rs )
{
    SAFE_RELEASE( rs->cullNone );
    SAFE_RELEASE( rs->cullBack );
    SAFE_RELEASE( rs->cullFront );

    Com_Memset( rs, 0, sizeof( d3dRasterStates_t ) );
}

void InitDepthStates( d3dDepthStates_t* ds )
{
    Com_Memset( ds, 0, sizeof( d3dDepthStates_t ) );

    // 
    // No depth write or test
    //
    D3D11_DEPTH_STENCIL_DESC dsd;
    ZeroMemory( &dsd, sizeof( dsd ) );
    dsd.DepthEnable = FALSE;
    dsd.StencilEnable = FALSE;
    g_pDevice->CreateDepthStencilState( &dsd, &ds->none );
}

void DestroyDepthStates( d3dDepthStates_t* ds )
{
    SAFE_RELEASE( ds->none );

    Com_Memset( ds, 0, sizeof( d3dDepthStates_t ) );
}

void InitBlendStates( d3dBlendStates_t* bs )
{
    Com_Memset( bs, 0, sizeof( d3dBlendStates_t ) );

    // 
    // No blending
    //
    D3D11_BLEND_DESC bsd;
    ZeroMemory( &bsd, sizeof( bsd ) );
    bsd.RenderTarget[0].BlendEnable = FALSE;
    bsd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    g_pDevice->CreateBlendState( &bsd, &bs->opaque );
}

void DestroyBlendStates( d3dBlendStates_t* bs )
{
    SAFE_RELEASE( bs->opaque );

    Com_Memset( bs, 0, sizeof( d3dBlendStates_t ) );
}

static ID3D11Buffer* CreateTessIndexBuffer()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

    // @pjb: todo: glIndex_t is apparently 32 bit :(
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = (UINT)sizeof(glIndex_t) * SHADER_MAX_VERTEXES;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	
	ID3D11Buffer* buffer;
	g_pDevice->CreateBuffer(&bd, NULL, &buffer);
    if ( !buffer ) {
        ri.Error( ERR_FATAL, "Could not create tess index buffer.\n" );
    }
    
    return buffer;
}

static ID3D11Buffer* CreateTessVertexBuffer( size_t size )
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = (UINT)size * SHADER_MAX_VERTEXES;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	
	ID3D11Buffer* buffer;
	g_pDevice->CreateBuffer(&bd, NULL, &buffer);
    if ( !buffer ) {
        ri.Error( ERR_FATAL, "Could not create tess vertex buffer.\n" );
    }
    
    return buffer;
}

void InitTessBuffers( d3dTessBuffers_t* tess )
{
    Com_Memset( tess, 0, sizeof( *tess ) );

    // Index buffer
    tess->indexes = CreateTessIndexBuffer();

    // Vertex buffer
    tess->xyz = CreateTessVertexBuffer( sizeof(vec4_t) );

    //
    // Now set up the stages
    //
    for ( int stage = 0; stage < MAX_SHADER_STAGES; ++stage )
    {
        d3dTessStageBuffers_t* stageBufs = &tess->stages[stage];

        // Color buffer
        stageBufs->colors = CreateTessVertexBuffer( sizeof(color4ub_t) );

        // Texcoord buffers
        for ( int tex = 0; tex < NUM_TEXTURE_BUNDLES; ++tex )
        {
            // Color buffer
            stageBufs->texCoords[tex] = CreateTessVertexBuffer( sizeof(vec2_t) );
        }
    }
}

void DestroyTessBuffers( d3dTessBuffers_t* tess )
{
    SAFE_RELEASE( tess->indexes );
    SAFE_RELEASE( tess->xyz );

    for ( int stage = 0; stage < MAX_SHADER_STAGES; ++stage )
    {
        SAFE_RELEASE( tess->stages[stage].colors );
        for ( int tex = 0; tex < NUM_TEXTURE_BUNDLES; ++tex )
            SAFE_RELEASE( tess->stages[stage].texCoords[tex] );
    }

    Com_Memset( tess, 0, sizeof( *tess ) );
}

