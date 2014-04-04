#ifndef __D3D_WIN8_H__
#define __D3D_WIN8_H__

void D3DWin8_Init();
void D3DWin8_Shutdown();

void D3DWin8_InitDeferral();
void D3DWin8_NotifyWindowResize( int logicalSizeX, int logicalSizeY );

#endif
