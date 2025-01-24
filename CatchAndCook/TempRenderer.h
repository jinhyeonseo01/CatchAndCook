#pragma once

#include "Component.h"
#include "RendererBase.h"

class Material;
class Mesh;
class Shader;

class TempRenderer : public Component, public RendererBase
{

public:
	~TempRenderer() override;

	bool IsExecuteAble() override;
	void Init() override;
	void Start() override;
	void Update() override;
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void Destroy() override;
	void RenderBegin() override;
	void Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void DebugRendering() override;
	void SetDestroy() override;
	void DestroyComponentOnly() override;

	void Rendering(const std::shared_ptr<Material>& material) override;

	void SetMesh(const std::shared_ptr<Mesh>& _mesh);
	void SetMaterials(const std::vector<std::shared_ptr<Material>>& _materials);
	void AddMaterials(const std::vector<std::shared_ptr<Material>>& _materials);


private:
	std::shared_ptr<Mesh> _mesh;
	std::vector<std::shared_ptr<Material>> _materials;

	shared_ptr<Shader> _debugShader;






};

