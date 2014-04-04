// D3D headers
#include "d3d_common.h"
#include "d3d_driver.h"
#include "d3d_wnd.h"
#include "d3d_state.h"
#include "d3d_image.h"
#include "d3d_shaders.h"

// @pjb: this is just here to deliberately fuck the build if driver is used in here
#define driver #driver_disallowed

static const d3dImage_t* D3DR_GetAnimatedImage( textureBundle_t *bundle, float shaderTime ) {
	int		index;

	if ( bundle->isVideoMap ) {
		ri.CIN_RunCinematic(bundle->videoMapHandle);
		ri.CIN_UploadCinematic(bundle->videoMapHandle);
		return GetImageRenderInfo( tr.scratchImage[bundle->videoMapHandle] );
	}

	if ( bundle->numImageAnimations <= 1 ) {
		return GetImageRenderInfo( bundle->image[0] );
	}

	// it is necessary to do this messy calc to make sure animations line up
	// exactly with waveforms of the same frequency
	index = myftol( shaderTime * bundle->imageAnimationSpeed * FUNCTABLE_SIZE );
	index >>= FUNCTABLE_SIZE2;

	if ( index < 0 ) {
		index = 0;	// may happen with shader time offsets
	}
	index %= bundle->numImageAnimations;

    return GetImageRenderInfo( bundle->image[index] );
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
	for ( int stage = 0; stage < MAX_SHADER_STAGES; stage++ )
	{
		shaderStage_t *pStage = input->xstages[stage];

		if ( !pStage )
		{
			break;
		}

        // todo: bind color data in input->svars[stage].colors

 		//
		// do multitexture
		//
		if ( pStage->bundle[1].image[0] != 0 )
		{
            // todo: Draw MT
        }
        else
        {
            // todo: texcoords in input->svars[stage].texcoords[0]

            const d3dImage_t* texture = nullptr;
			if ( pStage->bundle[0].vertexLightmap && ( (r_vertexLight->integer && !r_uiFullScreen->integer) || vdConfig.hardwareType == GLHW_PERMEDIA2 ) && r_lightmap->integer )
			{
                texture = GetImageRenderInfo( tr.whiteImage );
			}
			else 
            {
                texture = D3DR_GetAnimatedImage( &pStage->bundle[0], input->shaderTime );
            }

			D3DDrv_SetState( pStage->stateBits );

            // todo: GLR_DrawElements( input, stage, input->numIndexes, input->indexes );
        }

		// allow skipping out to show just lightmaps during development
		if ( r_lightmap->integer && ( pStage->bundle[0].isLightmap || pStage->bundle[1].isLightmap || pStage->bundle[0].vertexLightmap ) )
		{
			break;
		}
    }
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

void D3DDrv_DebugDrawPolygon( int color, int numPoints, const float* points )
{

}
