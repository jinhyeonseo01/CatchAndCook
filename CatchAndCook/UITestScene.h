#pragma once
#include "Scene.h"
class UITestScene :public Scene
{
public:
	virtual void Init();
	virtual void Update();
	virtual void RenderBegin();
	virtual void Rendering();
	virtual void RenderEnd();
	virtual void Finish();
};


