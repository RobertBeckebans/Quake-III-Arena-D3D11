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

typedef struct graphicsApiLayer_s {
    __forceinline void            GFX_Shutdown( void );
    __forceinline void            GFX_UnbindResources( void );
    __forceinline size_t          GFX_LastError( void );
    __forceinline void            GFX_ReadPixels( int x, int y, int width, int height, imageFormat_t requestedFmt, void* dest );
    __forceinline void            GFX_ReadDepth( int x, int y, int width, int height, float* dest );
    __forceinline void            GFX_ReadStencil( int x, int y, int width, int height, byte* dest );
    __forceinline void            GFX_CreateImage( const image_t* image, const byte *pic, qboolean isLightmap );
    __forceinline void            GFX_DeleteImage( const image_t* image );
    __forceinline void            GFX_UpdateCinematic( const image_t* image, const byte* pic, int cols, int rows, qboolean dirty );
    __forceinline void            GFX_DrawImage( const image_t* image, const float* coords, const float* texcoords, const float* color );
    __forceinline imageFormat_t   GFX_GetImageFormat( const image_t* image );
    __forceinline void            GFX_SetGamma( unsigned char red[256], unsigned char green[256], unsigned char blue[256] );
    __forceinline int             GFX_GetFrameImageMemoryUsage( void );
    __forceinline void            GFX_GraphicsInfo( void );
    __forceinline void            GFX_Clear( unsigned long bits, const float* clearCol, unsigned long stencil, float depth );
    __forceinline void            GFX_SetProjectionMatrix( const float* projMatrix );
    __forceinline void            GFX_GetProjectionMatrix( float* projMatrix );
    __forceinline void            GFX_SetModelViewMatrix( const float* modelViewMatrix );
    __forceinline void            GFX_GetModelViewMatrix( float* modelViewMatrix );
    __forceinline void            GFX_SetViewport( int left, int top, int width, int height );
    __forceinline void            GFX_Flush( void );
    __forceinline void            GFX_SetState( unsigned long stateMask ); // Use GLS_* flags in tr_state.h
    __forceinline void            GFX_ResetState2D( void );
    __forceinline void            GFX_ResetState3D( void );
    __forceinline void            GFX_SetPortalRendering( qboolean enabled, const float* flipMatrix, const float* plane );
    __forceinline void            GFX_SetDepthRange( float minRange, float maxRange );
    __forceinline void            GFX_SetDrawBuffer( int buffer );
    __forceinline void            GFX_EndFrame( void );
    __forceinline void            GFX_MakeCurrent( qboolean current );
    __forceinline void            GFX_ShadowSilhouette( const float* edges, int edgeCount );
    __forceinline void            GFX_ShadowFinish( void );
    __forceinline void            GFX_DrawSkyBox( const skyboxDrawInfo_t* skybox, const float* eye_origin, const float* colorTint );
    __forceinline void            GFX_DrawBeam( const image_t* image, const float* color, const vec3_t startPoints[], const vec3_t endPoints[], int segs );
    __forceinline void            GFX_DrawStageGeneric( const shaderCommands_t *input );
    __forceinline void            GFX_DrawStageVertexLitTexture( const shaderCommands_t *input );
    __forceinline void            GFX_DrawStageLightmappedMultitexture( const shaderCommands_t *input );
    __forceinline void            GFX_BeginTessellate( const shaderCommands_t* input );
    __forceinline void            GFX_EndTessellate( const shaderCommands_t* input );
    __forceinline void            GFX_DebugDrawAxis( void );
    __forceinline void            GFX_DebugDrawNormals( const shaderCommands_t *input );
    __forceinline void            GFX_DebugDrawTris( const shaderCommands_t *input );
    __forceinline void            GFX_DebugSetOverdrawMeasureEnabled( qboolean enabled );
    __forceinline void            GFX_DebugSetTextureMode( const char* mode );
    __forceinline void            GFX_DebugDrawPolygon( int color, int numPoints, const float* points );
} graphicsApiLayer_t;

#endif

// @pjb: proxy uses this to validate the proxied drivers
void R_ValidateGraphicsLayer();



#endif
