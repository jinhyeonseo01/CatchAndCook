#pragma once
#include "Core.h"

class StructuredBuffer
{
public:
	void Init(uint32 size, uint32 elementCount);
	void InitDefualtHeap(uint32 size, uint32 elementCount);

	template<class T>
	void CopyToDefaultHeap(vector<T>& vec)
	{
		assert(vec.size() <= _elementCount); 
		memcpy(static_cast<uint8*>(_mappedData), vec.data(), vec.size() * sizeof(T));
		_writeOffsetIndex = vec.size();
		_writeByteSizeIndex = sizeof(T) * vec.size();

		auto commandList = Core::main->GetResCmdList();

		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			_structuredDefaultBuffer.Get(),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_COPY_DEST));

		commandList->CopyResource(_structuredDefaultBuffer.Get(), _structuredUploadBuffer.Get());

		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			_structuredDefaultBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

		Core::main->ExcuteCommandQueue();
	}

	template<class T>
	void AddData(const T& data)
	{
		assert(_writeOffsetIndex < _elementCount);

		memcpy(static_cast<uint8*>(_mappedData) + _writeByteSizeIndex, &data, sizeof(T));
		_writeOffsetIndex++;
		_writeByteSizeIndex += sizeof(T);
	}

	template<class T>
	T* GetDataAddress(int count)
	{
		assert(_writeOffsetIndex + count < _elementCount);

		T* addr = reinterpret_cast<T*>(static_cast<uint8*>(_mappedData) + _writeByteSizeIndex);
		_writeOffsetIndex += count;
		_writeByteSizeIndex += sizeof(T) * count;
		return addr;
	}

	void Clear()
	{
		_writeOffsetIndex = 0;
		_writeByteSizeIndex = 0;
	}

	int GetOffset() const
	{
		return _writeOffsetIndex;
	}
	int GetByteSize() const
	{
		return _writeByteSizeIndex;
	}

public:
	ComPtr<ID3D12Resource>& GetDefaultBuffer() { _structuredDefaultBuffer; }
	ComPtr<ID3D12Resource>& GetUploadBuffer() { _structuredUploadBuffer; }

	D3D12_CPU_DESCRIPTOR_HANDLE& GetSRVHandle() { return _srvHandle; }
	D3D12_CPU_DESCRIPTOR_HANDLE& GetUAVHandle() { return _uavHandle; }
	int32 GetCount() const {return _writeOffsetIndex;}

private:
	ComPtr<ID3D12Resource> _structuredDefaultBuffer;
	ComPtr<ID3D12Resource> _structuredUploadBuffer;
	D3D12_CPU_DESCRIPTOR_HANDLE  _srvHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE  _uavHandle;
	uint32 _elementCount;
	uint64 _elementSize;

	void* _mappedData = nullptr;
	int _writeOffsetIndex = 0;
	int _writeByteSizeIndex = 0;
};

