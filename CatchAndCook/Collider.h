#pragma once

#include "ColliderManager.h"
#include "Component.h"
#include "Game.h"



enum class CollisionType
{
	Box,
	Sphere,
	Frustum
};

union BoundingUnion
{
	BoundingOrientedBox box;
	BoundingSphere sphere;
	BoundingFrustum frustum;
};

struct BoundingData
{
	CollisionType type;
	BoundingUnion bound;
	BoundingData();
	BoundingData(const CollisionType& type, const BoundingUnion& bounding);
	~BoundingData();
	Vector3 GetCenter();
	Vector3 SetCenter(const Vector3& center);
	float GetRadius();
};



class Collider : public Component
{
public:
	Collider();
	~Collider() override;
	bool IsExecuteAble() override;
	void Init() override;
	void Start() override;
	void Update() override;
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider,const std::shared_ptr<Collider>& other) override;
	void SetDestroy() override;
	void Destroy() override;


public:
	CollisionType GetType() { return _type; }
	int GetGroupID() const { return groupId; }
	vec3 GetCenter();
	bool CheckCollision(const std::shared_ptr<Collider>& other);
	bool CheckCollision(const CollisionType& type, const BoundingUnion& bound);
	bool CheckCollision(const BoundingData& bound);

	bool RayCast(const Ray& ray, const float& dis, RayHit& hit);
	void SetBoundingBox(vec3 center,vec3 extents);
	void SetBoundingSphere(vec3 center,float radius);
	void SetBoundingFrustum(BoundingFrustum& boundingFrustum);
	void CalculateBounding();
	bool SetTrigger(bool trigger) { return isTrigger = trigger; };
	bool IsTrigger() { return isTrigger; };
	
	pair<vec3, vec3> GetMinMax();
	BoundingUnion& GetBoundUnion() {return _bound;}
	CollisionType GetBoundType() { return _type; }

	BoundingData GetBoundingData() { return { _type, _bound }; };

	Vector3 GetBoundCenter();

	static Vector3 GetContactPoint(const BoundingData& data1, const BoundingData& data2);
	static Vector3 GetContactPoint(const BoundingOrientedBox& obb, const BoundingSphere& sphere);
	static Vector3 GetContactPoint(const BoundingSphere& a, const BoundingSphere& b);
	static Vector3 GetContactPoint(const BoundingOrientedBox& a, const BoundingOrientedBox& b);

private:
	friend class ColliderManager;

private:
	CollisionType _type;
	BoundingUnion _orgin;
	BoundingUnion _bound;

	bool isTrigger = false;

	int groupId = 0;
	std::weak_ptr<GameObject> groupRootObject;
};
