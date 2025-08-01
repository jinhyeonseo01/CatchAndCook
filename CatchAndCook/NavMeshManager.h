#pragma once

struct NavMeshPathData;

class NavMeshData
{
public:
	NavMeshData();
	virtual ~NavMeshData();

	Vector3 position;
	int index;
	std::vector<int> adjectIndexs;
};



struct NavMeshPathData
{
	NavMeshData* data = nullptr;
	float g = 0;
	float h = 0;
	int id = 0;
	int parentId = 0;
	bool closed = false;
	bool opened = false;

	NavMeshPathData() = default;
	NavMeshPathData(const NavMeshData& data, float g = 0, float h = 0);

	float GetF() const { return g + h; };
	void SetClose() { closed = true; }
	void SetOpen() { opened = true; }
	const bool& IsClose() const { return closed; }
	const bool& IsOpen() const { return opened; }

	bool operator<(const NavMeshPathData& other) const;
	bool operator==(const NavMeshPathData& other) const;
};

struct NavMeshPathDataPtrCompare {
	bool operator()(const NavMeshPathData* lhs, const NavMeshPathData* rhs) const {
		// 내부 operator<를 사용하여 두 객체를 비교합니다.
		return (*lhs) < (*rhs);
	}
};

namespace std {
	template<>
	struct hash<NavMeshPathData> {
		std::size_t operator()(const NavMeshPathData& obj) const {
			return std::hash<int>()(obj.id);
		}
	};
}


struct IndexEdge
{
	int u, v;
	bool operator==(IndexEdge const& o) const noexcept
	{
		return u == o.u && v == o.v;
	}
};
struct IndexEdgeHash {
	size_t operator()(IndexEdge const& e) const noexcept {
		return (size_t(e.u) << 32) ^ size_t(e.v);
	}
};
struct TriangleNode
{
	float g, f;
	int parentIndex;
	bool open, closed;
};

class NavMeshManager
{
public:
	static std::unique_ptr<NavMeshManager> main;

	NavMeshManager();
	virtual ~NavMeshManager();

	void Init();
	void Release();

	std::vector<NavMeshData>& GetNavMeshData();
	void SetNavMeshData(const std::vector<NavMeshData>& data);
	void SetTriangles(const std::vector<int>& tris);


	static float Area2_3D(const Vector3& A, const Vector3& B, const Vector3& C) {
		return (B.x - A.x) * (C.z - A.z) - (B.z - A.z) * (C.x - A.x);
	}

	static bool PointInTriangle3D( const Vector3& P, const Vector3& A, const Vector3& B, const Vector3& C)
	{
		float w1 = Area2_3D(P, A, B), w2 = Area2_3D(P, B, C), w3 = Area2_3D(P, C, A);
		return (w1 >= 0 && w2 >= 0 && w3 >= 0)
			|| (w1 <= 0 && w2 <= 0 && w3 <= 0);
	}
	static Vector3 ClosestPointOnTriangle(const Vector3& P, const Vector3& A, const Vector3& B, const Vector3& C)
	{
		// edge 벡터
		Vector3 AB = B - A;
		Vector3 AC = C - A;
		Vector3 AP = P - A;

		// barycentric 좌표 계산
		float d1 = AB.Dot(AP);
		float d2 = AC.Dot(AP);
		if (d1 <= 0 && d2 <= 0) return A; // A vertex

		Vector3 BP = P - B;
		float d3 = AB.Dot(BP);
		float d4 = AC.Dot(BP);
		if (d3 >= 0 && d4 <= d3) return B; // B vertex

		float vc = d1 * d4 - d3 * d2;
		if (vc <= 0 && d1 >= 0 && d3 <= 0) {
			float v = d1 / (d1 - d3);
			return A + AB * v; // AB edge
		}

		Vector3 CP = P - C;
		float d5 = AB.Dot(CP);
		float d6 = AC.Dot(CP);
		if (d6 >= 0 && d5 <= d6) return C; // C vertex

		float vb = d5 * d2 - d1 * d6;
		if (vb <= 0 && d2 >= 0 && d6 <= 0) {
			float w = d2 / (d2 - d6);
			return A + AC * w; // AC edge
		}

		float va = d3 * d6 - d5 * d4;
		if (va <= 0 && (d4 - d3) >= 0 && (d5 - d6) >= 0) {
			Vector3 BC = C - B;
			float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
			return B + BC * w; // BC edge
		}

		// face 영역 내부
		float denom = 1.0f / (va + vb + vc);
		float v = vb * denom;
		float w = vc * denom;
		return A + AB * v + AC * w;
	}

	static float Area2_2D(const Vector2& A, const Vector2& B, const Vector2& C) {
		return (B.x - A.x) * (C.y - A.y) - (B.y - A.y) * (C.x - A.x);
	}

	static Vector2 Normalize2D(const Vector2& v) {
		float l = std::sqrt(v.x * v.x + v.y * v.y);
		return Vector2(v.x / l, v.y / l);
	}

	static bool Vec3Equal(const Vector3& a, const Vector3& b, float eps = 1e-6f) {
		return fabs(a.x - b.x) < eps
			&& fabs(a.y - b.y) < eps
			&& fabs(a.z - b.z) < eps;
	}

	static Vector3 Interpolate(const Vector3& P, const Vector3& A, const Vector3& B, const Vector3& C)
	{

		// 1) XZ 평면에서의 벡터
		Vector3 v0 = B - A;
		Vector3 v1 = C - A;
		Vector3 v2 = P - A;

		// 2) 내적 계산
		float d00 = v0.Dot(v0);
		float d01 = v0.Dot(v1);
		float d11 = v1.Dot(v1);
		float d20 = v2.Dot(v0);
		float d21 = v2.Dot(v1);

		// 3) 바리센트릭 좌표 (v, w) 구하기
		float denom = d00 * d11 - d01 * d01;
		float v = (d11 * d20 - d01 * d21) / denom;
		float w = (d00 * d21 - d01 * d20) / denom;
		float u = 1.0f - v - w;

		// 4) y 값 보간
		float yInterp = u * A.y
			+ v * B.y
			+ w * C.y;

		// P 의 높이를 보간된 y 로 설정
		return Vector3(u * A.x + v * B.x + w * C.x, u * A.y + v * B.y + w * C.y, u * A.z + v * B.z + w * C.z);
	}

	static std::vector<Vector3> StringPull(
		const Vector3& start,
		const Vector3& end,
		const std::vector<std::pair<Vector3, Vector3>>& portals)
	{
		std::vector<Vector3> path;
		path.reserve(portals.size());

		Vector3 apex = start;
		Vector3 leftPt = portals[0].first;
		Vector3 rightPt = portals[0].second;
		int idxApex = 0, idxLeft = 0, idxRight = 0;

		path.push_back(apex);
		for (int i = 1; i < (int)portals.size(); ++i) {
			auto nl = portals[i].first;
			auto nr = portals[i].second;

			if (Area2_3D(apex, rightPt, nr) >= 0) {
				if (Vec3Equal(apex, rightPt)
					|| Area2_3D(apex, leftPt, nr) < 0)
				{
					rightPt = nr; idxRight = i;
				}
				else {
					apex = leftPt;
					path.push_back(apex);
					idxApex = idxLeft;
					leftPt = apex; rightPt = apex;
					idxLeft = idxApex; idxRight = idxApex;
					i = idxApex;
					continue;
				}
			}

			if (Area2_3D(apex, leftPt, nl) <= 0) {
				if (Vec3Equal(apex, leftPt)
					|| Area2_3D(apex, rightPt, nl) > 0)
				{
					leftPt = nl; idxLeft = i;
				}
				else {
					apex = rightPt;
					path.push_back(apex);
					idxApex = idxRight;
					leftPt = apex; rightPt = apex;
					idxLeft = idxApex; idxRight = idxApex;
					i = idxApex;
					continue;
				}
			}
		}
		path.push_back(end);
		return path;
	}


	std::vector<Vector3> NavMeshManager::CalculatePath(const Vector3& startPos, const Vector3& endPos) const;


	std::vector<NavMeshData> _datas;
	std::vector<array<int, 3>> _triangles;
	std::vector<int> _triangleIndexDatas;
	std::unordered_map<IndexEdge, std::vector<int>, IndexEdgeHash> _edgeToTriangles;
	std::vector<std::vector<int>> _triangleAdjects;
	

	bool _gizmoDebug = false;
};



