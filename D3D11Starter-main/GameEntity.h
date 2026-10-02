#pragma once
#include "Transform.h"
#include "Mesh.h"
#include <memory>

class GameEntity
{
private: 
	std::shared_ptr<Mesh> mesh;
	Transform transform;

public:
	GameEntity(std::shared_ptr<Mesh> mesh);

	std::shared_ptr<Mesh> GetMesh();
	Transform* GetTransform();

	void Draw();
};
