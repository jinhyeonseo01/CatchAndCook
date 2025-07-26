#pragma once
class MainMenuScene : public Scene
{
public:
	void Init() override;
	void Update() override;
	void RenderBegin() override;
	void Rendering() override;
	void DebugRendering() override;
	void RenderEnd() override;
	void Finish() override;
	~MainMenuScene() override;

	void ComputePass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList) override;
};
#pragma once
