#include "Material.h"
#include "Graphics.h"

Material::Material(
    DirectX::XMFLOAT4 colorTint, 
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader, 
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader)
	: colorTint(colorTint), 
      uvScale(1.0f, 1.0f), 
      uvOffset(0.0f, 0.0f), 
      vertexShader(vertexShader), 
      pixelShader(pixelShader)
{
}

DirectX::XMFLOAT4 Material::GetColorTint() const
{
    return colorTint;
}

void Material::SetColorTint(DirectX::XMFLOAT4 colorTint)
{
    this->colorTint = colorTint;
}

DirectX::XMFLOAT2 Material::GetUVSccale() const
{
    return uvScale;
}

void Material::SetUVScale(DirectX::XMFLOAT2 scale)
{
    uvScale = scale;
}

DirectX::XMFLOAT2 Material::GetUVOffset() const
{
    return uvOffset;
}

void Material::SetUVOffset(DirectX::XMFLOAT2 offset)
{
    uvOffset = offset;
}

Microsoft::WRL::ComPtr<ID3D11VertexShader> Material::GetVertexShader()
{
    return vertexShader;
}

Microsoft::WRL::ComPtr<ID3D11PixelShader> Material::GetPixelShader()
{
    return pixelShader;
}

void Material::AddTextureSRV(unsigned int slot, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv)
{
    textureSRVs[slot] = srv;
}

void Material::AddSampler(unsigned int slot, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler)
{
    samplers[slot] = sampler;
}

void Material::BindTexturesAndSamplers()
{
    for (const auto& texture : textureSRVs)
    {
        unsigned int slot = texture.first;

        Graphics::Context->PSSetShaderResources(
            slot,
            1, 
            texture.second.GetAddressOf());
    }

    for (const auto& sampler : samplers)
    {
        unsigned int slot = sampler.first;

        Graphics::Context->PSSetSamplers(
            slot,
            1,
            sampler.second.GetAddressOf());
    }
}

const auto& Material::GetTextures() const
{
    return textureSRVs;
}
