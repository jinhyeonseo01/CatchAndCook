#pragma once
#include "RendererBase.h"
#include "Component.h"

class ParticleComponent;

class ParticleRenderer : public Component,  public RendererBase
{

public:
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

public:
	void SetScreenSpace(bool b) { _screenSpace = b; }
	void SetTexture(shared_ptr<Texture> texture) { _particleTexture = texture; }


private:
	virtual void Rendering(Material* material, Mesh* mesh, int instanceCount = 1);
	virtual void DebugRendering();

private:
	bool _screenSpace = false;
	TableContainer _tableContainer{};

	static shared_ptr<Shader> _particleComputeShader;
	static shared_ptr<Shader> _particleRenderingShader;
	static shared_ptr<Shader> _particleRenderingShader2D;
	shared_ptr<ParticleComponent> _particleComponent;
	shared_ptr<Texture> _particleTexture;



};

