﻿#pragma once
#include "Core.h"

class StructuredBuffer
{
public:
	void Init(uint32 size, uint32 elementCount);

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
	D3D12_CPU_DESCRIPTOR_HANDLE& GetSRVHandle() { return _srvHandle; }
	int32 GetCount() const {return _writeOffsetIndex;}

private:
	ComPtr<ID3D12Resource> _structuredBuffer;
	D3D12_CPU_DESCRIPTOR_HANDLE  _srvHandle;
	uint32 _elementCount;
	uint64 _elementSize;

	void* _mappedData = nullptr;
	int _writeOffsetIndex = 0;
	int _writeByteSizeIndex = 0;
};
