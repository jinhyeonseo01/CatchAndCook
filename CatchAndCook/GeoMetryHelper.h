#pragma once

class Mesh;

class GeoMetryHelper
{

public:
	static shared_ptr<Mesh> LoadPyramidMesh();
	static shared_ptr<Mesh> LoadRectangleBox(const float scale = 1.0f);
	static shared_ptr<Mesh> LoadRectangleBoxWithColor(const float scale = 1.0f , vec4 color = vec4(1,1,1,1));
	static shared_ptr<Mesh> LoadRectangleMesh(const float scale = 1.0f);
	static shared_ptr<Mesh> LoadGripMeshControlPoints(float width, float height, int division_x, int division_z, bool move =true);
	static shared_ptr<Mesh> LoadGripMesh(float width, float height, int division_x, int division_z);
	static shared_ptr<Mesh> LoadSprtieMesh();
	static shared_ptr<Mesh> LoadRectMesh();
	static shared_ptr<Mesh> LoadRectCenterMesh();
	static shared_ptr<Mesh> LoadRectCenterOneScaleMesh();
};

