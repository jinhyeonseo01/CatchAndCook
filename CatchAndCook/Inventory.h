#pragma once

#include "Component.h"
#include "RendererBase.h"

class Sprite;

class Inventory : public Sprite
{
public:
	Inventory();
	virtual ~Inventory();

public:
	void Init() override;
	void Start() override;
	void Update()override;
	void Update2()override;
	void Enable()override;
	void Disable()override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void SetDestroy() override;
	void Destroy() override;
	void SetData(Material* material = nullptr) override;

public:
	void AddItem(const std::shared_ptr<Texture>& itemTexture);

private:
	vec2 _startPos = { 35,60 };
	vec2 _cellsize = { 35,35 };
	int32 _itemCount = 0;

private:
	vector<shared_ptr<GameObject>> _itemList;

};

