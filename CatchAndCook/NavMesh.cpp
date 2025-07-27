#include "pch.h"
#include "NavMesh.h"

#include "Gizmo.h"

NavMesh::~NavMesh()
{
}

bool NavMesh::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void NavMesh::Init()
{
	Component::Init();
}

void NavMesh::Start()
{
	Component::Start();
	NavMeshManager::main->SetNavMeshData(_datas);
	NavMeshManager::main->SetTriangles(_tris);
}

void NavMesh::Update()
{
	Component::Update();

	if (HasGizmoFlag(Gizmo::main->_flags, GizmoFlags::NavMesh))
	{
		Gizmo::Width(0.05);
		for (auto edge : _edges)
			Gizmo::Line(edge[0], edge[1], Vector4(0, 0, 1, 1));
		Gizmo::WidthRollBack();
		Gizmo::Width(0.05);
		for (int i = 0; i < _tris.size(); i+=3)
		{
			Gizmo::Line(_datas[_tris[i]].position, _datas[_tris[i+1]].position, Vector4(0, 1, 0, 1));
			Gizmo::Line(_datas[_tris[i + 1]].position, _datas[_tris[i + 2]].position, Vector4(0, 1, 0, 1));
			Gizmo::Line(_datas[_tris[i]].position, _datas[_tris[i + 2]].position, Vector4(0, 1, 0, 1));
		}

		Gizmo::WidthRollBack();
	}
}

void NavMesh::Update2()
{
	Component::Update2();
}

void NavMesh::Enable()
{
	Component::Enable();
}

void NavMesh::Disable()
{
	Component::Disable();
}

void NavMesh::RenderBegin()
{
	Component::RenderBegin();
}

void NavMesh::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void NavMesh::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void NavMesh::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void NavMesh::SetDestroy()
{
	Component::SetDestroy();
}

void NavMesh::Destroy()
{
	Component::Destroy();
}

std::vector<NavMeshData>& NavMesh::GetNavMeshData()
{
	return _datas;
}

void NavMesh::SetNavMeshData(const std::vector<NavMeshData>& data)
{
	_datas = data;
}

std::vector< std::array<Vector3, 2>>& NavMesh::GetNavMeshEdgeData()
{
	return _edges;
}

void NavMesh::SetNavMeshEdgeData(const std::vector< std::array<Vector3, 2>>& data)
{
	_edges = data;
}

void NavMesh::SetTris(const std::vector<int>& data)
{
	_tris = data;
}

