#pragma once



class PathStamp
{
public:
	static unique_ptr<PathStamp> main;

public:

	void Init();
	void Run();
private:
	vec3 BezierQuadratic(vec3& p0, vec3& p1, vec3& p2, float& t);
	vec3 BezierCubic(const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3, float& t);
	void SaveData();

	vector<vec3> _controllPoint;
	vector<vec3> _lineData;

	const int _segmentCount = 30;
};

