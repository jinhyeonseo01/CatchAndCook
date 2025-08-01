#pragma once
#include "Component.h"
class ShowGameMoeny :public Component
{

public:
	ShowGameMoeny();
	virtual ~ShowGameMoeny();
	void Init() override;
	void Start() override;
	void Update() override;
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void SetDestroy() override;
	void Destroy() override;


private:
	int prevMoney = -1;
	shared_ptr<GameObject>  _text;
};

