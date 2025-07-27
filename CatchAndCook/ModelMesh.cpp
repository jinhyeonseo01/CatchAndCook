#include "pch.h"
#include "ModelMesh.h"

void ModelMesh::ExportPreprocess()
{
	ModelImExporter::ExportPreprocess();
	if (Add(this))
	{

	}
}

void ModelMesh::ExportModel(json& j, const wstring& path, const wstring& subKey)
{
	ModelImExporter::ExportModel(j, path, subKey);

	j["_type"] = "ModelMesh";
	for (auto& data : staticMeshList)
		j["_staticMeshList"].push_back(data);
	j["_type"] = "Vertex_Static";
	j["_name"] = _name;
	j["_index"] = _index;
}

void ModelMesh::ImportModel(json& j, const std::wstring& path, const std::wstring& subKey)
{
    // 공통 필드(부모)부터 불러오기
    ModelImExporter::ImportModel(j, path, subKey);

    // 1) staticMeshList
    staticMeshList.clear();
    if (j.contains("_staticMeshList") && j["_staticMeshList"].is_array()) {
        for (auto const& item : j["_staticMeshList"]) {
            // to_json/from_json 정의가 되어 있으면 .get<Vertex_Static>() 로 바로 역직렬화
            staticMeshList.push_back(item.get<Vertex_Static>());
        }
    }

    // 2) _type (VertexType)
    if (j.contains("_type") && j["_type"].is_string()) {
        std::string ts = j["_type"].get<std::string>();
        if (ts == "Vertex_Static") _type = VertexType::Vertex_Static;
        else if (ts == "Vertex_Skinned") _type = VertexType::Vertex_Skinned;
        // … 필요에 따라 다른 enum 값 처리 …
    }

    // 3) _name, _index
    if (j.contains("_name")) {
        _name = j["_name"].get<std::string>();
    }
    if (j.contains("_index")) {
        _index = j["_index"].get<int>();
    }
}

void ModelMesh::ImpoetPostprocess()
{
	ModelImExporter::ImpoetPostprocess();


}

ModelMesh::~ModelMesh()
{

}
