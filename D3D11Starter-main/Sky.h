#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include <string>
#include "Mesh.h"
#include "Graphics.h"
#include "WICTextureLoader.h"
#include "PathHelpers.h"
#include <d3dcompiler.h>

class Sky
{
public:
	Sky(
		std::shared_ptr<Mesh> mesh,
		Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler,
		const wchar_t* right, const wchar_t* left,
		const wchar_t* up, const wchar_t* down,
		const wchar_t* front, const wchar_t* back,
		const std::wstring& vertexShaderFile, const std::wstring& pixelShaderFile
		);
	~Sky();
	void Draw();	

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubeMap(
		const wchar_t* right, const wchar_t* left,
		const wchar_t* up, const wchar_t* down,
		const wchar_t* front, const wchar_t* back
	);

	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;

	std::shared_ptr<Mesh> mesh;
};

