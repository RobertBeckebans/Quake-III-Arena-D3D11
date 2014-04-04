#include "d3d_common.h"
#include "d3d_compiler.h"
#include "d3d_shaders.h"

// Shader cvars
cvar_t* d3d_compileDebug = nullptr;
cvar_t* d3d_compileAvoidFlowControl = nullptr;
cvar_t* d3d_compileStrict = nullptr;
cvar_t* d3d_compileOptLevel = nullptr;
cvar_t* d3d_compileSkipValidation = nullptr;
cvar_t* d3d_compileWarningsAsErrors = nullptr;
cvar_t* d3d_vsTarget = nullptr;
cvar_t* d3d_psTarget = nullptr;


void CompileShader( const char* filename, const char* target )
{
    void* buf;
    int length = ri.FS_ReadFile( filename, &buf );
    if ( length <= 0 ) {
        ri.Error( ERR_FATAL, "Couldn't load D3D11 shader '%s'\n", filename );
    }

    // @pjb: might want some other macros here one day
    D3D_SHADER_MACRO macros[] = {
        { nullptr, nullptr },
        { nullptr, nullptr }
    };

    unsigned flags = 0;

    if ( d3d_compileDebug->integer ) {
        macros[0].Name = "DEBUG";
        macros[0].Definition = "1";
        flags |= D3DCOMPILE_DEBUG;
    }

    if ( d3d_compileAvoidFlowControl->integer ) {
        flags |= D3DCOMPILE_AVOID_FLOW_CONTROL;
    }

    if ( d3d_compileStrict->integer ) {
        flags |= D3DCOMPILE_IEEE_STRICTNESS | D3DCOMPILE_ENABLE_STRICTNESS;
    }

    switch ( d3d_compileOptLevel->integer )
    {
    case -1: flags |= D3DCOMPILE_SKIP_OPTIMIZATION; break;
    case 0:  flags |= D3DCOMPILE_OPTIMIZATION_LEVEL0; break;
    case 1:  flags |= D3DCOMPILE_OPTIMIZATION_LEVEL1; break;
    case 2:  flags |= D3DCOMPILE_OPTIMIZATION_LEVEL2; break;
    default: flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3; break;
    }

    if ( d3d_compileSkipValidation->integer ) { 
        flags |= D3DCOMPILE_SKIP_VALIDATION;
    }

    if ( d3d_compileWarningsAsErrors ) {
        flags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
    }

    ID3DBlob* errors = nullptr;
    ID3DBlob* blob = QD3D::CompileShaderFromSource(
        (LPCSTR) buf,
        "Main",
        target,
        flags,
        macros,
        &errors,
        filename );

    if ( errors ) {
        CHAR textBuf[1024];
        QD3D::BlobToString( errors, textBuf, sizeof( textBuf ) );

        if ( blob ) {
            ri.Printf( PRINT_WARNING, "Warnings in '%s': %s\n", filename, textBuf );
        } else {
            ri.Error( ERR_FATAL, "Errors in '%s': %s\n", filename, textBuf );
        }
    }



    ri.FS_FreeFile( buf );
}

void InitShaders()
{
    //
    // Get Cvars
    //
#ifdef _DEBUG
    d3d_compileDebug = Cvar_Get( "d3d_compileDebug", "1", CVAR_ARCHIVE | CVAR_LATCH );
    d3d_compileWarningsAsErrors = Cvar_Get( "d3d_compileWarningsAsErrors", "0", CVAR_ARCHIVE | CVAR_LATCH );
#else
    d3d_compileDebug = Cvar_Get( "d3d_compileDebug", "0", CVAR_ARCHIVE | CVAR_LATCH );
    d3d_compileWarningsAsErrors = Cvar_Get( "d3d_compileWarningsAsErrors", "1", CVAR_ARCHIVE | CVAR_LATCH );
#endif
    d3d_compileAvoidFlowControl = Cvar_Get( "d3d_compileAvoidFlowControl", "0", CVAR_ARCHIVE | CVAR_LATCH );
    d3d_compileStrict = Cvar_Get( "d3d_compileStrict", "0", CVAR_ARCHIVE | CVAR_LATCH );
    d3d_compileOptLevel = Cvar_Get( "d3d_compileOptLevel", "3", CVAR_ARCHIVE | CVAR_LATCH );
    d3d_compileSkipValidation = Cvar_Get( "d3d_compileSkipValidation", "0", CVAR_ARCHIVE | CVAR_LATCH );
    d3d_vsTarget = Cvar_Get( "d3d_vsTarget", "vs_5_0", CVAR_ARCHIVE | CVAR_LATCH );
    d3d_psTarget = Cvar_Get( "d3d_psTarget", "ps_5_0", CVAR_ARCHIVE | CVAR_LATCH );

    //
    // Load shaders
    //
    CompileShader( "d3d11/fsq_vs.hlsl", d3d_vsTarget->string );
    CompileShader( "d3d11/fsq_ps.hlsl", d3d_psTarget->string );
    // @pjb: todo: cache these and use them!
}

void DestroyShaders()
{

}
