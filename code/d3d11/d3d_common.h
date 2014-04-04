#ifndef __D3D_COMMON_H__
#define __D3D_COMMON_H__

#define D3D_PUBLIC  extern "C"

#ifndef __cplusplus
#   error This must only be inluded from C++ source files.
#endif

#include <assert.h>
#include <d3d11_2.h>

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
}

#endif
