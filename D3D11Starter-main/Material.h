#pragma once
#include "Vertex.h"
#include "wrl/client.h"
#include "d3d11.h"
#include <unordered_map>

class Material
{
private:
	DirectX::XMFLOAT4 colorTint;
    DirectX::XMFLOAT2 uvScale;
    DirectX::XMFLOAT2 uvOffset;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;

    // Texture Resource
    std::unordered_map<
        unsigned int,
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>
    > textureSRVs;

    // Sampler State
    std::unordered_map<
        unsigned int,
        Microsoft::WRL::ComPtr<ID3D11SamplerState>
    > samplers;

public:
    Material(
        DirectX::XMFLOAT4 colorTint, 
        Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader, 
        Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader);

    // Color tint
	DirectX::XMFLOAT4 GetColorTint() const;
	void SetColorTint(DirectX::XMFLOAT4 colorTint);

    // UV Scale
    DirectX::XMFLOAT4 GetUVSccale() const;
    void SetUVScale(DirectX::XMFLOAT2 scale);
    
    // UV Offset
    DirectX::XMFLOAT4 GetUVOffset() const;
    void SetUVOffset(DirectX::XMFLOAT4 offset);

    //Shaders
    Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader();
    Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader();

    // Texture and sampler management
    void AddTextureSRV();
    void AddSampler();
    void BindTexturesAndSameplers();

    // Access texture resources for ImGui
    //const GetTextures() const; something like this
};

