#pragma once
#include "Vertex.h"
#include "wrl/client.h"
#include "d3d11.h"

class Material
{
private:
	DirectX::XMFLOAT4 colorTint;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
public:
    Material(
        DirectX::XMFLOAT4 colorTint, 
        Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader, 
        Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader);

	DirectX::XMFLOAT4 GetColorTint();
	void SetColorTint(DirectX::XMFLOAT4 colorTint);

    Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader();
    Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader();
};

