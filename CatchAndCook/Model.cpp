#include "pch.h"
#include "Model.h"

#include "Mesh.h"


AssimpPack::AssimpPack()
{
}

AssimpPack::~AssimpPack()
{
}

void AssimpPack::Init(std::wstring path, bool xFlip)
{
    importer = std::make_shared<Assimp::Importer>();

    unsigned int flag = //aiProcess_MakeLeftHanded | // �޼� ��ǥ��� ����
        //aiProcess_ConvertToLeftHanded |
        aiProcess_FlipWindingOrder | //CW, CCW �ٲٴ°���.
        aiProcess_FlipUVs | // ���״�� uv�� y���� ������. �׸��� bitangent�� ������.
        aiProcess_Triangulate | // 4���� 5������ 3��������
        //aiProcess_GenSmoothNormals | // Normal�� ������ Smmoth Normal ����
        aiProcess_GenNormals | // Normal�� ������ Normal ����
        //aiProcess_ImproveCacheLocality | // �ﰢ�� ����. �� �Ǹ� �Ѻ��� ĳ����Ʈ���� ���� �ﰢ�� ��������.
        //aiProcess_GenUVCoords | // UV������ UV ����ϰ�[ ��Ű��
        aiProcess_CalcTangentSpace | // ź��Ʈ ���
        //aiProcess_SplitLargeMeshes |// �Ž��� �ʹ� Ŭ�� �ɰ��°� �Ž� Ŭ�� ������ ����.
        //aiProcess_Debone | �սǾ��� �� ����. �� ��������.
        //aiProcess_RemoveComponent | // (animations, materials, light sources, cameras, textures, vertex components ����
        //aiProcess_PreTransformVertices | // root Node�� ������ ��� ���� ���� ���� ��źȭ. ���� ����.
        //aiProcess_ValidateDataStructure | // ���� ��ȿ�� �˻�
        //aiProcess_RemoveRedundantMaterials | // �ߺ��̳� �Ⱦ��°� ����
        //aiProcess_FixInfacingNormals | //�߸� ����Ǽ� ���峭 ��� ���� ����
        //aiProcess_FindDegenerates | //�ﰢ������ ���� ���Ĺ����� �����̳� ���� �ǹ����µ�, �̰� Line�̳� Point�� ��ȯ�ϴ°���. �Ⱦ��°� ����.
        //aiProcess_FindInvalidData | //��ȿ���� �ʴ� ������ ������ ����, ���� UV�� ������. �̷��� �����ϰ� ���� aiProcess_GenNormals������ ���Ӱ� �������ٰ���. �ִϸ��̼ǿ����� ������ �ִٰ���.
        //aiProcess_GenUVCoords  | //UV�� ��ü������ �����. �𵨸������� �����ϴ°� ��õ�ϰ�, UV�� ������ ���Ӱ� �����ϴ°���.
        //aiProcess_FindInstances | //�ʹ� �Ž��� ������ Ű����. �����ٴ°Ű���. ���� ������ �Ž����� �ϳ��� ���Ĺ����� ����ε�.
        //aiProcess_OptimizeMeshes |// �Ž� �� �ٿ��ִ� ����ȭ �ɼ��ε�. aiProcess_OptimizeGraph�� ���� ���°� ����, #aiProcess_SplitLargeMeshes and #aiProcess_SortByPType.�� ȣȯ��.
        //�� ��Ű�°� ������. ���� �ؿ� �ɼ��̶� ȣȯ�Ǵ� ����ε�, �ؿ� �ɼ��� ����.
        //aiProcess_OptimizeGraph |//�ʿ���� ��带 ������. ��尡 �±׷� ���϶� �����Ǵ� ������ �ճ���, ��Ű�°� ������. ���������� �սǵȴٰ� ��.
        aiProcess_TransformUVCoords | //UV�� ���ؼ� ��ȯó�� �Ѵٰ� �ϴ°Ű���. �ؽ��� �̻������� ����������
        aiProcess_JoinIdenticalVertices// �ߺ����� �� �ε��� ���� ������� ��ȯ
        //aiProcess_SortByPType // �������� Ÿ�Ժ��� ��������. aiProcess_Triangulate ���� ������ �ﰢ���� ���Ƽ� �ʿ� ����. �ϴ� �־�~ 
        | aiProcess_GlobalScale;
    if (!xFlip)
    {
        flag |= aiProcess_MakeLeftHanded;
        scene = importer->ReadFile(std::to_string(path + L"\0"), flag);
    }
    else
    {
        scene = importer->ReadFile(std::to_string(path + L"\0"), flag);
        MakeLeftHandedProcess leftHandedProcess;
        leftHandedProcess.Execute(const_cast<aiScene*>(scene));
    }
}

void Model::DebugLog()
{

}


template <>
void Model::LoadVertex<Vertex_Skinned>(aiMesh* assimp_mesh, std::vector<Vertex_Skinned>& vertexs)
{
	auto mesh2 = std::make_shared<Mesh>();

	vertexs.reserve(assimp_mesh->mNumVertices);
	Vertex_Skinned vert;
	for (int j = 0; j < assimp_mesh->mNumVertices; j++)
	{
		vert = {};
		vert.position = convert_assimp::Format(assimp_mesh->mVertices[j]);
		if (assimp_mesh->HasNormals())
			vert.normal = convert_assimp::Format(assimp_mesh->mNormals[j]);
		if (assimp_mesh->HasVertexColors(0))
			vert.color = convert_assimp::Format(assimp_mesh->mColors[0][j]);
		if (assimp_mesh->HasTangentsAndBitangents())
			vert.tangent = convert_assimp::Format(assimp_mesh->mTangents[j]);
		for (int k = 0; k < 3; k++)
			if (assimp_mesh->HasTextureCoords(k))
				vert.uvs[k] = vec2(convert_assimp::Format(assimp_mesh->mTextureCoords[k][j]));
		vert.boneWeight = Vector4::Zero;
		vert.boneId = Vector4(-1, -1, -1, -1);
		vertexs.push_back(vert);
	}
}

template <>
void Model::LoadVertex<Vertex_Static>(aiMesh* assimp_mesh, std::vector<Vertex_Static>& vertexs)
{
	auto mesh2 = std::make_shared<Mesh>();
	vertexs.reserve(assimp_mesh->mNumVertices);
	Vertex_Static vert;
	for (int j = 0; j < assimp_mesh->mNumVertices; j++)
	{
		vert = {};
		vert.position = convert_assimp::Format(assimp_mesh->mVertices[j]);
		if (assimp_mesh->HasNormals())
			vert.normal = convert_assimp::Format(assimp_mesh->mNormals[j]);
		if (assimp_mesh->HasTextureCoords(0))
			vert.uv = vec2(convert_assimp::Format(assimp_mesh->mTextureCoords[0][j]));
		vertexs.push_back(vert);
	}
}

void Model::LoadIndex(aiMesh* assimp_mesh, std::vector<uint32_t>& indexs)
{
	indexs.reserve(assimp_mesh->mNumFaces * 3);

	for (int j = 0; j < assimp_mesh->mNumFaces; j++)
	{
		auto& polygon = assimp_mesh->mFaces[j];
		if (polygon.mNumIndices == 3)
		{
			indexs.push_back(polygon.mIndices[0]);
			indexs.push_back(polygon.mIndices[1]);
			indexs.push_back(polygon.mIndices[2]);
		}
	}
}


void ModelNode::Init(shared_ptr<Model> model, aiNode* node)
{
	_model = model;
	std::string name = convert_assimp::Format(node->mName);
	SetName(name);
	SetLocalSRT(convert_assimp::Format(node->mTransformation));

	_meshIndexList.reserve(node->mNumMeshes);
	for (int i = 0; i < node->mNumMeshes; i++)
		_meshIndexList.push_back(node->mMeshes[i]);
	std::vector<std::shared_ptr<Mesh>> meshList;
	for (auto& meshIndex : _meshIndexList)
		meshList.push_back(model->_meshes[meshIndex]);

	model->_nameToObjectTable[name] = GetCast<ModelNode>();
	model->_nameToMeshsTable[name] = meshList;

}

void ModelNode::SetParent(const std::shared_ptr<ModelNode>& object)
{
	if (object != nullptr)
	{
		_parent = object;
		object->AddChild(GetCast<ModelNode>());
	}
}

void ModelNode::AddChild(const std::shared_ptr<ModelNode>& object)
{
	_childs.push_back(object);
}
