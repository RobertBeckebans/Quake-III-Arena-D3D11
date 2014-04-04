//@pjb : the API abstraction layer

#ifndef __TR_LAYER_H__
#define __TR_LAYER_H__

// @pjb: whichever graphics driver the user chooses, it'll have pointers to its
// specific utilities
typedef struct graphicsApiLayer_s {
    void            (* CreateImage)( const image_t* image, const byte *pic, qboolean isLightmap );
    void            (* DeleteImage)( const image_t* image );
    imageFormat_t   (* GetImageFormat)( const image_t* image );
    void            (* SetGamma)( unsigned char red[256], unsigned char green[256], unsigned char blue[256] );
    int             (* GetFrameImageMemoryUsage)( void );
    void            (* GraphicsInfo)( void );
    void            (* Clear)( float r, float g, float b, float a );
    void            (* SetProjection)( const float* projMatrix );
    void            (* SetViewport)( int left, int top, int width, int height );
    void            (* Flush)( void );
} graphicsApiLayer_t;

extern graphicsApiLayer_t      vdLayer;

// @pjb: proxy uses this to validate the proxied drivers
void R_ValidateGraphicsLayer( graphicsApiLayer_t* layer );



#endif
