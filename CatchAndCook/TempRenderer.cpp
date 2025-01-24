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
}

void TempRenderer::Start()
{
	auto& size = _mesh->GetVertexSize();

	D3D12_RESOURCE_DESC uavBufferDesc = {};
	uavBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	uavBufferDesc.Width = size; 
	uavBufferDesc.Height = 1;
	uavBufferDesc.DepthOrArraySize = 1;
	uavBufferDesc.MipLevels = 1;
	uavBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavBufferDesc.SampleDesc.Count = 1;
	uavBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	uavBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	ThrowIfFailed(Core::main->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&uavBufferDesc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(&_uavBuffer)));

	D3D12_RESOURCE_DESC readBackDesc = uavBufferDesc;
	readBackDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	ThrowIfFailed(Core::main->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK),
		D3D12_HEAP_FLAG_NONE,
		&readBackDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&_readBackBuffer)));

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = _mesh->GetVertexCount();
	uavDesc.Buffer.StructureByteStride = _mesh->GetPerVertexSize();

	Core::main->GetBufferManager()->GetTextureBufferPool()->AllocSRVDescriptorHandle(&_uavDescriptorHandle);
	Core::main->GetDevice()->CreateUnorderedAccessView(_uavBuffer.Get(), nullptr, &uavDesc, _uavDescriptorHandle);

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

	_tableContainer = Core::main->GetBufferManager()->GetTable()->Alloc(UAV_TABLE_REGISTER_COUNT);
	Core::main->GetBufferManager()->GetTable()->CopyHandle(_tableContainer.CPUHandle, _uavDescriptorHandle, 0);

}

void TempRenderer::Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void TempRenderer::DebugRendering()
{
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

	cmdList->SetGraphicsRootDescriptorTable(UAV_TABLE_INDEX, _tableContainer.GPUHandle);

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

	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		_uavBuffer.Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COPY_SOURCE));

	cmdList->CopyResource(_readBackBuffer.Get(), _uavBuffer.Get());

	void* mappedData = nullptr;
	_readBackBuffer->Map(0, nullptr, &mappedData);

	Vertex_Static* vertexData = static_cast<Vertex_Static*>(mappedData);

	vector<Vertex_Static> v;

	for (int i = 0; i < _mesh->GetVertexCount(); ++i)
	{
		auto& vertex = vertexData[i];
		v.push_back(vertex);
	}

	_readBackBuffer->Unmap(0, nullptr);

	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		_uavBuffer.Get(),
		D3D12_RESOURCE_STATE_COPY_SOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS));


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

