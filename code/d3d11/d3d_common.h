#ifndef __D3D_COMMON_H__
#define __D3D_COMMON_H__

extern "C" {
#   include "../renderer/tr_local.h"
#   include "../renderer/tr_layer.h"
#   include "../qcommon/qcommon.h"
}

#define D3D_PUBLIC  extern "C"

#ifndef __cplusplus
#   error This must only be inluded from C++ source files.
#endif

#include <assert.h>
//#include <d3d11_2.h>
#include <d3d11_1.h>

#ifndef V_SOFT
#   define V_SOFT(x)       { HRESULT _hr=(x); if (FAILED(_hr)) g_hrLastError = _hr; }
#endif

#ifndef V
#   define V(x)            { HRESULT _hr=(x); if (FAILED(_hr)) Com_Error( ERR_FATAL, "Direct3D Error 0x%08X: %s", _hr, #x); }
#endif

#ifndef SAFE_RELEASE
#	define SAFE_RELEASE(x) if(x) { x->Release(); x = nullptr; }
#endif

#ifndef ASSERT_RELEASE
#   ifdef _DEBUG
#      define ASSERT_RELEASE(x) if (x) { \
        ULONG refCount = x->Release(); \
        assert(refCount == 0); \
        x = nullptr; }
#   else
#       define ASSERT_RELEASE(x)    SAFE_RELEASE(x)
#   endif
#endif

#ifndef SAFE_DELETE
#	define SAFE_DELETE(x) if(x) { delete x; x = nullptr; }
#endif

#ifndef SAFE_DELETE_ARRAY
#	define SAFE_DELETE_ARRAY(x) if(x) { delete [] x; x = nullptr; }
#endif

#ifndef V_RETURN
#	define V_RETURN(x) { HRESULT hr = (x); assert(SUCCEEDED(hr)); }
#endif

#ifndef ASSERT
#	include <assert.h>
#	define ASSERT(x)	assert(x)
#endif

template<class T> __forceinline T* ADDREF(T* object)
{
	object->AddRef();
	return object;
}

template<class T> __forceinline T* SAFE_ADDREF(T* object)
{
	if (object) { object->AddRef(); }
	return object;
}

// Makes sure we don't release other if ptr == other
template<class T> __forceinline void SAFE_SWAP(T*& ptr, T* other)
{
	if (ptr != other)
	{
		if (other) { other->AddRef(); }
		if (ptr) { ptr->Release(); }
		ptr = other;
	}
}

namespace QD3D
{
	//----------------------------------------------------------------------------
	// 
	//----------------------------------------------------------------------------
	class SavedRasterizerState
	{
	private:

		ID3D11DeviceContext* m_Context;
		ID3D11RasterizerState* m_PrevState;
		
	public:

		inline SavedRasterizerState(
			_In_ ID3D11DeviceContext* context)
			: m_Context(SAFE_ADDREF(context))
		{
			m_Context->RSGetState(&m_PrevState);
		}

		inline ~SavedRasterizerState()
		{
			m_Context->RSSetState(m_PrevState);
			SAFE_RELEASE(m_PrevState);
			SAFE_RELEASE(m_Context);
		}
	};

	//----------------------------------------------------------------------------
	// 
	//----------------------------------------------------------------------------
	class ScopedRasterizerState
	{
	private:

		SavedRasterizerState m_PrevState;
		
	public:

		inline ScopedRasterizerState(
			_In_ ID3D11DeviceContext* context, 
			_In_opt_ ID3D11RasterizerState* state)
			: m_PrevState(context)
		{
			context->RSSetState(state);
		}
	};

	//----------------------------------------------------------------------------
	// 
	//----------------------------------------------------------------------------
	class SavedBlendState
	{
	private:

		ID3D11DeviceContext* m_Context;
		ID3D11BlendState* m_PrevState;
		float m_PrevBlendFactor[4];
		UINT m_PrevMask;
		
	public:

		inline SavedBlendState(
			_In_ ID3D11DeviceContext* context)
			: m_Context(SAFE_ADDREF(context))
		{
			m_Context->OMGetBlendState(&m_PrevState, m_PrevBlendFactor, &m_PrevMask);
		}

		inline ~SavedBlendState()
		{
			m_Context->OMSetBlendState(m_PrevState, m_PrevBlendFactor, m_PrevMask);
			SAFE_RELEASE(m_PrevState);
			SAFE_RELEASE(m_Context);
		}
	};

	//----------------------------------------------------------------------------
	// 
	//----------------------------------------------------------------------------
	class ScopedBlendState
	{
	private:

		SavedBlendState m_PrevState;
		
	public:

		inline ScopedBlendState(
			_In_ ID3D11DeviceContext* context, 
			_In_opt_ ID3D11BlendState* state, 
			_In_opt_ const FLOAT* blendFactors, 
			_In_ UINT sampleMask)
			: m_PrevState(SAFE_ADDREF(context))
		{
			context->OMSetBlendState(state, blendFactors, sampleMask);
		}

		inline ScopedBlendState(
			_In_ ID3D11DeviceContext* context, 
			_In_opt_ ID3D11BlendState* state)
			: m_PrevState(SAFE_ADDREF(context))
		{
			const FLOAT blendFactors[] = {1, 1, 1, 1};
			const UINT sampleMask = 0xFFFFFFFF;
			context->OMSetBlendState(state, blendFactors, sampleMask);
		}
	};

	//----------------------------------------------------------------------------
	// 
	//----------------------------------------------------------------------------
	class SavedDepthStencilState
	{
	private:

		ID3D11DeviceContext* m_Context;
		ID3D11DepthStencilState* m_PrevState;
		UINT m_PrevMask;
		
	public:

		inline SavedDepthStencilState(
			_In_ ID3D11DeviceContext* context)
			: m_Context(SAFE_ADDREF(context))
		{
			m_Context->OMGetDepthStencilState(&m_PrevState, &m_PrevMask);
		}

		inline ~SavedDepthStencilState()
		{
			m_Context->OMSetDepthStencilState(m_PrevState, m_PrevMask);
			SAFE_RELEASE(m_PrevState);
			SAFE_RELEASE(m_Context);
		}
	};

	//----------------------------------------------------------------------------
	// 
	//----------------------------------------------------------------------------
	class ScopedDepthStencilState
	{
	private:

		SavedDepthStencilState m_PrevState;
		
	public:

		inline ScopedDepthStencilState(
			_In_ ID3D11DeviceContext* context, 
			_In_opt_ ID3D11DepthStencilState* state, 
			_In_ UINT stencilRef)
			: m_PrevState(SAFE_ADDREF(context))
		{
			context->OMSetDepthStencilState(state, stencilRef);
		}
	};

	//----------------------------------------------------------------------------
	// Creates a device with the default settings and returns the maximum feature 
	// level
	//----------------------------------------------------------------------------
	HRESULT 
	CreateDefaultDevice(
		_In_ D3D_DRIVER_TYPE driver, 
		_Out_ ID3D11Device** device,
		_Out_ ID3D11DeviceContext** context,
		_Out_ D3D_FEATURE_LEVEL* featureLevel);

	//----------------------------------------------------------------------------
	// Creates a default 8-bit swap chain description with no MSAA
	//----------------------------------------------------------------------------
	void
	GetDefaultSwapChainDesc(
		_In_ HWND hWnd,
		_Out_ DXGI_SWAP_CHAIN_DESC* swapChainDesc);

	//----------------------------------------------------------------------------
	// Creates a multisampled swap chain description.
	// Automatically selects the highest possible quality level for the MSAA.
	//----------------------------------------------------------------------------
	HRESULT 
	GetMultiSampledSwapChainDesc(
		_In_ HWND hWnd, 
		_In_ ID3D11Device* device,
		_In_ UINT multiSampleCount,
		_Out_ DXGI_SWAP_CHAIN_DESC* swapChainDesc);

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
		_Out_ DXGI_SWAP_CHAIN_DESC* swapChainDesc);
	
	//----------------------------------------------------------------------------
	// Creates a swap chain
	//----------------------------------------------------------------------------
	HRESULT
	CreateSwapChain(
		_In_ ID3D11Device* device,
		_In_ DXGI_SWAP_CHAIN_DESC* swapChainDesc,
		_Out_ IDXGISwapChain** swapChain);

	//----------------------------------------------------------------------------
	// Extracts the back buffer from the swap chain
	//----------------------------------------------------------------------------
	HRESULT 
	GetBackBuffer(
		_In_ IDXGISwapChain* swapChain,
		_Out_ ID3D11Texture2D** backBufferTexture);

	//----------------------------------------------------------------------------
	// Fills a viewport struct for a render target.
	// This is for convenience only; you shouldn't call this every frame.
	//----------------------------------------------------------------------------
	HRESULT
	GetRenderTargetViewport(
		_In_ ID3D11RenderTargetView* pRTV,
		_Out_ D3D11_VIEWPORT* pViewport);

    //----------------------------------------------------------------------------
    // Returns the number of bits in the particular pixel format
   	//----------------------------------------------------------------------------
    HRESULT
    GetBitDepthForFormat(
        _In_ DXGI_FORMAT fmt,
        _Out_ DWORD* bits );

    //----------------------------------------------------------------------------
    // Returns the number of bits in the particular pixel format
   	//----------------------------------------------------------------------------
    HRESULT
    GetBitDepthForDepthStencilFormat(
        _In_ DXGI_FORMAT fmt,
        _Out_ DWORD* depthBits,
        _Out_ DWORD* stencilBits );

    //----------------------------------------------------------------------------
    // Helper function for creating 2D textures
   	//----------------------------------------------------------------------------
	ID3D11Texture2D* CreateTexture2D(
        _In_ ID3D11Device* device, 
        _In_ UINT width,
        _In_ UINT height, 
        _In_ DXGI_FORMAT t2d_format, 
        _In_ LPCVOID pData,
        _In_opt_ UINT mipLevels = 1,
        _In_opt_ UINT msaaSamples = 1, 
        _In_opt_ UINT msaaQuality = 0, 
        _In_opt_ UINT bindFlags = D3D11_BIND_SHADER_RESOURCE );

    //----------------------------------------------------------------------------
    // Helper function for creating 3D textures
   	//----------------------------------------------------------------------------
	ID3D11Texture3D* CreateTexture3D(
        _In_ ID3D11Device* device, 
        _In_ UINT width, 
        _In_ UINT height, 
        _In_ UINT depth, 
        _In_ DXGI_FORMAT t3d_format, 
        _In_ LPCVOID pData,
        _In_opt_ UINT bindFlags = D3D11_BIND_SHADER_RESOURCE );

    //----------------------------------------------------------------------------
    // Helper function for creating a view of the back buffer
   	//----------------------------------------------------------------------------
	ID3D11RenderTargetView* CreateBackBufferView(
        _In_ IDXGISwapChain* swapChain,
        _In_ ID3D11Device* device, 
        _Out_opt_ D3D11_TEXTURE2D_DESC* optionalOut_BBDesc = NULL);

    //----------------------------------------------------------------------------
    // Helper function for creating a view of a depth buffer
   	//----------------------------------------------------------------------------
	ID3D11DepthStencilView* CreateDepthBufferView(
        _In_ ID3D11Device* device, 
        _In_ UINT width, 
        _In_ UINT height, 
        _In_ DXGI_FORMAT t2d_format, 
        _In_ DXGI_FORMAT dsv_format, 
        _In_opt_ UINT msaaSamples = 1, 
        _In_opt_ UINT msaaQuality = 0, 
        _In_opt_ UINT bindFlags = 0);

    //----------------------------------------------------------------------------
    // Helper function for creating a view of a render target
   	//----------------------------------------------------------------------------
	ID3D11RenderTargetView* CreateRenderTargetView(
        _In_ ID3D11Device* device, 
        _In_ UINT width, 
        _In_ UINT height, 
        _In_ DXGI_FORMAT t2d_format, 
        _In_ DXGI_FORMAT rtv_format, 
        _In_opt_ UINT msaaSamples = 1, 
        _In_opt_ UINT msaaQuality = 0, 
        _In_opt_ UINT bindFlags = 0);

    //----------------------------------------------------------------------------
    // Helper function for creating a view of a 2D render target
   	//----------------------------------------------------------------------------
	ID3D11RenderTargetView* CreateTexture2DRenderTargetView(
        _In_ ID3D11Device* device, 
        _In_ ID3D11Texture2D* texture, 
        _In_ DXGI_FORMAT rtv_format);

    //----------------------------------------------------------------------------
    // Helper function for creating a view of a 2D depth buffer
   	//----------------------------------------------------------------------------
	ID3D11DepthStencilView* CreateTexture2DDepthBufferView(
        _In_ ID3D11Device* device, 
        _In_ ID3D11Texture2D* texture, 
        _In_ DXGI_FORMAT dsv_format);

    //----------------------------------------------------------------------------
    // Helper function for creating a view of a 2D texture
   	//----------------------------------------------------------------------------
	ID3D11ShaderResourceView* CreateTexture2DShaderResourceView(
        _In_ ID3D11Device* device, 
        _In_ ID3D11Texture2D* texture, 
        _In_ DXGI_FORMAT srv_format);
}

#endif
