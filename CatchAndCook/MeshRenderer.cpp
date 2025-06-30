#include "pch.h"
#include "Mesh.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "BufferManager.h"
#include "BufferPool.h"
#include "GameObject.h"
#include "Gizmo.h"
#include "InitComponent.h"
#include "SceneManager.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"
#include "LightManager.h"
#include "ObjectSettingComponent.h"

MeshRenderer::~MeshRenderer()
{

}

bool MeshRenderer::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void MeshRenderer::Init()
{
	Component::Init();

	GetOwner()->_renderer = GetCast<MeshRenderer>();

	//_setter_ForwardLight = std::make_shared<ForwardLightSetter>();
	//_setter_ForwardLight->Init(GetOwner().get());
}

void MeshRenderer::Start()
{
	Component::Start();
	//AddStructuredSetter(_setter_ForwardLight, BufferType::ForwardLightParam);

	if (auto objectSettingComponent = GetOwner()->GetComponentWithParents<ObjectSettingComponent>())
		AddStructuredSetter(objectSettingComponent, BufferType::ObjectSettingParam);
	else
	{
		//AddStructuredSetter(GetOwner()->AddComponent<ObjectSettingComponent>(), BufferType::ObjectSettingParam);
		AddStructuredSetter(make_shared<ObjectSettingComponent>(), BufferType::ObjectSettingParam);
	}

	if (auto tagsComponent = GetOwner()->GetComponentWithParents<TagsComponent>())
	{
		if (HasTag(tagsComponent->GetOwner()->GetTag(), GameObjectTag::NonInstancing))
			SetInstancing(false);
		if (HasTag(tagsComponent->GetOwner()->GetTag(), GameObjectTag::NonCulling))
			SetCulling(false);
	}

	auto owner = GetOwner();

	if (owner && owner->GetType() == GameObjectType::Static)
	{
		
		Matrix matrix;
		owner->_transform->GetLocalToWorldMatrix_BottomUp(matrix);
		BoundingBox totalBox;
		for (int i = 0; i < _mesh.size(); i++) 
		{
			auto currentMesh = _mesh[i];
			auto bound = currentMesh->CalculateBound(matrix);
			if (i == 0) totalBox = bound;
			else  BoundingBox::CreateMerged(totalBox, totalBox, bound);
		}

		SetBound(totalBox);

		BoundingBox localBox;
		for (int i = 0; i < _mesh.size(); i++)
		{
			auto currentMesh = _mesh[i];
			auto bound = currentMesh->CalculateBound(Matrix::Identity);
			if (i == 0) localBox = bound;
			else BoundingBox::CreateMerged(localBox, localBox, bound);
		}

		SetOriginBound(localBox);
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
		SetOriginBound(localBox);
	}


	SetSpecialMaterials();
};

void MeshRenderer::Update()
{
	Component::Update();

}

void MeshRenderer::Update2()
{
	Component::Update2();
}

void MeshRenderer::Enable()
{
	Component::Enable();
}

void MeshRenderer::Disable()
{
	Component::Disable();

}

void MeshRenderer::Destroy()
{
	Component::Destroy();

	//RemoveStructuredSetter(_setter_ForwardLight);
	RemoveStructuredSetter(BufferType::ForwardLightParam);
	RemoveStructuredSetter(BufferType::ObjectSettingParam);
}

void MeshRenderer::RenderBegin()
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
		if(SceneManager::main->GetCurrentScene()->GetSceneType()==SceneType::Sea01)
			Gizmo::Width(3.0f);
		else
			Gizmo::Width(0.3f);
		Gizmo::Box(GetBound(), vec4(0, 0, 1.0f, 1.0f));
	}

	if (GetOwner()->GetName() == L"Harpoon")
	{
		int a = 5;
	}
	
	for (int i = 0; i < _mesh.size(); i++)
	{
		auto& currentMesh = _mesh[i];
		auto& currentMaterial = _uniqueMaterials[i % std::min(_mesh.size(), _uniqueMaterials.size())];

		SceneManager::main->GetCurrentScene()->AddRenderer(currentMaterial.get(), currentMesh.get(), this);
	}

	for (auto& ele : _depthNormalMaterials) 
	{
		auto& currentMesh = _mesh[ele.first];
		SceneManager::main->GetCurrentScene()->AddRenderer(ele.second.get(), currentMesh.get(), this);
	}

	for (auto& ele : _shadowMaterials) 
	{
		auto& currentMesh = _mesh[ele.first];
		SceneManager::main->GetCurrentScene()->AddRenderer(ele.second.get(), currentMesh.get(), this);
	}

	for (int j = 0; j < _sharedMaterials.size(); j++)
	{
		auto currentMaterial = _sharedMaterials[j];

		for (int i = 0; i < _mesh.size(); i++)
		{
			auto& currentMesh = _mesh[i];
			SceneManager::main->GetCurrentScene()->AddRenderer(currentMaterial.get(), currentMesh.get(), this);
		}
	}
}

void MeshRenderer::Rendering(Material* material, Mesh* mesh, int instanceCount)
{
	auto& cmdList = Core::main->GetCmdList();

	for (auto& data : _cbufferSetters) //transform , etc 
		data->SetData(material);

	if (material != nullptr)
		material->SetData();


	if (instanceCount <= 1 && HasInstanceBuffer())
	{
		cmdList->IASetVertexBuffers(1, 1, &_staticInstanceBuffer->_bufferView);
		instanceCount = _staticInstanceBuffer->writeIndex;
	}

	mesh->Redner(instanceCount);

}


void MeshRenderer::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);


}

void MeshRenderer::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{

}

void MeshRenderer::DebugRendering()
{

}

void MeshRenderer::SetDestroy()
{
	Component::SetDestroy();
}


void MeshRenderer::AddMesh(const std::shared_ptr<Mesh>& _mesh)
{
	this->_mesh.push_back(_mesh);
}

void MeshRenderer::SetMesh(const std::vector<std::shared_ptr<Mesh>>& mesh)
{
	_mesh = mesh;
}

void MeshRenderer::SetMaterials(const std::vector<std::shared_ptr<Material>>& _materials)
{
	this->_uniqueMaterials = _materials;
}

void MeshRenderer::AddMaterials(const std::vector<std::shared_ptr<Material>>& _materials)
{
	for (auto& ele : _materials)
		this->_uniqueMaterials.push_back(ele);
}

void MeshRenderer::AddMaterial(shared_ptr<Material>& material)
{
	this->_uniqueMaterials.push_back(material);
}

std::vector<std::shared_ptr<Material>>& MeshRenderer::GetMaterials()
{
	return _uniqueMaterials;
}

std::shared_ptr<Material> MeshRenderer::GetMaterial(int index)
{
	if (index >= _uniqueMaterials.size())
		return nullptr;

	return _uniqueMaterials[index];
}

void MeshRenderer::SetSharedMaterials(const std::vector<std::shared_ptr<Material>>& _materials)
{
	this->_sharedMaterials = _materials;
}

void MeshRenderer::AddSharedMaterials(const std::vector<std::shared_ptr<Material>>& _materials)
{
	for (auto& ele : _materials)
		this->_sharedMaterials.push_back(ele);
}

void MeshRenderer::SetSpecialMaterials()
{
	_depthNormalMaterials.clear();
	_shadowMaterials.clear();

	for (int i = 0; i < _mesh.size(); i++)
	{
		auto currentMaterial = _uniqueMaterials[i % std::min(_mesh.size(), _uniqueMaterials.size())];

		if (RENDER_PASS::HasFlag(currentMaterial->GetPass(), RENDER_PASS::Forward) && currentMaterial->GetPreDepthNormal())
		{
			auto depthNormalMaterial = currentMaterial->Clone();
			if (HasInstanceBuffer())
				depthNormalMaterial->SetShader(ResourceManager::main->_depthNormal_Instanced->GetShader());
			else
				depthNormalMaterial->SetShader(ResourceManager::main->_depthNormal->GetShader());
			depthNormalMaterial->SetPass(RENDER_PASS::Deferred);
			depthNormalMaterial->_instanceID = currentMaterial->GetID() + 2000000;
			_depthNormalMaterials.push_back(make_pair(i, depthNormalMaterial));
		}


		if ((RENDER_PASS::HasFlag(currentMaterial->GetPass(), RENDER_PASS::Forward)
			|| RENDER_PASS::HasFlag(currentMaterial->GetPass(), RENDER_PASS::Deferred)) && currentMaterial->GetShadowCasting())
		{

			auto shadowMaterial = ((HasInstanceBuffer() ? ResourceManager::main->_shadowCaster_Early_Instanced : ResourceManager::main->_shadowCaster_Early));
			auto mainTexture = currentMaterial->GetPropertyTexture("_BaseMap");
			if (mainTexture != nullptr && mainTexture->_isAlpha)
			{
				shadowMaterial = ((HasInstanceBuffer() ? ResourceManager::main->_shadowCaster_Instanced : ResourceManager::main->_shadowCaster));
				shadowMaterial = shadowMaterial->Clone();
				currentMaterial->CopyProperties(shadowMaterial);
				shadowMaterial->_instanceID = currentMaterial->GetID() + 1000000;
			}
			_shadowMaterials.push_back(make_pair(i, shadowMaterial));
		}
	}
}



