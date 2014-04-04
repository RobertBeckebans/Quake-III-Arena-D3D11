//@pjb : the API abstraction layer

#ifndef __TR_LAYER_H__
#define __TR_LAYER_H__

#include "tr_state.h"

extern const float s_identityMatrix[16];

// @pjb: whichever graphics driver the user chooses, it'll have pointers to its
// specific utilities
typedef struct graphicsApiLayer_s {
    void            (* Shutdown)( void );
    void            (* UnbindResources)( void );
    size_t          (* LastError)( void );
    void            (* ReadPixels)( int x, int y, int width, int height, imageFormat_t requestedFmt, void* dest );
    void            (* ReadDepth)( int x, int y, int width, int height, float* dest );
    void            (* ReadStencil)( int x, int y, int width, int height, byte* dest );
    void            (* CreateImage)( const image_t* image, const byte *pic, qboolean isLightmap );
    void            (* DeleteImage)( const image_t* image );
    void            (* UpdateCinematic)( const image_t* image, const byte* pic, int cols, int rows, qboolean dirty );
    void            (* DrawImage)( const image_t* image, const float* coords, const float* texcoords, const float* color );
    imageFormat_t   (* GetImageFormat)( const image_t* image );
    void            (* SetGamma)( unsigned char red[256], unsigned char green[256], unsigned char blue[256] );
    int             (* GetFrameImageMemoryUsage)( void );
    void            (* GraphicsInfo)( void );
    void            (* Clear)( unsigned long bits, const float* clearCol, unsigned long stencil, float depth );
    void            (* SetProjectionMatrix)( const float* projMatrix );
    void            (* GetProjectionMatrix)( float* projMatrix );
    void            (* SetModelViewMatrix)( const float* modelViewMatrix );
    void            (* GetModelViewMatrix)( float* modelViewMatrix );
    void            (* SetViewport)( int left, int top, int width, int height );
    void            (* Flush)( void );
    void            (* SetState)( unsigned long stateMask ); // Use GLS_* flags in tr_state.h
    void            (* ResetState2D)( void );
    void            (* ResetState3D)( void );
    void            (* SetPortalRendering)( qboolean enabled, const float* flipMatrix, const float* plane );
    void            (* SetDepthRange)( float minRange, float maxRange );
    void            (* SetDrawBuffer)( int buffer );
    void            (* EndFrame)( void );
    void            (* MakeCurrent)( qboolean current );
    void            (* ShadowSilhouette)( const float* edges, int edgeCount );
    void            (* ShadowFinish)( void );
    void            (* DrawSkyBox)( const skyboxDrawInfo_t* skybox, const float* eye_origin, const float* colorTint );
    void            (* DrawBeam)( const image_t* image, const float* color, const vec3_t startPoints[], const vec3_t endPoints[], int segs );
    void            (* DrawStageGeneric)( const shaderCommands_t *input );
    void            (* DrawStageVertexLitTexture)( const shaderCommands_t *input );
    void            (* DrawStageLightmappedMultitexture)( const shaderCommands_t *input );
    void            (* BeginTessellate)( const shaderCommands_t* input );
    void            (* EndTessellate)( const shaderCommands_t* input );

    void            (* DebugDrawAxis)( void );
    void            (* DebugDrawNormals)( const shaderCommands_t *input );
    void            (* DebugDrawTris)( const shaderCommands_t *input );
    void            (* DebugSetOverdrawMeasureEnabled)( qboolean enabled );
    void            (* DebugSetTextureMode)( const char* mode );
    void            (* DebugDrawPolygon)( int color, int numPoints, const float* points );
} graphicsApiLayer_t;

extern graphicsApiLayer_t      graphicsDriver;

// @pjb: proxy uses this to validate the proxied drivers
void R_ValidateGraphicsLayer( graphicsApiLayer_t* layer );



#endif
