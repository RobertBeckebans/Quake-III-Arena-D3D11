#include "d3d_common.h"

namespace QD3D
{
	//----------------------------------------------------------------------------
	// Creates a device with the default settings and returns the maximum feature 
	// level
	//----------------------------------------------------------------------------
	HRESULT 
	CreateDefaultDevice(
		_In_ D3D_DRIVER_TYPE driver, 
		_Out_ ID3D11Device** device,
		_Out_ ID3D11DeviceContext** context,
		_Out_ D3D_FEATURE_LEVEL* featureLevel)
	{
		*device = NULL;
		*context = NULL;

		UINT flags = 0;
#ifdef _DEBUG
		flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		return D3D11CreateDevice(
					NULL, // TODO: individual adapters
					driver,
					NULL,
					flags,
					NULL,
					0,
					D3D11_SDK_VERSION,
					device,
					featureLevel,
					context);
	}

	//----------------------------------------------------------------------------
	// Creates a default 8-bit swap chain description with no MSAA
	//----------------------------------------------------------------------------
	void
	GetDefaultSwapChainDesc(
		_In_ HWND hWnd,
		_Out_ DXGI_SWAP_CHAIN_DESC* scd)
	{
		ZeroMemory(scd, sizeof(*scd));

		scd->BufferCount = 1;
		scd->BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scd->BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd->OutputWindow = hWnd;
		scd->SampleDesc.Count = 1;
		scd->Windowed = TRUE;
	}

	//----------------------------------------------------------------------------
	// Creates a multisampled swap chain description.
	// Automatically selects the highest possible quality level for the MSAA.
	//----------------------------------------------------------------------------
	HRESULT 
	GetMultiSampledSwapChainDesc(
		_In_ HWND hWnd, 
		_In_ ID3D11Device* device,
		_In_ UINT msaaSamples,
		_Out_ DXGI_SWAP_CHAIN_DESC* out)
	{
		ZeroMemory(out, sizeof(*out));

		const DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

		UINT qualityLevels = 0;
		HRESULT hr = device->CheckMultisampleQualityLevels(format, msaaSamples, &qualityLevels);
		if (FAILED(hr))
		{
			return FALSE;
		}

		if (!qualityLevels)
		{
			return E_FAIL;
		}

		out->BufferCount = 1;
		out->BufferDesc.Format = format;
		out->BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		out->OutputWindow = hWnd;
		out->SampleDesc.Count = msaaSamples;
		out->SampleDesc.Quality = 0;
		out->Windowed = TRUE;

		return S_OK;
	}

	//----------------------------------------------------------------------------
	// Returns the highest possible quality swap chain description, starting with
	// full MSAA and working downwards until the driver accepts the input values.
	// 
	// This will try 16x MSAA, 8x, 4x, 2x and no MSAA. It is guaranteed to return
	// a valid swap chain description.
	//----------------------------------------------------------------------------
	void 
	GetBestQualitySwapChainDesc(
		_In_ HWND hWnd, 
		_In_ ID3D11Device* device,
		_Out_ DXGI_SWAP_CHAIN_DESC* scd)
	{
		const UINT multiSampleLevels[] = {16, 8, 4, 2, 0};
		UINT multiSampleIndex = 0;
		while (multiSampleLevels[multiSampleIndex] != 0)
		{
			if (SUCCEEDED(GetMultiSampledSwapChainDesc(hWnd, device, multiSampleLevels[multiSampleIndex], scd)))
			{
				break;
			}
			multiSampleIndex++;
		}

		if (multiSampleLevels[multiSampleIndex] == 0)
		{
			GetDefaultSwapChainDesc(hWnd, scd);
		}
	}

	//----------------------------------------------------------------------------
	// Creates a swap chain
	//----------------------------------------------------------------------------
	HRESULT
	CreateSwapChain(
		_In_ ID3D11Device* device,
		_In_ DXGI_SWAP_CHAIN_DESC* scd,
		_Out_ IDXGISwapChain** swapChain)
	{
		// Get the factory associated with the device
		IDXGIDevice* dxgiDevice;
		device->QueryInterface(__uuidof(IDXGIDevice), (void **)&dxgiDevice);
		IDXGIAdapter* dxgiAdapter;
		dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&dxgiAdapter);
		IDXGIFactory* dxgiFactory;
		dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&dxgiFactory);
	
		// Create the swap chain
		HRESULT hr = dxgiFactory->CreateSwapChain(device, scd, swapChain);

        SAFE_RELEASE(dxgiDevice);
        SAFE_RELEASE(dxgiAdapter);
        SAFE_RELEASE(dxgiFactory);
        
        return hr;
	}

	//----------------------------------------------------------------------------
	// Extracts the back buffer from the swap chain
	//----------------------------------------------------------------------------
	HRESULT 
	GetBackBuffer(
		_In_ IDXGISwapChain* swapChain,
		_Out_ ID3D11Texture2D** backBufferTexture)
	{
		return swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)backBufferTexture);
	}

	//----------------------------------------------------------------------------
	// Fills a viewport struct for a render target
	//----------------------------------------------------------------------------
	HRESULT
	GetRenderTargetViewport(
		_In_ ID3D11RenderTargetView* pRTV,
		_Out_ D3D11_VIEWPORT* pViewport)
	{
		if (!pRTV || !pViewport)
			return E_INVALIDARG;

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		pRTV->GetDesc(&rtvDesc);

		if (rtvDesc.ViewDimension != D3D11_RTV_DIMENSION_TEXTURE2D &&
			rtvDesc.ViewDimension != D3D11_RTV_DIMENSION_TEXTURE2DMS)
			return E_FAIL;

		ID3D11Texture2D* pTexture = nullptr;
		pRTV->GetResource((ID3D11Resource**) &pTexture);

		D3D11_TEXTURE2D_DESC t2dDesc;
		pTexture->GetDesc(&t2dDesc);

        SAFE_RELEASE(pTexture);

		pViewport->Width = (FLOAT) t2dDesc.Width;
		pViewport->Height = (FLOAT) t2dDesc.Height;

		pViewport->TopLeftX = 0;
		pViewport->TopLeftY = 0;
		pViewport->MinDepth = 0;
		pViewport->MaxDepth = 1;

		return S_OK;
	}
}
