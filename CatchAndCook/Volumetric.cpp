#include "pch.h"
#include "Volumetric.h"
#include "LightManager.h"
#include "Mesh.h"
#include "GeoMetryHelper.h"

unique_ptr<Volumetric> Volumetric::main;

Volumetric::Volumetric()
{
}

Volumetric::~Volumetric()
{
}

void Volumetric::Init()
{
	if (_texture)
	{
		Core::main->GetBufferManager()->GetTextureBufferPool()->FreeSRVHandle(_texture->GetSRVCpuHandle());
		Core::main->GetBufferManager()->GetTextureBufferPool()->FreeSRVHandle(_texture->GetRTVCpuHandle());
	}

	_viewport = D3D12_VIEWPORT{ 0.0f,0.0f,static_cast<float>(WINDOW_WIDTH),static_cast<float>(WINDOW_HEIGHT), 0,1.0f };
	_scissorRect = D3D12_RECT{ 0,0, static_cast<LONG>(WINDOW_WIDTH),static_cast<LONG>(WINDOW_HEIGHT) };

	_texture = make_shared<Texture>();
	_texture->CreateStaticTexture(DXGI_FORMAT_R32G32B32A32_FLOAT, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::RTV | TextureUsageFlags::SRV, false, true);

	if (_shader == nullptr)
	{


		ShaderInfo info;
		info._zTest = false;
		info._stencilTest = false;
		info._zWrite = false;
		info._blendEnable = true;
		info.cullingType = CullingType::NONE;

		info._blendType[0] = BlendType::PremultipliedAlpha;
		info.renderTargetCount = 1;

		info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetPass(RENDER_PASS::Forward);
		shader->Init(L"UnderwaterVolumetric.hlsl", GeoMetryProp, ShaderArg{}, info);

		_shader = shader;
	}

	_mesh = GeoMetryHelper::LoadRectangleMesh();
	
	ImguiManager::main->_volumetricData = &_data;
}


void Volumetric::Render()
{
	Core::main->GetCmdList()->SetPipelineState(_shader->_pipelineState.Get());
	_texture->ResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET);

	auto CbufferContainer =  Core::main->GetBufferManager()->CreateAndGetBufferPool(BufferType::VolumetricData, sizeof(VolumetricData), 1)->Alloc(1);

	_data.lightDir = LightManager::main->GetMainLight()->direction;

	memcpy(CbufferContainer->ptr, (void*)&_data, sizeof(VolumetricData));
	Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(5, CbufferContainer->GPUAdress);


	//Core::main->GetCmdList()->RSSetViewports(1, &_viewport);
	//Core::main->GetCmdList()->RSSetScissorRects(1, &_scissorRect);
	//Core::main->GetCmdList()->OMSetRenderTargets(1, &_texture->GetRTVCpuHandle(), false, &_texture->GetSharedDSVHandle());
	//float clearColor[4] = { 0.0f,0.0f,0.0f,0.0f };
	//Core::main->GetCmdList()->ClearRenderTargetView(_texture->GetRTVCpuHandle(), clearColor, 0, nullptr);
	Core::main->GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	Core::main->GetCmdList()->DrawInstanced(4, _data.numSlices, 0, 0);
	_texture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

}
