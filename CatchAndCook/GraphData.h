#pragma once
class GraphData
{

public:
	static void GenVertices(const vec3& center, const vec3& extents , float spacing);


public:
	static std::unordered_map<int, vec3> datas;             

};

