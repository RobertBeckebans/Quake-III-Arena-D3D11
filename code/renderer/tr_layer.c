#include "tr_local.h"
#include "tr_layer.h"

#ifndef WIN8
void            (* GFX_Shutdown)( void ) = NULL;
void            (* GFX_UnbindResources)( void ) = NULL;
size_t          (* GFX_LastError)( void ) = NULL;
void            (* GFX_ReadPixels)( int x, int y, int width, int height, imageFormat_t requestedFmt, void* dest ) = NULL;
void            (* GFX_ReadDepth)( int x, int y, int width, int height, float* dest ) = NULL;
void            (* GFX_ReadStencil)( int x, int y, int width, int height, byte* dest ) = NULL;
void            (* GFX_CreateImage)( const image_t* image, const byte *pic, qboolean isLightmap ) = NULL;
void            (* GFX_DeleteImage)( const image_t* image ) = NULL;
void            (* GFX_UpdateCinematic)( const image_t* image, const byte* pic, int cols, int rows, qboolean dirty ) = NULL;
void            (* GFX_DrawImage)( const image_t* image, const float* coords, const float* texcoords, const float* color ) = NULL;
imageFormat_t   (* GFX_GetImageFormat)( const image_t* image ) = NULL;
void            (* GFX_SetGamma)( unsigned char red[256], unsigned char green[256], unsigned char blue[256] ) = NULL;
int             (* GFX_GetFrameImageMemoryUsage)( void ) = NULL;
void            (* GFX_GraphicsInfo)( void ) = NULL;
void            (* GFX_Clear)( unsigned long bits, const float* clearCol, unsigned long stencil, float depth ) = NULL;
void            (* GFX_SetProjectionMatrix)( const float* projMatrix ) = NULL;
void            (* GFX_GetProjectionMatrix)( float* projMatrix ) = NULL;
void            (* GFX_SetModelViewMatrix)( const float* modelViewMatrix ) = NULL;
void            (* GFX_GetModelViewMatrix)( float* modelViewMatrix ) = NULL;
void            (* GFX_SetViewport)( int left, int top, int width, int height ) = NULL;
void            (* GFX_Flush)( void ) = NULL;
void            (* GFX_SetState)( unsigned long stateMask ) = NULL; // Use GLS_* flags in tr_state.h
void            (* GFX_ResetState2D)( void ) = NULL;
void            (* GFX_ResetState3D)( void ) = NULL;
void            (* GFX_SetPortalRendering)( qboolean enabled, const float* flipMatrix, const float* plane ) = NULL;
void            (* GFX_SetDepthRange)( float minRange, float maxRange ) = NULL;
void            (* GFX_SetDrawBuffer)( int buffer ) = NULL;
void            (* GFX_EndFrame)( void ) = NULL;
void            (* GFX_MakeCurrent)( qboolean current ) = NULL;
void            (* GFX_ShadowSilhouette)( const float* edges, int edgeCount ) = NULL;
void            (* GFX_ShadowFinish)( void ) = NULL;
void            (* GFX_DrawSkyBox)( const skyboxDrawInfo_t* skybox, const float* eye_origin, const float* colorTint ) = NULL;
void            (* GFX_DrawBeam)( const image_t* image, const float* color, const vec3_t startPoints[], const vec3_t endPoints[], int segs ) = NULL;
void            (* GFX_DrawStageGeneric)( const shaderCommands_t *input ) = NULL;
void            (* GFX_DrawStageVertexLitTexture)( const shaderCommands_t *input ) = NULL;
void            (* GFX_DrawStageLightmappedMultitexture)( const shaderCommands_t *input ) = NULL;
void            (* GFX_BeginTessellate)( const shaderCommands_t* input ) = NULL;
void            (* GFX_EndTessellate)( const shaderCommands_t* input ) = NULL;
void            (* GFX_DebugDrawAxis)( void ) = NULL;
void            (* GFX_DebugDrawNormals)( const shaderCommands_t *input ) = NULL;
void            (* GFX_DebugDrawTris)( const shaderCommands_t *input ) = NULL;
void            (* GFX_DebugSetOverdrawMeasureEnabled)( qboolean enabled ) = NULL;
void            (* GFX_DebugSetTextureMode)( const char* mode ) = NULL;
void            (* GFX_DebugDrawPolygon)( int color, int numPoints, const float* points ) = NULL;
#endif
