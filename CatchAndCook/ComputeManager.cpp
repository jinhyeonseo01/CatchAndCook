#include "pch.h"
#include "ComputeManager.h"
#include "LightManager.h"
#include "Camera.h"
#include "CameraManager.h"
#include "LightComponent.h"
#include "Shader.h"
#include "Texture.h"

unique_ptr<ComputeManager> ComputeManager::main = nullptr;

/*************************
*    ComputeBase         *
**************************/

ComputeBase::ComputeBase()
{
}

ComputeBase::~ComputeBase()
{
}


/*************************
*			Blur         *
**************************/

Blur::Blur()
{
}

Blur::~Blur()
{
}

void Blur::Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture)
{
	_pingtexture = pingTexture;
	_pongtexture = pongTexture;


	{
		_XBlurshader = make_shared<Shader>();
		ShaderInfo info;
		info._computeShader = true;

		_XBlurshader->Init(L"xblur.hlsl", {}, ShaderArg{ {{"CS_Main","cs"}}}, info);
		ResourceManager::main->Add<Shader>(L"xblur", _XBlurshader);
	}
	
	{
		_YBlurshader = make_shared<Shader>();
		ShaderInfo info;
		info._computeShader = true;

		_YBlurshader->Init(L"yblur.hlsl", {}, ShaderArg{ {{"CS_Main","cs"}} }, info);
		ResourceManager::main->Add<Shader>(L"yblur", _YBlurshader);
	}
	
#ifdef IMGUI_ON
	ImguiManager::main->_blurPtr = &_on;
#endif 


};

void Blur::DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	auto& intermediateTexutre = Core::main->GetRenderTarget()->GetRenderTarget();

	intermediateTexutre->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
	_pingtexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);

	cmdList->CopyResource(_pingtexture->GetResource().Get(), intermediateTexutre->GetResource().Get());
}

void Blur::Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	if (_on == false)
		return;

	DispatchBegin(cmdList);

	for (int i = 0; i < _blurCount; ++i)
	{
		XBlur(cmdList, x, y, z);
		YBlur(cmdList, x, y, z);
	}

	DispatchEnd(cmdList);
}

void Blur::DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	auto& intermediateTexutre = Core::main->GetRenderTarget()->GetRenderTarget();
	_pongtexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
	intermediateTexutre->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList->CopyResource(intermediateTexutre->GetResource().Get(), _pongtexture->GetResource().Get());
}

void Blur::XBlur(ComPtr<ID3D12GraphicsCommandList>& cmdList,int x, int y, int z)
{
	auto& table = Core::main->GetBufferManager()->GetTable();
	cmdList->SetPipelineState(_XBlurshader->_pipelineState.Get());

	_pingtexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
	_pongtexture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	_tableContainer = table->Alloc(10);

	table->CopyHandle(_tableContainer.CPUHandle, _pingtexture->GetSRVCpuHandle(), 0);
	table->CopyHandle(_tableContainer.CPUHandle, _pongtexture->GetUAVCpuHandle(), 5);

	cmdList->SetComputeRootDescriptorTable(10, _tableContainer.GPUHandle);
	cmdList->Dispatch(x, y, z);
}

void Blur::YBlur(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	auto& table = Core::main->GetBufferManager()->GetTable();
	cmdList->SetPipelineState(_YBlurshader->_pipelineState.Get());

	_pingtexture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	_pongtexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	_tableContainer = table->Alloc(10);

	table->CopyHandle(_tableContainer.CPUHandle, _pingtexture->GetUAVCpuHandle(), 5);
	table->CopyHandle(_tableContainer.CPUHandle, _pongtexture->GetSRVCpuHandle(), 0);

	cmdList->SetComputeRootDescriptorTable(10, _tableContainer.GPUHandle);
	cmdList->Dispatch(x, y, z);
}

void Blur::Resize()
{
	
}

/*************************
*			Bloom        *
**************************/
Bloom::Bloom()
{
}

Bloom::~Bloom()
{
}

void Bloom::Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture)
{
	_pingtexture = pingTexture;
	_pongtexture = pongTexture;

	_bloomTexture = make_shared<Texture>();
	_bloomTexture->CreateStaticTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::UAV 
		,false, false);

	_XBlurshader = ResourceManager::main->Get<Shader>(L"xblur");
	_YBlurshader = ResourceManager::main->Get<Shader>(L"yblur");

	{
		_BlackShader = make_shared<Shader>();
		ShaderInfo info;
		info._computeShader = true;
		_BlackShader->Init(L"blackShader.hlsl", {}, ShaderArg{ {{"CS_Main","cs"}} }, info);
		ResourceManager::main->Add<Shader>(L"blackShader", _BlackShader);
	}

	{
		_Bloomshader = make_shared<Shader>();
		ShaderInfo info;
		info._computeShader = true;
		_Bloomshader->Init(L"bloomShader.hlsl", {}, ShaderArg{ {{"CS_Main","cs"}} }, info);
		ResourceManager::main->Add<Shader>(L"bloom", _Bloomshader);
	}


#ifdef IMGUI_ON
	ImguiManager::main->_bloomPtr = &_on;
#endif 

	
}

void Bloom::DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{

}

void Bloom::Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	if (_on == false)
		return;

	Black(cmdList, x, y, z);

	for (int i = 0; i < _blurCount; ++i)
	{
		XBlur(cmdList, x, y, z);
		YBlur(cmdList, x, y, z);
	}

	Blooming(cmdList, x, y, z);

	DispatchEnd(cmdList);

}

void Bloom::DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	auto& renderTarget = Core::main->GetRenderTarget()->GetRenderTarget();
	_bloomTexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList->CopyResource(renderTarget->GetResource().Get(), _bloomTexture->GetResource().Get());
}

void Bloom::Resize()
{
	auto& textureBufferPool = Core::main->GetBufferManager()->GetTextureBufferPool();
	textureBufferPool->FreeSRVHandle(_bloomTexture->GetUAVCpuHandle());

	_bloomTexture->CreateStaticTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::UAV
		, false, false);

}

void Bloom::Black(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	auto& renderTarget = Core::main->GetRenderTarget()->GetRenderTarget();
	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
	auto& MAOTexture = Core::main->GetGBuffer()->GetTexture(3);
	MAOTexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
	_pingtexture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	auto& table = Core::main->GetBufferManager()->GetTable();
	cmdList->SetPipelineState(_BlackShader->_pipelineState.Get());
	_tableContainer = table->Alloc(10);
	table->CopyHandle(_tableContainer.CPUHandle, renderTarget->GetSRVCpuHandle(), 0);
	table->CopyHandle(_tableContainer.CPUHandle, MAOTexture->GetSRVCpuHandle(), 1);
	table->CopyHandle(_tableContainer.CPUHandle, _pingtexture->GetUAVCpuHandle(), 5);
	cmdList->SetComputeRootDescriptorTable(10, _tableContainer.GPUHandle);
	cmdList->Dispatch(x, y, z);
}

void Bloom::XBlur(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	auto& table = Core::main->GetBufferManager()->GetTable();
	cmdList->SetPipelineState(_XBlurshader->_pipelineState.Get());
	_pingtexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
	_pongtexture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	_tableContainer = table->Alloc(10);
	table->CopyHandle(_tableContainer.CPUHandle, _pingtexture->GetSRVCpuHandle(), 0);
	table->CopyHandle(_tableContainer.CPUHandle, _pongtexture->GetUAVCpuHandle(), 5);
	cmdList->SetComputeRootDescriptorTable(10, _tableContainer.GPUHandle);
	cmdList->Dispatch(x, y, z);
}

void Bloom::YBlur(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	auto& table = Core::main->GetBufferManager()->GetTable();
	cmdList->SetPipelineState(_YBlurshader->_pipelineState.Get());
	_pingtexture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	_pongtexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
	_tableContainer = table->Alloc(10);
	table->CopyHandle(_tableContainer.CPUHandle, _pingtexture->GetUAVCpuHandle(), 5);
	table->CopyHandle(_tableContainer.CPUHandle, _pongtexture->GetSRVCpuHandle(), 0);
	cmdList->SetComputeRootDescriptorTable(10, _tableContainer.GPUHandle);
	cmdList->Dispatch(x, y, z);
}

void Bloom::Blooming(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	auto& table = Core::main->GetBufferManager()->GetTable();
	cmdList->SetPipelineState(_Bloomshader->_pipelineState.Get());

	auto& renderTarget  = Core::main->GetRenderTarget()->GetRenderTarget();
	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
	_pongtexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
	_bloomTexture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	_tableContainer = table->Alloc(10);

	table->CopyHandle(_tableContainer.CPUHandle, _pongtexture->GetSRVCpuHandle(), 0);
	table->CopyHandle(_tableContainer.CPUHandle, renderTarget->GetSRVCpuHandle(), 1);
	table->CopyHandle(_tableContainer.CPUHandle, _bloomTexture->GetUAVCpuHandle(), 5);
	cmdList->SetComputeRootDescriptorTable(10, _tableContainer.GPUHandle);
	cmdList->Dispatch(x, y, z);

}

GodRay::GodRay()
{
}

GodRay::~GodRay()
{
}

void GodRay::Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture)
{
	_pingtexture = pingTexture;
	_pongtexture = pongTexture;

	_rayEmissionTexture = make_shared<Texture>();
	_rayEmissionTexture->CreateStaticTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::UAV
		, false, false);

	{
		_BlackShader = make_shared<Shader>();
		ShaderInfo info;
		info._computeShader = true;
		_BlackShader->Init(L"blackShader_ray.hlsl", {}, ShaderArg{ {{"CS_Main","cs"}} }, info);
		ResourceManager::main->Add<Shader>(L"blackShader_ray", _BlackShader);
	}

	{
		_RayShader = make_shared<Shader>();
		ShaderInfo info;
		info._computeShader = true;
		_RayShader->Init(L"GodRay.hlsl", {}, ShaderArg{ {{"CS_Main","cs"}} }, info);
		ResourceManager::main->Add<Shader>(L"GodRay", _RayShader);
	}



#ifdef IMGUI_ON
	ImguiManager::main->_godRayPtr = &_on;
#endif 
}

void GodRay::Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	if (_on == false)
		return;

	if (auto mainLight = LightComponent::GetMainLight()->GetLight())
	{
		param.lightWorldPos = Vector4(mainLight->position);
		param.lightWorldPos.w = 1;

		auto cameraParam = CameraManager::main->GetActiveCamera()->GetCameraParam();
		Vector4 ndc;
		Vector4::Transform(cameraParam.cameraPos - mainLight->direction * 100, cameraParam.VPMatrix, ndc);

		ndc /= abs(ndc.w);
		ndc.y *= -1;
		ndc.x = ndc.x * 0.5 + 0.5;
		ndc.y = ndc.y * 0.5 + 0.5;
		param.lightScreenUV = Vector2(ndc.x, ndc.y);

		param.sampleCount = 50;
		param.decay = 0.95;
		param.exposure = 0.235 * std::clamp((mainLight->direction.Dot(-Vector3(cameraParam.cameraLook)) * 0.5 + 0.5) * 1.5, 0.0, 1.0)
			* std::clamp((1 - mainLight->intensity) * 1.5f, 0.0f, 1.0f);
		param.exposure = std::max(param.exposure, 0.0f);

		if (param.exposure <= 0.01)
			return;

		Black(cmdList, x, y, z);
		Ray(cmdList, x, y, z);
		DispatchEnd(cmdList);
	}
}




void GodRay::DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
}

void GodRay::DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	auto& depthTexture = Core::main->GetRenderTarget()->GetDSTexture();
	auto& renderTarget = Core::main->GetRenderTarget()->GetRenderTarget();
	_rayEmissionTexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList->CopyResource(renderTarget->GetResource().Get(), _rayEmissionTexture->GetResource().Get());

	depthTexture->ResourceBarrier(D3D12_RESOURCE_STATE_DEPTH_WRITE);
}

void GodRay::Resize()
{
	auto& textureBufferPool = Core::main->GetBufferManager()->GetTextureBufferPool();
	textureBufferPool->FreeSRVHandle(_rayEmissionTexture->GetUAVCpuHandle());

	_rayEmissionTexture->CreateStaticTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::UAV | TextureUsageFlags::SRV
		, false, false);
}

void GodRay::Black(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	auto& renderTarget = Core::main->GetRenderTarget()->GetRenderTarget();
	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
	auto& MAOTexture = Core::main->GetGBuffer()->GetTexture(3);

	auto& depthTexture = Core::main->GetRenderTarget()->GetDSTexture();
	depthTexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	MAOTexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
	_pingtexture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	auto& table = Core::main->GetBufferManager()->GetTable();
	cmdList->SetPipelineState(_BlackShader->_pipelineState.Get());
	_tableContainer = table->Alloc(10);
	table->CopyHandle(_tableContainer.CPUHandle, renderTarget->GetSRVCpuHandle(), 0);
	table->CopyHandle(_tableContainer.CPUHandle, MAOTexture->GetSRVCpuHandle(), 1);
	table->CopyHandle(_tableContainer.CPUHandle, depthTexture->GetSRVCpuHandle(), 2);
	table->CopyHandle(_tableContainer.CPUHandle, _pingtexture->GetUAVCpuHandle(), 5);
	cmdList->SetComputeRootDescriptorTable(10, _tableContainer.GPUHandle);
	cmdList->Dispatch(x, y, z);
}

void GodRay::Ray(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	auto& renderTarget = Core::main->GetRenderTarget()->GetRenderTarget();
	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	_pingtexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	_rayEmissionTexture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	auto& table = Core::main->GetBufferManager()->GetTable();
	cmdList->SetPipelineState(_RayShader->_pipelineState.Get());

	_tableContainer = table->Alloc(10);


	table->CopyHandle(_tableContainer.CPUHandle, renderTarget->GetSRVCpuHandle(), 1);

	table->CopyHandle(_tableContainer.CPUHandle, _pingtexture->GetSRVCpuHandle(), 2);

	table->CopyHandle(_tableContainer.CPUHandle, _rayEmissionTexture->GetUAVCpuHandle(), 5);


	auto CbufferContainer = Core::main->GetBufferManager()->CreateAndGetBufferPool(BufferType::GodRayParam, sizeof(GodRayParam), 1)->Alloc(1);
	memcpy(CbufferContainer->ptr, (void*)&param, sizeof(GodRayParam));
	cmdList->SetComputeRootConstantBufferView(1, CbufferContainer->GPUAdress);

	cmdList->SetComputeRootDescriptorTable(10, _tableContainer.GPUHandle);
	cmdList->Dispatch(x, y, z);
	
}

FXAA::FXAA()
{
}

FXAA::~FXAA()
{
}

void FXAA::Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture)
{
	_pingtexture = pingTexture;

	{
		_AAShader = make_shared<Shader>();
		ShaderInfo info;
		info._computeShader = true;
		_AAShader->Init(L"FXAA.hlsl", {}, ShaderArg{ {{"CS_Main","cs"}} }, info);
		ResourceManager::main->Add<Shader>(L"FXAA", _AAShader);
	}

#ifdef IMGUI_ON
	ImguiManager::main->_fxaa = &_on;
#endif
}

void FXAA::Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	if (!_on)
		return;
	auto& renderTarget = Core::main->GetRenderTarget()->GetRenderTarget();
	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	_pingtexture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);


	auto& table = Core::main->GetBufferManager()->GetTable();
	cmdList->SetPipelineState(_AAShader->_pipelineState.Get());

	_tableContainer = table->Alloc(10);



	table->CopyHandle(_tableContainer.CPUHandle, renderTarget->GetSRVCpuHandle(), 0);
	table->CopyHandle(_tableContainer.CPUHandle, _pingtexture->GetUAVCpuHandle(), 5);


	auto CbufferContainer = Core::main->GetBufferManager()->CreateAndGetBufferPool(BufferType::FXAAParams, sizeof(FXAAParams), 1)->Alloc(1);
	params.uQualityEdge = 6;
	params.uQualitySubpix = 0.75;
	params.uQualityEdgeThreshold = 0.0225;
	params.uQualityEdgeThresholdMin = 0.01;

	memcpy(CbufferContainer->ptr, (void*)&params, sizeof(FXAAParams));
	cmdList->SetComputeRootConstantBufferView(1, CbufferContainer->GPUAdress);

	cmdList->SetComputeRootDescriptorTable(10, _tableContainer.GPUHandle);
	cmdList->Dispatch(x, y, z);

	_pingtexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList->CopyResource(renderTarget->GetResource().Get(), _pingtexture->GetResource().Get());
}

void FXAA::DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
}

void FXAA::DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
}

void FXAA::Resize()
{
}


DOF::DOF()
{
}

DOF::~DOF()
{
}

void DOF::Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture)
{
	_pingtexture = pingTexture;
	_pongtexture = pongTexture;


	{
		_XBlurshader = make_shared<Shader>();
		ShaderInfo info;
		info._computeShader = true;


		_XBlurshader->Init(L"DOF_xblur.hlsl", {}, ShaderArg{ {{"CS_Main","cs"}} }, info);
		ResourceManager::main->Add<Shader>(L"DOF_xblur", _XBlurshader);
	}

	{
		_YBlurshader = make_shared<Shader>();
		ShaderInfo info;
		info._computeShader = true;

		_YBlurshader->Init(L"DOF_yblur.hlsl", {}, ShaderArg{ {{"CS_Main","cs"}} }, info);
		ResourceManager::main->Add<Shader>(L"DOF_yblur", _YBlurshader);
	}

#ifdef IMGUI_ON
	ImguiManager::main->_dofPtr = &_on;
#endif 


};

void DOF::DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	auto& intermediateTexutre = Core::main->GetRenderTarget()->GetRenderTarget();

	intermediateTexutre->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
	_pingtexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);

	cmdList->CopyResource(_pingtexture->GetResource().Get(), intermediateTexutre->GetResource().Get());
}

void DOF::Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	if (_on == false)
		return;

	DispatchBegin(cmdList);

	DOFParam param;
	param.g_fogMin = 8;
	param.g_fogMax = 350;

	auto CbufferContainer = Core::main->GetBufferManager()->CreateAndGetBufferPool(BufferType::DOFParam, sizeof(DOFParam), 1)->Alloc(1);
	memcpy(CbufferContainer->ptr, (void*)&param, sizeof(DOFParam));
	cmdList->SetComputeRootConstantBufferView(1, CbufferContainer->GPUAdress);

	for (int i = 0; i < _blurCount; ++i)
	{
		XBlur(cmdList, x, y, z);
		YBlur(cmdList, x, y, z);
	}

	DispatchEnd(cmdList);
}

void DOF::DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	auto& intermediateTexutre = Core::main->GetRenderTarget()->GetRenderTarget();
	_pingtexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
	intermediateTexutre->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList->CopyResource(intermediateTexutre->GetResource().Get(), _pingtexture->GetResource().Get());

	auto& depthTexture = Core::main->GetRenderTarget()->GetDSTexture();
	depthTexture->ResourceBarrier(D3D12_RESOURCE_STATE_DEPTH_WRITE);
}

void DOF::XBlur(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	auto& table = Core::main->GetBufferManager()->GetTable();
	cmdList->SetPipelineState(_XBlurshader->_pipelineState.Get());

	_pingtexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
	_pongtexture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	_tableContainer = table->Alloc(10);


	auto& depthTexture = Core::main->GetRenderTarget()->GetDSTexture();
	depthTexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	table->CopyHandle(_tableContainer.CPUHandle, depthTexture->GetSRVCpuHandle(), 1);

	table->CopyHandle(_tableContainer.CPUHandle, _pingtexture->GetSRVCpuHandle(), 0);
	table->CopyHandle(_tableContainer.CPUHandle, _pongtexture->GetUAVCpuHandle(), 5);

	cmdList->SetComputeRootDescriptorTable(10, _tableContainer.GPUHandle);
	cmdList->Dispatch(x, y, z);
}

void DOF::YBlur(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	auto& table = Core::main->GetBufferManager()->GetTable();
	cmdList->SetPipelineState(_YBlurshader->_pipelineState.Get());

	_pingtexture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	_pongtexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	_tableContainer = table->Alloc(10);


	auto& depthTexture = Core::main->GetRenderTarget()->GetDSTexture();
	depthTexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	table->CopyHandle(_tableContainer.CPUHandle, depthTexture->GetSRVCpuHandle(), 1);

	table->CopyHandle(_tableContainer.CPUHandle, _pingtexture->GetUAVCpuHandle(), 5);
	table->CopyHandle(_tableContainer.CPUHandle, _pongtexture->GetSRVCpuHandle(), 0);

	cmdList->SetComputeRootDescriptorTable(10, _tableContainer.GPUHandle);
	cmdList->Dispatch(x, y, z);
}

void DOF::Resize()
{

}

/*************************
*	DepthRender          *
**************************/

DepthRender::DepthRender()
{
}

DepthRender::~DepthRender()
{
}

void DepthRender::Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture)
{
	_pingTexture = pingTexture;
	

	_shader = make_shared<Shader>();
	ShaderInfo info;
	info._computeShader = true;
	_shader->Init(L"depthRender.hlsl", {}, ShaderArg{ {{"CS_Main","cs"}} }, info);


#ifdef IMGUI_ON
	ImguiManager::main->_fogParam = &_fogParam;
#endif // IMGUI_ON

	
}

void DepthRender::Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	if (_fogParam.depthRendering == 0)
		return;

	auto& table = Core::main->GetBufferManager()->GetTable();
	cmdList->SetPipelineState(_shader->_pipelineState.Get());
	_pingTexture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	_tableContainer = table->Alloc(10);

	auto& depthTexture = Core::main->GetRenderTarget()->GetDSTexture();
	depthTexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	auto& renderTarget = Core::main->GetRenderTarget()->GetRenderTarget();
	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	auto& PositionTexture =Core::main->GetGBuffer()->GetTexture(0);
	PositionTexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	table->CopyHandle(_tableContainer.CPUHandle, depthTexture->GetSRVCpuHandle(), 0);
	table->CopyHandle(_tableContainer.CPUHandle, renderTarget->GetSRVCpuHandle(), 1);
	table->CopyHandle(_tableContainer.CPUHandle, PositionTexture->GetSRVCpuHandle(), 2);
	table->CopyHandle(_tableContainer.CPUHandle, _pingTexture->GetUAVCpuHandle(), 5);

	cmdList->SetComputeRootDescriptorTable(10, _tableContainer.GPUHandle);


	auto CbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::FogParam)->Alloc(1);
	memcpy(CbufferContainer->ptr, (void*)&_fogParam, sizeof(FogParam));
	cmdList->SetComputeRootConstantBufferView(1, CbufferContainer->GPUAdress);
	

	cmdList->Dispatch(x, y, z);


	_pingTexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList->CopyResource(renderTarget->GetResource().Get(), _pingTexture->GetResource().Get());

	depthTexture->ResourceBarrier(D3D12_RESOURCE_STATE_DEPTH_WRITE);
}

void DepthRender::DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
}

void DepthRender::DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{

}

void DepthRender::Resize()
{

}

FieldFogRender::FieldFogRender()
{
}

FieldFogRender::~FieldFogRender()
{
}

void FieldFogRender::Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture)
{
	_pingTexture = pingTexture;

	_shader = make_shared<Shader>();
	ShaderInfo info;
	info._computeShader = true;
	_shader->Init(L"depthRender_fieldFog.hlsl", {}, ShaderArg{ {{"CS_Main","cs"}} }, info);

	_fogParam.g_fogColor = vec3(0.48f, 0.56f, 0.72f) * 0.8;
	_fogParam.power = 1.0f;
	_fogParam.g_fogMin = 40.0f;
	_fogParam.g_fogMax = 350;

	ImguiManager::main->mainField_fog = &_onOff;
}

void FieldFogRender::Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	if (!_onOff)
		return;
	auto& table = Core::main->GetBufferManager()->GetTable();
	cmdList->SetPipelineState(_shader->_pipelineState.Get());
	_pingTexture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	_tableContainer = table->Alloc(10);

	auto& depthTexture = Core::main->GetRenderTarget()->GetDSTexture();
	depthTexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	auto& renderTarget = Core::main->GetRenderTarget()->GetRenderTarget();
	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	auto& PositionTexture = Core::main->GetGBuffer()->GetTexture(0);
	PositionTexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	table->CopyHandle(_tableContainer.CPUHandle, depthTexture->GetSRVCpuHandle(), 0);
	table->CopyHandle(_tableContainer.CPUHandle, renderTarget->GetSRVCpuHandle(), 1);
	table->CopyHandle(_tableContainer.CPUHandle, PositionTexture->GetSRVCpuHandle(), 2);
	table->CopyHandle(_tableContainer.CPUHandle, _pingTexture->GetUAVCpuHandle(), 5);

	cmdList->SetComputeRootDescriptorTable(10, _tableContainer.GPUHandle);


	auto CbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::FogParam)->Alloc(1);
	memcpy(CbufferContainer->ptr, (void*)&_fogParam, sizeof(FogParam));
	cmdList->SetComputeRootConstantBufferView(1, CbufferContainer->GPUAdress);


	cmdList->Dispatch(x, y, z);


	_pingTexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList->CopyResource(renderTarget->GetResource().Get(), _pingTexture->GetResource().Get());

	depthTexture->ResourceBarrier(D3D12_RESOURCE_STATE_DEPTH_WRITE);
}

void FieldFogRender::DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{

}

void FieldFogRender::DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{

}

void FieldFogRender::Resize()
{
	

}

/*********************************************
*	         UnderWaterEffect				 *
**********************************************/

UnderWaterEffect::UnderWaterEffect()
{
}

UnderWaterEffect::~UnderWaterEffect()
{
}

void UnderWaterEffect::Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture)
{
	_pingTexture = pingTexture;

	_shader = make_shared<Shader>();
	ShaderInfo info;
	info._computeShader = true;
	_shader->Init(L"underWaterEffect.hlsl", {}, ShaderArg{ {{"CS_Main","cs"}} }, info);

	_colorGrading = make_shared<Texture>();
	_colorGrading->Init(L"../Resources/Textures/sea/TCP2_CustomRamp.png");



#ifdef IMGUI_ON
	ImguiManager::main->_underWaterParam = &_underWaterParam;
#endif // IMGUI_ON

}

void UnderWaterEffect::Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	if (_underWaterParam.g_on == -1)
		return;

	auto& table = Core::main->GetBufferManager()->GetTable();
	cmdList->SetPipelineState(_shader->_pipelineState.Get());
	_pingTexture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	_tableContainer = table->Alloc(10);

	auto& depthTexture = Core::main->GetRenderTarget()->GetDSTexture();
	depthTexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	auto& renderTarget = Core::main->GetRenderTarget()->GetRenderTarget();
	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	auto& PositionTexture = Core::main->GetGBuffer()->GetTexture(0);
	PositionTexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	auto& NormalTexture = Core::main->GetGBuffer()->GetTexture(1);
	PositionTexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	auto& MAOTexture = Core::main->GetGBuffer()->GetTexture(3);
	MAOTexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);


	table->CopyHandle(_tableContainer.CPUHandle, depthTexture->GetSRVCpuHandle(), 0);
	table->CopyHandle(_tableContainer.CPUHandle, renderTarget->GetSRVCpuHandle(), 1);
	table->CopyHandle(_tableContainer.CPUHandle, PositionTexture->GetSRVCpuHandle(), 2);
	table->CopyHandle(_tableContainer.CPUHandle, NormalTexture->GetSRVCpuHandle(), 3);
	
	table->CopyHandle(_tableContainer.CPUHandle, _pingTexture->GetUAVCpuHandle(), 5);

	cmdList->SetComputeRootDescriptorTable(10, _tableContainer.GPUHandle);

	auto CbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::UnderWaterParam)->Alloc(1);
	memcpy(CbufferContainer->ptr, (void*)&_underWaterParam, sizeof(_underWaterParam));
	cmdList->SetComputeRootConstantBufferView(1, CbufferContainer->GPUAdress);

	cmdList->Dispatch(x, y, z);

	_pingTexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList->CopyResource(renderTarget->GetResource().Get(), _pingTexture->GetResource().Get());
	depthTexture->ResourceBarrier(D3D12_RESOURCE_STATE_DEPTH_WRITE);
}

void UnderWaterEffect::DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
}

void UnderWaterEffect::DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
}

void UnderWaterEffect::Resize()
{
	

}

VignetteRender::VignetteRender()
{
}

VignetteRender::~VignetteRender()
{
}

void VignetteRender::Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture)
{
	_pingTexture = pingTexture;

	_shader = make_shared<Shader>();
	ShaderInfo info;
	info._computeShader = true;
	_shader->Init(L"vignette.hlsl", {}, ShaderArg{ {{"CS_Main","cs"}} }, info);

	ImguiManager::main->mainField_vignette = &_onOff;
}

void VignetteRender::Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	if (!_onOff)
		return;
	auto& table = Core::main->GetBufferManager()->GetTable();
	cmdList->SetPipelineState(_shader->_pipelineState.Get());
	_pingTexture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	_tableContainer = table->Alloc(10);

	auto& renderTarget = Core::main->GetRenderTarget()->GetRenderTarget();
	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	table->CopyHandle(_tableContainer.CPUHandle, renderTarget->GetSRVCpuHandle(), 1);
	table->CopyHandle(_tableContainer.CPUHandle, _pingTexture->GetUAVCpuHandle(), 5);

	cmdList->SetComputeRootDescriptorTable(10, _tableContainer.GPUHandle);


	auto CbufferContainer = Core::main->GetBufferManager()->CreateAndGetBufferPool(BufferType::VignetteParam, sizeof(VignetteParam), 1)->Alloc(1);
	memcpy(CbufferContainer->ptr, (void*)&_vignetteParam, sizeof(VignetteParam));
	cmdList->SetComputeRootConstantBufferView(1, CbufferContainer->GPUAdress);

	cmdList->Dispatch(x, y, z);


	_pingTexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList->CopyResource(renderTarget->GetResource().Get(), _pingTexture->GetResource().Get());
}

void VignetteRender::DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
}

void VignetteRender::DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
}

void VignetteRender::Resize()
{

}

SSAORender::SSAORender()
{
}

SSAORender::~SSAORender()
{
}

void SSAORender::Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture)
{
	_pingTexture = pingTexture;

	_ssaoTexture = make_shared<Texture>();
	_ssaoTexture->CreateStaticTexture(DXGI_FORMAT_R32_FLOAT, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::UAV | TextureUsageFlags::SRV, false, false);

	_shader = make_shared<Shader>();
	ShaderInfo info;
	info._computeShader = true;
	_shader->Init(L"SSAO.hlsl", {}, ShaderArg{ {{"CS_Main","cs"}} }, info);

#ifdef IMGUI_ON
	ImguiManager::main->_ssaoOnOff = &ssaoOnOff;
#endif // IMGUI_ON
}

void SSAORender::Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	if (!ssaoOnOff)
		return;

	auto& table = Core::main->GetBufferManager()->GetTable();
	cmdList->SetPipelineState(_shader->_pipelineState.Get());

	_pingTexture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	_ssaoTexture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	_tableContainer = table->Alloc(10);

	auto& depthTexture = Core::main->GetRenderTarget()->GetDSTexture();
	depthTexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	auto& renderTarget = Core::main->GetRenderTarget()->GetRenderTarget();
	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	auto& NormalTexture = Core::main->GetGBuffer()->GetTexture(1);
	NormalTexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	auto& PositionTexture = Core::main->GetGBuffer()->GetTexture(0);
	PositionTexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	auto& MAOETexture = Core::main->GetGBuffer()->GetTexture(3);
	MAOETexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	table->CopyHandle(_tableContainer.CPUHandle, depthTexture->GetSRVCpuHandle(), 0);
	table->CopyHandle(_tableContainer.CPUHandle, renderTarget->GetSRVCpuHandle(), 1);
	table->CopyHandle(_tableContainer.CPUHandle, PositionTexture->GetSRVCpuHandle(), 2);
	table->CopyHandle(_tableContainer.CPUHandle, NormalTexture->GetSRVCpuHandle(), 3);
	table->CopyHandle(_tableContainer.CPUHandle, MAOETexture->GetSRVCpuHandle(), 4);

	table->CopyHandle(_tableContainer.CPUHandle, _pingTexture->GetUAVCpuHandle(), 5);
	table->CopyHandle(_tableContainer.CPUHandle, _ssaoTexture->GetUAVCpuHandle(), 6);

	cmdList->SetComputeRootDescriptorTable(10, _tableContainer.GPUHandle);


	cmdList->Dispatch(x, y, z);

	_pingTexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList->CopyResource(renderTarget->GetResource().Get(), _pingTexture->GetResource().Get());

	depthTexture->ResourceBarrier(D3D12_RESOURCE_STATE_DEPTH_WRITE);
}

void SSAORender::DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{

}

void SSAORender::DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{

}

void SSAORender::Resize()
{
	auto& textureBufferPool = Core::main->GetBufferManager()->GetTextureBufferPool();
	textureBufferPool->FreeSRVHandle(_ssaoTexture->GetUAVCpuHandle());
	_ssaoTexture->CreateStaticTexture(DXGI_FORMAT_R32_FLOAT, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::UAV | TextureUsageFlags::SRV, false, false);

}

ColorGradingRender::ColorGradingRender()
{
}

ColorGradingRender::~ColorGradingRender()
{
}

void ColorGradingRender::Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture)
{
	_pingTexture = pingTexture;

	_shader = make_shared<Shader>();
	ShaderInfo info;
	info._computeShader = true;
	_shader->Init(L"colorGrading.hlsl", {}, ShaderArg{ {{"CS_Main","cs"}} }, info);

#ifdef IMGUI_ON
	ImguiManager::main->_colorGradingOnOff = &colorGradingOnOff;
#endif // IMGUI_ON
}

void ColorGradingRender::Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	if(!colorGradingOnOff)
		return;

	auto& table = Core::main->GetBufferManager()->GetTable();
	cmdList->SetPipelineState(_shader->_pipelineState.Get());
	_pingTexture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	_tableContainer = table->Alloc(10);

	auto& renderTarget = Core::main->GetRenderTarget()->GetRenderTarget();
	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	table->CopyHandle(_tableContainer.CPUHandle, renderTarget->GetSRVCpuHandle(), 1);
	table->CopyHandle(_tableContainer.CPUHandle, _pingTexture->GetUAVCpuHandle(), 5);

	cmdList->SetComputeRootDescriptorTable(10, _tableContainer.GPUHandle);
	cmdList->Dispatch(x, y, z);

	_pingTexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList->CopyResource(renderTarget->GetResource().Get(), _pingTexture->GetResource().Get());
}

void ColorGradingRender::DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
}

void ColorGradingRender::DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
}

void ColorGradingRender::Resize()
{

}


/*************************
*	ComputeManager       *
**************************/

void ComputeManager::Init()
{

	_pingTexture = make_shared<Texture>();
	_pingTexture->CreateStaticTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::UAV
		| TextureUsageFlags::SRV, false, false);

	_pongTexture = make_shared<Texture>();
	_pongTexture->CreateStaticTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::UAV
		| TextureUsageFlags::SRV, false, false);

	_blur = make_shared<Blur>();
	_blur->Init(_pingTexture,_pongTexture);

	_bloom = make_shared<Bloom>();
	_bloom->Init(_pingTexture, _pongTexture);


	_depthRender = make_shared<DepthRender>();
	_depthRender->Init(_pingTexture, _pongTexture);

	_underWaterEffect = make_shared<UnderWaterEffect>();
	_underWaterEffect->Init(_pingTexture, _pongTexture);

	_ssaoRender = make_shared<SSAORender>();
	_ssaoRender->Init(_pingTexture, _pongTexture);

	_vignetteRender = make_shared<VignetteRender>();
	_vignetteRender->Init(_pingTexture, _pongTexture);

	_fieldFogRender = make_shared<FieldFogRender>();
	_fieldFogRender->Init(_pingTexture, _pongTexture);

	_colorGradingRender = make_shared<ColorGradingRender>();
	_colorGradingRender->Init(_pingTexture, _pongTexture);

	_godrayRender = std::make_shared<GodRay>();
	_godrayRender->Init(_pingTexture, _pongTexture);

	_fxaaRender = std::make_shared<FXAA>();
	_fxaaRender->Init(_pingTexture, _pongTexture);

	_dofRender = std::make_shared<DOF>();
	_dofRender->Init(_pingTexture, _pongTexture);

	_colorGradingSea = std::make_shared<Scattering>();
	_colorGradingSea->Init(_pingTexture, _pongTexture);

	_changeSceneCompute = make_shared<ChangeSceneCompute>();
	_changeSceneCompute->Init(_pingTexture, _pongTexture);

	ImguiManager::main->mainField_total = &_mainFieldTotalOn;
}

void ComputeManager::DispatchAfterDeferred(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	if (!_mainFieldTotalOn)
		return;
	const int threadGroupSizeX = 16;
	const int threadGroupSizeY = 16;

	int dispatchX = static_cast<int>(std::ceil(static_cast<float>(WINDOW_WIDTH) / threadGroupSizeX));
	int dispatchY = static_cast<int>(std::ceil(static_cast<float>(WINDOW_HEIGHT) / threadGroupSizeY));

	int32 dispath[3] = { dispatchX,dispatchY,1 };

	_ssaoRender->Dispatch(cmdList, dispath[0], dispath[1], dispath[2]);


	Core::main->GetRenderTarget()->GetRenderTarget()->ResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET);
}

void ComputeManager::DispatchMainField(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	if (!_mainFieldTotalOn)
		return;
	const int threadGroupSizeX = 16;
	const int threadGroupSizeY = 16;

	int dispatchX = static_cast<int>(std::ceil(static_cast<float>(WINDOW_WIDTH) / threadGroupSizeX));
	int dispatchY = static_cast<int>(std::ceil(static_cast<float>(WINDOW_HEIGHT) / threadGroupSizeY));

	int32 dispath[3] = { dispatchX,dispatchY,1 };



	_depthRender->Dispatch(cmdList, dispath[0], dispath[1], dispath[2]);

	_fieldFogRender->Dispatch(cmdList, dispath[0], dispath[1], dispath[2]);

	_blur->Dispatch(cmdList, dispath[0], dispath[1], dispath[2]);

	_bloom->Dispatch(cmdList, dispath[0], dispath[1], dispath[2]);

	_godrayRender->Dispatch(cmdList, dispath[0], dispath[1], dispath[2]);

	_fxaaRender->Dispatch(cmdList, dispath[0], dispath[1], dispath[2]);

	_dofRender->Dispatch(cmdList, dispath[0], dispath[1], dispath[2]);

	_colorGradingRender->Dispatch(cmdList, dispath[0], dispath[1], dispath[2]);

	_vignetteRender->Dispatch(cmdList, dispath[0], dispath[1], dispath[2]);


	Core::main->GetRenderTarget()->GetRenderTarget()->ResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET);
	auto& depthTexture = Core::main->GetRenderTarget()->GetDSTexture();
	depthTexture->ResourceBarrier(D3D12_RESOURCE_STATE_DEPTH_WRITE);
}

void ComputeManager::Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	const int threadGroupSizeX = 16;
	const int threadGroupSizeY = 16;

	int dispatchX = static_cast<int>(std::ceil(static_cast<float>(WINDOW_WIDTH) / threadGroupSizeX));
	int dispatchY = static_cast<int>(std::ceil(static_cast<float>(WINDOW_HEIGHT) / threadGroupSizeY));

	int32 dispath[3] = {dispatchX,dispatchY,1};

	_depthRender->Dispatch(cmdList, dispath[0], dispath[1], dispath[2]);

	_underWaterEffect->Dispatch(cmdList, dispath[0], dispath[1], dispath[2]);

	_colorGradingSea->Dispatch(cmdList, dispath[0], dispath[1], dispath[2]);

	Core::main->GetRenderTarget()->GetRenderTarget()->ResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET);
	auto& depthTexture = Core::main->GetRenderTarget()->GetDSTexture();
	depthTexture->ResourceBarrier(D3D12_RESOURCE_STATE_DEPTH_WRITE);

}

void ComputeManager::ChangeSceneDispatch()
{
	if (_changeSceneCompute->GetState() != ChangeSceneState::None)
	{
		const int threadGroupSizeX = 16;
		const int threadGroupSizeY = 16;

		int dispatchX = static_cast<int>(std::ceil(static_cast<float>(WINDOW_WIDTH) / threadGroupSizeX));
		int dispatchY = static_cast<int>(std::ceil(static_cast<float>(WINDOW_HEIGHT) / threadGroupSizeY));

		_changeSceneCompute->Dispatch(Core::main->GetCmdList(), dispatchX, dispatchY, 1);
	}
}

void ComputeManager::Resize()
{

	auto& textureBufferPool = Core::main->GetBufferManager()->GetTextureBufferPool();
	textureBufferPool->FreeSRVHandle(_pingTexture->GetSRVCpuHandle());
	textureBufferPool->FreeSRVHandle(_pingTexture->GetUAVCpuHandle());
	textureBufferPool->FreeSRVHandle(_pongTexture->GetSRVCpuHandle());
	textureBufferPool->FreeSRVHandle(_pongTexture->GetUAVCpuHandle());

	_pingTexture->CreateStaticTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::SRV | TextureUsageFlags::UAV
		, false, false);
	_pongTexture->CreateStaticTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::SRV | TextureUsageFlags::UAV
		, false, false);

	_blur->Resize();
	_bloom->Resize();
	_depthRender->Resize();
	_underWaterEffect->Resize();
	_vignetteRender->Resize();
	_ssaoRender->Resize();
	_fieldFogRender->Resize();
	_colorGradingRender->Resize();
	_godrayRender->Resize();
	_fxaaRender->Resize();
	_dofRender->Resize();
	_colorGradingSea->Resize();
	_changeSceneCompute->Resize();
}


Scattering::Scattering()
{
}

Scattering::~Scattering()
{
}

void Scattering::Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture)
{

	_pingTexture = pingTexture;

	_shader = make_shared<Shader>();
	ShaderInfo info;
	info._computeShader = true;
	_shader->Init(L"scattering.hlsl", {}, ShaderArg{ {{"CS_Main","cs"}} }, info);

#ifdef IMGUI_ON
	ImguiManager::main->_scatteringData = &_scatteringData;
	ImguiManager::main->_scattering = &_scattering;
#endif // IMGUI_ON
}

void Scattering::Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	if (!_scattering)
		return;
	
	auto& table = Core::main->GetBufferManager()->GetTable();
	cmdList->SetPipelineState(_shader->_pipelineState.Get());
	_pingTexture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	_tableContainer = table->Alloc(10);


	auto CbufferContainer = Core::main->GetBufferManager()->CreateAndGetBufferPool(BufferType::ScatteringData, sizeof(ScatteringData), 1)->Alloc(1);
	memcpy(CbufferContainer->ptr, (void*)&_scatteringData, sizeof(ScatteringData));
	cmdList->SetComputeRootConstantBufferView(5, CbufferContainer->GPUAdress);

	
	auto& renderTarget = Core::main->GetRenderTarget()->GetRenderTarget();
	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	auto& depthTexture = Core::main->GetRenderTarget()->GetDSTexture();
	depthTexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	table->CopyHandle(_tableContainer.CPUHandle, renderTarget->GetSRVCpuHandle(), 0);
	table->CopyHandle(_tableContainer.CPUHandle, depthTexture->GetSRVCpuHandle(), 1);
	table->CopyHandle(_tableContainer.CPUHandle, _pingTexture->GetUAVCpuHandle(), 5);

	cmdList->SetComputeRootDescriptorTable(10, _tableContainer.GPUHandle);
	cmdList->Dispatch(x, y, z);

	_pingTexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList->CopyResource(renderTarget->GetResource().Get(), _pingTexture->GetResource().Get());
}

void Scattering::DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
}

void Scattering::DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
}

void Scattering::Resize()
{
}


ChangeSceneCompute::ChangeSceneCompute()
{
}

ChangeSceneCompute::~ChangeSceneCompute()
{
}

void ChangeSceneCompute::Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture)
{
	_pingTexture = pingTexture;
	_pongTexture = pongTexture;

	_shader = make_shared<Shader>();
	ShaderInfo info;
	info._computeShader = true;
	_shader->Init(L"changeScene.hlsl", {}, ShaderArg{ {{"CS_Main","cs"}} }, info);

}

void ChangeSceneCompute::Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	auto& table = Core::main->GetBufferManager()->GetTable();
	cmdList->SetPipelineState(_shader->_pipelineState.Get());
	_pingTexture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	_tableContainer = table->Alloc(10);

	auto CbufferContainer = Core::main->GetBufferManager()->CreateAndGetBufferPool(BufferType::ChangeSceneData, sizeof(ChangeSceneData), 1)->Alloc(1);
	memcpy(CbufferContainer->ptr, (void*)&_data, sizeof(_data));
	cmdList->SetComputeRootConstantBufferView(4, CbufferContainer->GPUAdress);

	auto& renderTarget = Core::main->GetRenderTarget()->GetRenderTarget();
	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	table->CopyHandle(_tableContainer.CPUHandle, renderTarget->GetSRVCpuHandle(), 0);
	table->CopyHandle(_tableContainer.CPUHandle, _pingTexture->GetUAVCpuHandle(), 5);

	cmdList->SetComputeRootDescriptorTable(10, _tableContainer.GPUHandle);
	cmdList->Dispatch(x, y, z);

	_pingTexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList->CopyResource(renderTarget->GetResource().Get(), _pingTexture->GetResource().Get());


	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET);

	switch (_state)
	{
	case ChangeSceneState::FadeIn:
	{
		_data.toblack += _speed * Time::main->GetDeltaTime();

		if (_data.toblack >= 1.0f)
		{
			_data.toblack = 1.0f;
			SetState(ChangeSceneState::None);
		}
		break;
	}
	case ChangeSceneState::FadeOutIn:
	{
		_data.toblack -= _speed * Time::main->GetDeltaTime();

		if (_data.toblack <= 0.0f)
		{
			_data.toblack = 0.0f;
			SetState(ChangeSceneState::FadeIn);
		}
		break;
	}
	case ChangeSceneState::FadeOut:
	{
		_data.toblack -= _speed * Time::main->GetDeltaTime();

		if (_data.toblack <= 0.0f)
		{
			_data.toblack = 0.0f;
			SetState(ChangeSceneState::FadeOutEnd);
		}
		break;
	}
	}

}


void ChangeSceneCompute::DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
}

void ChangeSceneCompute::DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
}

void ChangeSceneCompute::Resize()
{
}
