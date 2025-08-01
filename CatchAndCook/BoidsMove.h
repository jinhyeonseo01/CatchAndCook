#pragma once
#include "Component.h"
class BoidsMove :public Component
{
public:
	BoidsMove();
	~BoidsMove();

	void SetDestroy() override;
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
	bool IsExecuteAble() override;

private:
	void UpdateBoids();

	void FindNeighbor();
	vec3 Separate();
	vec3 Align();
	vec3 Cohesion();
	vec3 LimitForce(vec3& force, float maxForce);

private:
	static vector<GameObject*> _objects;
	vector<GameObject*> _neighbors;

	const float _neighborDist = 10000.0f;		 // 이웃 판단 거리
	const float _desiredSeparation = 100.0f;   // 최소 거리 (분리 기준)
	const float _maxSpeed = 300.0f;            // 최대 속도
	const float _maxForce = 50.0f;             // 최대 가속도
	float _speed = 10.0f;


};

