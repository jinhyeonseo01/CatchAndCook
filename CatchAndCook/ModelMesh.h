#pragma once

class ModelMesh
{
	std::shared_ptr<Mesh> _mesh;
	VertexType _type = VertexType::Vertex_Static;
	std::string _name;
	int _index = -1;

public:
	std::shared_ptr<Mesh> GetMesh() { return _mesh; };
	void SetMesh(const std::shared_ptr<Mesh>& mesh) { _mesh = mesh; };
	VertexType GetType() { return _type; };
	void SetType(const VertexType& type) { _type = type; };
	void SetIndex(int index) { _index = index; };
	int GetIndex() { return _index; };
	void SetName(const std::string& name) { _name = name; };
	std::string& GetName() { return  _name; };

	std::vector<Vertex_Skinned> skinnedMeshList;
	std::vector<Vertex_Static> staticMeshList;
};
