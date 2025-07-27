#pragma once
#include "GameObject.h"


class Animation;
class Scene;
class Bone;
class ModelMesh;
class ModelNode;

class AssimpPack : public std::enable_shared_from_this<AssimpPack>
{
	std::wstring name;
	std::wstring path;
	const aiScene* scene;
	std::shared_ptr<Assimp::Importer> importer;
public:
	AssimpPack();
	virtual ~AssimpPack();
	const aiScene* GetScene() const { return scene; };

	void Init(std::wstring path, bool xFlip = false);
};

class Model : public IGuid
{
public:
	std::vector<std::shared_ptr<ModelMesh>> _modelMeshList;
	std::vector<std::shared_ptr<ModelNode>> _modelNodeList;
	std::vector<std::shared_ptr<ModelNode>> _modelOriginalNodeList;
	std::vector<std::shared_ptr<Bone>> _modelBoneList;

	std::unordered_map<std::string, std::vector<std::shared_ptr<ModelMesh>>> _nameToMeshsTable;
	std::unordered_map<std::string, std::shared_ptr<ModelNode>> _nameToNodeTable;
	std::unordered_map<std::string,std::shared_ptr<ModelNode>> _nameToOriginalNodeTable;
	std::unordered_map<std::string, std::shared_ptr<Bone>> _nameToBoneTable;
	std::unordered_map<std::string, std::shared_ptr<Animation>> _nameToAnimationTable;

	std::vector<std::shared_ptr<Animation>> _animationList;
	std::shared_ptr<ModelNode> _rootNode;
	std::shared_ptr<ModelNode> _rootBoneNode;


	std::shared_ptr<ModelNode> FindNodeByName(const string& name) { return _nameToNodeTable[name]; };
	std::shared_ptr<ModelNode> FindOriginalNodeByName(const string& name) { return _nameToOriginalNodeTable[name]; };
	std::vector<std::shared_ptr<ModelMesh>> FindMeshsByName(const string& name) { return _nameToMeshsTable[name]; };
	std::shared_ptr<ModelMesh> FindMeshByIndex(const int& index) { return _modelMeshList[index]; };
	std::shared_ptr<Bone> FindBoneByName(const string& name) { return _nameToBoneTable[name]; };

	std::shared_ptr<GameObject> CreateGameObject(const std::shared_ptr<Scene>& scene);

	string _modelName;

	wstring _path;

	void Init(const wstring& path, VertexType vertexType);
	void ExportBinary(const wstring& path, const wstring& subKey);
	void ImportBinary(const wstring& path, const wstring& subKey);
	void DebugLog();

	void SetName(const string& name)
	{
		_modelName = name;
	};
	string& GetName() { return _modelName; };
public:

	template<class T>
	static void LoadVertex(aiMesh* assimp_mesh, std::shared_ptr<Mesh> mesh, std::vector<T>& vertexs);
	template<>
	static void LoadVertex<Vertex_Skinned>(aiMesh* assimp_mesh, std::shared_ptr<Mesh> mesh, std::vector<Vertex_Skinned>& vertexs);
	template<>
	static void LoadVertex<Vertex_Static>(aiMesh* assimp_mesh, std::shared_ptr<Mesh> mesh, std::vector<Vertex_Static>& vertexs);

	static void LoadIndex(aiMesh* assimp_mesh, std::vector<uint32_t>& indexs);

	void LoadBone(aiMesh* currentAIMesh, const std::shared_ptr<ModelMesh>& currentModelMesh);
	void LoadNode(aiNode* root);
	void LoadAnimation(aiAnimation* aiAnim, aiNode* root);
	void LoadRootBone(aiScene* scene);
	void SetNodeData();


	std::vector<std::shared_ptr<ModelNode>>& GetNodeList() { return  _modelNodeList; }
	std::vector<std::shared_ptr<Bone>>& GetBoneList() { return  _modelBoneList; }

private:
	int AllocBoneID() { return _boneAllocator++; };
	void AddBone(const std::shared_ptr<Bone>& bone);
	std::shared_ptr<ModelNode> AddNode(aiNode* rootNode);
	void SetOriginalParentWithChilds(const std::shared_ptr<ModelNode>& prent);

private:
	int _boneAllocator = 0;
};