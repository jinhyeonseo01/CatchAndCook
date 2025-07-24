#include "pch.h"
#include "Mesh.h"
#include "Core.h"

uint32 Mesh::_instanceIDGenator=0;

Mesh::Mesh()
{
	_instanceID = _instanceIDGenator++;
}
Mesh::~Mesh()
{


}

void Mesh::Redner(int instanceCount)
{
	if (instanceCount == 0)
		return;

	auto& cmdList = Core::main->GetCmdList();
	cmdList->IASetPrimitiveTopology(_topology);

	if (_UseBuffer)
	{
		if (_indexCount != 0)
		{
			cmdList->IASetVertexBuffers(0, 1, &_vertexBufferView);
			cmdList->IASetIndexBuffer(&_indexBufferView);
			cmdList->DrawIndexedInstanced(_indexCount, instanceCount, 0, 0, 0);
		}
		else
		{
			cmdList->IASetVertexBuffers(0, 1, &_vertexBufferView);
			cmdList->DrawInstanced(_vertexCount, instanceCount, 0, 0);
		}
	}

	else
	{
 		cmdList->DrawInstanced(_vertexCount, _InstacnceCount, 0, 0);
	}
};

void Mesh::CreateIndexBuffer(vector<uint32>& vec)
{

	_indexCount = static_cast<uint32>(vec.size());
	uint32 bufferSize = _indexCount * sizeof(uint32);

	//DEFAULT 버퍼 생성
	ThrowIfFailed(Core::main->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&_IndexBuffer)));



	//UPLOAD 버퍼 생성
	ID3D12Resource* uploadBuffer = nullptr;

	ThrowIfFailed(Core::main->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadBuffer)));


	void* data = nullptr;
	CD3DX12_RANGE readRange(0, 0);
	ThrowIfFailed(uploadBuffer->Map(0, &readRange, &data));
	::memcpy(data, &vec[0], bufferSize);
	uploadBuffer->Unmap(0, nullptr);

	auto& list  = Core::main->GetResCmdList();
	//복사작업
	list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_IndexBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
	list->CopyBufferRegion(_IndexBuffer.Get(), 0, uploadBuffer, 0, bufferSize);
	list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_IndexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER));

	Core::main->ExcuteCommandQueue();

	_indexBufferView.BufferLocation = _IndexBuffer->GetGPUVirtualAddress();
	_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	_indexBufferView.SizeInBytes = bufferSize;

	if (uploadBuffer)
	{
		uploadBuffer->Release();
		uploadBuffer = nullptr;
	}

}

BoundingBox Mesh::CalculateBound(const Matrix& worldMatrix) const
{
	BoundingBox _finalBound;
	_originalBound.Transform(_finalBound, worldMatrix);
	return _finalBound;
};
