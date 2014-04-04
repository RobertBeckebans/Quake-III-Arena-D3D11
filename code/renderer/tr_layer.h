//@pjb : the API abstraction layer

#ifndef __TR_LAYER_H__
#define __TR_LAYER_H__

#include "tr_state.h"

extern const float s_identityMatrix[16];

#ifndef WIN8

// @pjb: whichever graphics driver the user chooses, it'll have pointers to its
// specific utilities
extern void            (* GFX_Shutdown)( void );
extern void            (* GFX_UnbindResources)( void );
extern size_t          (* GFX_LastError)( void );
extern void            (* GFX_ReadPixels)( int x, int y, int width, int height, imageFormat_t requestedFmt, void* dest );
extern void            (* GFX_ReadDepth)( int x, int y, int width, int height, float* dest );
extern void            (* GFX_ReadStencil)( int x, int y, int width, int height, byte* dest );
extern void            (* GFX_CreateImage)( const image_t* image, const byte *pic, qboolean isLightmap );
extern void            (* GFX_DeleteImage)( const image_t* image );
extern void            (* GFX_UpdateCinematic)( const image_t* image, const byte* pic, int cols, int rows, qboolean dirty );
extern void            (* GFX_DrawImage)( const image_t* image, const float* coords, const float* texcoords, const float* color );
extern imageFormat_t   (* GFX_GetImageFormat)( const image_t* image );
extern void            (* GFX_SetGamma)( unsigned char red[256], unsigned char green[256], unsigned char blue[256] );
extern int             (* GFX_GetFrameImageMemoryUsage)( void );
extern void            (* GFX_GraphicsInfo)( void );
extern void            (* GFX_Clear)( unsigned long bits, const float* clearCol, unsigned long stencil, float depth );
extern void            (* GFX_SetProjectionMatrix)( const float* projMatrix );
extern void            (* GFX_GetProjectionMatrix)( float* projMatrix );
extern void            (* GFX_SetModelViewMatrix)( const float* modelViewMatrix );
extern void            (* GFX_GetModelViewMatrix)( float* modelViewMatrix );
extern void            (* GFX_SetViewport)( int left, int top, int width, int height );
extern void            (* GFX_Flush)( void );
extern void            (* GFX_SetState)( unsigned long stateMask ); // Use GLS_* flags in tr_state.h
extern void            (* GFX_ResetState2D)( void );
extern void            (* GFX_ResetState3D)( void );
extern void            (* GFX_SetPortalRendering)( qboolean enabled, const float* flipMatrix, const float* plane );
extern void            (* GFX_SetDepthRange)( float minRange, float maxRange );
extern void            (* GFX_SetDrawBuffer)( int buffer );
extern void            (* GFX_EndFrame)( void );
extern void            (* GFX_MakeCurrent)( qboolean current );
extern void            (* GFX_ShadowSilhouette)( const float* edges, int edgeCount );
extern void            (* GFX_ShadowFinish)( void );
extern void            (* GFX_DrawSkyBox)( const skyboxDrawInfo_t* skybox, const float* eye_origin, const float* colorTint );
extern void            (* GFX_DrawBeam)( const image_t* image, const float* color, const vec3_t startPoints[], const vec3_t endPoints[], int segs );
extern void            (* GFX_DrawStageGeneric)( const shaderCommands_t *input );
extern void            (* GFX_DrawStageVertexLitTexture)( const shaderCommands_t *input );
extern void            (* GFX_DrawStageLightmappedMultitexture)( const shaderCommands_t *input );
extern void            (* GFX_BeginTessellate)( const shaderCommands_t* input );
extern void            (* GFX_EndTessellate)( const shaderCommands_t* input );
extern void            (* GFX_DebugDrawAxis)( void );
extern void            (* GFX_DebugDrawNormals)( const shaderCommands_t *input );
extern void            (* GFX_DebugDrawTris)( const shaderCommands_t *input );
extern void            (* GFX_DebugSetOverdrawMeasureEnabled)( qboolean enabled );
extern void            (* GFX_DebugSetTextureMode)( const char* mode );
extern void            (* GFX_DebugDrawPolygon)( int color, int numPoints, const float* points );

#else

// On fixed platforms we can point this straight at D3D functions.
#include "../d3d11/d3d_driver.h"

#define GFX_Shutdown                         D3DDrv_Shutdown                 
#define GFX_UnbindResources                  D3DDrv_UnbindResources
#define GFX_LastError                        D3DDrv_LastError
#define GFX_ReadPixels                       D3DDrv_ReadPixels
#define GFX_ReadDepth                        D3DDrv_ReadDepth
#define GFX_ReadStencil                      D3DDrv_ReadStencil
#define GFX_CreateImage                      D3DDrv_CreateImage
#define GFX_DeleteImage                      D3DDrv_DeleteImage
#define GFX_UpdateCinematic                  D3DDrv_UpdateCinematic
#define GFX_DrawImage                        D3DDrv_DrawImage
#define GFX_GetImageFormat                   D3DDrv_GetImageFormat
#define GFX_SetGamma                         D3DDrv_SetGamma
#define GFX_GetFrameImageMemoryUsage         D3DDrv_SumOfUsedImages
#define GFX_GraphicsInfo                     D3DDrv_GfxInfo
#define GFX_Clear                            D3DDrv_Clear
#define GFX_SetProjectionMatrix              D3DDrv_SetProjection
#define GFX_GetProjectionMatrix              D3DDrv_GetProjection
#define GFX_SetModelViewMatrix               D3DDrv_SetModelView
#define GFX_GetModelViewMatrix               D3DDrv_GetModelView
#define GFX_SetViewport                      D3DDrv_SetViewport
#define GFX_Flush                            D3DDrv_Flush
#define GFX_SetState                         D3DDrv_SetState
#define GFX_ResetState2D                     D3DDrv_ResetState2D
#define GFX_ResetState3D                     D3DDrv_ResetState3D
#define GFX_SetPortalRendering               D3DDrv_SetPortalRendering
#define GFX_SetDepthRange                    D3DDrv_SetDepthRange
#define GFX_SetDrawBuffer                    D3DDrv_SetDrawBuffer
#define GFX_EndFrame                         D3DDrv_EndFrame
#define GFX_MakeCurrent                      D3DDrv_MakeCurrent
#define GFX_ShadowSilhouette                 D3DDrv_ShadowSilhouette
#define GFX_ShadowFinish                     D3DDrv_ShadowFinish
#define GFX_DrawSkyBox                       D3DDrv_DrawSkyBox
#define GFX_DrawBeam                         D3DDrv_DrawBeam
#define GFX_DrawStageGeneric                 D3DDrv_DrawStageGeneric
#define GFX_DrawStageVertexLitTexture        D3DDrv_DrawStageVertexLitTexture
#define GFX_DrawStageLightmappedMultitexture D3DDrv_DrawStageLightmappedMultitexture
#define GFX_BeginTessellate                  D3DDrv_BeginTessellate
#define GFX_EndTessellate                    D3DDrv_EndTessellate
#define GFX_DebugDrawAxis                    D3DDrv_DebugDrawAxis
#define GFX_DebugDrawNormals                 D3DDrv_DebugDrawNormals
#define GFX_DebugDrawTris                    D3DDrv_DebugDrawTris
#define GFX_DebugSetOverdrawMeasureEnabled   D3DDrv_DebugSetOverdrawMeasureEnabled
#define GFX_DebugSetTextureMode              D3DDrv_DebugSetTextureMode
#define GFX_DebugDrawPolygon                 D3DDrv_DebugDrawPolygon

#endif

// @pjb: proxy uses this to validate the proxied drivers
void R_ValidateGraphicsLayer();



#endif
