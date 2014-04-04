#pragma once

#include <d3dcompiler.h>

namespace QD3D
{
    enum PREDEFINED_SHADER_COMPILE_FLAGS
    {
        SHADER_COMPILE_DEBUG = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        SHADER_COMPILE_RELEASE = D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_WARNINGS_ARE_ERRORS,

#ifdef _DEBUG
        SHADER_COMPILE_DEFAULT = SHADER_COMPILE_DEBUG
#else
        SHADER_COMPILE_DEFAULT = SHADER_COMPILE_RELEASE
#endif
    };

	//----------------------------------------------------------------------------
    // Compiles a shader from a file
	//----------------------------------------------------------------------------
	ID3DBlob* CompileShaderFromFile(
		_In_z_ LPCSTR source_file, 
		_In_ ID3DInclude* includeCB,
		_In_z_ LPCSTR entry_point, 
		_In_z_ LPCSTR target,
        _In_ DWORD compileFlags,
		_In_opt_z_ const D3D_SHADER_MACRO* defines,
		_Out_opt_ ID3DBlob** errors);

	//----------------------------------------------------------------------------
    // Compiles a shader from source in memory
	//----------------------------------------------------------------------------
    ID3DBlob* CompileShaderFromSource(
        _In_z_ LPCSTR source,
		_In_z_ LPCSTR entry_point, 
		_In_z_ LPCSTR target,
        _In_ DWORD compileFlags,
		_In_opt_z_ const D3D_SHADER_MACRO* defines,
		_Out_opt_ ID3DBlob** errors,
        _In_opt_z_ LPCSTR source_name = "<source>");

	//----------------------------------------------------------------------------
    // Converts an ID3DBlob to a string
	//----------------------------------------------------------------------------
    HRESULT BlobToString(
        _In_ ID3DBlob* blob,
        _Out_writes_(bufSize) LPSTR buffer,
        _In_ SIZE_T bufSize);
}



