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

