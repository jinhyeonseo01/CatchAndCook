#include "pch.h"
#include "ColliderManager.h"
#include <algorithm>
#include "Gizmo.h"
#include "Collider.h"
#include "Terrain.h"
#include "TerrainManager.h"

unique_ptr<ColliderManager> ColliderManager::main = nullptr;

vec3 ColliderManager::GetGridCell(const vec3& position) const
{
	return vec3(
		std::floor(position.x / _cellSize),
		std::floor(position.y / _cellSize),
		std::floor(position.z / _cellSize)
	);
}

vector<vec3> ColliderManager::GetOccupiedCells(const shared_ptr<Collider>& collider) const
{
	auto& [min, max] = collider->GetMinMax();
	vec3 minCell = GetGridCell(min);
	vec3 maxCell = GetGridCell(max);
	int dx = maxCell.x - minCell.x + 1;
	int dy = maxCell.y - minCell.y + 1;
	int dz = maxCell.z - minCell.z + 1;
	size_t total = size_t(dx) * dy * dz;
	vector<vec3> occupiedCells;
	occupiedCells.reserve(total);

	for (int x = static_cast<int>(minCell.x); x <= static_cast<int>(maxCell.x); ++x)
	{
		for (int y = static_cast<int>(minCell.y); y <= static_cast<int>(maxCell.y); ++y)
		{
			for (int z = static_cast<int>(minCell.z); z <= static_cast<int>(maxCell.z); ++z)
			{
				occupiedCells.emplace_back(x, y, z);
			}
		}
	}

	return occupiedCells;
}

vector<vec3> ColliderManager::GetOccupiedCellsDirect(CollisionType type, BoundingUnion bound) const
{
	vec3 min, max;
	if (type == CollisionType::Box)
	{
		auto rot = Matrix::CreateFromQuaternion(bound.box.Orientation);
		vec3 center = bound.box.Center;
		vec3 extent = bound.box.Extents;


		// 2) 행렬 원소의 절댓값을 이용해 새 extents 계산
		vec3 abs0 = vec3(fabs(rot.m[0][0]), fabs(rot.m[0][1]), fabs(rot.m[0][2]));
		vec3 abs1 = vec3(fabs(rot.m[1][0]), fabs(rot.m[1][1]), fabs(rot.m[1][2]));
		vec3 abs2 = vec3(fabs(rot.m[2][0]), fabs(rot.m[2][1]), fabs(rot.m[2][2]));

		vec3 newExtents;
		newExtents.x = abs0.x * extent.x + abs1.x * extent.y + abs2.x * extent.z;
		newExtents.y = abs0.y * extent.x + abs1.y * extent.y + abs2.y * extent.z;
		newExtents.z = abs0.z * extent.x + abs1.z * extent.y + abs2.z * extent.z;

		// 3) AABB 구성
		min = center - newExtents;
		max = center + newExtents;
	}

	else if (type == CollisionType::Sphere)
	{
		min = bound.sphere.Center - vec3(bound.sphere.Radius);
		max = bound.sphere.Center + vec3(bound.sphere.Radius);
	}

	vec3 minCell = GetGridCell(min);
	vec3 maxCell = GetGridCell(max);

	int dx = maxCell.x - minCell.x + 1;
	int dy = maxCell.y - minCell.y + 1;
	int dz = maxCell.z - minCell.z + 1;
	size_t total = size_t(dx) * dy * dz;
	vector<vec3> occupiedCells;
	occupiedCells.reserve(total);

	for (int x = static_cast<int>(minCell.x); x <= static_cast<int>(maxCell.x); ++x)
	{
		for (int y = static_cast<int>(minCell.y); y <= static_cast<int>(maxCell.y); ++y)
		{
			for (int z = static_cast<int>(minCell.z); z <= static_cast<int>(maxCell.z); ++z)
			{
				occupiedCells.emplace_back(x, y, z);
			}
		}
	}
	return occupiedCells;
}


void ColliderManager::AddCollider(const std::shared_ptr<Collider>& collider)
{
	auto occupiedCells = GetOccupiedCells(collider);

	const auto& SceneType = SceneManager::main->GetCurrentScene()->GetSceneType();

	for (const auto& cell : occupiedCells)
	{
		if (collider->GetOwner()->GetType() == GameObjectType::Static)
		{
			if(std::ranges::find(_staticColliderGrids[SceneType][cell], collider) == _staticColliderGrids[SceneType][cell].end())
				_staticColliderGrids[SceneType][cell].push_back(collider);
		}
		else if (collider->GetOwner()->GetType() == GameObjectType::Dynamic)
		{
			if (std::ranges::find(_dynamicColliderList[SceneType], collider) == _dynamicColliderList[SceneType].end())
				_dynamicColliderList[SceneType].push_back(collider);
			/*if (std::ranges::find(_dynamicColliderGrids[cell], collider) == _dynamicColliderGrids[cell].end())
				_dynamicColliderGrids[cell].push_back(collider);
			_dynamicColliderCashing[collider].push_back(cell);*/
		}
	}

	if (_colliderLinkTable[SceneType].find(collider) == _colliderLinkTable[SceneType].end()) {
		_colliderLinkTable[SceneType][collider] = {};
	}
}

void ColliderManager::AddColliderForRay(const std::shared_ptr<Collider>& collider)
{
	const auto& SceneType = SceneManager::main->GetCurrentScene()->GetSceneType();

	auto it = std::ranges::find(_collidersForRay[SceneType], collider);
	if (it == _collidersForRay[SceneType].end())
		_collidersForRay[SceneType].push_back(collider);
}

void ColliderManager::RemoveAColliderForRay(const std::shared_ptr<Collider>& collider)
{

	const auto& SceneType = SceneManager::main->GetCurrentScene()->GetSceneType();

	auto it = std::ranges::find(_collidersForRay[SceneType], collider);
	if (it != _collidersForRay[SceneType].end())
		_collidersForRay[SceneType].erase(it);
}

void ColliderManager::RemoveCollider(const std::shared_ptr<Collider>& collider)
{
	auto occupiedCells = GetOccupiedCells(collider);

	const auto& SceneType = SceneManager::main->GetCurrentScene()->GetSceneType();

	for (const auto& cell : occupiedCells)
	{
		if (collider->GetOwner()->GetType() == GameObjectType::Static)
		{
			auto it = _staticColliderGrids[SceneType].find(cell);

			if (it != _staticColliderGrids[SceneType].end())
			{
				auto& colliders = it->second;
				auto colliderIt = std::ranges::find(colliders, collider);
				if (colliderIt != colliders.end())
					colliders.erase(colliderIt);
			}
		}

		else if (collider->GetOwner()->GetType() == GameObjectType::Dynamic)
		{
			auto it = _dynamicColliderGrids[SceneType].find(cell);

			if (it != _dynamicColliderGrids[SceneType].end())
			{
				auto& colliders = it->second;
				auto colliderIt = std::ranges::find(colliders, collider);
				if (colliderIt != colliders.end())
					colliders.erase(colliderIt);
			}

			auto it2 = std::ranges::find(_dynamicColliderList[SceneType], collider);
			if (it2 != _dynamicColliderList[SceneType].end())
				_dynamicColliderList[SceneType].erase(it2);
		}
	}

	auto colliderIt = _colliderLinkTable[SceneType].find(collider);

	if (colliderIt != _colliderLinkTable[SceneType].end())
	{
		for (auto& other : colliderIt->second)
		{
			CallBackEnd(collider, other);
			CallBackEnd(other, collider);
			_colliderLinkTable[SceneType][other].erase(collider);
		}
		_colliderLinkTable[SceneType].erase(collider);
	}

	{
		auto& list = _dynamicColliderList[SceneType];

		list.erase(std::remove(list.begin(), list.end(), collider),list.end());

	}
	{

		auto& list = _collidersForRay[SceneType];
		list.erase(std::remove(list.begin(), list.end(), collider), list.end());
	}

	{
		auto& list = _dynamicColliderCashing[SceneType];
		list.erase(collider); 
	}


}



std::unordered_set<std::shared_ptr<Collider>> ColliderManager::GetPotentialCollisions(std::shared_ptr<Collider>& collider) 
{
	std::unordered_set<std::shared_ptr<Collider>> potentialCollisions;
	potentialCollisions.reserve(32);

	vector<vec3> occupiedCells;
	const auto& SceneType = SceneManager::main->GetCurrentScene()->GetSceneType();

	if (_dynamicColliderCashing[SceneType].find(collider) != _dynamicColliderCashing[SceneType].end())
	{
		occupiedCells = (_dynamicColliderCashing[SceneType][collider]);
	}
	else
	{
		occupiedCells = GetOccupiedCells(collider);
	}

	for (const auto& cell : occupiedCells)
	{
		{
			auto it = _dynamicColliderGrids[SceneType].find(cell);

			if (it != _dynamicColliderGrids[SceneType].end())
			{
				auto& colliders = it->second;
				potentialCollisions.insert(colliders.begin(), colliders.end());
			}
		}

		{
			auto it = _staticColliderGrids[SceneType].find(cell);

			if (it != _staticColliderGrids[SceneType].end())
			{
				auto& colliders = it->second;
				potentialCollisions.insert(colliders.begin(), colliders.end());
			}
		}

	};

	return potentialCollisions;
}

std::unordered_set<std::shared_ptr<Collider>> ColliderManager::GetPotentialCollisionsDirect(const vector<vec3>& vec)
{
	std::unordered_set<std::shared_ptr<Collider>> potentialCollisions;
	potentialCollisions.reserve(32);
	const auto& occupiedCells = vec;
	const auto& SceneType = SceneManager::main->GetCurrentScene()->GetSceneType();

	for (const auto& cell : occupiedCells)
	{
		{
			auto it = _dynamicColliderGrids[SceneType].find(cell);

			if (it != _dynamicColliderGrids[SceneType].end())
			{
				auto& colliders = it->second;
				potentialCollisions.insert(colliders.begin(), colliders.end());
			}
		}

		{
			auto it = _staticColliderGrids[SceneType].find(cell);

			if (it != _staticColliderGrids[SceneType].end())
			{
				auto& colliders = it->second;
				potentialCollisions.insert(colliders.begin(), colliders.end());
			}
		}

	};

	return potentialCollisions;
}

void ColliderManager::DebugPrint()
{
	
	static int count = 0;

	{

		for (auto& [pos, vec] : _staticColliderGrids)
		{
			count += vec.size();
		}
		cout << "_staticColliderGrids : " << count << endl;
		count = 0;
	}


	{

		for (auto& [pos, vec] : _dynamicColliderGrids)
		{
			count += vec.size();

		}
		cout << "_dynamicColliderGrids : " << count << endl;
		count = 0;
	}

	{

		for (auto& [pos, vec] : _dynamicColliderCashing)
		{
			count += vec.size();
		}
		cout << "_dynamicColliderCashing : " << count << endl;
		count = 0;
	}

	{

		
		count += _dynamicColliderList.size();
		cout << "_dynamicColliderList : " << count << endl;
		count = 0;
	}

	{

		for (auto& [pos, vec] : _colliderLinkTable)
		{
			count += vec.size();
		}
		cout << "_colliderLinkTable : " << count << endl;
		count = 0;
	}


}

void ColliderManager::Update()
{

	const auto& SceneType = SceneManager::main->GetCurrentScene()->GetSceneType();

	if (HasGizmoFlag(Gizmo::main->_flags, GizmoFlags::DivideSpace))
	{
		for (auto& [cell, colliders] : _staticColliderGrids[SceneType])
		{
			for (auto& collider : colliders)
			{
				VisualizeOccupiedCells(cell, collider);
			}
		}
	}

	UpdateDynamicCells();

	for (auto& [cell, colliders] : _dynamicColliderGrids[SceneType])
	{
		if (colliders.size() <= 1  && _staticColliderGrids[SceneType][cell].size() ==0 ) continue;

		for (auto& collider : colliders)
		{
			VisualizeOccupiedCells(cell,collider);

			auto potentialCollisions = GetPotentialCollisions(collider);

			for (auto& other : potentialCollisions)
			{
				if (other == collider) continue;

				if (TotalCheckCollision(collider, other))
				{
					if (_colliderLinkTable[SceneType][collider].contains(other) == false)
					{
						_colliderLinkTable[SceneType][collider].insert(other);
						_colliderLinkTable[SceneType][other].insert(collider);
						CallBackBegin(collider, other);
						CallBackBegin(other, collider);
					}
				}
				else
				{
					if (_colliderLinkTable[SceneType][collider].contains(other))
					{
						_colliderLinkTable[SceneType][collider].erase(other);
						_colliderLinkTable[SceneType][other].erase(collider);
						CallBackEnd(collider, other);
						CallBackEnd(other, collider);
					}
				}
			}
		}
	}

	_dynamicColliderGrids.clear();
	_dynamicColliderCashing.clear();
}

void ColliderManager::UpdateDynamicCells()
{

	const auto& SceneType = SceneManager::main->GetCurrentScene()->GetSceneType();

	for (auto& collider : _dynamicColliderList[SceneType])
	{
		auto occupiedCells = GetOccupiedCells(collider);

		for (const auto& cell : occupiedCells)
		{
			if (std::ranges::find(_dynamicColliderGrids[SceneType][cell], collider) == _dynamicColliderGrids[SceneType][cell].end())
			{
				_dynamicColliderGrids[SceneType][cell].push_back(collider);
			}
			if (std::ranges::find(_dynamicColliderCashing[SceneType][collider], cell) == _dynamicColliderCashing[SceneType][collider].end())
			{
				_dynamicColliderCashing[SceneType][collider].push_back(cell);
			}
		}
	}
}

bool ColliderManager::CollisionCheckDirect(CollisionType type, BoundingUnion bound)
{
	std::shared_ptr<Collider> collider;
	return CollisionCheckDirect(type, bound, collider);
}

bool ColliderManager::CollisionCheckDirect(CollisionType type, BoundingUnion bound, std::shared_ptr<Collider>& collider)
{
	auto cells = GetOccupiedCellsDirect(type, bound);
	auto potentialCollisions = GetPotentialCollisionsDirect(cells);

	for (auto& other : potentialCollisions)
	{
		if (other->GetOwner()->GetActive() && other->CheckCollision(type, bound))
		{
			collider = other;
			return true;
		}
	}
	return false;
}

bool ColliderManager::CollisionChecksDirect(CollisionType type, BoundingUnion bound, std::vector<std::shared_ptr<Collider>>& colliders)
{
	auto cells = GetOccupiedCellsDirect(type, bound);
	auto potentialCollisions = GetPotentialCollisionsDirect(cells);

	for (auto& other : potentialCollisions)
	{
		if (other->GetOwner()->GetActive() && other->CheckCollision(type, bound))
		{
			colliders.push_back(other);
		}
	}
	return colliders.size() != 0;
}

bool ColliderManager::CollisionCheckDirect(const BoundingData& bound)
{
	return CollisionCheckDirect(bound.type, bound.bound);
}

bool ColliderManager::CollisionCheckDirect(const BoundingData& bound, std::shared_ptr<Collider>& collider)
{
	return CollisionCheckDirect(bound.type, bound.bound, collider);
}

bool ColliderManager::CollisionChecksDirect(const BoundingData& bound, std::vector<std::shared_ptr<Collider>>& colliders)
{
	return CollisionChecksDirect(bound.type, bound.bound, colliders);
}

//무언가와 충돌하고 있는지 체크
bool ColliderManager::IsCollision(const std::shared_ptr<Collider>& src)
{
	const auto& SceneType = SceneManager::main->GetCurrentScene()->GetSceneType();

	auto it = _colliderLinkTable[SceneType].find(src);
	return (it != _colliderLinkTable[SceneType].end() && !it->second.empty());
}
//특정누군가와 충돌하고 있는지 체크

bool ColliderManager::IsCollision(const std::shared_ptr<Collider>& src, const std::shared_ptr<Collider>& dest)
{
	const auto& SceneType = SceneManager::main->GetCurrentScene()->GetSceneType();


	auto it = _colliderLinkTable[SceneType].find(src);
	if (it != _colliderLinkTable[SceneType].end())
		return it->second.contains(dest);

	return IsCollision(dest);
}

std::unordered_set<std::shared_ptr<Collider>>& ColliderManager::GetCollisionList(const std::shared_ptr<Collider>& src)
{
	const auto& SceneType = SceneManager::main->GetCurrentScene()->GetSceneType();

	return _colliderLinkTable[SceneType].find(src)->second;
}

bool ColliderManager::TotalCheckCollision(const std::shared_ptr<Collider>& src, const std::shared_ptr<Collider>& dest)
{
	if (src->groupId == dest->groupId) return false;
	if (!src->GetOwner()->GetActive() || !dest->GetOwner()->GetActive()) return false;
	return src->CheckCollision(dest);
}

void ColliderManager::VisualizeOccupiedCells(const vec3& cell, const shared_ptr<Collider>& collider)
{
	if (HasGizmoFlag(Gizmo::main->_flags, GizmoFlags::DivideSpace))
	{
		Gizmo::Width(0.5f);

		vec3 min = cell * _cellSize;
		vec3 max = min + vec3(_cellSize, _cellSize, _cellSize);

		if (collider->GetOwner()->GetType() == GameObjectType::Static)
			Gizmo::main->Box(min, max, vec4(0.0f, 1.0f, 1.0f, 1));
		else
			Gizmo::main->Box(min, max, vec4(0.5f, 0.1f, 0.1f, 1));

	}
}

void ColliderManager::CallBackBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	auto& components = collider->GetOwner()->GetComponentAll();
	for (auto& component : components)
		component->CollisionBegin(collider, other);
	if (collider->groupRootObject.lock() != nullptr && collider->groupRootObject.lock() != collider->GetOwner())
		for (auto& component : collider->groupRootObject.lock()->GetComponentAll())
			component->CollisionBegin(collider, other);
}

void ColliderManager::CallBackEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	auto& components = collider->GetOwner()->GetComponentAll();
	for (auto& component : components)
		component->CollisionEnd(collider, other);
	if (collider->groupRootObject.lock() != nullptr && collider->groupRootObject.lock() != collider->GetOwner())
		for (auto& component : collider->groupRootObject.lock()->GetComponentAll())
			component->CollisionEnd(collider, other);
}

RayHit ColliderManager::RayCast(const Ray& ray, const float& dis, shared_ptr<GameObject>& owner) const
{
	RayHit closestHit;
	closestHit.distance = dis;

	const auto& SceneType = SceneManager::main->GetCurrentScene()->GetSceneType();

	for (const auto& collider : _collidersForRay.at(SceneType))
	{
		if (collider->GetOwner() == owner) continue;

		RayHit currentHit;
		currentHit.distance = dis;  

		if (collider->RayCast(ray, dis, currentHit))
		{
			if (currentHit.distance < closestHit.distance)
			{
				closestHit = currentHit;
			}
		}
	}

	return closestHit;
}

bool ColliderManager::RayCastSpeed(const Ray& ray, float maxDist, shared_ptr<GameObject>& owner, GameObjectTag tag) const
{
	const auto& sceneType = SceneManager::main->GetCurrentScene()->GetSceneType();

	if (ray.direction == vec3::Zero)
		return false;

	float dist = 0;

	auto it = _collidersForRay.find(sceneType);

	if (it == _collidersForRay.end())
		return false;

	for (const auto& collider : it->second)
	{
		if (collider->GetOwner() == owner) continue;
		if (collider->GetOwner()->GetRoot()->HasTag(tag)) continue;

		switch (collider->GetType())
		{
		case CollisionType::Box:
			if (ray.Intersects(collider->GetBoundUnion().box, dist))
			{
				if (dist < maxDist)
				{
					return true;
				}
			}

			break;
		case CollisionType::Sphere:
			if (ray.Intersects(collider->GetBoundUnion().sphere, dist))
			{
				if (dist < maxDist)
				{
					return true;
				}
			}
			break;

		case CollisionType::Frustum:
			break;

		default:
			break;
		}
	}

	return false;
}





RayHit ColliderManager::RayCastForMyCell(const Ray& ray, const float& dis, shared_ptr<GameObject>& owner) 
{

	shared_ptr<Collider> Mycollider = owner->GetComponentWithChilds<Collider>();

	UpdateDynamicCells();

	std::unordered_set<std::shared_ptr<Collider>> potencialColliders = GetPotentialCollisions(Mycollider);

	RayHit closestHit;
	closestHit.distance = dis;
	bool hitFound = false;

	if (ray.direction == vec3::Zero)
		return closestHit;

	for (const auto& collider : potencialColliders)
	{
		if (collider == Mycollider) continue;
		if (collider->isTrigger) continue;

		RayHit currentHit;
		currentHit.distance = dis;
		if (collider->RayCast(ray, dis, currentHit))
		{
			if (currentHit.distance < closestHit.distance)
			{
				closestHit = currentHit;
				hitFound = true;
			}
		}
	}

	return closestHit;

}

bool ColliderManager::RayCastAll(const Ray& ray, const float& dis, std::vector<RayHit>& hitList) const
{
	RayHit closestHit;
	closestHit.distance = dis;
	bool hitFound = false;

	const auto& sceneType = SceneManager::main->GetCurrentScene()->GetSceneType();


	for (const auto& collider : _collidersForRay.at(sceneType)) 
	{
		RayHit currentHit;
		currentHit.distance = dis;  // 최대 거리로 초기화
		if (collider->RayCast(ray, dis, currentHit))
		{
			hitList.push_back(currentHit);
			hitFound = true;
		}
	}

	for (auto& terrain : TerrainManager::main->_terrains)
	{
		RayHit currentHit;
		if (terrain->RayCast(ray, dis, currentHit))
		{
			hitList.push_back(currentHit);
			hitFound = true;
		}
	}
	std::ranges::sort(hitList, [&](const RayHit& hit, const RayHit& hit2)
		{
			return hit.distance < hit2.distance;
		});


	return hitFound;
}

bool ColliderManager::RayCastAllForMyCell(const Ray& ray, const float& dis, std::vector<RayHit>& hitList, const shared_ptr<GameObject>& owner)
{
	shared_ptr<Collider> Mycollider = owner->GetComponentWithChilds<Collider>();
	std::unordered_set<std::shared_ptr<Collider>> potencialColliders = GetPotentialCollisions(Mycollider);

	RayHit closestHit;
	closestHit.distance = dis;
	bool hitFound = false;

	for (const auto& collider : potencialColliders)
	{
		RayHit currentHit;
		currentHit.distance = dis;  // 최대 거리로 초기화
		if (collider->RayCast(ray, dis, currentHit))
		{
			hitList.push_back(currentHit);
			hitFound = true;
		}
	}

	for (auto& terrain : TerrainManager::main->_terrains)
	{
		RayHit currentHit;
		if (terrain->RayCast(ray, dis, currentHit))
		{
			hitList.push_back(currentHit);
			hitFound = true;
		}
	}
	std::ranges::sort(hitList, [&](const RayHit& hit, const RayHit& hit2)
		{
			return hit.distance < hit2.distance;
		});


	return hitFound;
}

bool ColliderManager::RayCastAllForMyCellDirect(const Ray& ray, const float& dis, std::vector<RayHit>& hitList,
	BoundingData data)
{
	auto cells = GetOccupiedCellsDirect(data.type, data.bound);
	std::unordered_set<std::shared_ptr<Collider>> potencialColliders = GetPotentialCollisionsDirect(cells);

	RayHit closestHit;
	closestHit.distance = dis;
	bool hitFound = false;

	for (const auto& collider : potencialColliders)
	{


		RayHit currentHit;
		currentHit.distance = dis;  // 최대 거리로 초기화
		if (collider->RayCast(ray, dis, currentHit))
		{
			hitList.push_back(currentHit);
			hitFound = true;
		}
	}

	for (auto& terrain : TerrainManager::main->_terrains)
	{
		RayHit currentHit;
		if (terrain->RayCast(ray, dis, currentHit))
		{
			hitList.push_back(currentHit);
			hitFound = true;
		}
	}
	std::ranges::sort(hitList, [&](const RayHit& hit, const RayHit& hit2)
		{
			return hit.distance < hit2.distance;
		});


	return hitFound;
}

bool ColliderManager::RayCastAll(const std::vector<std::shared_ptr<Collider>>& colliders, const Ray& ray,
                                 const float& dis, std::vector<RayHit>& hitList)
{
	RayHit closestHit;
	closestHit.distance = dis;
	bool hitFound = false;

	for (const auto& collider : colliders)
	{
		RayHit currentHit;
		currentHit.distance = dis;  // 최대 거리로 초기화
		if (collider->RayCast(ray, dis, currentHit))
		{
			hitList.push_back(currentHit);
			hitFound = true;
		}
	}

	for (auto& terrain : TerrainManager::main->_terrains)
	{
		RayHit currentHit;
		if (terrain->RayCast(ray, dis, currentHit))
		{
			hitList.push_back(currentHit);
			hitFound = true;
		}
	}
	std::ranges::sort(hitList, [&](const RayHit& hit, const RayHit& hit2)
		{
			return hit.distance < hit2.distance;
		});


	return hitFound;
}

void ColliderManager::Clear()
{
	_staticColliderGrids.clear();
	_dynamicColliderGrids.clear();
	_dynamicColliderCashing.clear();;
	_dynamicColliderList.clear();;
	_colliderLinkTable.clear();;
	_collidersForRay.clear();;
}


