#include "pch.h"
#include "ModelNode.h"

#include "GameObject.h"
#include "ModelMesh.h"
#include "Transform.h"
#include "Component.h"
#include "MeshRenderer.h"


void ModelNode::Init(shared_ptr<Model> model, aiNode* node)
{
	_model = model;
	std::string name = convert_assimp::Format(node->mName);
	SetName(name);
	SetLocalSRT(convert_assimp::Format(node->mTransformation));

	_meshIndexList.reserve(node->mNumMeshes);
	for (int i = 0; i < node->mNumMeshes; i++)
		_meshIndexList.push_back(node->mMeshes[i]);
	std::vector<std::shared_ptr<ModelMesh>> meshList;
	for (auto& meshIndex : _meshIndexList)
		meshList.push_back(model->_modelMeshList[meshIndex]);

	model->_modelNodeList.push_back(shared_from_this());
	model->_nameToNodeTable[name] = shared_from_this();
	model->_nameToMeshsTable[name] = meshList;
	if(GetName().find("$AssimpFbx$") == std::string::npos)
	{
		model->_modelOriginalNodeList.push_back(shared_from_this());
		model->_nameToOriginalNodeTable[name] = shared_from_this();
	}

}

void ModelNode::SetParent(const std::shared_ptr<ModelNode>& object)
{
	if (object != nullptr)
	{
		_parent = object;
		object->AddChild(shared_from_this());
	}
}

std::shared_ptr<ModelNode> ModelNode::GetParent() const
{
	return _parent.lock();
}

void ModelNode::SetOriginalParent(const std::shared_ptr<ModelNode>& object)
{
	_originParent = object;
}

std::shared_ptr<ModelNode> ModelNode::GetOriginalParent() const
{
	return _originParent.lock();
}

void ModelNode::AddChild(const std::shared_ptr<ModelNode>& object)
{
	_childs.push_back(object);
}

std::vector<std::weak_ptr<ModelNode>>& ModelNode::GetChilds()
{
	return _childs;
}

void ModelNode::CalculateTPoseNode(const std::shared_ptr<ModelNode>& parent)
{
	Quaternion rotation = GetLocalPreRotation()  * GetLocalRotation() * GetLocalPostRotation(); //
	//Quaternion rotation = GetLocalPostRotation() * GetLocalRotation() * GetLocalPreRotation();
	//GetLocalPostRotation() * 
	_localTPose = Matrix::CreateScale(_localScale) * Matrix::CreateFromQuaternion(rotation) * Matrix::CreateTranslation(_localPosition);
	_localInvertTPose = _localTPose.Invert();
	//Matrix animLocal = Matrix::CreateFromQuaternion(_animModelNode->GetLocalRotation() * _animModelNode->GetLocalPreRotation());
	//animLocal = animLocal * Matrix::CreateTranslation(_animModelNode->GetLocalPosition());

	std::shared_ptr<ModelNode> originalParent;
	
	if (originalParent = GetOriginalParent())
	{
		//std::cout << GetName() << "\n";
		//std::cout << originalParent->GetName() << "\n\n";

		_globalTPose = _localTPose * originalParent->_globalTPose;
		_parent_globalTPose = originalParent->_globalTPose;
		_parent_globalInvertTPose = originalParent->_globalInvertTPose;

		_globalTPoseQuat = originalParent->_globalTPoseQuat * rotation;
		_parent_globalTPoseQuat = originalParent->_globalTPoseQuat;
		_parent_globalInvertTPoseQuat = originalParent->_globalInvertTPoseQuat;

		_globalTPose.Invert(_globalInvertTPose);
		_globalTPoseQuat.Inverse(_globalInvertTPoseQuat);
	}
	else
	{
		_globalTPose = _localTPose;
		_globalTPoseQuat = rotation;

		_globalTPose.Invert(_globalInvertTPose);
		_globalTPoseQuat.Inverse(_globalInvertTPoseQuat);
	}

	for (auto& child : _childs)
		child.lock()->CalculateTPoseNode(shared_from_this());
}

std::shared_ptr<GameObject> ModelNode::CreateGameObject(const std::shared_ptr<Scene>& scene,
	const std::shared_ptr<GameObject>& parent)
{
	if (GetName().find("$AssimpFbx$") != std::string::npos)
		return nullptr;

	auto currentGameObject = scene->CreateGameObject(std::to_wstring(GetName()),
		IsDynamic() ? GameObjectType::Dynamic : GameObjectType::Deactivate);
	currentGameObject->_transform->SetLocalSRTMatrix(_localTransform);
	currentGameObject->SetParent(parent);

	for (auto& meshIndex : _meshIndexList)
	{
		auto meshRenderer = currentGameObject->AddComponent<MeshRenderer>();
		meshRenderer->AddMesh(_model.lock()->FindMeshByIndex(meshIndex)->GetMesh());
		std::shared_ptr<Material> material = std::make_shared<Material>();
		material->SetShader(ResourceManager::main->Get<Shader>(L"DefaultForward"));
		material->SetPass(RENDER_PASS::Forward);
		material->SetTexture("g_tex_0", ResourceManager::main->Get<Texture>(L"None_Debug"));
		meshRenderer->AddMaterials({ material });
	}
	for (auto& child : _childs)
		child.lock()->CreateGameObject(scene, currentGameObject);

	return currentGameObject;
}

void ModelNode::ExportPreprocess()
{
	ModelImExporter::ExportPreprocess();

	if (Add(this))
	{

	}
}

void ModelNode::ExportModel(json& j, const wstring& path, const wstring& subKey)
{
	ModelImExporter::ExportModel(j, path, subKey);
	j["type"] = "ModelNode";

	for (int idx : _meshIndexList)
		j["_meshIndexList"].push_back(idx);

	for (int idx : _boneIndexList)
		j["_boneIndexList"].push_back(idx);

	// --- weak_ptr<Model> ---
	if (auto sp = _model.lock())
		j["_model"] = sp->GetMGuid();
	else
		j["_model"] = nullptr;

	// --- weak_ptr<ModelNode> 부모들 ---
	if (auto sp = _parent.lock())
		j["_parent"] = sp->GetMGuid();
	else
		j["_parent"] = nullptr;

	if (auto sp = _originParent.lock())
		j["_originParent"] = sp->GetMGuid();
	else
		j["_originParent"] = nullptr;

	// --- 자식 노드 리스트 ---
	j["_childs"] = json::array();
	for (auto const& wp : _childs) {
		if (auto sp = wp.lock())
			j["_childs"].push_back(sp->GetMGuid());
		else
			j["_childs"].push_back(nullptr);
	}

	// --- 문자열 필드들 ---
	j["_name"] = _name;
	j["_originalName"] = _originalName;

	// --- 불린 플래그 ---
	j["_isDynamic"] = _isDynamic;

	// --- 변환 행렬들 (Matrix) ---
	j["_localTransform"] = json::array({
		_localTransform._11, _localTransform._12, _localTransform._13, _localTransform._14,
		_localTransform._21, _localTransform._22, _localTransform._23, _localTransform._24,
		_localTransform._31, _localTransform._32, _localTransform._33, _localTransform._34,
		_localTransform._41, _localTransform._42, _localTransform._43, _localTransform._44
		});

	j["_localTPose"] = json::array({
		_localTPose._11, _localTPose._12, _localTPose._13, _localTPose._14,
		_localTPose._21, _localTPose._22, _localTPose._23, _localTPose._24,
		_localTPose._31, _localTPose._32, _localTPose._33, _localTPose._34,
		_localTPose._41, _localTPose._42, _localTPose._43, _localTPose._44
		});

	j["_localInvertTPose"] = json::array({
		_localInvertTPose._11, _localInvertTPose._12, _localInvertTPose._13, _localInvertTPose._14,
		_localInvertTPose._21, _localInvertTPose._22, _localInvertTPose._23, _localInvertTPose._24,
		_localInvertTPose._31, _localInvertTPose._32, _localInvertTPose._33, _localInvertTPose._34,
		_localInvertTPose._41, _localInvertTPose._42, _localInvertTPose._43, _localInvertTPose._44
		});

	j["_globalTPose"] = json::array({
		_globalTPose._11, _globalTPose._12, _globalTPose._13, _globalTPose._14,
		_globalTPose._21, _globalTPose._22, _globalTPose._23, _globalTPose._24,
		_globalTPose._31, _globalTPose._32, _globalTPose._33, _globalTPose._34,
		_globalTPose._41, _globalTPose._42, _globalTPose._43, _globalTPose._44
		});

	j["_globalInvertTPose"] = json::array({
		_globalInvertTPose._11, _globalInvertTPose._12, _globalInvertTPose._13, _globalInvertTPose._14,
		_globalInvertTPose._21, _globalInvertTPose._22, _globalInvertTPose._23, _globalInvertTPose._24,
		_globalInvertTPose._31, _globalInvertTPose._32, _globalInvertTPose._33, _globalInvertTPose._34,
		_globalInvertTPose._41, _globalInvertTPose._42, _globalInvertTPose._43, _globalInvertTPose._44
		});

	j["_parent_globalTPose"] = json::array({
		_parent_globalTPose._11, _parent_globalTPose._12, _parent_globalTPose._13, _parent_globalTPose._14,
		_parent_globalTPose._21, _parent_globalTPose._22, _parent_globalTPose._23, _parent_globalTPose._24,
		_parent_globalTPose._31, _parent_globalTPose._32, _parent_globalTPose._33, _parent_globalTPose._34,
		_parent_globalTPose._41, _parent_globalTPose._42, _parent_globalTPose._43, _parent_globalTPose._44
		});

	j["_parent_globalInvertTPose"] = json::array({
		_parent_globalInvertTPose._11, _parent_globalInvertTPose._12, _parent_globalInvertTPose._13, _parent_globalInvertTPose._14,
		_parent_globalInvertTPose._21, _parent_globalInvertTPose._22, _parent_globalInvertTPose._23, _parent_globalInvertTPose._24,
		_parent_globalInvertTPose._31, _parent_globalInvertTPose._32, _parent_globalInvertTPose._33, _parent_globalInvertTPose._34,
		_parent_globalInvertTPose._41, _parent_globalInvertTPose._42, _parent_globalInvertTPose._43, _parent_globalInvertTPose._44
		});

	// --- 쿼터니언들(Quaternion) ---
	j["_globalTPoseQuat"] = json::array({
		_globalTPoseQuat.x,
		_globalTPoseQuat.y,
		_globalTPoseQuat.z,
		_globalTPoseQuat.w
		});

	j["_globalInvertTPoseQuat"] = json::array({
		_globalInvertTPoseQuat.x,
		_globalInvertTPoseQuat.y,
		_globalInvertTPoseQuat.z,
		_globalInvertTPoseQuat.w
		});

	j["_parent_globalTPoseQuat"] = json::array({
		_parent_globalTPoseQuat.x,
		_parent_globalTPoseQuat.y,
		_parent_globalTPoseQuat.z,
		_parent_globalTPoseQuat.w
		});

	j["_parent_globalInvertTPoseQuat"] = json::array({
		_parent_globalInvertTPoseQuat.x,
		_parent_globalInvertTPoseQuat.y,
		_parent_globalInvertTPoseQuat.z,
		_parent_globalInvertTPoseQuat.w
		});

	// --- Vector3 들(3 요소) ---
	j["_localPosition"] = json::array({ _localPosition.x, _localPosition.y, _localPosition.z });
	j["_localPreRotation"] = json::array({ _localPreRotation.x, _localPreRotation.y, _localPreRotation.z, _localPreRotation.w });
	j["_localRotation"] = json::array({ _localRotation.x, _localRotation.y, _localRotation.z, _localRotation.w });
	j["_localPostRotation"] = json::array({ _localPostRotation.x, _localPostRotation.y, _localPostRotation.z, _localPostRotation.w });
	j["_localScale"] = json::array({ _localScale.x, _localScale.y, _localScale.z });
}

void ModelNode::ImportModel(json& j, const wstring& path, const wstring& subKey)
{
	ModelImExporter::ImportModel(j, path, subKey);
}

void ModelNode::ImpoetPostprocess()
{
	ModelImExporter::ImpoetPostprocess();
}

ModelNode::~ModelNode()
{
}

