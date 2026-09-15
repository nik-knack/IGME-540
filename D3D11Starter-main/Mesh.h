#pragma once

#include <d3d11.h>
#include <wrl/client.h>

class Mesh
{
public:
	Mesh();
	~Mesh();

	// Primary functions
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBufer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();

	unsigned int GetVertexCount();
	unsigned int GetIndexCount();
	
	void Draw(float deltaTime, float totalTime);


private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	
	unsigned int indexCount;
	unsigned int vertexCount;
};

