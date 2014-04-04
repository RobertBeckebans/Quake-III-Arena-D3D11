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

    //----------------------------------------------------------------------------
    // Returns the number of bits in the particular pixel format
   	//----------------------------------------------------------------------------
    HRESULT
    GetBitDepthForFormat(
        _In_ DXGI_FORMAT fmt,
        _Out_ DWORD* bits )
    {
        switch (fmt)
        {
        case DXGI_FORMAT_R32G32B32A32_TYPELESS      :
        case DXGI_FORMAT_R32G32B32A32_FLOAT         :
        case DXGI_FORMAT_R32G32B32A32_UINT          :
        case DXGI_FORMAT_R32G32B32A32_SINT          :
            *bits = 128; break;
        case DXGI_FORMAT_R32G32B32_TYPELESS         :
        case DXGI_FORMAT_R32G32B32_FLOAT            :
        case DXGI_FORMAT_R32G32B32_UINT             :
        case DXGI_FORMAT_R32G32B32_SINT             :
            *bits = 96; break;
        case DXGI_FORMAT_R16G16B16A16_TYPELESS      :
        case DXGI_FORMAT_R16G16B16A16_FLOAT         :
        case DXGI_FORMAT_R16G16B16A16_UNORM         :
        case DXGI_FORMAT_R16G16B16A16_UINT          :
        case DXGI_FORMAT_R16G16B16A16_SNORM         :
        case DXGI_FORMAT_R16G16B16A16_SINT          :
        case DXGI_FORMAT_R32G32_TYPELESS            :
        case DXGI_FORMAT_R32G32_FLOAT               :
        case DXGI_FORMAT_R32G32_UINT                :
        case DXGI_FORMAT_R32G32_SINT                :
        case DXGI_FORMAT_R32G8X24_TYPELESS          :
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT       :
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS   :
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT    :
            *bits = 64; break;
        case DXGI_FORMAT_R10G10B10A2_TYPELESS       :
        case DXGI_FORMAT_R10G10B10A2_UNORM          :
        case DXGI_FORMAT_R10G10B10A2_UINT           :
        case DXGI_FORMAT_R11G11B10_FLOAT            :
        case DXGI_FORMAT_R8G8B8A8_TYPELESS          :
        case DXGI_FORMAT_R8G8B8A8_UNORM             :
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB        :
        case DXGI_FORMAT_R8G8B8A8_UINT              :
        case DXGI_FORMAT_R8G8B8A8_SNORM             :
        case DXGI_FORMAT_R8G8B8A8_SINT              :
        case DXGI_FORMAT_R16G16_TYPELESS            :
        case DXGI_FORMAT_R16G16_FLOAT               :
        case DXGI_FORMAT_R16G16_UNORM               :
        case DXGI_FORMAT_R16G16_UINT                :
        case DXGI_FORMAT_R16G16_SNORM               :
        case DXGI_FORMAT_R16G16_SINT                :
        case DXGI_FORMAT_R32_TYPELESS               :
        case DXGI_FORMAT_D32_FLOAT                  :
        case DXGI_FORMAT_R32_FLOAT                  :
        case DXGI_FORMAT_R32_UINT                   :
        case DXGI_FORMAT_R32_SINT                   :
        case DXGI_FORMAT_R24G8_TYPELESS             :
        case DXGI_FORMAT_D24_UNORM_S8_UINT          :
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS      :
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT       :
            *bits = 32; break;
        case DXGI_FORMAT_R8G8_TYPELESS              :
        case DXGI_FORMAT_R8G8_UNORM                 :
        case DXGI_FORMAT_R8G8_UINT                  :
        case DXGI_FORMAT_R8G8_SNORM                 :
        case DXGI_FORMAT_R8G8_SINT                  :
        case DXGI_FORMAT_R16_TYPELESS               :
        case DXGI_FORMAT_R16_FLOAT                  :
        case DXGI_FORMAT_D16_UNORM                  :
        case DXGI_FORMAT_R16_UNORM                  :
        case DXGI_FORMAT_R16_UINT                   :
        case DXGI_FORMAT_R16_SNORM                  :
        case DXGI_FORMAT_R16_SINT                   :
            *bits = 16; break;
        case DXGI_FORMAT_R8_TYPELESS                :
        case DXGI_FORMAT_R8_UNORM                   :
        case DXGI_FORMAT_R8_UINT                    :
        case DXGI_FORMAT_R8_SNORM                   :
        case DXGI_FORMAT_R8_SINT                    :
        case DXGI_FORMAT_A8_UNORM                   :
            *bits = 8; break;
        case DXGI_FORMAT_R1_UNORM                   : 
            *bits = 1; break;
        case DXGI_FORMAT_R9G9B9E5_SHAREDEXP         :
        case DXGI_FORMAT_R8G8_B8G8_UNORM            :
        case DXGI_FORMAT_G8R8_G8B8_UNORM            :
            *bits = 32; break;
        case DXGI_FORMAT_BC1_TYPELESS               :
        case DXGI_FORMAT_BC1_UNORM                  :
        case DXGI_FORMAT_BC1_UNORM_SRGB             :
        case DXGI_FORMAT_BC2_TYPELESS               :
        case DXGI_FORMAT_BC2_UNORM                  :
        case DXGI_FORMAT_BC2_UNORM_SRGB             :
        case DXGI_FORMAT_BC3_TYPELESS               :
        case DXGI_FORMAT_BC3_UNORM                  :
        case DXGI_FORMAT_BC3_UNORM_SRGB             :
        case DXGI_FORMAT_BC4_TYPELESS               :
        case DXGI_FORMAT_BC4_UNORM                  :
        case DXGI_FORMAT_BC4_SNORM                  :
        case DXGI_FORMAT_BC5_TYPELESS               :
        case DXGI_FORMAT_BC5_UNORM                  :
        case DXGI_FORMAT_BC5_SNORM                  :
            return E_FAIL; // compressed
        case DXGI_FORMAT_B5G6R5_UNORM               :
        case DXGI_FORMAT_B5G5R5A1_UNORM             :
            *bits = 16; break;
        case DXGI_FORMAT_B8G8R8A8_UNORM             :
        case DXGI_FORMAT_B8G8R8X8_UNORM             :
        case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM :
        case DXGI_FORMAT_B8G8R8A8_TYPELESS          :
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB        :
        case DXGI_FORMAT_B8G8R8X8_TYPELESS          :
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB        :
            *bits = 32; break;
        case DXGI_FORMAT_BC6H_TYPELESS              :
        case DXGI_FORMAT_BC6H_UF16                  :
        case DXGI_FORMAT_BC6H_SF16                  :
        case DXGI_FORMAT_BC7_TYPELESS               :
        case DXGI_FORMAT_BC7_UNORM                  :
        case DXGI_FORMAT_BC7_UNORM_SRGB             :
            return E_FAIL; // compressed
        case DXGI_FORMAT_AYUV                       :
            *bits = 32; break;
        case DXGI_FORMAT_Y410                       :
        case DXGI_FORMAT_Y416                       :
        case DXGI_FORMAT_NV12                       :
        case DXGI_FORMAT_P010                       :
        case DXGI_FORMAT_P016                       :
        case DXGI_FORMAT_420_OPAQUE                 :
            *bits = 16; break;
        case DXGI_FORMAT_YUY2                       :
            *bits = 32; break;
        case DXGI_FORMAT_Y210                       :
        case DXGI_FORMAT_Y216                       :
            *bits = 64; break;
        case DXGI_FORMAT_NV11                       :
            *bits = 16; break;
        case DXGI_FORMAT_AI44                       :
        case DXGI_FORMAT_IA44                       :
        case DXGI_FORMAT_P8                         :
        case DXGI_FORMAT_A8P8                       :
            return E_FAIL; // palettized
        case DXGI_FORMAT_B4G4R4A4_UNORM             :
            *bits = 64; break;
        default:
            return E_INVALIDARG;
        }
        return S_OK;
    }
    
    //----------------------------------------------------------------------------
    // Returns the number of bits in the particular pixel format
   	//----------------------------------------------------------------------------
    HRESULT
    GetBitDepthForDepthStencilFormat(
        _In_ DXGI_FORMAT fmt,
        _Out_ DWORD* depthBits,
        _Out_ DWORD* stencilBits )
    {
        switch (fmt)
        {
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT       :
            *depthBits = 32; *stencilBits = 8; break;
        case DXGI_FORMAT_D32_FLOAT                  :
            *depthBits = 32; *stencilBits = 0; break;
        case DXGI_FORMAT_D24_UNORM_S8_UINT          :
            *depthBits = 32; *stencilBits = 8; break;
        case DXGI_FORMAT_D16_UNORM                  :
            *depthBits = 16; *stencilBits = 0; break;
        default:
            return E_INVALIDARG;
        }
        return S_OK;
    }




}
