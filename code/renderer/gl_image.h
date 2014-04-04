#ifndef __GL_IMAGE_H__
#define __GL_IMAGE_H__

typedef struct image_s image_t;

void GL_CreateImage( const image_t* image, const byte *pic, qboolean isLightmap );
void GL_DeleteImage( const image_t* image );

imageFormat_t GL_GetImageFormat( const image_t* image );
int GL_SumOfUsedImages( void );

void GL_SetImageBorderColor( const image_t* image, const float* borderColor );


#endif
