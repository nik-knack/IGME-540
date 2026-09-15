#include "Mesh.h"
#include "Graphics.h"

using namespace DirectX;

Mesh::Mesh(const Vertex* vertices, unsigned int vertexCount, const unsigned int* indices, unsigned int indexCount)
	: vertexCount(vertexCount), indexCount(indexCount)
{	
	// Create a VERTEX BUFFER
	{
		// First, we need to describe the buffer we want Direct3D to make on the GPU
		D3D11_BUFFER_DESC vbd = {};
		vbd.Usage = D3D11_USAGE_IMMUTABLE;	
		vbd.ByteWidth = sizeof(Vertex) * vertexCount;       
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER; 
		vbd.CPUAccessFlags = 0;	
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;

		// Create the proper struct to hold the initial vertex data
		D3D11_SUBRESOURCE_DATA initialVertexData = {};
		initialVertexData.pSysMem = vertices; 

		// Actually create the buffer on the GPU with the initial data
		Graphics::Device->CreateBuffer(&vbd, &initialVertexData, vertexBuffer.GetAddressOf());
	}

	// Create an INDEX BUFFER
	{
		// Describe the buffer, as we did above, with two major differences
		D3D11_BUFFER_DESC ibd = {};
		ibd.Usage = D3D11_USAGE_IMMUTABLE;	
		ibd.ByteWidth = sizeof(unsigned int) * indexCount;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;	
		ibd.CPUAccessFlags = 0;	
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;

		// Specify the initial data for this buffer, similar to above
		D3D11_SUBRESOURCE_DATA initialIndexData = {};
		initialIndexData.pSysMem = indices; 

		// Actually create the buffer with the initial data
		Graphics::Device->CreateBuffer(&ibd, &initialIndexData, indexBuffer.GetAddressOf());
	}
}

Mesh::~Mesh()
{
	// Leave this empty
}

Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetVertexBufer()
{
	return vertexBuffer.Get();
}

Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetIndexBuffer()
{
	return indexBuffer.Get();
}

unsigned int Mesh::GetVertexCount()
{
	return vertexCount;
}

unsigned int Mesh::GetIndexCount()
{
	return indexCount;
}

void Mesh::Draw()
{
	// DRAW geometry
	{
		// Set buffers in the input assembler (IA) stage
		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		Graphics::Context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
		Graphics::Context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		// Tell Direct3D to draw
		Graphics::Context->DrawIndexed(
			indexCount,     // The number of indices to use (we could draw a subset if we wanted)
			0,     
			0);    
	}
}
