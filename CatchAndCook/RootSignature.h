#pragma once
class RootSignature
{
public:

	void Init();
	ComPtr<ID3D12RootSignature>	GetGraphicsRootSignature() { return _graphicsRootSignature; }
	ComPtr<ID3D12RootSignature>	GetComputeRootSignature() {return	_computeRootSignature;}

private:
	void InitGraphicsRootSignature();
	void InitComputeRootSignature();

private:
	ComPtr<ID3D12RootSignature>	_graphicsRootSignature;
	ComPtr<ID3D12RootSignature>	_computeRootSignature;

};

