#pragma once

#include <string>
#include <vector>
#include <d3d11.h>

std::wstring GetRandomOverlayName();
bool LoadTextureFromMemory(ID3D11Device* device, const unsigned char* data, size_t size, ID3D11ShaderResourceView** out_srv);
