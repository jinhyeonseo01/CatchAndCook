#pragma once

class Texture;

class Mesh 
{
public:

	Mesh();
	~Mesh();

	template<typename T>
	void Init(vector<T>& vec, vector<uint32>& index)
	{
		CreateVertxBuffer(vec);
		CreateIndexBuffer(index);
	}

	template<typename T>
	void Init(vector<T>& vec)
	{
		CreateVertxBuffer(vec);
	}

	void Redner(int instanceCount = 1);

private:

	template<typename T>
	void CreateVertxBuffer(vector<T>& vec)
	{
		_vertexCount = static_cast<uint32>(vec.size());
		uint32 bufferSize = _vertexCount * sizeof(T);

		//DEFAULT 버퍼 생성
		ThrowIfFailed( Core::main->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&_vertexBuffer)));


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


		auto& list  =Core::main->GetResCmdList();
		list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_vertexBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
		list->CopyBufferRegion(_vertexBuffer.Get(), 0, uploadBuffer, 0, bufferSize);
		list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

		Core::main->ExcuteCommandQueue();

		_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
		_vertexBufferView.StrideInBytes = sizeof(T);
		_vertexBufferView.SizeInBytes = bufferSize;

		if (uploadBuffer)
		{
			uploadBuffer->Release();
			uploadBuffer = nullptr;
		}

	}

	void CreateIndexBuffer(vector<uint32>& vec);

public:
	D3D12_VERTEX_BUFFER_VIEW& GetVertexView() { return _vertexBufferView; }
	D3D12_INDEX_BUFFER_VIEW& GetIndexView() { return _indexBufferView; }
	uint32& GetVertexCount() { return _vertexCount; }
	uint32& GetIndexCount() { return _indexCount; }
	void SetTopolgy(D3D_PRIMITIVE_TOPOLOGY topology) { _topology = topology; }
	D3D_PRIMITIVE_TOPOLOGY&  GetTopology() { return _topology; }

	void SetBound(BoundingBox box){_originalBound = box;};
	BoundingBox GetBound() const {return _originalBound;};
	BoundingBox CalculateBound(const Matrix& worldMatrix) const;
	uint32 GetID() {return _instanceID;}

	bool GetUseBuffer() const { return _UseBuffer; }
	void SetUseBuffer(bool useBuffer) { _UseBuffer =useBuffer; }
	void SetDrawCall(int vertexCount, int InstacnceCount) 
	{
		SetUseBuffer(false);
		_vertexCount = vertexCount; _InstacnceCount = InstacnceCount;
	}


private:
	bool _UseBuffer = true;
	int _InstacnceCount = 0;

	D3D_PRIMITIVE_TOPOLOGY _topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ComPtr<ID3D12Resource>		_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW	_vertexBufferView = {};
	uint32 _vertexCount = 0;

	ComPtr<ID3D12Resource>		_IndexBuffer;
	D3D12_INDEX_BUFFER_VIEW		_indexBufferView = {};
	uint32 _indexCount = 0;

	uint32 _instanceID=0;
	static uint32 _instanceIDGenator;

	BoundingBox _originalBound;
};

