#pragma once
#include "Scene.h"
class Scene_Sea01 :public Scene
{
public:
	void Init() override;
	void Update() override;
	void RenderBegin() override;
	void Rendering() override;
	void DebugRendering() override;
	void RenderEnd() override;
	void Finish() override;

	void SetSeaGlobalData();
	virtual void ShadowPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList);
	virtual void UiPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList);
	virtual void TransparentPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList);
	virtual void ForwardPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList);
	virtual void DeferredPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList);
	virtual void FinalRender(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList);
	virtual void ComputePass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList);
	void NoEffectPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList);
	void ParticlePass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList);


	shared_ptr<GameObject>& GetInteractiveBox() { return _interactiveBox; }

private:
	shared_ptr<Texture> caustics;
	shared_ptr<GameObject> _interactiveBox;
};

