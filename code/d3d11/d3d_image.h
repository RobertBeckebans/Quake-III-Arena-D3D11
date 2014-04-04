#pragma once

// @pjb: image loading functions

void D3DDrv_CreateImage( const image_t* image, const byte *pic, qboolean isLightmap );
void D3DDrv_DeleteImage( const image_t* image );
void D3DDrv_UpdateCinematic( const image_t* image, const byte* pic, int cols, int rows, qboolean dirty );
imageFormat_t D3DDrv_GetImageFormat( const image_t* image );
int D3DDrv_SumOfUsedImages( void );

