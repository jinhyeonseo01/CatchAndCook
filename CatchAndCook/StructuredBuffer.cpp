#include "pch.h"
#include "StructuredBuffer.h"
#include "Core.h"

void StructuredBuffer::Init(uint32 size, uint32 elementCount)
{
	_elementSize = size;
	_elementCount = elementCount;

	uint64 bufferSize = _elementSize * _elementCount;

	auto& device = Core::main->GetDevice();

	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_structuredUploadBuffer)));

	CD3DX12_RANGE readRange(0, 0);

	ThrowIfFailed(_structuredUploadBuffer->Map(0, &readRange, &_mappedData));


	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.NumElements = _elementCount;
	srvDesc.Buffer.StructureByteStride = _elementSize;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	Core::main->GetBufferManager()->GetTextureBufferPool()->AllocSRVDescriptorHandle(&_srvHandle);
	device->CreateShaderResourceView(_structuredUploadBuffer.Get(), &srvDesc, _srvHandle);


}

void StructuredBuffer::InitDefualtHeap(uint32 size, uint32 elementCount)
{
	_elementSize = size;
	_elementCount = elementCount;

	uint64 bufferSize = _elementSize * _elementCount;

	auto& device = Core::main->GetDevice();

	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(&_structuredDefaultBuffer)));

	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_structuredUploadBuffer)));


	CD3DX12_RANGE readRange(0, 0);
	ThrowIfFailed(_structuredUploadBuffer->Map(0, &readRange, &_mappedData));

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.NumElements = elementCount;
	srvDesc.Buffer.StructureByteStride = _elementSize;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;

	Core::main->GetBufferManager()->GetTextureBufferPool()->AllocSRVDescriptorHandle(&_srvHandle);

	device->CreateShaderResourceView(_structuredDefaultBuffer.Get(), &srvDesc, _srvHandle);

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = elementCount;
	uavDesc.Buffer.StructureByteStride = _elementSize;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;

	Core::main->GetBufferManager()->GetTextureBufferPool()->AllocSRVDescriptorHandle(&_uavHandle);

	device->CreateUnorderedAccessView(_structuredDefaultBuffer.Get(), nullptr, &uavDesc, _uavHandle);
};

