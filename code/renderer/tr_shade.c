/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
// tr_shade.c

#include "tr_local.h"
#include "tr_layer.h"

/*

  THIS ENTIRE FILE IS BACK END

  This file deals with applying shaders to surface data in the tess struct.
*/


shaderCommands_t	tess;

/*
===================
Perform dynamic lighting with another rendering pass
===================
*/
static void GLR_ComputeDlightProjection( shaderCommands_t* input, int stage ) {
	int		i, l;
#if idppc_altivec
	vec_t	origin0, origin1, origin2;
	float   texCoords0, texCoords1;
	vector float floatColorVec0, floatColorVec1;
	vector float modulateVec, colorVec, zero;
	vector short colorShort;
	vector signed int colorInt;
	vector unsigned char floatColorVecPerm, modulatePerm, colorChar;
	vector unsigned char vSel = (vector unsigned char)(0x00, 0x00, 0x00, 0xff,
							   0x00, 0x00, 0x00, 0xff,
							   0x00, 0x00, 0x00, 0xff,
							   0x00, 0x00, 0x00, 0xff);
#else
	vec3_t	origin;
#endif
	float	*texCoords;
	byte	*colors;
	byte	clipBits[SHADER_MAX_VERTEXES];
	float	scale;
	float	radius;
	vec3_t	floatColor;
	float	modulate;
    int     numDlights = min( backEnd.refdef.num_dlights, MAX_DLIGHTS );

	if ( !backEnd.refdef.num_dlights ) {
		return;
	}

#if idppc_altivec
	// There has to be a better way to do this so that floatColor 
	// and/or modulate are already 16-byte aligned.
	floatColorVecPerm = vec_lvsl(0,(float *)floatColor);
	modulatePerm = vec_lvsl(0,(float *)&modulate);
	modulatePerm = (vector unsigned char)vec_splat((vector unsigned int)modulatePerm,0);
	zero = (vector float)vec_splat_s8(0);
#endif

    input->dlightCount = numDlights;

	for ( l = 0 ; l < numDlights ; l++ ) {
		dlight_t	*dl;
        dlightProjectionInfo_t* dlInfo = &input->dlightInfo[l];

        dlInfo->numIndexes = 0;

		if ( !( input->dlightBits & ( 1 << l ) ) ) {
			continue;	// this surface definately doesn't have any of this light
		}
		texCoords = dlInfo->texCoordsArray[0];
		colors = dlInfo->colorArray[0];

		dl = &backEnd.refdef.dlights[l];
#if idppc_altivec
		origin0 = dl->transformed[0];
		origin1 = dl->transformed[1];
		origin2 = dl->transformed[2];
#else
		VectorCopy( dl->transformed, origin );
#endif
		radius = dl->radius;
		scale = 1.0f / radius;

		floatColor[0] = dl->color[0] * 255.0f;
		floatColor[1] = dl->color[1] * 255.0f;
		floatColor[2] = dl->color[2] * 255.0f;
#if idppc_altivec
		floatColorVec0 = vec_ld(0, floatColor);
		floatColorVec1 = vec_ld(11, floatColor);
		floatColorVec0 = vec_perm(floatColorVec0,floatColorVec0,floatColorVecPerm);
#endif
		for ( i = 0 ; i < input->numVertexes ; i++, texCoords += 2, colors += 4 ) {
#if idppc_altivec
			vec_t dist0, dist1, dist2;
#else
			vec3_t	dist;
#endif
			int		clip;

			backEnd.pc.c_dlightVertexes++;

#if idppc_altivec
			//VectorSubtract( origin, input->xyz[i], dist );
			dist0 = origin0 - input->xyz[i][0];
			dist1 = origin1 - input->xyz[i][1];
			dist2 = origin2 - input->xyz[i][2];
			texCoords0 = 0.5f + dist0 * scale;
			texCoords1 = 0.5f + dist1 * scale;

			clip = 0;
			if ( texCoords0 < 0.0f ) {
				clip |= 1;
			} else if ( texCoords0 > 1.0f ) {
				clip |= 2;
			}
			if ( texCoords1 < 0.0f ) {
				clip |= 4;
			} else if ( texCoords1 > 1.0f ) {
				clip |= 8;
			}
			texCoords[0] = texCoords0;
			texCoords[1] = texCoords1;
			
			// modulate the strength based on the height and color
			if ( dist2 > radius ) {
				clip |= 16;
				modulate = 0.0f;
			} else if ( dist2 < -radius ) {
				clip |= 32;
				modulate = 0.0f;
			} else {
				dist2 = Q_fabs(dist2);
				if ( dist2 < radius * 0.5f ) {
					modulate = 1.0f;
				} else {
					modulate = 2.0f * (radius - dist2) * scale;
				}
			}
			clipBits[i] = clip;

			modulateVec = vec_ld(0,(float *)&modulate);
			modulateVec = vec_perm(modulateVec,modulateVec,modulatePerm);
			colorVec = vec_madd(floatColorVec0,modulateVec,zero);
			colorInt = vec_cts(colorVec,0);	// RGBx
			colorShort = vec_pack(colorInt,colorInt);		// RGBxRGBx
			colorChar = vec_packsu(colorShort,colorShort);	// RGBxRGBxRGBxRGBx
			colorChar = vec_sel(colorChar,vSel,vSel);		// RGBARGBARGBARGBA replace alpha with 255
			vec_ste((vector unsigned int)colorChar,0,(unsigned int *)colors);	// store color
#else
			VectorSubtract( origin, input->xyz[i], dist );
			texCoords[0] = 0.5f + dist[0] * scale;
			texCoords[1] = 0.5f + dist[1] * scale;

			clip = 0;
			if ( texCoords[0] < 0.0f ) {
				clip |= 1;
			} else if ( texCoords[0] > 1.0f ) {
				clip |= 2;
			}
			if ( texCoords[1] < 0.0f ) {
				clip |= 4;
			} else if ( texCoords[1] > 1.0f ) {
				clip |= 8;
			}
			// modulate the strength based on the height and color
			if ( dist[2] > radius ) {
				clip |= 16;
				modulate = 0.0f;
			} else if ( dist[2] < -radius ) {
				clip |= 32;
				modulate = 0.0f;
			} else {
				dist[2] = Q_fabs(dist[2]);
				if ( dist[2] < radius * 0.5f ) {
					modulate = 1.0f;
				} else {
					modulate = 2.0f * (radius - dist[2]) * scale;
				}
			}
			clipBits[i] = clip;

			colors[0] = myftol(floatColor[0] * modulate);
			colors[1] = myftol(floatColor[1] * modulate);
			colors[2] = myftol(floatColor[2] * modulate);
			colors[3] = 255;
#endif
		}

        dlInfo->additive = dl->additive;

		// build a list of triangles that need light
		dlInfo->numIndexes = 0;
		for ( i = 0 ; i < input->numIndexes ; i += 3 ) {
			int		a, b, c;

			a = input->indexes[i];
			b = input->indexes[i+1];
			c = input->indexes[i+2];
			if ( clipBits[a] & clipBits[b] & clipBits[c] ) {
				continue;	// not lighted
			}
			dlInfo->hitIndexes[dlInfo->numIndexes] = a;
			dlInfo->hitIndexes[dlInfo->numIndexes+1] = b;
			dlInfo->hitIndexes[dlInfo->numIndexes+2] = c;
			dlInfo->numIndexes += 3;
		}

		backEnd.pc.c_totalIndexes += dlInfo->numIndexes;
		backEnd.pc.c_dlightIndexes += dlInfo->numIndexes;
	}
}

/*
==============
RB_BeginSurface

We must set some things up before beginning any tesselation,
because a surface may be forced to perform a RB_End due
to overflow.
==============
*/
void RB_BeginSurface( shader_t *shader, int fogNum ) {

	shader_t *state = (shader->remappedShader) ? shader->remappedShader : shader;

	tess.numIndexes = 0;
	tess.numVertexes = 0;
	tess.shader = state;
	tess.fogNum = fogNum;
	tess.dlightBits = 0;		// will be OR'd in by surface functions
	tess.xstages = state->stages;
	tess.numPasses = state->numUnfoggedPasses;
	tess.currentStageIteratorFunc = state->optimalStageIteratorFunc;

	tess.shaderTime = backEnd.refdef.floatTime - tess.shader->timeOffset;
	if (tess.shader->clampTime && tess.shaderTime >= tess.shader->clampTime) {
		tess.shaderTime = tess.shader->clampTime;
	}

    graphicsDriver.BeginTessellate( &tess );
}


/*
===============
ComputeColors
===============
*/
static void ComputeColors( shaderCommands_t* input, int stage, shaderStage_t *pStage )
{
	int		i;

	//
	// rgbGen
	//
	switch ( pStage->rgbGen )
	{
		case CGEN_IDENTITY:
			Com_Memset( input->svars[stage].colors, 0xff, input->numVertexes * 4 );
			break;
		default:
		case CGEN_IDENTITY_LIGHTING:
			Com_Memset( input->svars[stage].colors, tr.identityLightByte, input->numVertexes * 4 );
			break;
		case CGEN_LIGHTING_DIFFUSE:
			RB_CalcDiffuseColor( ( unsigned char * ) input->svars[stage].colors );
			break;
		case CGEN_EXACT_VERTEX:
			Com_Memcpy( input->svars[stage].colors, input->vertexColors, input->numVertexes * sizeof( input->vertexColors[0] ) );
			break;
		case CGEN_CONST:
			for ( i = 0; i < input->numVertexes; i++ ) {
				*(int *)input->svars[stage].colors[i] = *(int *)pStage->constantColor;
			}
			break;
		case CGEN_VERTEX:
			if ( tr.identityLight == 1 )
			{
				Com_Memcpy( input->svars[stage].colors, input->vertexColors, input->numVertexes * sizeof( input->vertexColors[0] ) );
			}
			else
			{
				for ( i = 0; i < input->numVertexes; i++ )
				{
					input->svars[stage].colors[i][0] = input->vertexColors[i][0] * tr.identityLight;
					input->svars[stage].colors[i][1] = input->vertexColors[i][1] * tr.identityLight;
					input->svars[stage].colors[i][2] = input->vertexColors[i][2] * tr.identityLight;
					input->svars[stage].colors[i][3] = input->vertexColors[i][3];
				}
			}
			break;
		case CGEN_ONE_MINUS_VERTEX:
			if ( tr.identityLight == 1 )
			{
				for ( i = 0; i < input->numVertexes; i++ )
				{
					input->svars[stage].colors[i][0] = 255 - input->vertexColors[i][0];
					input->svars[stage].colors[i][1] = 255 - input->vertexColors[i][1];
					input->svars[stage].colors[i][2] = 255 - input->vertexColors[i][2];
				}
			}
			else
			{
				for ( i = 0; i < input->numVertexes; i++ )
				{
					input->svars[stage].colors[i][0] = ( 255 - input->vertexColors[i][0] ) * tr.identityLight;
					input->svars[stage].colors[i][1] = ( 255 - input->vertexColors[i][1] ) * tr.identityLight;
					input->svars[stage].colors[i][2] = ( 255 - input->vertexColors[i][2] ) * tr.identityLight;
				}
			}
			break;
		case CGEN_FOG:
			{
				fog_t		*fog;

				fog = tr.world->fogs + input->fogNum;

				for ( i = 0; i < input->numVertexes; i++ ) {
					* ( int * )&input->svars[stage].colors[i] = fog->colorInt;
				}
			}
			break;
		case CGEN_WAVEFORM:
			RB_CalcWaveColor( &pStage->rgbWave, ( unsigned char * ) input->svars[stage].colors );
			break;
		case CGEN_ENTITY:
			RB_CalcColorFromEntity( ( unsigned char * ) input->svars[stage].colors );
			break;
		case CGEN_ONE_MINUS_ENTITY:
			RB_CalcColorFromOneMinusEntity( ( unsigned char * ) input->svars[stage].colors );
			break;
	}

	//
	// alphaGen
	//
	switch ( pStage->alphaGen )
	{
	case AGEN_SKIP:
		break;
	case AGEN_IDENTITY:
		if ( pStage->rgbGen != CGEN_IDENTITY ) {
			if ( ( pStage->rgbGen == CGEN_VERTEX && tr.identityLight != 1 ) ||
				 pStage->rgbGen != CGEN_VERTEX ) {
				for ( i = 0; i < input->numVertexes; i++ ) {
					input->svars[stage].colors[i][3] = 0xff;
				}
			}
		}
		break;
	case AGEN_CONST:
		if ( pStage->rgbGen != CGEN_CONST ) {
			for ( i = 0; i < input->numVertexes; i++ ) {
				input->svars[stage].colors[i][3] = pStage->constantColor[3];
			}
		}
		break;
	case AGEN_WAVEFORM:
		RB_CalcWaveAlpha( &pStage->alphaWave, ( unsigned char * ) input->svars[stage].colors );
		break;
	case AGEN_LIGHTING_SPECULAR:
		RB_CalcSpecularAlpha( ( unsigned char * ) input->svars[stage].colors );
		break;
	case AGEN_ENTITY:
		RB_CalcAlphaFromEntity( ( unsigned char * ) input->svars[stage].colors );
		break;
	case AGEN_ONE_MINUS_ENTITY:
		RB_CalcAlphaFromOneMinusEntity( ( unsigned char * ) input->svars[stage].colors );
		break;
    case AGEN_VERTEX:
		if ( pStage->rgbGen != CGEN_VERTEX ) {
			for ( i = 0; i < input->numVertexes; i++ ) {
				input->svars[stage].colors[i][3] = input->vertexColors[i][3];
			}
		}
        break;
    case AGEN_ONE_MINUS_VERTEX:
        for ( i = 0; i < input->numVertexes; i++ )
        {
			input->svars[stage].colors[i][3] = 255 - input->vertexColors[i][3];
        }
        break;
	case AGEN_PORTAL:
		{
			unsigned char alpha;

			for ( i = 0; i < input->numVertexes; i++ )
			{
				float len;
				vec3_t v;

				VectorSubtract( input->xyz[i], backEnd.viewParms.or.origin, v );
				len = VectorLength( v );

				len /= input->shader->portalRange;

				if ( len < 0 )
				{
					alpha = 0;
				}
				else if ( len > 1 )
				{
					alpha = 0xff;
				}
				else
				{
					alpha = len * 0xff;
				}

				input->svars[stage].colors[i][3] = alpha;
			}
		}
		break;
	}

	//
	// fog adjustment for colors to fade out as fog increases
	//
	if ( input->fogNum )
	{
		switch ( pStage->adjustColorsForFog )
		{
		case ACFF_MODULATE_RGB:
			RB_CalcModulateColorsByFog( ( unsigned char * ) input->svars[stage].colors );
			break;
		case ACFF_MODULATE_ALPHA:
			RB_CalcModulateAlphasByFog( ( unsigned char * ) input->svars[stage].colors );
			break;
		case ACFF_MODULATE_RGBA:
			RB_CalcModulateRGBAsByFog( ( unsigned char * ) input->svars[stage].colors );
			break;
		case ACFF_NONE:
			break;
		}
	}
}

/*
===============
ComputeTexCoords
===============
*/
static void ComputeTexCoords( shaderCommands_t* input, int stage, shaderStage_t *pStage ) {
	int		i;
	int		b;

	for ( b = 0; b < NUM_TEXTURE_BUNDLES; b++ ) {
		int tm;

		//
		// generate the texture coordinates
		//
		switch ( pStage->bundle[b].tcGen )
		{
		case TCGEN_IDENTITY:
			Com_Memset( input->svars[stage].texcoords[b], 0, sizeof( float ) * 2 * input->numVertexes );
			break;
		case TCGEN_TEXTURE:
			for ( i = 0 ; i < input->numVertexes ; i++ ) {
				input->svars[stage].texcoords[b][i][0] = input->texCoords[i][0][0];
				input->svars[stage].texcoords[b][i][1] = input->texCoords[i][0][1];
			}
			break;
		case TCGEN_LIGHTMAP:
			for ( i = 0 ; i < input->numVertexes ; i++ ) {
				input->svars[stage].texcoords[b][i][0] = input->texCoords[i][1][0];
				input->svars[stage].texcoords[b][i][1] = input->texCoords[i][1][1];
			}
			break;
		case TCGEN_VECTOR:
			for ( i = 0 ; i < input->numVertexes ; i++ ) {
				input->svars[stage].texcoords[b][i][0] = DotProduct( input->xyz[i], pStage->bundle[b].tcGenVectors[0] );
				input->svars[stage].texcoords[b][i][1] = DotProduct( input->xyz[i], pStage->bundle[b].tcGenVectors[1] );
			}
			break;
		case TCGEN_FOG:
			RB_CalcFogTexCoords( ( float * ) input->svars[stage].texcoords[b] );
			break;
		case TCGEN_ENVIRONMENT_MAPPED:
			RB_CalcEnvironmentTexCoords( ( float * ) input->svars[stage].texcoords[b] );
			break;
		case TCGEN_BAD:
			return;
		}

		//
		// alter texture coordinates
		//
		for ( tm = 0; tm < pStage->bundle[b].numTexMods ; tm++ ) {
			switch ( pStage->bundle[b].texMods[tm].type )
			{
			case TMOD_NONE:
				tm = TR_MAX_TEXMODS;		// break out of for loop
				break;

			case TMOD_TURBULENT:
				RB_CalcTurbulentTexCoords( &pStage->bundle[b].texMods[tm].wave, 
						                 ( float * ) input->svars[stage].texcoords[b] );
				break;

			case TMOD_ENTITY_TRANSLATE:
				RB_CalcScrollTexCoords( backEnd.currentEntity->e.shaderTexCoord,
									 ( float * ) input->svars[stage].texcoords[b] );
				break;

			case TMOD_SCROLL:
				RB_CalcScrollTexCoords( pStage->bundle[b].texMods[tm].scroll,
										 ( float * ) input->svars[stage].texcoords[b] );
				break;

			case TMOD_SCALE:
				RB_CalcScaleTexCoords( pStage->bundle[b].texMods[tm].scale,
									 ( float * ) input->svars[stage].texcoords[b] );
				break;
			
			case TMOD_STRETCH:
				RB_CalcStretchTexCoords( &pStage->bundle[b].texMods[tm].wave, 
						               ( float * ) input->svars[stage].texcoords[b] );
				break;

			case TMOD_TRANSFORM:
				RB_CalcTransformTexCoords( &pStage->bundle[b].texMods[tm],
						                 ( float * ) input->svars[stage].texcoords[b] );
				break;

			case TMOD_ROTATE:
				RB_CalcRotateTexCoords( pStage->bundle[b].texMods[tm].rotateSpeed,
										( float * ) input->svars[stage].texcoords[b] );
				break;

			default:
				ri.Error( ERR_DROP, "ERROR: unknown texmod '%d' in shader '%s'\n", pStage->bundle[b].texMods[tm].type, input->shader->name );
				break;
			}
		}
	}
}


/*
** RB_StageIteratorGeneric
*/
void RB_StageIteratorGeneric( void )
{
    int stage;

	RB_DeformTessGeometry();

	for ( stage = 0; stage < MAX_SHADER_STAGES; stage++ )
	{
		shaderStage_t *pStage = tess.xstages[stage];

		if ( !pStage )
		{
			break;
		}

		ComputeColors( &tess, stage, pStage );
		ComputeTexCoords( &tess, stage, pStage );
    }

    GLR_ComputeDlightProjection( &tess, 0 ); 

    graphicsDriver.DrawStageGeneric( &tess );
}

/*
** RB_StageIteratorVertexLitTexture
*/
void RB_StageIteratorVertexLitTexture( void )
{
	RB_CalcDiffuseColor( ( unsigned char * ) tess.svars[0].colors );

    GLR_ComputeDlightProjection( &tess, 0 ); 

    graphicsDriver.DrawStageVertexLitTexture( &tess );
}

/*
** RB_StageIteratorLightmappedMultitexture
*/
void RB_StageIteratorLightmappedMultitexture( void ) {

    GLR_ComputeDlightProjection( &tess, 0 ); 

    graphicsDriver.DrawStageLightmappedMultitexture( &tess );
}

/*
** RB_EndSurface
*/
void RB_EndSurface( void ) {
	shaderCommands_t *input;

	input = &tess;

    graphicsDriver.EndTessellate( input );

	if (input->numIndexes == 0) {
		return;
	}

	if (input->indexes[SHADER_MAX_INDEXES-1] != 0) {
		ri.Error (ERR_DROP, "RB_EndSurface() - SHADER_MAX_INDEXES hit");
	}	
	if (input->xyz[SHADER_MAX_VERTEXES-1][0] != 0) {
		ri.Error (ERR_DROP, "RB_EndSurface() - SHADER_MAX_VERTEXES hit");
	}

	if ( tess.shader == tr.shadowShader ) {
		RB_ShadowTessEnd();
		return;
	}

	// for debugging of sort order issues, stop rendering after a given sort value
	if ( r_debugSort->integer && r_debugSort->integer < tess.shader->sort ) {
		return;
	}

	//
	// update performance counters
	//
	backEnd.pc.c_shaders++;
	backEnd.pc.c_vertexes += tess.numVertexes;
	backEnd.pc.c_indexes += tess.numIndexes;
	backEnd.pc.c_totalIndexes += tess.numIndexes * tess.numPasses;

	//
	// call off to shader specific tess end function
	//
	tess.currentStageIteratorFunc();

	//
	// draw debugging stuff
	//
	if ( r_showtris->integer ) {
		graphicsDriver.DebugDrawTris(input);
	}
	if ( r_shownormals->integer ) {
		graphicsDriver.DebugDrawNormals (input);
	}
	// clear shader so we can tell we don't have any unclosed surfaces
	tess.numIndexes = 0;

	GLimp_LogComment( "----------\n" );
}

