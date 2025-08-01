#include "pch.h"
#include "BufferPool.h"
#include "Core.h"

/*************************
*                        *
*  ConstantBuffer Pool   *
*                        *
**************************/

CBufferPool::CBufferPool()
{
}

CBufferPool::~CBufferPool()
{
	_resource->Unmap(0,nullptr);
}

void CBufferPool::Init(uint32 size, uint32 count)
{

	_elementSize = (size + 255) & ~255;
	_count = count;

	uint64 bufferSize = _elementSize * _count;

	Core::main->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_resource));


	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = _count;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ThrowIfFailed(Core::main->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_heap)));

	
	BYTE* ptr =nullptr;
	CD3DX12_RANGE writeRange(0, 0);
	_resource->Map(0, &writeRange, reinterpret_cast<void**>(&ptr));

	_container.resize(_count);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = _resource->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = _elementSize;


	CD3DX12_CPU_DESCRIPTOR_HANDLE	heapHandle(_heap->GetCPUDescriptorHandleForHeapStart());
	uint32	DescriptorSize = Core::main->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	for (int i = 0; i < _count; i++)
	{
		Core::main->GetDevice()->CreateConstantBufferView(&cbvDesc, heapHandle);

		_container[i].CPUHandle = heapHandle;
		_container[i].GPUAdress = cbvDesc.BufferLocation;
		_container[i].ptr = ptr;

		heapHandle.Offset(1, DescriptorSize);
		cbvDesc.BufferLocation += _elementSize;
		ptr += _elementSize;
	}
}

void CBufferPool::Reset()
{
	_currentIndex = 0;
}

CBufferContainer* CBufferPool::Alloc(uint32 count)
{
	assert(_currentIndex < _count);
	CBufferContainer* data = &_container[_currentIndex];
	_currentIndex += data->count = count;
	data->isAlloc = true;
	return data;
}



/*************************
*                        *
*  TextureBuffer Pool    *
*                        *
**************************/
D3D12_CPU_DESCRIPTOR_HANDLE CBufferPool::cbufferTableHandle = {0};

void CBufferPool::SetCBufferTable(CBufferContainer* cbuffer, int registerIndex)
{
	int index = registerIndex - CBV_ROOT_INDEX_COUNT;
	assert(index >= 0);
	TableContainer table = Core::main->GetBufferManager()->GetTable()->Alloc(CBV_TABLE_COUNT);
	if (cbufferTableHandle.ptr != NULL)
		Core::main->GetBufferManager()->GetTable()->CopyHandles(table.CPUHandle, cbufferTableHandle, CBV_TABLE_COUNT);
	cbufferTableHandle = table.CPUHandle;
	Core::main->GetBufferManager()->GetTable()->CopyHandle(cbufferTableHandle, cbuffer->CPUHandle, index);
	Core::main->GetCmdList()->SetGraphicsRootDescriptorTable(CBV_TABLE_INDEX, table.GPUHandle);
}

void TextureBufferPool::Init(int32 SrvUavCount, int32 RTVCount ,int32 DSVCount)
{
	{
		_srvHeap.heapSize = SrvUavCount;
		_srvHeap.handleIncrementSize = Core::main->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		_srvHeap.indexGenator.resize(_srvHeap.heapSize, -1);

		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = SrvUavCount;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;  // Ensure shader visibility if needed

		ThrowIfFailed(Core::main->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&_srvHeap.heap)));
	}

	{
		_rtvHeap.heapSize = RTVCount;
		_rtvHeap.handleIncrementSize = Core::main->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		_rtvHeap.indexGenator.resize(_rtvHeap.heapSize, -1);

		D3D12_DESCRIPTOR_HEAP_DESC RTVHeapDesc = {};
		RTVHeapDesc.NumDescriptors = RTVCount;
		RTVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		RTVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;  // Ensure shader visibility if needed

		ThrowIfFailed(Core::main->GetDevice()->CreateDescriptorHeap(&RTVHeapDesc, IID_PPV_ARGS(&_rtvHeap.heap)));
	}

	{
		_dsvHeap.heapSize = DSVCount;
		_dsvHeap.handleIncrementSize = Core::main->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

		_dsvHeap.indexGenator.resize(_dsvHeap.heapSize, -1);

		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = DSVCount;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;  // Ensure shader visibility if needed

		ThrowIfFailed(Core::main->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&_dsvHeap.heap)));

	}

}

void TextureBufferPool::FreeSRVHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE start = _srvHeap.heap->GetCPUDescriptorHandleForHeapStart();
	DWORD index = (DWORD)(handle.ptr - start.ptr) / _srvHeap.handleIncrementSize;

	assert(index >= 0);

	_srvHeap.indexGenator[index] = -1;
	_srvHeap.currentIndex--;
}

void TextureBufferPool::FreeRTVHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE start = _rtvHeap.heap->GetCPUDescriptorHandleForHeapStart();
	DWORD index = (DWORD)(handle.ptr - start.ptr) / _rtvHeap.handleIncrementSize;

	assert(index >= 0);

	_rtvHeap.indexGenator[index] = -1;
	_rtvHeap.currentIndex--;

}

void TextureBufferPool::FreeDSVHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE start = _dsvHeap.heap->GetCPUDescriptorHandleForHeapStart();
	DWORD index = (DWORD)(handle.ptr - start.ptr) / _dsvHeap.handleIncrementSize;

	assert(index >= 0);

	_dsvHeap.indexGenator[index] = -1;
	_dsvHeap.currentIndex--;
}

void TextureBufferPool::AllocSRVDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE* handle)
{

	assert(_srvHeap.currentIndex < _srvHeap.heapSize);

	int32 index = AllocSRV();

	if (index == -1) {
		throw std::runtime_error("No free descriptor handles available");
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHandle(_srvHeap.heap->GetCPUDescriptorHandleForHeapStart(), index, _srvHeap.handleIncrementSize);
	*handle = DescriptorHandle;
	_srvHeap.currentIndex++;

}

void TextureBufferPool::AllocRTVDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE* handle)
{

	assert(_rtvHeap.currentIndex < _rtvHeap.heapSize);

	int32 index = AllocRTV();

	if (index == -1) {
		throw std::runtime_error("No free descriptor handles available");
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHandle(_rtvHeap.heap->GetCPUDescriptorHandleForHeapStart(), index, _rtvHeap.handleIncrementSize);
	*handle = DescriptorHandle;
	_rtvHeap.currentIndex++;
}

void TextureBufferPool::AllocDSVDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE* handle)
{

	assert(_dsvHeap.currentIndex < _dsvHeap.heapSize);

	int32 index = AllocDSV();

	if (index == -1) {
		throw std::runtime_error("No free descriptor handles available");
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHandle(_dsvHeap.heap->GetCPUDescriptorHandleForHeapStart(), index, _dsvHeap.handleIncrementSize);
	//_dsvHeap.heap->GetGPUDescriptorHandleForHeapStart()
	*handle = DescriptorHandle;
	_dsvHeap.currentIndex++;

}

int32 TextureBufferPool::AllocSRV()
{
	for (int i = 0; i < _srvHeap.indexGenator.size(); ++i)
	{
		if (_srvHeap.indexGenator[i] == -1)
		{
			_srvHeap.indexGenator[i] = 1;
			return i;
		}
	}

	return -1;  // No available slots
}

int32 TextureBufferPool::AllocRTV()
{
	for (int i = 0; i < _rtvHeap.indexGenator.size(); ++i)
	{
		if (_rtvHeap.indexGenator[i] == -1)
		{
			_rtvHeap.indexGenator[i] = 1;
			return i;
		}
	}

	return -1;  // No available slots
}

int32 TextureBufferPool::AllocDSV()
{
	for (int i = 0; i < _dsvHeap.indexGenator.size(); ++i)
	{
		if (_dsvHeap.indexGenator[i] == -1)
		{
			_dsvHeap.indexGenator[i] = 1;
			return i;
		}
	}

	return -1;  // No available slots
}


///////////////////////////////////////////////////////////


/*************************
*                        *
*    DescriptorTable     *
*                        *
**************************/

void DescritporTable::Init(uint32 count)
{

	_count = count;

	D3D12_DESCRIPTOR_HEAP_DESC desc;
	desc.NumDescriptors = _count;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.NodeMask = 0;

	ThrowIfFailed(Core::main->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_heap)));

	_size = Core::main->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	_cpuHandle = _heap->GetCPUDescriptorHandleForHeapStart();
	_gpuHandle = _heap->GetGPUDescriptorHandleForHeapStart();

}

TableContainer DescritporTable::Alloc(uint32 count)
{
	if (count + _currentIndex >= _count) assert(false);

	TableContainer container;

	container.CPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_cpuHandle, _currentIndex, _size);
	container.GPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(_gpuHandle, _currentIndex, _size);

	_currentIndex += count;
	return container;
}

void DescritporTable::CopyHandle(D3D12_CPU_DESCRIPTOR_HANDLE& destHandle, D3D12_CPU_DESCRIPTOR_HANDLE& sourceHandle, uint32 index)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE dest(destHandle, index, _size);
	Core::main->GetDevice()->CopyDescriptorsSimple(1, dest, sourceHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void DescritporTable::CopyHandles(D3D12_CPU_DESCRIPTOR_HANDLE& destHandle, D3D12_CPU_DESCRIPTOR_HANDLE& sourceHandle,
	uint32 size)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE dest(destHandle, 0, _size);
	Core::main->GetDevice()->CopyDescriptorsSimple(size, dest, sourceHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void DescritporTable::Reset()
{
	_currentIndex = 0;
}

InstanceBufferPool::InstanceBufferPool()
{
}

InstanceBufferPool::~InstanceBufferPool()
{
}

void InstanceBufferPool::Init(uint32 size, uint32 elementCount, uint32 bufferCount)
{

	_elementSize = size;
	_elementCount = elementCount;
	_bufferCount = bufferCount;
	_bufferSize = static_cast<uint64>(_elementSize) * _elementCount;

	uint64 fullSize = _bufferSize * _bufferCount;

	Core::main->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(fullSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_resource));


	BYTE* ptr = nullptr;
	CD3DX12_RANGE writeRange(0,0);
	_resource->Map(0,&writeRange,reinterpret_cast<void**>(&ptr));
	offsetPtr = ptr;
	_containers.resize(_bufferCount);


	D3D12_VERTEX_BUFFER_VIEW	_vertexBufferView = {};
	_vertexBufferView.BufferLocation = _resource->GetGPUVirtualAddress();
	_vertexBufferView.StrideInBytes = _elementSize;
	_vertexBufferView.SizeInBytes = _bufferSize;

	for(int i = 0; i < _bufferCount; i++)
	{

		_containers[i]._bufferView = _vertexBufferView;
		_containers[i].ptr = ptr;
		_containers[i].elementCount = _elementCount;
		_containers[i].writeOffset = 0;
		_containers[i].isAlloc = false;

		_vertexBufferView.BufferLocation += _bufferSize;
		ptr += _bufferSize;
	}
}

void InstanceBufferPool::Reset()
{
	for(int i=0; i< _containers.size(); i++)
	{
		_containers[i].isAlloc = false;
		_containers[i].writeOffset = 0;
	}
}

InstanceBufferContainer* InstanceBufferPool::Alloc()
{
	for(int i=0; i<	_containers.size(); i++)
	{
		if(!_containers[i].isAlloc)
		{
			_containers[i].isAlloc = true;
			_containers[i].writeOffset = 0;
			return &_containers[i];
		}
	}
	assert(false);
	return nullptr;
}