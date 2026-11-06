	#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include <stdexcept>
#include <cstring>
#include <WICTextureLoader.h>

#include <DirectXMath.h>

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// This code assumes files are in "ImGui" subfolder!
// Adjust as necessary for your own folder structure and project setup
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

// For the DirectX Math library
using namespace DirectX;


// --------------------------------------------------------
// The constructor is called after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
Game::Game()
{
	// Initialize ImGui itself & platform / renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(Window::Handle());
	ImGui_ImplDX11_Init(Graphics::Device.Get(), Graphics::Context.Get());
	// Pick a style (uncomment one of these 3)
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	//ImGui::StyleColorsClassic();

	
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();				// May adjust as see fit in the future
	CreateGeometry();

	// Obtain the D3D11 context
	HRESULT hr = Graphics::Context.As(&context1);

	if (FAILED(hr))
	{
		throw std::runtime_error("Direct3D context is unavailable");
	}

	// Reset ring buffer position
	cbHeapOffsetInBytes = 0;

	// Ensure the heap size is a multiple of 256
	cbHeapSizeInBytes = ((cbHeapSizeInBytes + 255) / 256) * 256;

	// Create the large constant buffer
	D3D11_BUFFER_DESC desc = {};

	desc.ByteWidth = cbHeapSizeInBytes;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	hr = Graphics::Device->CreateBuffer(
		&desc,
		nullptr,
		constantBufferHeap.GetAddressOf());

	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create constant buffer heap");
	}

	// Create the cameras
	cameras.push_back( std::make_shared<Camera>(
		Window::AspectRatio(), 0.0f, 0.0f, -12.0f, DirectX::XMConvertToRadians(60.0f)));

	cameras.push_back(std::make_shared<Camera>(
		Window::AspectRatio(), 0.0f, 0.0f, -24.0f, DirectX::XMConvertToRadians(90.0f)));

	// Camera 0 is the active camera 
	activeCameraIndex = 0;

	// Load textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> fabricTexture;
	DirectX::CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/fabric_pattern.png").c_str(),
		0,
		fabricTexture.GetAddressOf()
	);

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cuddlyTexture;
	DirectX::CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/cuddly_teddy.jpg").c_str(),
		0,
		cuddlyTexture.GetAddressOf()
	);

	// Create the sampler state
	D3D11_SAMPLER_DESC samplerDesc = {};

	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;

	hr = Graphics::Device->CreateSamplerState(
		&samplerDesc,
		sampler.GetAddressOf());

	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create texture sampler");
	}

	// Load shaders
	auto uvPS = LoadPixelShader(L"DebugUVsPS.cso");
	auto normalPS = LoadPixelShader(L"DebugNormalsPS.cso");
	auto customPS = LoadPixelShader(L"CustomPS.cso");
	auto combinedPS = LoadPixelShader(L"CombinedTexturePS.cso");

	// Create a vector of materials for the meshes
	//materials.push_back(std::make_shared<Material>(XMFLOAT4(1, 1, 1, 1), vertexShader, uvPS));			
	//materials.push_back(std::make_shared<Material>(XMFLOAT4(1, 1, 1, 1), vertexShader, normalPS));	
	materials.push_back(std::make_shared<Material>(XMFLOAT4(1, 1, 1, 1), vertexShader, pixelShader));
	materials.push_back(std::make_shared<Material>(XMFLOAT4(1, 1, 1, 1), vertexShader, pixelShader));
	materials.push_back(std::make_shared<Material>(XMFLOAT4(1, 1, 1, 1), vertexShader, combinedPS));
	materials.push_back(std::make_shared<Material>(XMFLOAT4(0, 1, 0, 1), vertexShader, customPS));

	// Fabric Material
	materials[0]->AddTextureSRV(0, fabricTexture);
	materials[0]->AddSampler(0, sampler);

	// Cuddly Material
	materials[1]->AddTextureSRV(0, cuddlyTexture);
	materials[1]->AddSampler(0, sampler);

	// Combined Texture
	materials[2]->AddTextureSRV(0, fabricTexture);
	materials[2]->AddTextureSRV(1, cuddlyTexture);
	materials[2]->AddSampler(0, sampler);

	// Keep plain color material
	materials[3]->SetColorTint(XMFLOAT4(0, 1, 0, 1));


	// Creating game entities
	entities.push_back(std::make_shared<GameEntity>(meshes[0], materials[0]));
	entities.push_back(std::make_shared<GameEntity>(meshes[1], materials[0]));
	entities.push_back(std::make_shared<GameEntity>(meshes[2], materials[0]));
	entities.push_back(std::make_shared<GameEntity>(meshes[3], materials[0]));
	entities.push_back(std::make_shared<GameEntity>(meshes[4], materials[0]));
	entities.push_back(std::make_shared<GameEntity>(meshes[5], materials[0]));
	entities.push_back(std::make_shared<GameEntity>(meshes[6], materials[0]));

	entities.push_back(std::make_shared<GameEntity>(meshes[0], materials[1]));
	entities.push_back(std::make_shared<GameEntity>(meshes[1], materials[1]));
	entities.push_back(std::make_shared<GameEntity>(meshes[2], materials[1]));
	entities.push_back(std::make_shared<GameEntity>(meshes[3], materials[1]));
	entities.push_back(std::make_shared<GameEntity>(meshes[4], materials[1]));
	entities.push_back(std::make_shared<GameEntity>(meshes[5], materials[1]));
	entities.push_back(std::make_shared<GameEntity>(meshes[6], materials[1]));

	entities.push_back(std::make_shared<GameEntity>(meshes[0], materials[2]));
	entities.push_back(std::make_shared<GameEntity>(meshes[1], materials[2]));
	entities.push_back(std::make_shared<GameEntity>(meshes[2], materials[2]));
	entities.push_back(std::make_shared<GameEntity>(meshes[3], materials[2]));
	entities.push_back(std::make_shared<GameEntity>(meshes[4], materials[3]));
	entities.push_back(std::make_shared<GameEntity>(meshes[5], materials[3]));
	entities.push_back(std::make_shared<GameEntity>(meshes[6], materials[3]));

	// Give each entity a different position
	entities[0]->GetTransform()->SetPosition(-9.0f, 0.0f, 0.0f);
	entities[1]->GetTransform()->SetPosition(-6.0f, 0.0f, 0.0f);
	entities[2]->GetTransform()->SetPosition(-3.0f, 0.0f, 0.0f);
	entities[3]->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
	entities[4]->GetTransform()->SetPosition(3.0f, 0.0f, 0.0f);
	entities[5]->GetTransform()->SetPosition(6.0f, 0.0f, 0.0f);
	entities[6]->GetTransform()->SetPosition(9.0f, 0.0f, 0.0f);

	entities[7]->GetTransform()->SetPosition(-9.0f, 3.0f, 0.0f);
	entities[8]->GetTransform()->SetPosition(-6.0f, 3.0f, 0.0f);
	entities[9]->GetTransform()->SetPosition(-3.0f, 3.0f, 0.0f);
	entities[10]->GetTransform()->SetPosition(0.0f, 3.0f, 0.0f);
	entities[11]->GetTransform()->SetPosition(3.0f, 3.0f, 0.0f);
	entities[12]->GetTransform()->SetPosition(6.0f, 3.0f, 0.0f);
	entities[13]->GetTransform()->SetPosition(9.0f, 3.0f, 0.0f);

	entities[14]->GetTransform()->SetPosition(-9.0f, -3.0f, 0.0f);
	entities[15]->GetTransform()->SetPosition(-6.0f, -3.0f, 0.0f);
	entities[16]->GetTransform()->SetPosition(-3.0f, -3.0f, 0.0f);
	entities[17]->GetTransform()->SetPosition(0.0f, -3.0f, 0.0f);
	entities[18]->GetTransform()->SetPosition(3.0f, -3.0f, 0.0f);
	entities[19]->GetTransform()->SetPosition(6.0f, -3.0f, 0.0f);
	entities[20]->GetTransform()->SetPosition(9.0f, -3.0f, 0.0f);

	// Create lights
	// Red light pointing forward
	Light dir1 = {};
	dir1.Type = LIGHT_TYPE_DIRECTIONAL;
	XMStoreFloat3(&dir1.Direction, XMVector3Normalize(XMVectorSet(1, 0, 0.5f, 0)));
	dir1.Color = XMFLOAT3(1, 0, 0);
	dir1.Intensity = 1.0f;
	lights.push_back(dir1);

	// White light pointing down
	Light dir2 = {};
	dir2.Type = LIGHT_TYPE_DIRECTIONAL;
	XMStoreFloat3(&dir2.Direction, XMVector3Normalize(XMVectorSet(-0.1f, -1, 0, 0)));
	dir2.Color = XMFLOAT3(0, 1, 0);
	dir2.Intensity = 1.0f;
	lights.push_back(dir2);

	// White light pointing up at an angle
	Light dir3 = {};
	dir3.Type = LIGHT_TYPE_DIRECTIONAL;
	XMStoreFloat3(&dir3.Direction, XMVector3Normalize(XMVectorSet(-1, 1, -0.5f, 0)));
	dir3.Color = XMFLOAT3(1, 1, 1);
	dir3.Intensity = 0.5f;
	lights.push_back(dir3);

	// White light
	Light point1 = {};
	point1.Type = LIGHT_TYPE_POINT;
	point1.Position = XMFLOAT3(-1.5f, 0, -2);
	point1.Color = XMFLOAT3(1, 1, 1);
	point1.Intensity = 1.0f;
	point1.Range = 10.0f;
	lights.push_back(point1);

	// Green light
	Light point2 = {};	
	point2.Type = LIGHT_TYPE_POINT;
	point2.Position = XMFLOAT3(1.5f, 0, -2);
	point2.Color = XMFLOAT3(0, 1, 0);
	point2.Intensity = 1.0f;
	point2.Range = 10.0f;
	lights.push_back(point2);

	// Blue light pointing down
	Light spot1 = {};
	spot1.Type = LIGHT_TYPE_SPOT;
	spot1.Position = XMFLOAT3(3, 3, -3);
	spot1.Direction = XMFLOAT3(0, -1, 0);
	spot1.Range = 15.0f;
	spot1.Color = XMFLOAT3(0, 0, 1);
	spot1.Intensity = 2.0f;
	spot1.SpotInnerAngle = DirectX::XMConvertToRadians(20.0f);
	spot1.SpotOuterAngle = DirectX::XMConvertToRadians(30.0f);
	lights.push_back(spot1);

	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Ensure the pipeline knows how to interpret all the numbers stored in
		// the vertex buffer. For this course, all of your vertices will probably
		// have the same layout, so we can just set this once at startup.
		Graphics::Context->IASetInputLayout(inputLayout.Get());

		// Set the active vertex and pixel shaders
		//  - Once you start applying different shaders to different objects,
		//    these calls will need to happen multiple times per frame
		Graphics::Context->VSSetShader(vertexShader.Get(), 0, 0);
		Graphics::Context->PSSetShader(pixelShader.Get(), 0, 0);
	}	
}


// --------------------------------------------------------
// Clean up memory or objects created by this class
// 
// Note: Using smart pointers means there probably won't
//       be much to manually clean up here!
// --------------------------------------------------------
Game::~Game()
{
	// ImGui clean up
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}


// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	// BLOBs (or Binary Large OBjects) for reading raw data from external files
	// - This is a simplified way of handling big chunks of external data
	// - Literally just a big array of bytes read from a file
	ID3DBlob* pixelShaderBlob;
	ID3DBlob* vertexShaderBlob;

	// Loading shaders
	//  - Visual Studio will compile our shaders at build time
	//  - They are saved as .cso (Compiled Shader Object) files
	//  - We need to load them when the application starts
	{
		// Read our compiled shader code files into blobs
		// - Essentially just "open the file and plop its contents here"
		// - Uses the custom FixPath() helper from Helpers.h to ensure relative paths
		// - Note the "L" before the string - this tells the compiler the string uses wide characters
		D3DReadFileToBlob(FixPath(L"PixelShader.cso").c_str(), &pixelShaderBlob);
		D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), &vertexShaderBlob);

		// Create the actual Direct3D shaders on the GPU
		Graphics::Device->CreatePixelShader(
			pixelShaderBlob->GetBufferPointer(),	// Pointer to blob's contents
			pixelShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			pixelShader.GetAddressOf());			// Address of the ID3D11PixelShader pointer

		Graphics::Device->CreateVertexShader(
			vertexShaderBlob->GetBufferPointer(),	// Get a pointer to the blob's contents
			vertexShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			vertexShader.GetAddressOf());			// The address of the ID3D11VertexShader pointer
	}

	// Create an input layout 
	//  - This describes the layout of data sent to a vertex shader
	//  - In other words, it describes how to interpret data (numbers) in a vertex buffer
	//  - Doing this NOW because it requires a vertex shader's byte code to verify against!
	//  - Luckily, we already have that loaded (the vertex shader blob above)
	{
		D3D11_INPUT_ELEMENT_DESC inputElements[3] = {};

		// Set up the first element - a position, which is 3 float values
		inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
		inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
		inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

		// Set up the second element - a color, which is 4 more float values
		inputElements[1].Format = DXGI_FORMAT_R32G32_FLOAT;			// 4x 32-bit floats
		inputElements[1].SemanticName = "TEXCOORD";							// Match our vertex shader input
		inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element
		
		// Set up the third element - a normal, which is 3 more float values
		inputElements[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;			// 4x 32-bit floats
		inputElements[2].SemanticName = "NORMAL";							// Match our vertex shader input
		inputElements[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

		// Create the input layout, verifying our description against actual shader code
		Graphics::Device->CreateInputLayout(
			inputElements,							// An array of descriptions
			ARRAYSIZE(inputElements),				// How many elements in that array?
			vertexShaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
			vertexShaderBlob->GetBufferSize(),		// Size of the shader code that uses this layout
			inputLayout.GetAddressOf());			// Address of the resulting ID3D11InputLayout pointer
	}
}


// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
{
	
	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	meshes.push_back(std::make_shared<Mesh>(FixPath("../../Assets/Meshes/cube.obj").c_str()));
	meshes.push_back(std::make_shared<Mesh>(FixPath("../../Assets/Meshes/cylinder.obj").c_str()));
	meshes.push_back(std::make_shared<Mesh>(FixPath("../../Assets/Meshes/helix.obj").c_str()));
	meshes.push_back(std::make_shared<Mesh>(FixPath("../../Assets/Meshes/sphere.obj").c_str()));	
	meshes.push_back(std::make_shared<Mesh>(FixPath("../../Assets/Meshes/torus.obj").c_str()));
	meshes.push_back(std::make_shared<Mesh>(FixPath("../../Assets/Meshes/quad.obj").c_str()));
	meshes.push_back(std::make_shared<Mesh>(FixPath("../../Assets/Meshes/quad_double_sided.obj").c_str()));
}

void Game::FillAndBindNextConstantBuffer(const void* data, unsigned int dataSizeInBytes, D3D11_SHADER_VERSION_TYPE shaderType, unsigned int registerSlot)
{
	if (!data || dataSizeInBytes == 0)
		return;

	// Reserve space in multiples of 256
	const  unsigned int reservationSize = ((dataSizeInBytes + 255) / 256) * 256;

	if (reservationSize > cbHeapSizeInBytes)
	{
		throw std::runtime_error("Constant buffer allocation excees heap size");
	}

	// Wrap around when the next allocation does not fit
	if (cbHeapOffsetInBytes + reservationSize > cbHeapSizeInBytes)
	{
		cbHeapOffsetInBytes = 0;
	}

	// Map the head without overweriting existing GPU data
	D3D11_MAPPED_SUBRESOURCE mapped = {};

	HRESULT hr = context1->Map(
		constantBufferHeap.Get(),
		0,
		D3D11_MAP_WRITE_NO_OVERWRITE,
		0,
		&mapped);

	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to map constant buffer heap");
	}

	// Copy data into this allocation
	void* destination = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(mapped.pData) + cbHeapOffsetInBytes);

	std::memcpy(
		destination,
		data,
		dataSizeInBytes);

	context1->Unmap(constantBufferHeap.Get(), 0);

	// Buffer bindings in 16 bytes
	const unsigned int firstConstant = cbHeapOffsetInBytes / 16;
	const unsigned int numConstants = reservationSize / 16;

	// Bind the appropriate region to the requested shader
	switch (shaderType)
	{
	case D3D11_SHVER_VERTEX_SHADER:
		context1->VSSetConstantBuffers1(
			registerSlot,
			1,
			constantBufferHeap.GetAddressOf(),
			&firstConstant,
			&numConstants);
		break;

	case D3D11_SHVER_PIXEL_SHADER:
		context1->PSSetConstantBuffers1(
			registerSlot,
			1,
			constantBufferHeap.GetAddressOf(),
			&firstConstant,
			&numConstants);
		break;

	default:
		throw std::invalid_argument("Unsupported shader");
	}

	// Advance to the next allocation
	cbHeapOffsetInBytes += reservationSize;
}


// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	// Update the projection matrix for every camera 
	for (const auto& camera : cameras)
	{
		if (camera)
		{
			camera->UpdateProjectionMatrix(Window::AspectRatio());
		}
	}
	
}


// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Update the active camera
	if (cameras.size() > 0)
	{
		cameras[activeCameraIndex]->Update(deltaTime);
	}

	for (auto& entity : entities)
	{
		entity->GetTransform()->Rotate(0.0f, deltaTime, 0.0f);
	}

	// Call helper method to update ImGui
	Game::ImGuiUpdate(deltaTime);

	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();
}


// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erase what's on screen) and depth buffer
		//const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };
		Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(),	color);
		Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}
		
	// Get camera's view and projection matrices shared by all entities
	//vertexShaderData.view = cameras[activeCameraIndex]->GetViewMatrix();
	//vertexShaderData.projection = cameras[activeCameraIndex]->GetProjectionMatrix();

	// Bind the drawing input layout and primitive topology
	Graphics::Context->IASetInputLayout(inputLayout.Get());	
	Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Lights and ambient color are shared by all entities
	pixelShaderData.ambientColor = ambientColor;
	memcpy(&pixelShaderData.lights, &lights[0], sizeof(Light) * lights.size());

	// Draw each game entity
	for (const auto& entity : entities)
	{
		// Get the entity's material
		auto material = entity->GetMaterial();	

		// Bind the entity's material
		Graphics::Context->VSSetShader(material->GetVertexShader().Get(), 0, 0);
		Graphics::Context->PSSetShader(material->GetPixelShader().Get(), 0, 0);

		// Get the entity's mesh and transform
		vertexShaderData.world = entity->GetTransform()->GetWorldMatrix();
		vertexShaderData.view = cameras[activeCameraIndex]->GetViewMatrix();
		vertexShaderData.projection = cameras[activeCameraIndex]->GetProjectionMatrix();
		vertexShaderData.worldInvTranspose = entity->GetTransform()->GetWorldInverseTransposeMatrix();

		// Upload and bind vertex shader with the constant buffer
		FillAndBindNextConstantBuffer(
			&vertexShaderData,
			sizeof(VertexShaderData),
			D3D11_SHVER_VERTEX_SHADER,
			0
		);

		// Update the pixel shader data
		pixelShaderData.colorTint = material->GetColorTint();
		pixelShaderData.uvScale = material->GetUVSccale();
		pixelShaderData.uvOffset = material->GetUVOffset();
		pixelShaderData.cameraPosition = cameras[activeCameraIndex]->GetTransform()->GetPosition();
		pixelShaderData.ambientColor = DirectX::XMFLOAT3(0.0f, 0.8f, 0.5f);

		// Upload and bind pixel shader with the constant buffer
		FillAndBindNextConstantBuffer(
			&pixelShaderData,
			sizeof(PixelShaderData),
			D3D11_SHVER_PIXEL_SHADER,
			0
		);

		// Bind textures and samplers
		material->BindTexturesAndSamplers();

		entity->Draw();
	}	


	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		ImGui::Render(); // Turns this frame’s UI into renderable triangles
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // Draws it to the screen
		
		// Present at the end of the frame
		bool vsync = Graphics::VsyncState();
		Graphics::SwapChain->Present(
			vsync ? 1 : 0,
			vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Re-bind back buffer and depth buffer after presenting
		Graphics::Context->OMSetRenderTargets(
			1,
			Graphics::BackBufferRTV.GetAddressOf(),
			Graphics::DepthBufferDSV.Get());
	}
}

// ImGui Helper Method	
// ImGui Helper Method	
// Updates the UI every frame
void Game::ImGuiUpdate(float deltaTime) {
	// Feed fresh data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)Window::Width();
	io.DisplaySize.y = (float)Window::Height();
	// Reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// Determine new input capture
	Input::SetKeyboardCapture(io.WantCaptureKeyboard);
	Input::SetMouseCapture(io.WantCaptureMouse);

	if (showDemoWindow)
	{
		ImGui::ShowDemoWindow();
	}

	ImGui::Begin("Inspector"); // Everything after is part of the window

	if (ImGui::TreeNode("App Details")) {
		ImGui::Text("Framerate: %f fps", ImGui::GetIO().Framerate);
		ImGui::Text("Window Resolution: %dx%d", Window::Width(), Window::Height());
		ImGui::TreePop();
		if (ImGui::Button(showDemoWindow
			? "Hide ImGui Demo Window"
			: "Show ImGui Demo Window"))
		{
			showDemoWindow = !showDemoWindow;
		}
	}

	if (ImGui::TreeNode("Lights"))
	{
		ImGui::ColorEdit3("Ambient Color", &ambientColor.x);

		for (int i = 0; i < (int)lights.size(); i++)
		{
			ImGui::PushID(i);

			if (ImGui::TreeNode(("Light " + std::to_string(i + 1)).c_str()))
			{
				ImGui::ColorEdit3("Color", &lights[i].Color.x);
				ImGui::DragFloat("Intensity", &lights[i].Intensity, 0.5f, 0.0f, 10.0f);
				ImGui::TreePop();
			}
			ImGui::PopID();
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Materials"))
	{
		for (int i = 0; i < static_cast<int>(materials.size()); i++)
		{
			ImGui::PushID(i);

			std::string materialName = "Material" + std::to_string(i + 1);

			auto material = materials[i];

			if (ImGui::TreeNode(materialName.c_str()))
			{
				// Color tint
				DirectX::XMFLOAT4 tint = material->GetColorTint();

				if (ImGui::ColorEdit4("Color Tint", &tint.x))
				{
					material->SetColorTint(tint);
				}

				// UV Scale
				DirectX::XMFLOAT2 scale = material->GetUVSccale();

				if (ImGui::DragFloat2("UV Scale", &scale.x, 0.05f))
				{
					material->SetUVScale(scale);
				}

				// UV Offset
				DirectX::XMFLOAT2 offset = material->GetUVOffset();

				if (ImGui::DragFloat2("UV Offset", &offset.x, 0.01f))
				{
					material->SetUVOffset(offset);
				}

				ImGui::TreePop();
			}
			ImGui::PopID();
		}
		ImGui::TreePop();
	}

	for (int i = 0; i < entities.size(); i++)
	{
		ImGui::PushID(i);

		std::string entityName = "Entity " + std::to_string(i + 1);

		if (ImGui::TreeNode(entityName.c_str()))
		{
			Transform* transform = entities[i]->GetTransform();
			XMFLOAT3 position = transform->GetPosition();
			XMFLOAT3 rotation = transform->GetRotation();
			XMFLOAT3 scale = transform->GetScale();

			if (ImGui::DragFloat3("Position", &position.x, 0.1f))
			{
				transform->SetPosition(position);
			}

			if (ImGui::DragFloat3("Rotation", &rotation.x, 0.1f))
			{
				transform->SetRotation(rotation);
			}

			if (ImGui::DragFloat3("Scale", &scale.x, 0.1f))
			{
				transform->SetScale(scale);
			}

			ImGui::TreePop();
		}
		ImGui::PopID();
	}

	if (ImGui::TreeNode("Cameras")) {

		for (int i = 0; i < cameras.size(); i++)
		{
			std::string cameraName = "Camera " + std::to_string(i + 1);

			// Update camera based on user input
			if (ImGui::RadioButton(cameraName.c_str(), activeCameraIndex == i))
			{
				activeCameraIndex = i;
			}
		}

		if (!cameras.empty() && activeCameraIndex >= 0 && activeCameraIndex < cameras.size())
		{
			std::shared_ptr<Camera> activeCamera = cameras[activeCameraIndex];

			ImGui::Separator();

			ImGui::Text("Active Camera Details: %d", activeCameraIndex + 1);

			// Display the active camera's position and rotation
			DirectX::XMFLOAT3 position = activeCamera->GetTransform()->GetPosition();

			ImGui::Text("Position: (%.2f, %.2f, %.2f)", position.x, position.y, position.z);

			// Display the field of view
			ImGui::Text("Field of View: %.2f degrees", DirectX::XMConvertToDegrees(activeCamera->GetFieldOfView()));

		}

		ImGui::TreePop();
	}

	ImGui::End(); // Ends the current window
}

// Helper method to load shaders
Microsoft::WRL::ComPtr<ID3D11PixelShader> Game::LoadPixelShader(const std::wstring& file)
{
	Microsoft::WRL::ComPtr<ID3D11PixelShader> ps;
	Microsoft::WRL::ComPtr<ID3DBlob> blob;
	D3DReadFileToBlob(FixPath(file).c_str(), blob.GetAddressOf());
	Graphics::Device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), 0, ps.GetAddressOf());
	
	return ps;
}



