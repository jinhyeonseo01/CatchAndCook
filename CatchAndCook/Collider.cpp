#include "pch.h"
#include "Collider.h"
#include "ColliderManager.h"
#include "GameObject.h"
#include "Gizmo.h"
#include "PhysicsComponent.h"
#include "simple_mesh_ext.h"
#include "Transform.h"


BoundingData::BoundingData() : bound(BoundingOrientedBox{})
{

}

BoundingData::BoundingData(const CollisionType& type, const BoundingUnion& bounding) : type(type), bound(bounding)
{

}

BoundingData::~BoundingData()
{
	if (type == CollisionType::Box)
		bound.box.~BoundingOrientedBox();
	if (type == CollisionType::Frustum)
		bound.frustum.~BoundingFrustum();
	if (type == CollisionType::Sphere)
		bound.sphere.~BoundingSphere();
}

Vector3 BoundingData::GetCenter()
{
	Vector3 center;
	if (type == CollisionType::Box)
		center = bound.box.Center;
	if (type == CollisionType::Sphere)
		center = bound.sphere.Center;
	return center;
}

float BoundingData::GetRadius()
{
	float radius = 0;
	if (type == CollisionType::Box)
		radius = Vector3(bound.box.Extents).Length();
	if (type == CollisionType::Sphere)
		radius = bound.sphere.Radius;
	return radius;
}

Vector3 BoundingData::SetCenter(const Vector3& center)
{
	if (type == CollisionType::Box)
		bound.box.Center = center;
	if (type == CollisionType::Sphere)
		bound.sphere.Center = center;
	return center;
}

Collider::Collider() : _orgin(BoundingOrientedBox()), _bound(BoundingOrientedBox())
{

}

Collider::~Collider()
{

}

bool Collider::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void Collider::Init()
{
	Component::Init();
}

void Collider::Start()
{
	Component::Start();

	groupId = PhysicsComponent::GetPhysicsGroupID(GetOwner());
	if (auto obj = GetOwner()->GetComponentWithParents<PhysicsComponent>()) {
		groupRootObject = obj->GetOwner();
	}

	if (GetOwner()->GetType() == GameObjectType::Static && GetOwner()->GetActive())
	{
		CalculateBounding();
		ColliderManager::main->AddCollider(GetCast<Collider>());
	}

}

void Collider::Update()
{
	Component::Update();

	CalculateBounding();
	ColliderManager::main->AddCollider(GetCast<Collider>());
}

void Collider::Update2()
{
	Component::Update2();

	CalculateBounding();
}

void Collider::Enable()
{
	Component::Enable();

	ColliderManager::main->AddColliderForRay(GetCast<Collider>());

	if (GetOwner()->GetType() == GameObjectType::Static)
		CalculateBounding();
	
	groupId = PhysicsComponent::GetPhysicsGroupID(GetOwner());
	if (auto obj = GetOwner()->GetComponentWithParents<PhysicsComponent>()) {
		groupRootObject = obj->GetOwner();
	}
}

void Collider::Disable()
{
	Component::Disable();

	ColliderManager::main->RemoveCollider(GetCast<Collider>());
	ColliderManager::main->RemoveAColliderForRay(GetCast<Collider>());
}

void Collider::RenderBegin()
{
	Component::RenderBegin();

	bool isCollision = ColliderManager::main->IsCollision(GetCast<Collider>());

	if (HasGizmoFlag(Gizmo::main->_flags, GizmoFlags::Collision))
	{

		if (_type == CollisionType::Box)
		{
			Gizmo::Width(0.1f);
			Gizmo::Box(_bound.box, !isCollision ? Vector4(0, 1, 0, 1) : Vector4(1, 0.5, 0, 1));
			Gizmo::WidthRollBack();
		}

		else if (_type == CollisionType::Sphere)
		{
			Gizmo::Width(0.1f);
			Gizmo::Sphere(_bound.sphere, !isCollision ? Vector4(0, 1, 0, 1) : Vector4(1, 0.5, 0, 1));
			Gizmo::WidthRollBack();
		}

		else if (_type == CollisionType::Frustum)
		{
			Gizmo::Width(0.1f);
			Gizmo::Frustum(_bound.frustum, !isCollision ? Vector4(0, 1, 0, 1) : Vector4(1, 0.5, 0, 1));
			Gizmo::WidthRollBack();
		}
	}
}

void Collider::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);

}

void Collider::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void Collider::SetDestroy()
{
	Component::SetDestroy();
}

void Collider::Destroy()
{
	Component::Destroy();
	cout << "DESTORY" << endl;
	ColliderManager::main->RemoveCollider(GetCast<Collider>());

}


vec3 Collider::GetCenter()
{
	if (_type == CollisionType::Box)
		return _bound.box.Center;
	else if (_type == CollisionType::Sphere)
		return _bound.sphere.Center;
	else if (_type == CollisionType::Frustum)
		return _bound.frustum.Origin;
}

bool Collider::CheckCollision(const std::shared_ptr<Collider>& other)
{
	if (_type == CollisionType::Box)
	{
		if (other->_type == CollisionType::Box)
		{
			return _bound.box.Intersects(other->_bound.box);
		}
		else if (other->_type == CollisionType::Sphere)
		{
			return _bound.box.Intersects(other->_bound.sphere);
		}
		else if (other->_type == CollisionType::Frustum)
		{
			return _bound.box.Intersects(other->_bound.frustum);
		}

	}
	else if (_type == CollisionType::Sphere)
	{
		if (other->_type == CollisionType::Box)
		{
			return _bound.sphere.Intersects(other->_bound.box);
		}
		else if (other->_type == CollisionType::Sphere)
		{
			return _bound.sphere.Intersects(other->_bound.sphere);
		}
		else if (other->_type == CollisionType::Frustum)
		{
			return _bound.sphere.Intersects(other->_bound.frustum);
		}
	}

	else if (_type == CollisionType::Frustum)
	{
		if (other->_type == CollisionType::Box)
		{
			return _bound.frustum.Intersects(other->_bound.box);
		}
		else if (other->_type == CollisionType::Sphere)
		{
			return _bound.frustum.Intersects(other->_bound.sphere);
		}
		else if (other->_type == CollisionType::Frustum)
		{
			return _bound.frustum.Intersects(other->_bound.frustum);
		}


	}

	return false;
}

bool Collider::CheckCollision(const CollisionType& type, const BoundingUnion& bound)
{
	if (_type == CollisionType::Box)
	{
		if (type == CollisionType::Box)
		{
			return _bound.box.Intersects(bound.box);
		}
		else if (type == CollisionType::Sphere)
		{
			return _bound.box.Intersects(bound.sphere);
		}
		else if (type == CollisionType::Frustum)
		{
			return _bound.box.Intersects(bound.frustum);
		}

	}
	else if (_type == CollisionType::Sphere)
	{
		if (type == CollisionType::Box)
		{
			return _bound.sphere.Intersects(bound.box);
		}
		else if (type == CollisionType::Sphere)
		{
			return _bound.sphere.Intersects(bound.sphere);
		}
		else if (type == CollisionType::Frustum)
		{
			return _bound.sphere.Intersects(bound.frustum);
		}
	}

	else if (_type == CollisionType::Frustum)
	{
		if (type == CollisionType::Box)
		{
			return _bound.frustum.Intersects(bound.box);
		}
		else if (type == CollisionType::Sphere)
		{
			return _bound.frustum.Intersects(bound.sphere);
		}
		else if (type == CollisionType::Frustum)
		{
			return _bound.frustum.Intersects(bound.frustum);
		}


	}

	return false;
}

bool Collider::CheckCollision(const BoundingData& bound)
{
	return CheckCollision(bound.type, bound.bound);
}

bool Collider::RayCast(const Ray& ray, const float& dis, RayHit& hit)
{
	hit.distance = dis;
	hit.collider = this;
	hit.gameObject = GetOwner();

	////// _bound.box를 지역 변수로 캐싱하여 반복 접근을 줄입니다.

	if (_type == CollisionType::Box)
	{
		const auto& box = _bound.box;
		if (ray.Intersects(box, hit.distance))
		{
			// 월드 좌표계에서 충돌 지점을 계산합니다.
			hit.worldPos = ray.position + ray.direction * hit.distance;
			// 박스의 로컬 좌표계로 충돌 지점을 변환합니다.
			// 로컬 좌표계에서는 박스가 축 정렬되어 있으므로 면들은 x, y, z 축에 평행합니다.
			const Quaternion orientation = box.Orientation;
			Quaternion invOrientation;
			orientation.Inverse(invOrientation);
			const Vector3 localHitPoint = Vector3::Transform(hit.worldPos - box.Center, invOrientation);

			// Determine on which face the hit occurred.
			// Since the box is axis-aligned in local space, one coordinate will be nearly equal to the corresponding extent.
			const float diffX = fabsf(fabs(localHitPoint.x) - box.Extents.x);
			const float diffY = fabsf(fabs(localHitPoint.y) - box.Extents.y);
			const float diffZ = fabsf(fabs(localHitPoint.z) - box.Extents.z);

			// 세 축 중 차이가 가장 작은 축이 충돌 면임을 판단합니다.
			if (diffX < diffY && diffX < diffZ)
			{
				// X면에서 충돌이 발생함 (로컬 X 좌표의 부호에 따라 결정)
				hit.normal = Vector3((localHitPoint.x > 0.0f) ? 1.0f : -1.0f, 0.0f, 0.0f);
			}
			else if (diffY < diffZ)
			{
				// Y면에서 충돌이 발생함
				hit.normal = Vector3(0.0f, (localHitPoint.y > 0.0f) ? 1.0f : -1.0f, 0.0f);
			}
			else
			{
				// Z면에서 충돌이 발생함
				hit.normal = Vector3(0.0f, 0.0f, (localHitPoint.z > 0.0f) ? 1.0f : -1.0f);
			}

			// 로컬 좌표계의 법선을 월드 좌표계로 변환합니다.
			hit.normal = Vector3::Transform(hit.normal, orientation);
			hit.normal.Normalize();
			hit.isHit = true;
			return true;
		}
	}

	if (_type == CollisionType::Sphere)
	{
		const auto& sphere = _bound.sphere;
		// Ray와 구(BoundingSphere) 간의 충돌 여부를 테스트합니다.
		if (ray.Intersects(sphere, hit.distance))
		{
			// 월드 좌표계에서 충돌 지점을 계산합니다.
			hit.worldPos = ray.position + ray.direction * hit.distance;
			// 구의 경우, 충돌한 표면의 법선은 (충돌 지점 - 구 중심)을 정규화한 값입니다.
			hit.normal = hit.worldPos - sphere.Center;
			hit.normal.Normalize();
			hit.isHit = true;
			return true;
		}
	}

	hit.isHit = false;
	return false;
}

void Collider::SetBoundingBox(vec3 center, vec3 extents)
{
	_type = CollisionType::Box;

	_orgin.box = BoundingOrientedBox(center, extents, Quaternion::Identity);
	_bound.box = BoundingOrientedBox(center, extents, Quaternion::Identity);

}

void Collider::SetBoundingSphere(vec3 center, float radius)
{
	_type = CollisionType::Sphere;
	_orgin.sphere = BoundingSphere(center, radius);
	_bound.sphere = BoundingSphere(center, radius);




}

void Collider::SetBoundingFrustum(BoundingFrustum& boundingFrustum)
{
	_type = CollisionType::Frustum;
	_orgin.frustum = boundingFrustum;
	_bound.frustum = boundingFrustum;
}

void Collider::CalculateBounding()
{
	if (_type == CollisionType::Box)
	{
		auto onwerTransform = GetOwner()->_transform;
		Matrix mat;
		onwerTransform->GetLocalToWorldMatrix_BottomUp(mat);

		if (onwerTransform->IsLocalToWorldChanged())
		{
			_orgin.box.Transform(_bound.box, mat);
			//_bound.box = TransformBoundingOrientedBox(_orgin.box, mat, onwerTransform->GetWorldScale(), onwerTransform->GetWorldRotation());
		}
	}

	else if (_type == CollisionType::Sphere)
	{
		auto onwerTransform = GetOwner()->_transform;
		Matrix mat;
		onwerTransform->GetLocalToWorldMatrix_BottomUp(mat);
		_orgin.sphere.Transform(_bound.sphere, mat);
	}

	else if (_type == CollisionType::Frustum)
	{
		auto onwerTransform = GetOwner()->_transform;
		Matrix mat;
		onwerTransform->GetLocalToWorldMatrix_BottomUp(mat);
		_orgin.frustum.Transform(_bound.frustum, mat);
	}
}

pair<vec3, vec3> Collider::GetMinMax()
{
	if (_type == CollisionType::Box)
	{
		auto rot = Matrix::CreateFromQuaternion(_bound.box.Orientation);
		vec3 center = _bound.box.Center;
		vec3 extent = _bound.box.Extents;

		// 2) 행렬 원소의 절댓값을 이용해 새 extents 계산
		vec3 abs0 = vec3(fabs(rot.m[0][0]), fabs(rot.m[0][1]), fabs(rot.m[0][2]));
		vec3 abs1 = vec3(fabs(rot.m[1][0]), fabs(rot.m[1][1]), fabs(rot.m[1][2]));
		vec3 abs2 = vec3(fabs(rot.m[2][0]), fabs(rot.m[2][1]), fabs(rot.m[2][2]));

		vec3 newExtents;
		newExtents.x = abs0.x * extent.x + abs1.x * extent.y + abs2.x * extent.z;
		newExtents.y = abs0.y * extent.x + abs1.y * extent.y + abs2.y * extent.z;
		newExtents.z = abs0.z * extent.x + abs1.z * extent.y + abs2.z * extent.z;

		// 3) AABB 구성
		vec3 worldMin = center - newExtents;
		vec3 worldMax = center + newExtents;
		return { worldMin, worldMax };
	}

	else if (_type == CollisionType::Sphere)
	{
		//return std::make_pair(_bound.sphere.Center - vec3(_bound.sphere.Radius), _bound.sphere.Center + vec3(_bound.sphere.Radius));
		vec3 c = _bound.sphere.Center;
		vec3 r = vec3(_bound.sphere.Radius);
		return { c - r, c + r };
	}

	else if (_type == CollisionType::Frustum)
	{
		return std::make_pair(vec3(0, 0, 0), vec3(0, 0, 0));
	}

	return std::make_pair(vec3(0, 0, 0), vec3(0, 0, 0));
}

Vector3 Collider::GetBoundCenter()
{
	if (_type == CollisionType::Box)
	{
		return _bound.box.Center;
	}
	if (_type == CollisionType::Sphere)
	{
		return _bound.sphere.Center;
	}
	return Vector3::Zero;
}

Vector3 Collider::GetContactPoint(const BoundingData& data1, const BoundingData& data2)
{
	Vector3 hitPosition;
	if (data1.type == CollisionType::Sphere && data2.type == CollisionType::Box)
		hitPosition = Collider::GetContactPoint(data2.bound.box, data1.bound.sphere);
	if (data1.type == CollisionType::Sphere && data2.type == CollisionType::Sphere)
		hitPosition = Collider::GetContactPoint(data2.bound.sphere, data1.bound.sphere);
	if (data1.type == CollisionType::Box && data2.type == CollisionType::Box)
		hitPosition = Collider::GetContactPoint(data2.bound.box, data1.bound.box);
	if (data1.type == CollisionType::Box && data2.type == CollisionType::Sphere)
		hitPosition = Collider::GetContactPoint(data1.bound.box, data2.bound.sphere);
	return hitPosition;
}

Vector3 Collider::GetContactPoint(const BoundingOrientedBox& obb, const BoundingSphere& sphere)
{
	// 1) 구 중심을 OBB 로컬 공간으로 변환
	Quaternion q = obb.Orientation;
	Quaternion invQ;
	q.Conjugate(invQ);
	Vector3   delta = sphere.Center - obb.Center;
	Vector3   local = Vector3::Transform(delta, invQ);

	// 2) 로컬 축별로 Extents 범위 안에 클램프 → AABB 기준 최단 접점
	Vector3 e = obb.Extents;
	Vector3 clamped(
		std::max(-e.x, std::min(local.x, e.x)),
		std::max(-e.y, std::min(local.y, e.y)),
		std::max(-e.z, std::min(local.z, e.z))
	);

	// 3) 클램프된 로컬 접점을 다시 월드 공간으로 복원
	Vector3 worldPoint = obb.Center + Vector3::Transform(clamped, q);

	return worldPoint;
}

Vector3 Collider::GetContactPoint(const BoundingSphere& a, const BoundingSphere& b)
{
	Vector3 delta = b.Center - a.Center;
	float   dist = delta.Length();

	// 중심이 완전히 겹쳐 있으면 a.Center 를 반환
	if (dist < 1e-6f)
		return a.Center;

	// a 표면 상 가장 가까운 점
	return a.Center + (delta / dist) * a.Radius;
}

Vector3 Collider::GetContactPoint(const BoundingOrientedBox& a, const BoundingOrientedBox& b)
{


	Vector3 ptA = GetContactPoint(a, BoundingSphere(b.Center, 0.0f));
	// 2) B의 표면에서 A의 중심에 가장 가까운 점
	Vector3 ptB = GetContactPoint(b, BoundingSphere(a.Center, 0.0f));
	// 3) 두 점의 중간을 접점으로 반환
	return (ptA + ptB) * 0.5f;
	
}
