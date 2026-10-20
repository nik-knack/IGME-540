#pragma once
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include <memory>

class GameEntity
{
private: 
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
	Transform transform;

public:
	GameEntity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);

	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Material> GetMaterial();
	void SetMaterial(std::shared_ptr<Material> material);
	Transform* GetTransform();

	void Draw();
};
