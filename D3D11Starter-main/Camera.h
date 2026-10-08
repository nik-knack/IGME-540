#pragma once
#include "Transform.h"
#include <DirectXMath.h>

class Camera
{
private:
	Transform transform;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;
	float fieldOfView;
	float nearClip;
	float farClip;
	float moveSpeed;
	float mouseLookSpeed;

public:
	// Constructor + Destructor
	Camera(float aspectRatio,float x, float y, float z);
	~Camera();

	// Getters
	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();
	Transform* GetTransform();

	// Update functions
	void UpdateProjectionMatrix(float aspectRatio);
	void UpdateViewMatrix();	
	void Update(float dt);
};

