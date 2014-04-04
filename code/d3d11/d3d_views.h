#ifndef __D3D_VIEWS_H__
#define __D3D_VIEWS_H__

#ifndef __cplusplus
#   error This must only be inluded from C++ source files.
#endif

namespace QD3D
{
	//----------------------------------------------------------------------------
    // Helpers for creating resource views.
	//----------------------------------------------------------------------------
	ID3D11RenderTargetView* CreateBackBufferView(_In_ IDXGISwapChain* swapChain, _In_ ID3D11Device* device, _Out_opt_ D3D11_TEXTURE2D_DESC* optionalOut_BBDesc = NULL);
	ID3D11DepthStencilView* CreateDepthBufferView(_In_ ID3D11Device* device, UINT width, UINT height, DXGI_FORMAT t2d_format, DXGI_FORMAT dsv_format, UINT msaaSamples = 1, UINT msaaQuality = 0, UINT bindFlags = 0);
	ID3D11RenderTargetView* CreateRenderTargetView(_In_ ID3D11Device* device, UINT width, UINT height, DXGI_FORMAT t2d_format, DXGI_FORMAT rtv_format, UINT msaaSamples = 1, UINT msaaQuality = 0, UINT bindFlags = 0);

	ID3D11RenderTargetView* CreateTexture2DRenderTargetView(_In_ ID3D11Device* device, _In_ ID3D11Texture2D* texture, DXGI_FORMAT rtv_format);
	ID3D11DepthStencilView* CreateTexture2DDepthBufferView(_In_ ID3D11Device* device, _In_ ID3D11Texture2D* texture, DXGI_FORMAT dsv_format);
	ID3D11ShaderResourceView* CreateTexture2DShaderResourceView(_In_ ID3D11Device* device, _In_ ID3D11Texture2D* texture, DXGI_FORMAT srv_format);
}

#endif
