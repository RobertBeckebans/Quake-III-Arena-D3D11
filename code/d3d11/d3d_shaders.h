#pragma once

// @pjb: holds all the relevant d3d shaders

void InitShaders();
void DestroyShaders();

ID3D11PixelShader* CompilePixelShader( const char* name );
ID3D11VertexShader* CompileVertexShader( const char* name, ID3DBlob** ppBlob );
