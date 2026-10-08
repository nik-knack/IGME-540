#include "Camera.h"
#include "Input.h"

using namespace DirectX;

Camera::Camera(float aspectRatio, float x, float y, float z) :
    fieldOfView(XMConvertToRadians(60.0f)),
    nearClip(0.1f),
    farClip(100.0f),
    moveSpeed(2.0f),
    mouseLookSpeed(0.000001f)
{
    // Set the initial camera position
    transform.SetPosition(x, y, z);

	// Build the initial view and projection matrices based on the camera's position and orientation
    UpdateViewMatrix();
	UpdateProjectionMatrix(aspectRatio);
}

Camera::~Camera()
{
}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
    return viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
    return projectionMatrix;
}

Transform* Camera::GetTransform()
{
    return &transform;
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	// Rebuild the camera's projection matrix based on the current field of view, aspect ratio, and near/far clipping planes
    XMMATRIX projection = XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearClip, farClip);

    XMStoreFloat4x4(&projectionMatrix, projection);
}

void Camera::UpdateViewMatrix()
{
	// Get the camera's position and forward direction
    XMFLOAT3 position = transform.GetPosition();
	XMFLOAT3 forward = transform.GetForward();
	XMFLOAT3 up = transform.GetUp();

    // Convert the position and forward direction to XMVECTORs
	XMVECTOR cameraPosition = XMLoadFloat3(&position);
	XMVECTOR cameraForward = XMLoadFloat3(&forward);
	XMVECTOR cameraUp = XMLoadFloat3(&up);

	// Make the view matrix using the camera's position, forward direction, and world up vector
	XMMATRIX view = XMMatrixLookToLH(cameraPosition, cameraForward, cameraUp);
    XMStoreFloat4x4(&viewMatrix, view);
}

void Camera::Update(float dt)
{
	// Handle camera movement based on user input
    if (Input::KeyDown('W'))
    {
		transform.MoveRelative(0.0f, 0.0f, moveSpeed * dt);
    }

    if (Input::KeyDown('S'))
    {
        transform.MoveRelative(0.0f, 0.0f, -moveSpeed * dt);
    }

    if (Input::KeyDown('A'))
    {
        transform.MoveRelative(-moveSpeed * dt, 0.0f, 0.0f);
    }

    if (Input::KeyDown('D'))
    {
        transform.MoveRelative(moveSpeed * dt, 0.0f, 0.0f);
    }

    if (Input::KeyDown(VK_SPACE))
    {
        transform.MoveRelative(0.0f, moveSpeed * dt, 0.0f);
    }

    if (Input::KeyDown('X'))
    {
        transform.MoveRelative(0.0f, -moveSpeed * dt, 0.0f);
    }

	// Only rotate the camera when the left mouse button is held down
    if (Input::MouseLeftDown())
    {
		// Get the current mouse position
        float mouseX = Input::GetMouseX();
		float mouseY = Input::GetMouseY();


		// Apply mouse movement to the camera's rotation, scaled by the mouse look speed
        transform.Rotate(mouseY * mouseLookSpeed, mouseX * mouseLookSpeed, 0.0f);

		// Clamp the pitch rotation to prevent the camera from flipping upside down
		XMFLOAT3 rotation = transform.GetRotation();

        if (rotation.x > XM_PIDIV2)
		{
			rotation.x = XM_PIDIV2;
		}
		else if (rotation.x < -XM_PIDIV2)
		{
			rotation.x = -XM_PIDIV2;
		}

		// Apply the clamped rotation back to the camera's transform
		transform.SetRotation(rotation.x, rotation.y, rotation.z);
    }

	// Update the view matrix after handling movement and rotation
	UpdateViewMatrix();
}
