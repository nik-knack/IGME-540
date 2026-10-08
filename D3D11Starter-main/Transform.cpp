#include "Transform.h"

using namespace DirectX;

Transform::Transform() :
	position(0, 0, 0),
	pitchYawRoll	(0, 0, 0),
	scale(1, 1, 1),
	dirty(false)
{
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
	XMVECTOR offset = XMVectorSet(x, y, z, 0.0f);

	XMVECTOR rotation = XMQuaternionRotationRollPitchYaw(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z);

	XMVECTOR rotatedOffset = XMVector3Rotate(offset, rotation);	

	XMVECTOR currentPosition = XMLoadFloat3(&position);

	currentPosition += rotatedOffset;
	XMStoreFloat3(&position, currentPosition);
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
	XMVECTOR directionVector = XMLoadFloat3(&direction);

	XMVECTOR rotation = XMQuaternionRotationRollPitchYaw(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z);

	directionVector = XMVector3Rotate(directionVector, rotation);

	XMFLOAT3 result;
	XMStoreFloat3(&result, directionVector	);
	
	return result;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if (dirty) {
		
		XMMATRIX scaleMatrix = XMMatrixScaling(
			scale.x, scale.y, scale.z);
		XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(
			pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z);
		XMMATRIX translationMatrix = XMMatrixTranslation(
			position.x, position.y, position.z);

		XMMATRIX worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
		XMStoreFloat4x4(&world, worldMatrix);
		XMStoreFloat4x4(
			&worldInverseTranspose,
			XMMatrixInverse(0, XMMatrixTranspose(worldMatrix))
		);

		dirty = false;
	}
	
	return world;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	return worldInverseTranspose;
}
