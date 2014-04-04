#include "tr_local.h"
#include "qgl.h"
#include "gl_common.h"
#include "gl_image.h"

glconfig_t	glConfig;
glstate_t	glState;

void ( APIENTRY * qglMultiTexCoord2fARB )( GLenum texture, GLfloat s, GLfloat t );
void ( APIENTRY * qglActiveTextureARB )( GLenum texture );
void ( APIENTRY * qglClientActiveTextureARB )( GLenum texture );

void ( APIENTRY * qglLockArraysEXT)( GLint, GLint);
void ( APIENTRY * qglUnlockArraysEXT) ( void );


/*
=============
RE_EndRegistration

Touch all images to make sure they are resident
=============
*/
void RE_EndRegistration( void ) {
	R_SyncRenderThread();
	if (!Sys_LowPhysicalMemory()) {
		GLRB_ShowImages();
	}
}

void GLRB_GetExports( refexport_t* re )
{
	re->Shutdown = RE_Shutdown;

	re->BeginRegistration = RE_BeginRegistration;
	re->RegisterModel = RE_RegisterModel;
	re->RegisterSkin = RE_RegisterSkin;
	re->RegisterShader = RE_RegisterShader;
	re->RegisterShaderNoMip = RE_RegisterShaderNoMip;
	re->LoadWorld = RE_LoadWorldMap;
	re->SetWorldVisData = RE_SetWorldVisData;
	re->EndRegistration = RE_EndRegistration;

	re->BeginFrame = RE_BeginFrame;
	re->EndFrame = RE_EndFrame;

	re->MarkFragments = R_MarkFragments;
	re->LerpTag = R_LerpTag;
	re->ModelBounds = R_ModelBounds;

	re->ClearScene = RE_ClearScene;
	re->AddRefEntityToScene = RE_AddRefEntityToScene;
	re->AddPolyToScene = RE_AddPolyToScene;
	re->LightForPoint = R_LightForPoint;
	re->AddLightToScene = RE_AddLightToScene;
	re->AddAdditiveLightToScene = RE_AddAdditiveLightToScene;
	re->RenderScene = RE_RenderScene;

	re->SetColor = RE_SetColor;
	re->DrawStretchPic = RE_StretchPic;
	re->DrawStretchRaw = RE_StretchRaw;
	re->UploadCinematic = RE_UploadCinematic;

	re->RegisterFont = RE_RegisterFont;
	re->RemapShader = R_RemapShader;
	re->GetEntityToken = R_GetEntityToken;
	re->inPVS = R_inPVS;
}

void GLRB_RestoreTextureState( void )
{
    // Flush textures; reset texture state
	Com_Memset( glState.currenttextures, 0, sizeof( glState.currenttextures ) );
	if ( qglBindTexture ) {
		if ( qglActiveTextureARB ) {
			GL_SelectTexture( 1 );
			qglBindTexture( GL_TEXTURE_2D, 0 );
			GL_SelectTexture( 0 );
			qglBindTexture( GL_TEXTURE_2D, 0 );
		} else {
			qglBindTexture( GL_TEXTURE_2D, 0 );
		}
	}


}

void GLRB_SetGamma( unsigned char red[256], unsigned char green[256], unsigned char blue[256] )
{
    if ( glConfig.deviceSupportsGamma )
	{
        GLimp_SetGamma( red, green, blue );
    }
}

/*
@@@@@@@@@@@@@@@@@@@@@

Returns the opengl graphics driver and sets up global state

@@@@@@@@@@@@@@@@@@@@@
*/
graphicsDriver_t* GLRB_DriverInit( void )
{
    static graphicsDriver_t openglDriver = {
        GL_CreateImage,
        GL_DeleteImage,
        GL_GetImageFormat,
        GLRB_SetGamma,
        GL_SumOfUsedImages
    };

    return &openglDriver;
}
