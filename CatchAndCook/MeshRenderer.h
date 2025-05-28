#pragma once
#include "Component.h"
#include "RendererBase.h"


class ForwardLightSetter;
class Material;
class Mesh;
class Shader;

class MeshRenderer : public Component, public RendererBase
{
public:
	~MeshRenderer() override;

	//Component
	bool IsExecuteAble() override;
	void Init() override;
	void Start() override;
	void Update() override;
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void Destroy() override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void SetDestroy() override;

	//RendererBase
	void Rendering(Material* material, Mesh* mesh, int instanceCount = 1) override;
	void DebugRendering() override;

	//MeshRenderer
	void AddMesh(const std::shared_ptr<Mesh>& _mesh);
	void SetMaterials(const std::vector<std::shared_ptr<Material>>& _materials);
	void AddMaterials(const std::vector<std::shared_ptr<Material>>& _materials);
	void AddMaterial(shared_ptr<Material>& material);
	std::vector<std::shared_ptr<Material>>& GetMaterials();
	std::shared_ptr<Material> GetMaterial(int index = 0);
	void SetSharedMaterials(const std::vector<std::shared_ptr<Material>>& _materials);
	void AddSharedMaterials(const std::vector<std::shared_ptr<Material>>& _materials);

	void SetSpecialMaterials();

	std::vector<std::shared_ptr<Mesh>> _mesh;
	std::vector<std::shared_ptr<Material>> _uniqueMaterials;
	std::vector<std::shared_ptr<Material>> _sharedMaterials;
private:

	std::vector<std::pair<int, std::shared_ptr<Material>>> _depthNormalMaterials;
	std::vector<std::pair<int, std::shared_ptr<Material>>> _shadowMaterials;

	//std::shared_ptr<ForwardLightSetter> _setter_ForwardLight;
};

