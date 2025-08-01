#include "pch.h"
#include "NavMeshManager.h"
#include <queue>

#include "Gizmo.h"


std::unique_ptr<NavMeshManager> NavMeshManager::main = nullptr;


NavMeshData::NavMeshData()
{
}

NavMeshData::~NavMeshData()
{
}

NavMeshPathData::NavMeshPathData(const NavMeshData& data, float g, float h)
{
	this->data = &const_cast<NavMeshData&>(data);
	this->id = data.index;
	this->g = g;
	this->h = h;
}

bool NavMeshPathData::operator<(const NavMeshPathData& other) const
{
	return GetF() > other.GetF();
}

bool NavMeshPathData::operator==(const NavMeshPathData& other) const
{
	return id == other.id;
}


NavMeshManager::NavMeshManager()
{
}

NavMeshManager::~NavMeshManager()
{
}

void NavMeshManager::Release()
{
}

std::vector<NavMeshData>& NavMeshManager::GetNavMeshData()
{
	return _datas;
}

void NavMeshManager::SetNavMeshData(const std::vector<NavMeshData>& data)
{
	_datas = data;
}

void NavMeshManager::SetTriangles(const std::vector<int>& tris)
{
    _triangleIndexDatas = tris;
    int triangleCount = _triangleIndexDatas.size() / 3;

	_triangles.resize(triangleCount);
    _triangleAdjects.resize(triangleCount);
    _edgeToTriangles.clear();

    auto makeIndexEdgeFunc = [&](int a, int b) {
        return IndexEdge{ std::min(a,b), std::max(a,b) };
    };


    for (int i = 0; i < triangleCount; ++i)
        _triangles[i] = { _triangleIndexDatas[3 * i + 0],  _triangleIndexDatas[3 * i + 1],  _triangleIndexDatas[3 * i + 2] };

    for (int t = 0; t < triangleCount; ++t) { // 트라이 앵글의 각 index를 엣지에 추가
        auto& tr = _triangles[t];
        _edgeToTriangles[makeIndexEdgeFunc(tr[0], tr[1])].push_back(t);
        _edgeToTriangles[makeIndexEdgeFunc(tr[1], tr[2])].push_back(t);
        _edgeToTriangles[makeIndexEdgeFunc(tr[2], tr[0])].push_back(t);
    }

    for (auto& triangleIndexToEdges : _edgeToTriangles)
    {
        if (triangleIndexToEdges.second.size() == 2) {
            int a = triangleIndexToEdges.second[0], b = triangleIndexToEdges.second[1];
            _triangleAdjects[a].push_back(b);
            _triangleAdjects[b].push_back(a);
        }
    }
}


std::vector<Vector3> NavMeshManager::CalculatePath(const Vector3& startPos, const Vector3& endPos) const
{
    int triangleCount = _triangles.size();
    // 1) 삼각형 배열 만들기

    Vector3 startPathPos = startPos;
    Vector3 endPathPos = endPos;
    // 4) 시작/끝 삼각형 찾기
    int startTri = 0, endTri = 0;
    bool foundStart = false;
    bool foundEnd = false;
    float bestStartDist2 = FLT_MAX;
    float bestEndDist2 = FLT_MAX;
    for (int t = 0; t < triangleCount; ++t) {
        auto& tr = _triangles[t];
        const Vector3& A = _datas[tr[0]].position;
        const Vector3& B = _datas[tr[1]].position;
        const Vector3& C = _datas[tr[2]].position;

        // containment 체크
        if (!foundStart && PointInTriangle3D(startPos, A, B, C)) {
            startTri = t;
            startPathPos.y = Interpolate(startPos, A, B, C).y;
            foundStart = true;
        }
        if (!foundEnd && PointInTriangle3D(endPos, A, B, C)) {
            endTri = t;
            endPathPos.y = Interpolate(endPos, A, B, C).y;
            foundEnd = true;
        }

        // containment 못 찾았으면 distance² 계산하여 후보로 등록
        if (!foundStart) {
            Vector3 closest = ClosestPointOnTriangle(startPos, A, B, C);
            float d2 = (Vector2(startPos.x, startPos.z) - Vector2(closest.x, closest.z)).LengthSquared();
            if (d2 < bestStartDist2) {
                bestStartDist2 = d2;
                startTri = t;
                startPathPos.y = closest.y;
            }
        }
        if (!foundEnd) {
            Vector3 closest = ClosestPointOnTriangle(endPos, A, B, C);
            float d2 = (Vector2(endPos.x, endPos.z) - Vector2(closest.x, closest.z)).LengthSquared();
            if (d2 < bestEndDist2) {
                bestEndDist2 = d2;
                endTri = t;
                endPathPos = closest; // 갈수있는곳까지만 가셔
            }
        }

        // 둘 다 찾았으면 빨리 탈출
        if (foundStart && foundEnd) break;
    }
    
    // 5) 삼각형 A* (centroid 휴리스틱)
    std::vector<TriangleNode> triangleNodes(triangleCount);
    auto GetTriangleIdCenterPosition = [&](int ti) {
        auto& tr = _triangles[ti];
        return (_datas[tr[0]].position + _datas[tr[1]].position + _datas[tr[2]].position) / 3.0f;
    };
    Vector3 centEnd = GetTriangleIdCenterPosition(endTri);

	for (int i = 0; i < triangleCount; ++i)
    {
        triangleNodes[i].g = FLT_MAX;
        triangleNodes[i].f = FLT_MAX;
        triangleNodes[i].open = false;
        triangleNodes[i].closed = false;
    }

    triangleNodes[startTri].g = 0;
    triangleNodes[startTri].f = Vector3::Distance(GetTriangleIdCenterPosition(startTri), centEnd);
    triangleNodes[startTri].parentIndex = startTri;
    triangleNodes[startTri].open = true;

    using PairFloatIndex = std::pair<float, int>;
    std::priority_queue<PairFloatIndex, std::vector<PairFloatIndex>, std::greater<PairFloatIndex>> priorityQueue;
    priorityQueue.push({ triangleNodes[startTri].f, startTri });

    bool isFound = false;
    while (!priorityQueue.empty())
    {
        auto [f, currentIndex] = priorityQueue.top();
    	priorityQueue.pop();

        if (triangleNodes[currentIndex].closed)
            continue;
        triangleNodes[currentIndex].closed = true;

        if (currentIndex == endTri)
        {
	        isFound = true;
        	break;
        }
        for (int nextAdjTriangleIndex : _triangleAdjects[currentIndex])
        {
            if (triangleNodes[nextAdjTriangleIndex].closed) // 닫혀있으면 캔슬
                continue;

            float ng = triangleNodes[currentIndex].g + Vector3::Distance(GetTriangleIdCenterPosition(currentIndex), GetTriangleIdCenterPosition(nextAdjTriangleIndex)); // 누적 가중치
            float nh = Vector3::Distance(GetTriangleIdCenterPosition(nextAdjTriangleIndex), centEnd);  // 도착점까지 휴리스틱
            float nf = ng + nh;

            if (!triangleNodes[nextAdjTriangleIndex].open || nf < triangleNodes[nextAdjTriangleIndex].f) // 아직 openNode가 아니거나 nf가 더 작은거면 값 갱신
            {
                triangleNodes[nextAdjTriangleIndex].g = ng;
            	triangleNodes[nextAdjTriangleIndex].f = nf;
            	triangleNodes[nextAdjTriangleIndex].parentIndex = currentIndex;

                if (!triangleNodes[nextAdjTriangleIndex].open) // 열고 추가
                {
                    triangleNodes[nextAdjTriangleIndex].open = true;
                    priorityQueue.push({ nf, nextAdjTriangleIndex });
                }
            }
        }
    }
    if (!isFound)
        return { };

    // 6) triPath 복원
    std::vector<int> triPath;
    for (int t = endTri;; t = triangleNodes[t].parentIndex) // 부모를 거슬러 올라가며 갱신
    {
        triPath.push_back(t);
        if (t == startTri)
            break;
    }
    ranges::reverse(triPath);

    // 7) 포털 생성 (좌→우)
    std::vector<std::pair<Vector3, Vector3>> portals;
    portals.reserve(triPath.size() + 1);
    portals.emplace_back(startPathPos, startPathPos);
    for (int i = 0; i + 1 < (int)triPath.size(); ++i) {
        int t0 = triPath[i], t1 = triPath[i + 1];
        for (auto& kv : _edgeToTriangles) {
            auto& v = kv.second;
            if (v.size() == 2 && ((v[0] == t0 && v[1] == t1) || (v[0] == t1 && v[1] == t0))) {
                auto a3 = _datas[kv.first.u].position;
                auto b3 = _datas[kv.first.v].position;

                // 이동 방향 기준 왼쪽/오른쪽 판정
                Vector2 c0 = Vector2(GetTriangleIdCenterPosition(t0).x, GetTriangleIdCenterPosition(t0).z);
                Vector2 c1 = Vector2(GetTriangleIdCenterPosition(t1).x, GetTriangleIdCenterPosition(t1).z);
                Vector2 dir = Normalize2D({ c1.x - c0.x,c1.y - c0.y });
                Vector2 mid = { (a3.x + b3.x) * 0.5f,(a3.z + b3.z) * 0.5f };

                if (Area2_2D(mid, { mid.x + dir.x,mid.y + dir.y }, Vector2(a3.x, a3.z)) >= 0)
                    portals.emplace_back(a3, b3);
                else
                    portals.emplace_back(b3, a3);
                break;
            }
        }
    }
    portals.emplace_back(startPathPos, endPathPos);

    // 8) Funnel 스무딩
    auto smooth = StringPull(startPathPos, endPathPos, portals);


    if (_gizmoDebug)
    {
        Gizmo::Width(0.1f);
        for (int i = 1; i < (int)triPath.size(); ++i)
        {
            Vector3 c = GetTriangleIdCenterPosition(triPath[i]);
            Vector3 p = GetTriangleIdCenterPosition(triPath[i-1]);
            Gizmo::Line( p + Vector3::Up * 0.2, c + Vector3::Up * 0.2, Vector4(1,0.5,0,1));
        }
        for (auto& pp : portals) {
            Gizmo::Line(pp.first + Vector3::Up * 0.1, pp.second + Vector3::Up * 0.1, Vector4(1, 1, 0, 1));
        }
        Gizmo::WidthRollBack();
        // 9) 그리기
        Gizmo::Width(0.2f);
        for (int i = 1; i < (int)smooth.size(); ++i) {
            Gizmo::Line(
                smooth[i - 1] + Vector3::Up * 3, smooth[i] + Vector3::Up * 3, Vector4(1, 0, 0, 1));
        }
        Gizmo::WidthRollBack();
    }
    return smooth;
}

void NavMeshManager::Init()
{
    ImguiManager::main->navMesh = &_gizmoDebug;
}

