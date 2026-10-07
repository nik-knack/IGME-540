#include "Camera.h"

Camera::Camera(float aspectRatio, DirectX::XMFLOAT3 position)
{
}

Camera::~Camera()
{
}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
    return DirectX::XMFLOAT4X4();
}

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
    return DirectX::XMFLOAT4X4();
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
}

void Camera::UpdateViewMatrix(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3 up)
{
}

void Camera::Update(float dt)
{
}
