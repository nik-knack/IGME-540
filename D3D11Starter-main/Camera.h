#pragma once
#include "Transform.h"

class Camera
{
private:
	Transform transform;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;
	float fov;
	float nearClip;
	float farClip;
	float moveSpeed;
	float mouseLookSpeed;

public:
	// Constructor + Destructor
	Camera(float aspectRatio,DirectX::XMFLOAT3 position);
	~Camera();

	// Getters
	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();

	// Update functions
	void UpdateProjectionMatrix(float aspectRatio);
	void UpdateViewMatrix(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3 up);	
	void Update(float dt);
};

