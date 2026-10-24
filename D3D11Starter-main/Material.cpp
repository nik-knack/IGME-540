#include "Material.h"

Material::Material(
    DirectX::XMFLOAT4 colorTint, 
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader, 
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader)
	: colorTint(colorTint), vertexShader(vertexShader), pixelShader(pixelShader)
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

Microsoft::WRL::ComPtr<ID3D11VertexShader> Material::GetVertexShader()
{
    return vertexShader;
}

Microsoft::WRL::ComPtr<ID3D11PixelShader> Material::GetPixelShader()
{
    return pixelShader;
}
