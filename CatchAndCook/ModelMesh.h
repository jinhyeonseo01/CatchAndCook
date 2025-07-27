#pragma once


inline void to_json(json& j, const Vertex_Static& v) {
	j = json{
		{"position", { v.position.x, v.position.y, v.position.z }},
		{"normal",   { v.normal.x,   v.normal.y,   v.normal.z   }},
		{"tangent",  { v.tangent.x,  v.tangent.y,  v.tangent.z  }},
		{"uv",       { v.uv.x,       v.uv.y         }}
	};
}

inline void from_json(const json& j, Vertex_Static& v) {
	auto arr = j.at("position");
	v.position.x = arr.at(0); v.position.y = arr.at(1); v.position.z = arr.at(2);

	arr = j.at("normal");
	v.normal.x = arr.at(0);   v.normal.y = arr.at(1);   v.normal.z = arr.at(2);

	arr = j.at("tangent");
	v.tangent.x = arr.at(0);  v.tangent.y = arr.at(1);  v.tangent.z = arr.at(2);

	arr = j.at("uv");
	v.uv.x = arr.at(0);       v.uv.y = arr.at(1);
}

class ModelMesh : public ModelImExporter
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
	std::string& GetName() { return  _name; }
	void ExportPreprocess() override;
	void ExportModel(json& j, const wstring& path, const wstring& subKey) override;
	void ImportModel(json& j, const wstring& path, const wstring& subKey) override;
	void ImpoetPostprocess() override;
	~ModelMesh() override;;

	std::vector<Vertex_Skinned> skinnedMeshList;
	std::vector<Vertex_Static> staticMeshList;
};
