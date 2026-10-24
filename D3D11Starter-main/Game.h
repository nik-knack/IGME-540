#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include <memory>
#include "Mesh.h"
#include "BufferStructs.h"
#include "GameEntity.h"
#include "Camera.h"

class Game
{
public:
	// Basic OOP setup
	Game();
	~Game();
	Game(const Game&) = delete; // Remove copy constructor
	Game& operator=(const Game&) = delete; // Remove copy-assignment operator

	// Primary functions
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	void OnResize();

	// Initialization of UI helper functions
	void ImGuiUpdate(float deltaTime);

	bool showDemoWindow = false;
	bool isChecked = false;
	int number = 0;
	float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

private:

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders();
	void CreateGeometry();

	// Store meshes
	std::vector<std::shared_ptr<Mesh>> meshes;

	// Store materials
	std::vector<std::shared_ptr<Material>> materials;

	// Store game entities
	std::vector<std::shared_ptr<GameEntity>> entities;

	// Store multiple camera
	std::vector<std::shared_ptr<Camera>> cameras;

	// Keep track of which camera is currently active
	int activeCameraIndex = 0;

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	// Store Vertex Shader Data
	VertexShaderData vertexShaderData;

	// Shaders and shader-related constructs
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> pixelShaderConstantBuffer;
	PixelShaderData pixelShaderData;

	// Constant buffer to hold data for the vertex shader
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexShaderConstantBuffer;
};

