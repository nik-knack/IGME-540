#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "Vertex.h"

class Mesh
{
public:
	Mesh(const Vertex* vertices, unsigned int vertexCount, const unsigned int* indices, unsigned int indexCount);
	~Mesh();

	// Primary functions
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBufer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();

	unsigned int GetVertexCount();
	unsigned int GetIndexCount();
	
	void Draw();


private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	
	unsigned int vertexCount;
	unsigned int indexCount;
};

