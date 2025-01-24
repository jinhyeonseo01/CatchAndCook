#include "pch.h"
#include "TempRenderer.h"
#include "Material.h"
#include "Mesh.h"
#include "Sprite.h"
#include "GameObject.h"
#include "Transform.h"
TempRenderer::~TempRenderer()
{
}

bool TempRenderer::IsExecuteAble()
{
	return false;
}

void TempRenderer::Init()
{
	_debugShader = ResourceManager::main->Get<Shader>(L"normalDraw_sea");
}

void TempRenderer::Start()
{
	
}

void TempRenderer::Update()
{
}

void TempRenderer::Update2()
{
}

void TempRenderer::Enable()
{
}

void TempRenderer::Disable()
{
}

void TempRenderer::Destroy()
{
}

void TempRenderer::RenderBegin()
{
	Component::RenderBegin();

	for (auto& ele : _materials)
	{
		ele->_tableContainer = Core::main->GetBufferManager()->GetTable()->Alloc(SRV_TABLE_REGISTER_COUNT);
		ele->PushData();
		SceneManager::main->GetCurrentScene()->AddRenderer(ele, static_pointer_cast<TempRenderer>(shared_from_this()));
	}

}

void TempRenderer::Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void TempRenderer::DebugRendering()
{
	auto& cmdList = Core::main->GetCmdList();
	cmdList->SetPipelineState(_debugShader->_pipelineState.Get());
	GetOwner()->_transform->SetData();

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	if (_mesh->GetVertexCount() != 0)
	{
		if (_mesh->GetIndexCount() != 0)
		{
			cmdList->IASetVertexBuffers(0, 1, &_mesh->GetVertexView());
			cmdList->IASetIndexBuffer(&_mesh->GetIndexView());
			cmdList->DrawIndexedInstanced(_mesh->GetIndexCount(), 1, 0, 0, 0);
		}
		else
		{
			cmdList->IASetVertexBuffers(0, 1, &_mesh->GetVertexView());
			cmdList->DrawInstanced(_mesh->GetVertexCount(), 1, 0, 0);
		}
	}
}

void TempRenderer::SetDestroy()
{
}

void TempRenderer::DestroyComponentOnly()
{
}

void TempRenderer::Rendering(const std::shared_ptr<Material>& material)
{
	auto& cmdList = Core::main->GetCmdList();

	if (material != nullptr)
		material->SetData();
	 
	GetOwner()->_transform->SetData();

	cmdList->IASetPrimitiveTopology(_mesh->GetTopology());

	if (_mesh->GetVertexCount() != 0)
	{

		if (_mesh->GetIndexCount() != 0)
		{
			cmdList->IASetVertexBuffers(0, 1, &_mesh->GetVertexView());
			cmdList->IASetIndexBuffer(&_mesh->GetIndexView());
			cmdList->DrawIndexedInstanced(_mesh->GetIndexCount(), 1, 0, 0, 0);
		}

		else
		{
			cmdList->IASetVertexBuffers(0, 1, &_mesh->GetVertexView());
			cmdList->DrawInstanced(_mesh->GetVertexCount(), 1, 0, 0);
		}
	}




}

void TempRenderer::SetMesh(const std::shared_ptr<Mesh>& _mesh)
{
	this->_mesh = _mesh;
}

void TempRenderer::SetMaterials(const std::vector<std::shared_ptr<Material>>& _materials)
{
	this->_materials = _materials;
}

void TempRenderer::AddMaterials(const std::vector<std::shared_ptr<Material>>& _materials)
{
	for (auto& ele : _materials)
		this->_materials.push_back(ele);
}

