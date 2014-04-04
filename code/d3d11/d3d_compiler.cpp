#include "d3d_common.h"
#include "d3d_compiler.h"

namespace QD3D
{


	//----------------------------------------------------------------------------
    // Converts an ID3DBlob to a string
	//----------------------------------------------------------------------------
    LPSTR BlobToString(ID3DBlob* blob, CHAR terminator)
    {
	    LPSTR buf = new CHAR[blob->GetBufferSize() + 2];

	    memcpy((void*)buf, blob->GetBufferPointer(), blob->GetBufferSize());

	    buf[blob->GetBufferSize()] = terminator;
	    buf[blob->GetBufferSize()+1] = 0;

	    return buf;
    }

    HRESULT BlobToString(ID3DBlob* blob, LPSTR buffer, SIZE_T bufSize)
    {
        if (bufSize < blob->GetBufferSize())
            return E_OUTOFMEMORY;

        memcpy((void*)buffer, blob->GetBufferPointer(), blob->GetBufferSize());
        return S_OK;
    }

    void OutputErrors(ID3DBlob* errors)
    {
    #ifdef _DEBUG
	    LPSTR error_string = BlobToString(errors, '\n');
	    OutputDebugStringA(error_string);
        delete [] error_string;
    #endif
    }

	//----------------------------------------------------------------------------
    // Compiles a shader from source in memory
	//----------------------------------------------------------------------------
    ID3DBlob* CompileShaderFromSource(
        _In_z_ LPCSTR source_data,
	    _In_z_ LPCSTR entry_point, 
	    _In_z_ LPCSTR target,
        _In_ DWORD compileFlags,
	    _In_opt_z_ const D3D_SHADER_MACRO* defines,
	    _Out_opt_ ID3DBlob** errors_out,
        _In_opt_z_ LPCSTR source_name)
    {
        if (!source_data)
            return nullptr;

	    //
	    // Open the file and get the data
	    //
	    UINT source_length = (UINT) strlen(source_data);
        if (!source_length)
            return nullptr;

	    //
	    // Compile the shader
	    //
	    ID3DBlob* binary;
	    ID3DBlob* errors;

	    D3DCompile(
		    source_data, 
		    source_length,
            source_name,
		    defines,
		    nullptr, 
		    entry_point, 
		    target, 
		    compileFlags, 
		    0, 
		    &binary, 
		    &errors);

	    //
	    // Log any errors
	    //
	    if (errors_out) 
	    {
		    *errors_out = NULL;
	    }

	    if (errors)
	    {
		    OutputErrors(errors);
	
		    if (errors_out) 
		    { 
			    *errors_out = errors;
		    }
		    else
		    {
			    SAFE_RELEASE(errors);
		    }
	    }

	    return binary;
    }

	//----------------------------------------------------------------------------
    // Compiles a shader from a file
	//----------------------------------------------------------------------------
    ID3DBlob* CompileShaderFromFile(
	    LPCSTR source_file, 
	    ID3DInclude* include,
	    LPCSTR entry_point, 
	    LPCSTR target,
        _In_ DWORD compileFlags,
	    const D3D_SHADER_MACRO* defines,
	    ID3DBlob** errors_out)
    {
	    //
	    // Open the file and get the data
	    //
	    CHAR* source_data = NULL;
	    UINT source_length = 0;
	    HRESULT result = include->Open(
            D3D_INCLUDE_LOCAL, 
            source_file, 
            NULL, 
            (LPCVOID*)&source_data, 
            &source_length);
	    if (result != S_OK)
	    {
		    return NULL;
	    }

	    //
	    // Compile the shader
	    //
	    ID3DBlob* binary;
	    ID3DBlob* errors;

	    result = D3DCompile(
		    source_data, 
		    source_length,
		    source_file,
		    defines,
		    include, 
		    entry_point, 
		    target, 
		    compileFlags, 
		    0, 
		    &binary, 
		    &errors);

	    //
	    // Close the file
	    //
	    include->Close(source_data);

	    //
	    // Log any errors
	    //
	    if (errors_out) 
	    {
		    *errors_out = NULL;
	    }

	    if (errors)
	    {
		    OutputErrors(errors);
	
		    if (errors_out) 
		    { 
			    *errors_out = errors;
		    }
		    else
		    {
			    SAFE_RELEASE(errors);
		    }
	    }

	    return binary;
    }
}