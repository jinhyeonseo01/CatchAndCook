#include "pch.h"
#include "Core.h"
#include "RenderTarget.h"
#include "RootSignature.h"
#include "Texture.h"
#include "BufferPool.h"
#include "Mesh.h"
#include "Shader.h"
#include "GameObject.h"
#include "BufferManager.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "Profiler.h"
#include "SceneManager.h"
#include "ComputeManager.h"
#include "ImguiManager.h"
#include "LightManager.h"

unique_ptr<Core> Core::main=nullptr;

Core::Core()
{

}

Core::~Core()
{
    Fence();
}

void Core::Init(HWND hwnd)
{

    _hwnd = hwnd;

    InitDirectX12();

    {
#ifdef  IMGUI_ON
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.NumDescriptors = 1;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; 
        _device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_imguiHeap));
#endif
    }

    _bufferManager = make_shared<BufferManager>();
    _bufferManager->Init();

    _renderTarget = make_shared<RenderTarget>();
    _renderTarget->Init(_swapChain);

	_gBuffer = make_shared<GBuffer>();
	_gBuffer->Init();

    _shadowBuffer = make_shared<ShadowBuffer>();
    _shadowBuffer->Init();

	_dsReadTexture = make_shared<Texture>();
	_dsReadTexture->CreateStaticTexture(DXGI_FORMAT_R32_TYPELESS, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::SRV, false, true);

    _rtReadTexture = make_shared<Texture>();
    _rtReadTexture->CreateStaticTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::SRV, false, true);

    _rootSignature = make_shared<RootSignature>();
    _rootSignature->Init();


    Initalize = true;
}

void Core::RenderBegin()
{
    ComPtr<ID3D12GraphicsCommandList> cmdList = _cmdList[CURRENT_CONTEXT_INDEX];
	ComPtr<ID3D12CommandAllocator> cmdMemory = _cmdMemory[CURRENT_CONTEXT_INDEX];

    ThrowIfFailed(cmdMemory->Reset());
    ThrowIfFailed(cmdList->Reset(cmdMemory.Get(), nullptr));
    cmdList->SetGraphicsRootSignature(_rootSignature->GetGraphicsRootSignature().Get());
    cmdList->SetComputeRootSignature(_rootSignature->GetComputeRootSignature().Get());
    cmdList->SetDescriptorHeaps(1, _bufferManager->GetTable()->GetDescriptorHeap().GetAddressOf());
}

void Core::RenderEnd()
{
    ComPtr<ID3D12GraphicsCommandList> cmdList = _cmdList[CURRENT_CONTEXT_INDEX];
    {
#ifdef  IMGUI_ON
		Core::main->GetRenderTarget()->GetRenderTarget()->ResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET);
        cmdList->SetDescriptorHeaps(1, _imguiHeap.GetAddressOf());
        ImguiManager::main->Render();
#endif //  IMGUI_ON
    }

    _renderTarget->RenderEnd();

    //IMGUIRENDER
    cmdList->Close();

    ID3D12CommandList* ppCommandLists[] = { cmdList.Get() };
    _cmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
 
    Fence();

    uint32 SyncInterval = 0;
    uint32 uiSyncInterval = SyncInterval;
    uint32 uiPresentFlags = 0;

    if (!uiSyncInterval)
    {
        uiPresentFlags = DXGI_PRESENT_ALLOW_TEARING;
    }

     ThrowIfFailed(_swapChain->Present(uiSyncInterval, uiPresentFlags));

    _renderTarget->ChangeIndex();

	CURRENT_CONTEXT_INDEX = (CURRENT_CONTEXT_INDEX + 1) % MAX_FRAME_COUNT;

    _bufferManager->Reset();
    g_debug_deferred_count = 0;
	g_debug_forward_count = 0;
	g_debug_draw_call = 0;
    g_debug_deferred_culling_count = 0;
    g_debug_forward_culling_count = 0;
    g_debug_shadow_draw_call = 0;

}


void Core::ExcuteCommandQueue()
{
    _resCmdList->Close();

    ID3D12CommandList* cmdListArr[] = { _resCmdList.Get() };
    _cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

    FenceCurrentFrame();

    _resCmdMemory->Reset();
    _resCmdList->Reset(_resCmdMemory.Get(), nullptr);
}

void Core::ResizeWindowSize()
{
    FenceCurrentFrame();
    _renderTarget->ResizeWindowSize(_swapChain,_swapChainFlags);
    _gBuffer->Init();
    ResizeTexture(_dsReadTexture, WINDOW_WIDTH, WINDOW_HEIGHT);
    ResizeTexture(_rtReadTexture, WINDOW_WIDTH, WINDOW_HEIGHT);
	ComputeManager::main->Resize();

}


void Core::Fence()
{
    _fenceValue++;
    _cmdQueue->Signal(_fence.Get(), _fenceValue);
	_lastFenceValue[CURRENT_CONTEXT_INDEX] = _fenceValue;

    uint64 nextFenceValue = _lastFenceValue[(CURRENT_CONTEXT_INDEX + 1) % MAX_FRAME_COUNT];

    if (_fence->GetCompletedValue() < nextFenceValue)
    { 
        _fence->SetEventOnCompletion(nextFenceValue, _fenceEvent);
        ::WaitForSingleObject(_fenceEvent, INFINITE);
    }
}

void Core::FenceCurrentFrame()
{
    _fenceValue++;
    _cmdQueue->Signal(_fence.Get(), _fenceValue);

    const uint64 ExpectedFenceValue = _fenceValue;

    if (_fence->GetCompletedValue() < ExpectedFenceValue)
    {
        _fence->SetEventOnCompletion(ExpectedFenceValue, _fenceEvent);
        WaitForSingleObject(_fenceEvent, INFINITE);
    }

}

//void Core::FenceAll()
//{
//    for (uint32 i = 0; i < MAX_FRAME_COUNT; ++i)
//    {
//        _fenceValue++;
//        _cmdQueue->Signal(_fence.Get(), _fenceValue);
//        _lastFenceValue[CURRENT_CONTEXT_INDEX] = _fenceValue;
//    }
//
//	for (uint32 i = 0; i < MAX_FRAME_COUNT; ++i)
//	{
//		if (_fence->GetCompletedValue() < _lastFenceValue[i])
//		{
//			_fence->SetEventOnCompletion(_lastFenceValue[i], _fenceEvent);
//			WaitForSingleObject(_fenceEvent, INFINITE);
//		}
//	}
//}


void Core::CopyTexture(const std::shared_ptr<Texture>& destTexture, const std::shared_ptr<Texture>& sourceTexture)
{
    ComPtr<ID3D12GraphicsCommandList> cmdList = Core::main->GetCmdList();

    auto prevDestState = destTexture->_state;
    auto prevSourceState = sourceTexture->_state;

    sourceTexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
    destTexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);

    cmdList->CopyResource(destTexture->GetResource().Get(), sourceTexture->GetResource().Get());

    sourceTexture->ResourceBarrier(prevSourceState);
    destTexture->ResourceBarrier(prevDestState);
}

void Core::ResizeTexture(std::shared_ptr<Texture>& texture, int w, int h)
{

    if (HasFlag(texture->_usageFlags, TextureUsageFlags::RTV)) {
        Core::main->GetBufferManager()->GetTextureBufferPool()->FreeRTVHandle(texture->GetRTVCpuHandle());
    }
    if (HasFlag(texture->_usageFlags, TextureUsageFlags::UAV)) {
        Core::main->GetBufferManager()->GetTextureBufferPool()->FreeSRVHandle(texture->GetUAVCpuHandle());
    }
    if (HasFlag(texture->_usageFlags, TextureUsageFlags::SRV)) {
        Core::main->GetBufferManager()->GetTextureBufferPool()->FreeSRVHandle(texture->GetSRVCpuHandle());
    }
    if (HasFlag(texture->_usageFlags, TextureUsageFlags::DSV)) {
        Core::main->GetBufferManager()->GetTextureBufferPool()->FreeDSVHandle(texture->GetDSVCpuHandle());
    }

    texture->CreateStaticTexture(texture->GetFormat(), D3D12_RESOURCE_STATE_COMMON, w, h, texture->_usageFlags, texture->_jump, texture->_detphShared);
}

void Core::InitDirectX12()
{
    CreateDevice(false, false);
    CreateCmdQueue();
    CreateSwapChain();
}

void Core::CreateDevice(bool EnableDebugLayer, bool EnableGBV)
{
    ComPtr<ID3D12Debug> pDebugController = nullptr;
    ComPtr<IDXGIAdapter1> bestAdapter = nullptr;
    DXGI_ADAPTER_DESC1 bestDesc;
    ZeroMemory(&bestDesc, sizeof(bestDesc));
    size_t bestMemory = 0; // VRAM �뷮

    DWORD dwCreateFlags = 0;
    DWORD dwCreateFactoryFlags = 0;


    // ����� ���̾� Ȱ��ȭ
    if (EnableDebugLayer)
    {
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pDebugController))))
        {
            pDebugController->EnableDebugLayer();
        }


        dwCreateFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

        if (EnableGBV)
        {
            ID3D12Debug5* pDebugController5 = nullptr;
            if (S_OK == pDebugController->QueryInterface(IID_PPV_ARGS(&pDebugController5)))
            {
                pDebugController5->SetEnableGPUBasedValidation(FALSE);
                pDebugController5->SetEnableAutoName(TRUE);
                pDebugController5->Release();
            }
        }
    }


    CreateDXGIFactory2(dwCreateFactoryFlags, IID_PPV_ARGS(&_factory));

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };

    DWORD FeatureLevelNum = _countof(featureLevels);

    // ����� ����
    for (UINT adapterIndex = 0; ; ++adapterIndex)
    {
        ComPtr<IDXGIAdapter1> pAdapter;
        HRESULT hr = _factory->EnumAdapters1(adapterIndex, &pAdapter);

        // ����Ͱ� ������ ����
        if (hr == DXGI_ERROR_NOT_FOUND)
            break;

        DXGI_ADAPTER_DESC1 AdapterDesc;
        pAdapter->GetDesc1(&AdapterDesc);


        size_t memory = AdapterDesc.DedicatedVideoMemory / (1024 * 1024);

 
        if (memory > bestMemory)
        {
            bestMemory = memory;
            bestAdapter = pAdapter;
            bestDesc = AdapterDesc;
        }
    }


    for (DWORD featerLevelIndex = 0; featerLevelIndex < FeatureLevelNum; featerLevelIndex++)
    {
        if (bestAdapter)
        {
            HRESULT hr = D3D12CreateDevice(bestAdapter.Get(), featureLevels[featerLevelIndex], IID_PPV_ARGS(&_device));

            if (SUCCEEDED(hr))
            {
                break;
            }
        }
    }

    _adapterDesc = bestDesc;

    if (pDebugController)
    {
        SetDebugLayerInfo();
    }

}

void Core::CreateSwapChain()
{
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width = WINDOW_WIDTH;
    swapChainDesc.Height = WINDOW_HEIGHT;
    swapChainDesc.Format = SWAP_CHAIN_FORMAT;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = SWAP_CHAIN_FRAME_COUNT;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Scaling = DXGI_SCALING_NONE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    _swapChainFlags = swapChainDesc.Flags;

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
    fsSwapChainDesc.Windowed = TRUE;

    IDXGISwapChain1* pSwapChain1 = nullptr;

    if (FAILED(_factory->CreateSwapChainForHwnd(_cmdQueue.Get(), _hwnd, &swapChainDesc, &fsSwapChainDesc, nullptr, &pSwapChain1)))
    {
        __debugbreak();
    }

    pSwapChain1->QueryInterface(IID_PPV_ARGS(&_swapChain));
    pSwapChain1->Release();
    pSwapChain1 = nullptr;


}

void Core::CreateCmdQueue()
{
    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    ThrowIfFailed(_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&_cmdQueue)));

    for (int i = 0; i < MAX_FRAME_COUNT; ++i)
    {
        ThrowIfFailed(_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdMemory[i])));
        ThrowIfFailed(_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdMemory[i].Get(), nullptr, IID_PPV_ARGS(&_cmdList[i])));
        ThrowIfFailed(_cmdList[i]->Close());
    }

   


    ThrowIfFailed(_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_resCmdMemory)));
    ThrowIfFailed(_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _resCmdMemory.Get(), nullptr, IID_PPV_ARGS(&_resCmdList)));

    ThrowIfFailed(_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence)));
    _fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void Core::SetDebugLayerInfo()
{
    ID3D12InfoQueue* pInfoQueue = nullptr;
    _device->QueryInterface(IID_PPV_ARGS(&pInfoQueue));
    if (pInfoQueue)
    {
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);

        D3D12_MESSAGE_ID hide[] =
        {
            D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
            D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
            // Workarounds for debug layer issues on hybrid-graphics systems
            D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE,
            D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
        };

        D3D12_INFO_QUEUE_FILTER filter = {};
        filter.DenyList.NumIDs = (UINT)_countof(hide);
        filter.DenyList.pIDList = hide;
        pInfoQueue->AddStorageFilterEntries(&filter);

        pInfoQueue->Release();
        pInfoQueue = nullptr;
    }
}




