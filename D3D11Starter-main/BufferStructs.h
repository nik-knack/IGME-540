#pragma once
#include <DirectXMath.h>
#include "Lights.h"

struct VertexShaderData {
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
	DirectX::XMFLOAT4X4 worldInvTranspose;
};

struct PixelShaderData {
	DirectX::XMFLOAT4 colorTint;
	DirectX::XMFLOAT2 uvScale;
	DirectX::XMFLOAT2 uvOffset;
	DirectX::XMFLOAT3 cameraPosition;
	float padding;
	DirectX::XMFLOAT3 ambientColor;
	float padding2; 
	Light lights[6];
};

struct SkyVertexShaderData {
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};