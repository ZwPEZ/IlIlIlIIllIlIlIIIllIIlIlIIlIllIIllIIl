#include "helpers.hpp"
#include <random>
#include <d3d11.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

std::wstring GetRandomOverlayName() {
    const std::wstring possible_characters = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, possible_characters.size() - 1);

    std::wstring random_string(12, L'\0');
    for (wchar_t& ch : random_string) {
        ch = possible_characters[distribution(generator)];
    }
    return random_string;
}

bool LoadTextureFromMemory(ID3D11Device* device, const unsigned char* data, size_t size, ID3D11ShaderResourceView** out_srv) {
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load_from_memory(data, size, &image_width, &image_height, nullptr, 4);
    if (image_data == nullptr)
        return false;

    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = image_width;
    desc.Height = image_height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    ID3D11Texture2D* pTexture = nullptr;
    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = image_data;
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;
    device->CreateTexture2D(&desc, &subResource, &pTexture);

    if (pTexture == nullptr) {
        stbi_image_free(image_data);
        return false;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    device->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
    pTexture->Release();

    stbi_image_free(image_data);

    return true;
}
