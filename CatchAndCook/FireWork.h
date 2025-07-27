#pragma once
#include "Component.h"
#include "FireBall.h"



class FireWork :public Component
{

public:
	~FireWork() override;

	void Init() override;
	void Start() override;
	void Update() override;
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current) override;
	void SetDestroy() override;
	void Destroy() override;

public: 
	void Shot() { _on = true; _currentTime = _fireBallResponTime; _fireBallCount = 0; }
	bool IsDone() const { return _fireBallCount == 0; }
private:
	void SpawnFireBall();

private:

	float _currentTime = 3.0f;
	const float _fireBallResponTime = 3.0f;
	bool _on =false;

	static const int _maxFireBall = 5;
	static const int _maxColor = 3;
	int _fireBallCount = 0;



};

