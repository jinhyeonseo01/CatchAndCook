﻿#include "pch.h"
#include "RenderTarget.h"
#include "Texture.h"
#include "Core.h"

RenderTarget::RenderTarget()
{
}
RenderTarget::~RenderTarget()
{
}
void RenderTarget::Init(ComPtr<IDXGISwapChain3>& swapchain)
{
	_swapChain = swapchain;

	for (int32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; i++)
	{
		_RenderTargets[i] = make_shared<Texture>();
	}

	_DSTexture = make_shared<Texture>();

	_viewport = D3D12_VIEWPORT{ 0.0f,0.0f,static_cast<float>(WINDOW_WIDTH),static_cast<float>(WINDOW_HEIGHT), 0,1.0f };
	_scissorRect = D3D12_RECT{ 0,0, static_cast<LONG>(WINDOW_WIDTH),static_cast<LONG>(WINDOW_HEIGHT) };

	for (int32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; i++)
		swapchain->GetBuffer(i, IID_PPV_ARGS(&_RenderTargets[i]->GetResource()));

	for (int32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; i++)
	{
		_RenderTargets[i]->CreateStaticTexture(SWAP_CHAIN_FORMAT, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::RTV, true, true);
	}

	_RenderTargetIndex = swapchain->GetCurrentBackBufferIndex();
	_DSTexture->CreateStaticTexture(DEPTH_STENCIL_FORMAT, D3D12_RESOURCE_STATE_DEPTH_WRITE, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::DSV, false, true);
}

void RenderTarget::ResizeWindowSize(ComPtr<IDXGISwapChain3> swapchain, uint32 swapChainFlags)
{
	Core::main->GetBufferManager()->GetTextureBufferPool()->FreeDSVHandle(_DSTexture->GetSharedDSVHandle());

	for (uint32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; i++)
	{
		Core::main->GetBufferManager()->GetTextureBufferPool()->FreeRTVHandle(_RenderTargets[i]->GetRTVCpuHandle());
		_RenderTargets[i]->GetResource().Reset();
	}

	ThrowIfFailed(swapchain->ResizeBuffers(SWAP_CHAIN_FRAME_COUNT, WINDOW_WIDTH, WINDOW_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM, swapChainFlags));

	_RenderTargetIndex = swapchain->GetCurrentBackBufferIndex();

	for (int32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; i++)
		swapchain->GetBuffer(i, IID_PPV_ARGS(&_RenderTargets[i]->GetResource()));

	for (int32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; ++i)
	{
		_RenderTargets[i]->CreateStaticTexture(SWAP_CHAIN_FORMAT, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::RTV, true, true);
	}

	_viewport = D3D12_VIEWPORT{ 0.0f,0.0f,static_cast<float>(WINDOW_WIDTH),static_cast<float>(WINDOW_HEIGHT), 0,1.0f };
	_scissorRect = D3D12_RECT{ 0,0, static_cast<LONG>(WINDOW_WIDTH),static_cast<LONG>(WINDOW_HEIGHT) };
	_DSTexture->CreateStaticTexture(DEPTH_STENCIL_FORMAT, D3D12_RESOURCE_STATE_DEPTH_WRITE, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::DSV, false, true);
}



void RenderTarget::RenderBegin()
{
	ComPtr<ID3D12GraphicsCommandList> cmdList = Core::main->GetCmdList();

	const float BackColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	_RenderTargets[_RenderTargetIndex]->ResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET);

	cmdList->RSSetViewports(1, &_viewport);
	cmdList->RSSetScissorRects(1, &_scissorRect);
	cmdList->ClearRenderTargetView(_RenderTargets[_RenderTargetIndex]->GetRTVCpuHandle(), BackColor, 0, nullptr);
	cmdList->OMSetRenderTargets(1, &_RenderTargets[_RenderTargetIndex]->GetRTVCpuHandle(), FALSE, &_DSTexture->GetSharedDSVHandle());
}

void RenderTarget::RenderEnd()
{
	ComPtr<ID3D12GraphicsCommandList> cmdList = Core::main->GetCmdList();

	_RenderTargets[_RenderTargetIndex]->ResourceBarrier(D3D12_RESOURCE_STATE_PRESENT);
}

void RenderTarget::ClearDepth()
{
	ComPtr<ID3D12GraphicsCommandList> cmdList = Core::main->GetCmdList();
	cmdList->ClearDepthStencilView(_DSTexture->GetSharedDSVHandle(), D3D12_CLEAR_FLAG_DEPTH , 1.0f, 0, 0, nullptr);
}

void RenderTarget::ChangeIndex()
{
	_RenderTargetIndex = _swapChain->GetCurrentBackBufferIndex();
}



/*************************
*                        *
*         GBuffer        *
*                        *
**************************/

GBuffer::GBuffer()
{

}

GBuffer::~GBuffer()
{

}

void GBuffer::Init()
{
	//Resize 할때 이미할당된 BufferPool 이있는경우 놓아줌.
	for(int i = 0; i < _count; ++i)
	{
		if(_textures[i])
		{
			Core::main->GetBufferManager()->GetTextureBufferPool()->FreeRTVHandle(_textures[i]->GetRTVCpuHandle());
			Core::main->GetBufferManager()->GetTextureBufferPool()->FreeSRVHandle(_textures[i]->GetSRVCpuHandle());
			Core::main->GetBufferManager()->GetTextureBufferPool()->FreeSRVHandle(_textures[i]->GetUAVCpuHandle());
		}
	}

	_viewport = D3D12_VIEWPORT{0.0f,0.0f,static_cast<float>(WINDOW_WIDTH),static_cast<float>(WINDOW_HEIGHT),0,1.0f};
	_scissorRect = D3D12_RECT{0,0,static_cast<LONG>(WINDOW_WIDTH),static_cast<LONG>(WINDOW_HEIGHT)};

	for(int i = 0; i < _count; ++i)
	{
		_textures[i] = make_shared<Texture>();
	}

	//position 정보
	_textures[0]->CreateStaticTexture(DXGI_FORMAT_R32G32B32A32_FLOAT,D3D12_RESOURCE_STATE_COMMON,WINDOW_WIDTH,WINDOW_HEIGHT,TextureUsageFlags::RTV| TextureUsageFlags::SRV | TextureUsageFlags::UAV,false,true);
	//normal 정보
	_textures[1]->CreateStaticTexture(DXGI_FORMAT_R32G32B32A32_FLOAT,D3D12_RESOURCE_STATE_COMMON,WINDOW_WIDTH,WINDOW_HEIGHT,TextureUsageFlags::RTV | TextureUsageFlags::SRV | TextureUsageFlags::UAV,false,true);
	//color 정보
	_textures[2]->CreateStaticTexture(DXGI_FORMAT_R8G8B8A8_UNORM,D3D12_RESOURCE_STATE_COMMON,WINDOW_WIDTH,WINDOW_HEIGHT,TextureUsageFlags::RTV | TextureUsageFlags::SRV | TextureUsageFlags::UAV,false,true);
	//depth 정보
	_textures[3]->CreateStaticTexture(DXGI_FORMAT_R8_UNORM,D3D12_RESOURCE_STATE_COMMON,WINDOW_WIDTH,WINDOW_HEIGHT,TextureUsageFlags::RTV | TextureUsageFlags::SRV | TextureUsageFlags::UAV,false,true);
}

void GBuffer::RenderBegin()
{
	auto& list = Core::main->GetCmdList();
	float arrFloat[4] = {0,0,0,0};

	for(uint32 i = 0; i < _count; i++)
	{
		_textures[i]->ResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET);
	}

	for(uint32 i = 0; i < _count; i++)
	{
		list->ClearRenderTargetView(_textures[i]->GetRTVCpuHandle(),arrFloat,0,nullptr);
	}

	list->RSSetViewports(1,&_viewport);
	list->RSSetScissorRects(1,&_scissorRect);
	list->OMSetRenderTargets(_count,&_textures[0]->GetRTVCpuHandle(),TRUE,&_textures[0]->GetSharedDSVHandle());

}

void GBuffer::RenderEnd()
{
	auto& list = Core::main->GetCmdList();
	auto& table = Core::main->GetBufferManager()->GetTable();

	for(uint32 i = 0; i < _count; i++)
	{
		_textures[i]->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
	}

	for(uint32 i = 0; i < _count; i++)
	{
		tableContainer container = table->Alloc(1);
		table->CopyHandle(container.CPUHandle,_textures[i]->GetSRVCpuHandle(),0);
		list->SetGraphicsRootDescriptorTable(GLOBAL_SRV_POSITION_INDEX+i,container.GPUHandle);
	}
}


shared_ptr<Texture>& GBuffer::GetTexture(int32 index)
{
	assert(index < _count);

	return _textures[index];
}