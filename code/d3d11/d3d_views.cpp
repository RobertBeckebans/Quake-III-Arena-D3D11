#include "d3d_common.h"
#include "d3d_views.h"

namespace QD3D
{
	ID3D11RenderTargetView* CreateBackBufferView(IDXGISwapChain* swapChain, ID3D11Device* device, D3D11_TEXTURE2D_DESC* opt_desc)
	{
		ID3D11Texture2D* back_buffer = NULL;
		GetBackBuffer(swapChain, &back_buffer);

		ID3D11RenderTargetView* view = NULL;

		D3D11_TEXTURE2D_DESC bbDesc;
		back_buffer->GetDesc(&bbDesc);
		device->CreateRenderTargetView(back_buffer, NULL, &view);
		back_buffer->Release();

		if (opt_desc) { *opt_desc = bbDesc; }
		
		return view;
	}

	ID3D11DepthStencilView* CreateDepthBufferView(
		ID3D11Device* device, 
		UINT width, 
		UINT height, 
		DXGI_FORMAT t2d_format,
		DXGI_FORMAT dsv_format,
		UINT sampleCount, 
		UINT quality,
		UINT bindFlags)
	{
		ID3D11Texture2D* depth_buffer = NULL;
		{
			D3D11_TEXTURE2D_DESC dsd;
			ZeroMemory(&dsd, sizeof(dsd));

			dsd.Width = width;
			dsd.Height = height;
			dsd.MipLevels = 1;
			dsd.ArraySize = 1;
			dsd.Format = t2d_format;
			dsd.SampleDesc.Count = sampleCount;
			dsd.SampleDesc.Quality = quality;
			dsd.Usage = D3D11_USAGE_DEFAULT;
			dsd.BindFlags = D3D11_BIND_DEPTH_STENCIL | bindFlags;
		
			device->CreateTexture2D(&dsd, NULL, &depth_buffer);

			ASSERT(depth_buffer);
		}

		ID3D11DepthStencilView* ds_view = NULL;
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC dsd;
			ZeroMemory(&dsd, sizeof(dsd));

			dsd.Format = dsv_format;
			dsd.ViewDimension = sampleCount > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
			dsd.Texture2D.MipSlice = 0;

			device->CreateDepthStencilView(depth_buffer, &dsd, &ds_view);
			depth_buffer->Release();
		}

		return ds_view;
	}

	ID3D11RenderTargetView* CreateRenderTargetView(
		_In_ ID3D11Device* device, 
		UINT width,
		UINT height, 
		DXGI_FORMAT t2d_format, 
		DXGI_FORMAT rtv_format, 
		UINT sampleCount, 
		UINT quality, 
		UINT bindFlags)
	{
		ID3D11Texture2D* texture = NULL;
		{
			D3D11_TEXTURE2D_DESC dsd;
			ZeroMemory(&dsd, sizeof(dsd));

			dsd.Width = width;
			dsd.Height = height;
			dsd.MipLevels = 1;
			dsd.ArraySize = 1;
			dsd.Format = t2d_format;
			dsd.SampleDesc.Count = sampleCount;
			dsd.SampleDesc.Quality = quality;
			dsd.Usage = D3D11_USAGE_DEFAULT;
			dsd.BindFlags = D3D11_BIND_RENDER_TARGET | bindFlags;
		
			device->CreateTexture2D(&dsd, NULL, &texture);

			ASSERT(texture);
		}

		ID3D11RenderTargetView* rt_view = NULL;
		{
			D3D11_RENDER_TARGET_VIEW_DESC rtvd;
			ZeroMemory(&rtvd, sizeof(rtvd));

			rtvd.Format = rtv_format;
			rtvd.ViewDimension = sampleCount > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvd.Texture2D.MipSlice = 0;

			device->CreateRenderTargetView(texture, &rtvd, &rt_view);
			texture->Release();
		}

		return rt_view;
	}

	ID3D11RenderTargetView* CreateTexture2DRenderTargetView(
		_In_ ID3D11Device* device, 
		ID3D11Texture2D* texture,
		DXGI_FORMAT rtv_format)
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvd;
		ZeroMemory(&rtvd, sizeof(rtvd));

		D3D11_TEXTURE2D_DESC t2dd;
		texture->GetDesc(&t2dd);

		rtvd.Format = rtv_format;
		rtvd.ViewDimension = t2dd.SampleDesc.Count > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvd.Texture2D.MipSlice = 0;

		ID3D11RenderTargetView* rt_view = NULL;
		device->CreateRenderTargetView(texture, &rtvd, &rt_view);

		return rt_view;
	}

	ID3D11DepthStencilView* CreateTexture2DDepthBufferView(
		ID3D11Device* device, 
		ID3D11Texture2D* depth_buffer,
		DXGI_FORMAT dsv_format)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dsd;
		ZeroMemory(&dsd, sizeof(dsd));

		D3D11_TEXTURE2D_DESC t2dd;
		depth_buffer->GetDesc(&t2dd);

		dsd.Format = dsv_format;
		dsd.ViewDimension = t2dd.SampleDesc.Count > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
		dsd.Texture2D.MipSlice = 0;

		ID3D11DepthStencilView* ds_view = NULL;
		device->CreateDepthStencilView(depth_buffer, &dsd, &ds_view);

		return ds_view;
	}

	ID3D11ShaderResourceView* CreateTexture2DShaderResourceView(_In_ ID3D11Device* device, _In_ ID3D11Texture2D* texture, DXGI_FORMAT srv_format)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		ZeroMemory(&srvd, sizeof(srvd));

		D3D11_TEXTURE2D_DESC t2dd;
		texture->GetDesc(&t2dd);

		srvd.Format = srv_format;
		srvd.ViewDimension = t2dd.SampleDesc.Count > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MipLevels = t2dd.MipLevels;
		srvd.Texture2D.MostDetailedMip = 0;

		ID3D11ShaderResourceView* view = NULL;
		device->CreateShaderResourceView(texture, &srvd, &view);
		return view;
	}

}
