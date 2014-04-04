//@pjb : the API abstraction layer

#ifndef __TR_LAYER_H__
#define __TR_LAYER_H__

#include "tr_state.h"

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
    void            (* UpdateCinematic)( image_t* image, const byte* pic, int cols, int rows, qboolean dirty );
    void            (* DrawImage)( const image_t* image, const float* coords, const float* texcoords, const float* color );
    imageFormat_t   (* GetImageFormat)( const image_t* image );
    void            (* SetGamma)( unsigned char red[256], unsigned char green[256], unsigned char blue[256] );
    int             (* GetFrameImageMemoryUsage)( void );
    void            (* GraphicsInfo)( void );
    void            (* Clear)( unsigned long bits, const float* clearCol, unsigned long stencil, float depth );
    void            (* SetProjection)( const float* projMatrix );
    void            (* SetViewport)( int left, int top, int width, int height );
    void            (* Flush)( void );
    void            (* SetState)( unsigned long stateMask ); // Use GLS_* flags in tr_state.h
    void            (* ResetState2D)( void );
    void            (* ResetState3D)( void );
    void            (* SetPortalRendering)( qboolean enabled, const float* flipMatrix, const float* plane );
    void            (* SetModelViewMatrix)( const float* modelViewMatrix );
    void            (* SetDepthRange)( float minRange, float maxRange );
    void            (* SetDrawBuffer)( int buffer );
    void            (* EndFrame)( void );
    void*           (* WaitForCommands)( void );
} graphicsApiLayer_t;

extern graphicsApiLayer_t      graphicsDriver;

// @pjb: proxy uses this to validate the proxied drivers
void R_ValidateGraphicsLayer( graphicsApiLayer_t* layer );



#endif
