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

void ModelMesh::ImportModel(json& j, const wstring& path, const wstring& subKey)
{
	ModelImExporter::ImportModel(j, path, subKey);

}

void ModelMesh::ImpoetPostprocess()
{
	ModelImExporter::ImpoetPostprocess();


}

ModelMesh::~ModelMesh()
{

}
