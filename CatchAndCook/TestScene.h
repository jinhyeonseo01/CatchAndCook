#pragma once
#include "Scene.h"

class TextHandle;

class TestScene : public Scene
{
public:
	virtual void Init();
	virtual void Update();
	virtual void RenderBegin();
	virtual void Rendering();
	virtual void RenderEnd();
	virtual void Finish();
	void DebugRendering() override;
	~TestScene() override;
	void UiPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList) override;
	void TransparentPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList) override;
	void ForwardPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList) override;
	void DeferredPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList) override;
	void ShadowPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList) override;
	void FinalRender(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList) override;
	void ComputePass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList) override;
	void SettingPrevData(RenderObjectStrucutre& data, const RENDER_PASS::PASS& pass) override;
};

