#include "pch.h"
#include "SkinnedMeshRenderer.h"
#include "GameObject.h"
#include "LightManager.h"
#include "Transform.h"
#include "Mesh.h"
#include "SkinnedHierarchy.h"
#include "Gizmo.h"
#include "InitComponent.h"
#include "ObjectSettingComponent.h"

SkinnedMeshRenderer::~SkinnedMeshRenderer()
{
}

bool SkinnedMeshRenderer::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void SkinnedMeshRenderer::Init()
{
	Component::Init();
	GetOwner()->_renderer = GetCast<SkinnedMeshRenderer>();

	//_setter_ForwardLight = std::make_shared<ForwardLightSetter>();
	//_setter_ForwardLight->Init(GetOwner().get());

}

void SkinnedMeshRenderer::Start()
{
	Component::Start();

	/*wcout << " 호출스택:" << debug_test++ << " 이름:" << GetOwner()->GetRoot()->GetName() << "SkinnedMeshRenderer" << endl;*/

	//AddStructuredSetter(_setter_ForwardLight, BufferType::ForwardLightParam);
	if (auto objectSettingComponent = GetOwner()->GetComponentWithParents<ObjectSettingComponent>())
		AddStructuredSetter(objectSettingComponent, BufferType::ObjectSettingParam);
	else
		AddStructuredSetter(make_shared<ObjectSettingComponent>(), BufferType::ObjectSettingParam);
	

	if (auto tagsComponent = GetOwner()->GetComponentWithParents<TagsComponent>())
	{
		if (HasTag(tagsComponent->GetOwner()->GetTag(), GameObjectTag::NonInstancing))
			SetInstancing(false);
		if (HasTag(tagsComponent->GetOwner()->GetTag(), GameObjectTag::NonCulling))
			SetCulling(false);
	}

	auto root = GetOwner()->GetRoot();

	if (root != nullptr)
	{
		auto hierarchys = GetOwner()->GetComponentWithParents<SkinnedHierarchy>();
		if (hierarchys)
			_hierarchy = hierarchys;
		else
			_hierarchy = root->AddComponent<SkinnedHierarchy>();
		_hierarchy.lock()->SetModel(_model);
	}
	else
	{
		cout << "퍽킹" << endl;
	}

	auto owner = GetOwner();

	if (owner && owner->GetType() == GameObjectType::Static)
	{

		Matrix matrix;
		owner->_transform->GetLocalToWorldMatrix_BottomUp(matrix);
		BoundingBox totalBox;
		for (int i = 0; i < _mesh.size(); i++) {
			auto currentMesh = _mesh[i];
			auto bound = currentMesh->CalculateBound(matrix);
			if (i == 0) totalBox = bound;
			else  BoundingBox::CreateMerged(totalBox, totalBox, bound);
		}
		SetBound(totalBox);
	}
	else
	{
		BoundingBox localBox;
		for (int i = 0; i < _mesh.size(); i++)
		{
			auto currentMesh = _mesh[i];
			auto bound = currentMesh->CalculateBound(Matrix::Identity);
			if (i == 0) localBox = bound;
			else BoundingBox::CreateMerged(localBox, localBox, bound);
		}
		BoundingBox::CreateMerged(localBox, localBox, BoundingBox(Vector3(0, 0, 0), Vector3(3, 3, 3)));
		SetOriginBound(localBox);
	}

	SetSpecialMaterials();
}

void SkinnedMeshRenderer::Update()
{
	Component::Update();
}

void SkinnedMeshRenderer::Update2()
{
	Component::Update2();
}

void SkinnedMeshRenderer::Enable()
{
	Component::Enable();
}

void SkinnedMeshRenderer::Disable()
{
	Component::Disable();
}

void SkinnedMeshRenderer::Destroy()
{
	Component::Destroy();

	//RemoveStructuredSetter(_setter_ForwardLight);
	RemoveStructuredSetter(BufferType::ForwardLightParam);
	RemoveStructuredSetter(BufferType::ObjectSettingParam);
}

void SkinnedMeshRenderer::RenderBegin()
{
	Component::RenderBegin();

	auto owner = GetOwner();

	if (owner && owner->GetType() == GameObjectType::Dynamic)
	{
		Matrix worldMatrix;
		owner->_transform->GetLocalToWorldMatrix_BottomUp(worldMatrix);
		_orginBound.Transform(_bound, worldMatrix);
	}

	if (HasGizmoFlag(Gizmo::main->_flags, GizmoFlags::Culling))
	{
		if (SceneManager::main->GetCurrentScene()->GetSceneType() == SceneType::Sea01)
			Gizmo::Width(3.0f);
		else
			Gizmo::Width(0.3f);
		Gizmo::Box(GetBound(), vec4(0, 0, 1.0f, 1.0f));
	}

	for(int i = 0; i < _mesh.size(); i++)
	{
		auto a = GetOwner();
		auto currentMesh = _mesh[i];
		auto currentMaterial = _uniqueMaterials[i % std::min(_mesh.size(), _uniqueMaterials.size())];

		SceneManager::main->GetCurrentScene()->AddRenderer(currentMaterial.get(),currentMesh.get(),this);
	}
	for (auto& ele : _depthNormalMaterials) {
		auto currentMesh = _mesh[ele.first];
		SceneManager::main->GetCurrentScene()->AddRenderer(ele.second.get(), currentMesh.get(), this);
	}
	for (auto& ele : _shadowMaterials) {
		auto currentMesh = _mesh[ele.first];
		SceneManager::main->GetCurrentScene()->AddRenderer(ele.second.get(), currentMesh.get(), this);
	}

	for(int j = 0; j < _sharedMaterials.size(); j++)
	{
		auto currentMaterial = _sharedMaterials[j];

		for(int i = 0; i < _mesh.size(); i++)
		{
			auto &currentMesh = _mesh[i];
			SceneManager::main->GetCurrentScene()->AddRenderer(currentMaterial.get(),currentMesh.get(),this);
		}
	}
}


void SkinnedMeshRenderer::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void SkinnedMeshRenderer::SetDestroy()
{
	Component::SetDestroy();
}

void SkinnedMeshRenderer::Rendering(Material* material, Mesh* mesh, int instanceCount)
{
	auto& cmdList = Core::main->GetCmdList();

	for(auto& data : _cbufferSetters)
		data->SetData(material);

	if(material != nullptr)
		material->SetData();

	mesh->Redner(instanceCount);
}


void SkinnedMeshRenderer::DebugRendering()
{

}


void SkinnedMeshRenderer::SetModel(const std::shared_ptr<Model>& model)
{
	_model = model;
}

void SkinnedMeshRenderer::AddMesh(const std::shared_ptr<Mesh>& _mesh)
{
	this->_mesh.push_back(_mesh);
}

void SkinnedMeshRenderer::SetMaterials(const std::vector<std::shared_ptr<Material>>& _materials)
{
	_uniqueMaterials = _materials;
}

void SkinnedMeshRenderer::AddMaterials(const std::vector<std::shared_ptr<Material>>& _materials)
{
	for(auto& ele : _materials)
		this->_uniqueMaterials.push_back(ele);
}

std::vector<std::shared_ptr<Material>>& SkinnedMeshRenderer::GetMaterials()
{
	return _uniqueMaterials;
}


void SkinnedMeshRenderer::SetSharedMaterials(const std::vector<std::shared_ptr<Material>>& _materials)
{
	this->_sharedMaterials = _materials;
}

void SkinnedMeshRenderer::AddSharedMaterials(const std::vector<std::shared_ptr<Material>>& _materials)
{
	for(auto& ele : _materials)
		this->_sharedMaterials.push_back(ele);
}

void SkinnedMeshRenderer::SetSpecialMaterials()
{
	_depthNormalMaterials.clear();
	_shadowMaterials.clear();
	for (int i = 0; i < _mesh.size(); i++)
	{
		auto currentMaterial = _uniqueMaterials[i % std::min(_mesh.size(), _uniqueMaterials.size())];

		if (RENDER_PASS::HasFlag(currentMaterial->GetPass(), RENDER_PASS::Forward) && currentMaterial->GetPreDepthNormal())
		{
			auto depthNormalMaterial = currentMaterial->Clone();
			depthNormalMaterial->SetShader(ResourceManager::main->_depthNormal_Skinned->GetShader());
			depthNormalMaterial->SetPass(RENDER_PASS::Deferred);
			depthNormalMaterial->_instanceID = currentMaterial->GetID() + 2000000;
			_depthNormalMaterials.push_back(make_pair(i, depthNormalMaterial));
		}


		if ((RENDER_PASS::HasFlag(currentMaterial->GetPass(), RENDER_PASS::Forward)
			|| RENDER_PASS::HasFlag(currentMaterial->GetPass(), RENDER_PASS::Deferred)) && currentMaterial->GetShadowCasting())
		{
			auto shadowMaterial = ResourceManager::main->_shadowCaster_Early_Skinned;
			if (currentMaterial->GetPropertyTexture("_BaseMap") != nullptr)
			{
				shadowMaterial = ResourceManager::main->_shadowCaster_Skinned;
				shadowMaterial = shadowMaterial->Clone();
				currentMaterial->CopyProperties(shadowMaterial);
				shadowMaterial->_instanceID = currentMaterial->GetID() + 1000000;
			}
			_shadowMaterials.push_back(make_pair(i, shadowMaterial));
		}
	}
}



