#include "Transform.h"

using namespace DirectX;

Transform::Transform() :
	position(0, 0, 0),
	pitchYawRoll	(0, 0, 0),
	scale(1, 1, 1),
	dirty(false)
{
	// Start with an identity world matrix and world inverse transpose matrix
	XMStoreFloat4x4(&world,XMMatrixIdentity());
	XMStoreFloat4x4(&worldInverseTranspose, XMMatrixIdentity());
}

Transform::~Transform()
{
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;

	// The position changed, so the world matrix needs to be recalculated
	dirty = true;
}

void Transform::MoveAbsolute(DirectX::XMFLOAT3 offset)
{
	position.x += offset.x;
	position.y += offset.y;
	position.z += offset.z;

	dirty = true;
}

void Transform::MoveRelative(float x, float y, float z)
{
	// Create a vector representing the offset in local space
	XMVECTOR offset = XMVectorSet(x, y, z, 0.0f);

	// Convert the pitch, yaw, and roll angles to a quaternion for rotation
	XMVECTOR rotation = XMQuaternionRotationRollPitchYaw(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z);

	// Converts the movement from local space to world space
	XMVECTOR rotatedOffset = XMVector3Rotate(offset, rotation);	

	XMVECTOR currentPosition = XMLoadFloat3(&position);

	// Apply the rotated offset to the current position
	currentPosition += rotatedOffset;
	XMStoreFloat3(&position, currentPosition);

	// The position changed, so the world matrix needs to be recalculated
	dirty = true;
}

void Transform::MoveRelative(DirectX::XMFLOAT3 offset)
{
	MoveRelative(offset.x, offset.y, offset.z);
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	pitchYawRoll.x += pitch;
	pitchYawRoll.y += yaw;
	pitchYawRoll.z += roll;

	// The rotation changed, so the world matrix needs to be recalculated
	dirty = true;
}

void Transform::Rotate(DirectX::XMFLOAT3 rotation)
{
	this->pitchYawRoll.x += rotation.x;
	this->pitchYawRoll.y += rotation.y;
	this->pitchYawRoll.z += rotation.z;

	dirty = true;
}

void Transform::Scale(float x, float y, float z)
{
	scale.x *= x;
	scale.y *= y;
	scale.z *= z;

	// The scale changed, so the world matrix needs to be recalculated
	dirty = true;
}

void Transform::Scale(DirectX::XMFLOAT3 scale)
{
	this->scale.x *= scale.x;
	this->scale.y *= scale.y;	
	this->scale.z *= scale.z;

	dirty = true;
}

void Transform::SetPosition(float x, float y, float z)
{
	position = XMFLOAT3(x, y, z);

	dirty = true;
}

void Transform::SetPosition(DirectX::XMFLOAT3 position)
{
	this->position = position;

	dirty = true;
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	pitchYawRoll = XMFLOAT3(pitch, yaw, roll);	

	dirty = true;	
}

void Transform::SetRotation(DirectX::XMFLOAT3 pitchYawRoll)
{
	this->pitchYawRoll = pitchYawRoll;

	dirty = true;
}

void Transform::SetScale(float x, float y, float z)
{
	scale = XMFLOAT3(x, y, z);

	dirty = true;
}

void Transform::SetScale(DirectX::XMFLOAT3 scale)
{
	this->scale = scale;

	dirty = true;
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
	return position;
}

DirectX::XMFLOAT3 Transform::GetRotation()
{
	return pitchYawRoll;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	return scale;
}

DirectX::XMFLOAT3 Transform::GetRight()
{	
	return GetDirection(XMFLOAT3(1.0f, 0.0f, 0.0f));
}

DirectX::XMFLOAT3 Transform::GetUp()
{
	return GetDirection(XMFLOAT3(0.0f, 1.0f, 0.0f));
}

DirectX::XMFLOAT3 Transform::GetForward()
{
	return GetDirection(XMFLOAT3(0.0f, 0.0f, 1.0f));
}

DirectX::XMFLOAT3 Transform::GetDirection(DirectX::XMFLOAT3 direction)
{
	// Convert the XMFLOAT3 into a XMVECTOR
	XMVECTOR directionVector = XMLoadFloat3(&direction);

	// Convert the Euler rotation to a quaternion
	XMVECTOR rotation = XMQuaternionRotationRollPitchYaw(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z);

	// Rotate the direction local space to world space
	directionVector = XMVector3Rotate(directionVector, rotation);

	// Convert the result back into an XMFLOAT3
	XMFLOAT3 result;
	XMStoreFloat3(&result, directionVector);
	
	return result;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	// Only rebuild the matrix if something has changed since the previous calculation
	if (dirty) {
		// Create the scale, rotation, and translation matrices
		XMMATRIX scaleMatrix = XMMatrixScaling(
			scale.x, scale.y, scale.z);
		XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(
			pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z);
		XMMATRIX translationMatrix = XMMatrixTranslation(
			position.x, position.y, position.z);

		// Combine the matrices to form the world matrix
		XMMATRIX worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
		XMStoreFloat4x4(&world, worldMatrix);

		// Calculate the world inverse transpose matrix for normal transformation
		XMStoreFloat4x4(
			&worldInverseTranspose,
			XMMatrixInverse(0, XMMatrixTranspose(worldMatrix))
		);

		// The cached matricces are now up to date
		dirty = false;
	}
	
	return world;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	return worldInverseTranspose;
}
