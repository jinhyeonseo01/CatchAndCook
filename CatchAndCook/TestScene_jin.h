#pragma once
class TestScene_jin : public Scene
{
public:
	void Init() override;
	void Update() override;
	void RenderBegin() override;
	void Rendering() override;
	void DebugRendering() override;
	void RenderEnd() override;
	void Finish() override;
	~TestScene_jin() override;
};

