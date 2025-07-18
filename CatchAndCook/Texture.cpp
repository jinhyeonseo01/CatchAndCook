
#include "pch.h"
#include "Texture.h"
#include "BufferPool.h"
#include "Core.h"
#include "BufferManager.h"
D3D12_CPU_DESCRIPTOR_HANDLE Texture::_SharedDSVHandle;

Texture::Texture()
{

}

Texture::~Texture()
{
	

}

void Texture::Init(const wstring& path, TextureType type, bool relativePath, bool createMip)
{

    wstring finalPath = path;
    ScratchImage  _image;

    if (relativePath)
        finalPath = _path + finalPath;

    wstring ext = fs::path(finalPath).extension();

    if (ext == L".dds" || ext == L".DDS")
        LoadFromDDSFile(finalPath.c_str(), DDS_FLAGS_NONE, nullptr, _image);
    else if (ext == L".hdr" || ext == L".HDR")
        LoadFromHDRFile(path.c_str(), nullptr, _image);
    else if (ext == L".tga" || ext == L".TGA")
        LoadFromTGAFile(finalPath.c_str(), nullptr, _image);
    else 
        LoadFromWICFile(finalPath.c_str(), WIC_FLAGS_NONE, nullptr, _image);

    if (!(ext == L".dds" || ext == L".DDS") && createMip)
    {
        ScratchImage mipmapImage;
        //TEX_FILTER_LINEAR 보간
        //TEX_FILTER_CUBIC 3차보간
        //TEX_FILTER_FANT 가우시안 비등방성 보간
        //TEX_FILTER_POINT
        GenerateMipMaps(
            _image.GetImages(), // 소스 이미지 배열
            _image.GetImageCount(), // 소스 이미지 개수
            _image.GetMetadata(), // 소스 이미지 메타데이터
            TEX_FILTER_CUBIC, // 필터 옵션 (기본값 사용) // CUBIC
            0, mipmapImage);
        _image.Release();
        _image = std::move(mipmapImage);
    }
    HRESULT hr = ::CreateTexture(Core::main->GetDevice().Get(), _image.GetMetadata(), &_resource);

    if (FAILED(hr))
        assert(nullptr);

    _isAlpha = !IsTextureFullyOpaque(_image, _image.GetMetadata());
    vector<D3D12_SUBRESOURCE_DATA> subResources;
	
    SetSize(Vector2(_image.GetMetadata().width, _image.GetMetadata().height));

    ThrowIfFailed(PrepareUpload(Core::main->GetDevice().Get(),
        _image.GetImages(),
        _image.GetImageCount(),
        _image.GetMetadata(),
        subResources));


    const UINT64 size = ::GetRequiredIntermediateSize(_resource.Get(), 0, static_cast<uint32>(subResources.size()));

    ComPtr<ID3D12Resource> textureUploadHeap;
    ThrowIfFailed( Core::main->GetDevice()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(size),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(textureUploadHeap.GetAddressOf())));

    ThrowIfFailed( Core::main->GetDevice()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Tex2D(
            _image.GetMetadata().format,
            _image.GetMetadata().width,
            _image.GetMetadata().height,
            _image.GetMetadata().arraySize,
            _image.GetMetadata().mipLevels),
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&_resource)));


    auto& list = Core::main->GetResCmdList();
  
    list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_resource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));

    int64 offset = UpdateSubresources(list.Get(),
        _resource.Get(), textureUploadHeap.Get(), 0, 0, static_cast<unsigned int>(subResources.size()), subResources.data());

    list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE));
   
    Core::main->ExcuteCommandQueue();

    Core::main->GetBufferManager()->GetTextureBufferPool()->AllocSRVDescriptorHandle(&_srvHandle);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

    srvDesc.Format = _image.GetMetadata().format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2D.MipLevels = _image.GetMetadata().mipLevels;

    _state = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;

    switch (type)
    {

    case TextureType::Texture2D:
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        break;
    case TextureType::CubeMap:
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
        break;
    default:
        break;

    }

    SetFormat(_image.GetMetadata().format);

    Core::main->GetDevice()->CreateShaderResourceView(_resource.Get(), &srvDesc, _srvHandle);
}


void Texture::Init(vector<wstring>& paths)
{
    HRESULT hr = S_OK;
    auto device = Core::main->GetDevice(); 
    auto cmdList = Core::main->GetResCmdList(); 

    // 1) 여러 이미지 로드
    // ------------------------------------
    const size_t numSlices = paths.size();
    
    // ScratchImage 여러 개 보관
    vector<ScratchImage> scratchImages(numSlices);

    // 임의로 첫 번째 텍스처의 메타데이터를 참조
    TexMetadata firstMeta = {};

    for (size_t i = 0; i < numSlices; ++i)
    {
        const wstring& path = paths[i];
        DirectX::TexMetadata meta = {};
        DirectX::ScratchImage tmpImage;

        wstring ext = std::filesystem::path(path).extension().wstring();
        if (ext == L".dds" || ext == L".DDS")
            hr = DirectX::LoadFromDDSFile(path.c_str(), DDS_FLAGS_NONE, &meta, tmpImage);
        else if (ext == L".tga" || ext == L".TGA")
            hr = DirectX::LoadFromTGAFile(path.c_str(), &meta, tmpImage);
        else if (ext == L".hdr" || ext == L".HDR")
            hr = DirectX::LoadFromHDRFile(path.c_str(), &meta, tmpImage);
        else
            hr = DirectX::LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, &meta, tmpImage);

        if (FAILED(hr))
        {
            assert(false);
        }

        // 첫 번째 텍스처 메타데이터 저장
        if (i == 0)
        {
            firstMeta = meta;
        }
        else
        {
            // 2) 모든 텍스처가 동일 스펙인지 검사 (해상도, 포맷, mipLevels 등)
            if (meta.width != firstMeta.width ||
                meta.height != firstMeta.height ||
                meta.format != firstMeta.format ||
                meta.mipLevels != firstMeta.mipLevels)
            {
                wcout << L"[Error] All textures must have the same size/format/mipLevels!" << endl;
                assert(false);
            }
        }

        // 결과 복사
        scratchImages[i] = std::move(tmpImage);
    }
    _isAlpha = !IsTextureFullyOpaque(scratchImages, firstMeta);
    SetSize(Vector2(firstMeta.width, firstMeta.height));

    // 이제 firstMeta에 공통 스펙이 들어있다고 가정
    const UINT texWidth = (UINT)firstMeta.width;
    const UINT texHeight = (UINT)firstMeta.height;
    const UINT16 arraySize = (UINT16)numSlices;
    const UINT16 mipLevels = (UINT16)firstMeta.mipLevels;
    DXGI_FORMAT format = firstMeta.format;

    // ------------------------------------
    D3D12_RESOURCE_DESC texDesc = {};
    texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Width = texWidth;
    texDesc.Height = texHeight;
    texDesc.DepthOrArraySize = arraySize;   // 배열 크기
    texDesc.MipLevels = mipLevels;
    texDesc.Format = format;
    texDesc.SampleDesc.Count = 1;
    texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    // CommittedResource 생성 (Default Heap)
    hr = device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &texDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&_resource)
    );
    if (FAILED(hr)) assert(false);


    const UINT subresourceCount = arraySize * mipLevels;
    vector<D3D12_SUBRESOURCE_DATA> subresources;
    subresources.resize(subresourceCount);

    // 각 이미지(mip)마다 pData, RowPitch, SlicePitch 세팅
    UINT idx = 0;
    for (UINT slice = 0; slice < arraySize; ++slice)
    {
        const DirectX::Image* imgs = scratchImages[slice].GetImages(); // mip별 Image 배열

        for (UINT m = 0; m < mipLevels; ++m)
        {
            D3D12_SUBRESOURCE_DATA& sd = subresources[idx++];
            sd.pData = imgs[m].pixels;
            sd.RowPitch = imgs[m].rowPitch;
            sd.SlicePitch = imgs[m].slicePitch;
        }
    }

    // 업로드 버퍼 크기 계산
    const UINT64 uploadBufferSize =
        GetRequiredIntermediateSize(_resource.Get(), 0, subresourceCount);

    ComPtr<ID3D12Resource> uploadHeap;
    hr = device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&uploadHeap)
    );

    if (FAILED(hr)) assert(false);

       UpdateSubresources(cmdList.Get(),
        _resource.Get(),
        uploadHeap.Get(),
        0, 0, subresourceCount,
        subresources.data());

    cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
        _resource.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE
    ));

    // 커맨드 제출 & 동기화
    Core::main->ExcuteCommandQueue();

    // 5) SRV 생성 (Texture2DArray)
    // ------------------------------------
    Core::main->GetBufferManager()->GetTextureBufferPool()->AllocSRVDescriptorHandle(&_srvHandle);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2DArray.MostDetailedMip = 0;
    srvDesc.Texture2DArray.MipLevels = mipLevels;
    srvDesc.Texture2DArray.FirstArraySlice = 0;
    srvDesc.Texture2DArray.ArraySize = arraySize;
    srvDesc.Texture2DArray.PlaneSlice = 0;
    srvDesc.Texture2DArray.ResourceMinLODClamp = 0.f;

    device->CreateShaderResourceView(_resource.Get(), &srvDesc, _srvHandle);


    _state = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
    _format = format;    
}



void Texture::ResourceBarrier(D3D12_RESOURCE_STATES after)
{
    if (_state == after)
    {
        return;
    }

    Core::main->GetCmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_resource.Get(), _state, after));
    _state = after;
}
 

void Texture::CreateStaticTexture(DXGI_FORMAT format, D3D12_RESOURCE_STATES initalState, uint32 width, uint32 height, TextureUsageFlags usageFlags ,bool jump, bool detphShared, vec4 clearValue )
{
    _format = format;
    _usageFlags = usageFlags;
    _jump = jump;
    _detphShared = detphShared;
    _clearValue = clearValue;

    SetSize(Vector2(width, height));

    D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height);
    desc.MipLevels = 1;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;
    
    if (HasFlag(usageFlags, TextureUsageFlags::RTV)) {
        desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    }
    if (HasFlag(usageFlags, TextureUsageFlags::UAV)) {
        desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }

    if (HasFlag(usageFlags, TextureUsageFlags::DSV)) {
        desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    }
     
	if (jump == false) 
    {
        D3D12_CLEAR_VALUE cvalue = {};

        if (HasFlag(usageFlags, TextureUsageFlags::RTV))
        {
            float arrFloat[4] = { clearValue.x, clearValue.y, clearValue.z, clearValue.w };
            cvalue = CD3DX12_CLEAR_VALUE(format, arrFloat);
        }

        if (HasFlag(usageFlags, TextureUsageFlags::DSV))
        {
            DXGI_FORMAT clearFormat = (format == DXGI_FORMAT_R32_TYPELESS) ? DXGI_FORMAT_D32_FLOAT : format;
            cvalue = CD3DX12_CLEAR_VALUE(clearFormat, 1.0f, 0);
            desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        }

        auto hr = Core::main->GetDevice()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &desc,
            initalState,
            HasFlag(usageFlags, TextureUsageFlags::RTV) || HasFlag(usageFlags, TextureUsageFlags::DSV) ? &cvalue : nullptr,
            IID_PPV_ARGS(&_resource));

        if (FAILED(hr))
        {
            throw std::runtime_error("텍스쳐 생성 실패");
        }
    }

    SetFormat(format);

    if (HasFlag(usageFlags, TextureUsageFlags::RTV))
    {
        Core::main->GetBufferManager()->GetTextureBufferPool()->AllocRTVDescriptorHandle(&_rtvHandle);
        Core::main->GetDevice()->CreateRenderTargetView(_resource.Get(), nullptr, _rtvHandle);
    }

    if (HasFlag(usageFlags, TextureUsageFlags::SRV))
    {
        Core::main->GetBufferManager()->GetTextureBufferPool()->AllocSRVDescriptorHandle(&_srvHandle);
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = (format == DXGI_FORMAT_R32_TYPELESS) ? DXGI_FORMAT_R32_FLOAT : format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Texture2D.MipLevels = 1;
        Core::main->GetDevice()->CreateShaderResourceView(_resource.Get(), &srvDesc, _srvHandle);
    }

    if (HasFlag(usageFlags, TextureUsageFlags::UAV))
    {
        Core::main->GetBufferManager()->GetTextureBufferPool()->AllocSRVDescriptorHandle(&_uavHandle);
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.Format = format;
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
        Core::main->GetDevice()->CreateUnorderedAccessView(_resource.Get(), nullptr, &uavDesc, _uavHandle);
    }

    if (HasFlag(usageFlags, TextureUsageFlags::DSV))
    {
        if (detphShared)
        {
            Core::main->GetBufferManager()->GetTextureBufferPool()->AllocDSVDescriptorHandle(&_SharedDSVHandle);
            D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
            dsvDesc.Format = (format == DXGI_FORMAT_R32_TYPELESS) ? DXGI_FORMAT_D32_FLOAT : format;
            dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
            Core::main->GetDevice()->CreateDepthStencilView(_resource.Get(), &dsvDesc, _SharedDSVHandle);
        }
        else
        {
            Core::main->GetBufferManager()->GetTextureBufferPool()->AllocDSVDescriptorHandle(&_dsvHandle);
            D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
            dsvDesc.Format = (format == DXGI_FORMAT_R32_TYPELESS) ? DXGI_FORMAT_D32_FLOAT : format;
            dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
            Core::main->GetDevice()->CreateDepthStencilView(_resource.Get(), &dsvDesc, _dsvHandle);
        }
    }

    _state = initalState;
}

bool Texture::IsTextureFullyOpaque(const std::vector<ScratchImage>& scratchImages, const TexMetadata& meta)
{

    if (meta.GetAlphaMode() == TEX_ALPHA_MODE_OPAQUE)
        return true;

    auto info = DirectX::BitsPerPixel(meta.format);
    if (info != 32) {
        return true;
    }

    // 3) 각 슬라이스별로 MIP0 의 알파만 32픽셀 단위로 샘플링

    const size_t numSlices = scratchImages.size();
    for (size_t slice = 0; slice < numSlices; ++slice)
    {
        const Image* images = scratchImages[slice].GetImages();
        size_t imgCount = scratchImages[slice].GetImageCount();
        if (imgCount == 0)
            continue;

        // MIP 0
        const Image& img0 = images[0];
        const uint8_t* basePtr = img0.pixels;
        size_t rowPitch = img0.rowPitch;
        size_t width = img0.width;
        size_t height = img0.height;

        // 32픽셀 단위로 샘플
        const size_t step = 32;
        for (size_t y = 0; y < height; y += step)
        {
            const uint8_t* row = basePtr + y * rowPitch;
            for (size_t x = 0; x < width; x += step)
            {
                const uint8_t* px = row + x * 4;
                if (px[3] != 0xFF)
                    return false;
            }
        }
    }
    return true;

}

bool Texture::IsTextureFullyOpaque(const ScratchImage& scratchImages, const TexMetadata& meta)
{
    if (meta.GetAlphaMode() == TEX_ALPHA_MODE_OPAQUE)
        return true;
    
    auto info = DirectX::BitsPerPixel(meta.format);
    if (info != 32) {
        return true;
    }

    // 3) 각 슬라이스별로 MIP0 의 알파만 32픽셀 단위로 샘플링

    const Image* images = scratchImages.GetImages();
    size_t imgCount = scratchImages.GetImageCount();
    if (imgCount == 0)
        return true;

    // MIP 0
    const Image& img0 = images[0];
    const uint8_t* basePtr = img0.pixels;
    size_t rowPitch = img0.rowPitch;
    size_t width = img0.width;
    size_t height = img0.height;

    // 32픽셀 단위로 샘플
    const size_t step = 32;
    for (size_t y = 0; y < height; y += step)
    {
        const uint8_t* row = basePtr + y * rowPitch;
        for (size_t x = 0; x < width; x += step)
        {
            const uint8_t* px = row + x * 4;
            if (px[3] != 0xFF)
                return false;
        }
    }

    return true;
}

void Texture::CreateDynamicTexture(DXGI_FORMAT format, uint32 width, uint32 height)
{
    _format = format;

    D3D12_RESOURCE_DESC desc = {};
    desc.MipLevels = 1;
    desc.Format = format;
    desc.Width = width;
    desc.Height = height;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;
    desc.DepthOrArraySize = 1;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

    SetSize(Vector2(width, height));

    auto& device  = Core::main->GetDevice();
    //GPU Memory
    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
        nullptr,
        IID_PPV_ARGS(&_resource)));

    uint64 uploadBufferSize = GetRequiredIntermediateSize(_resource.Get(), 0, 1);

    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&_uploadResource)));

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = (format == DXGI_FORMAT_R32_TYPELESS) ? DXGI_FORMAT_R32_FLOAT : format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    Core::main->GetBufferManager()->GetTextureBufferPool()->AllocSRVDescriptorHandle(&_srvHandle);

    device->CreateShaderResourceView(_resource.Get(), &srvDesc, _srvHandle);
}

void Texture::UpdateDynamicTexture(const BYTE* sysMemory, int dataSize)
{
    D3D12_RESOURCE_DESC Desc = _resource->GetDesc();
   
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT Footprint;
    UINT	Rows = 0;
    UINT64	RowSize = 0;
    UINT64	TotalBytes = 0;

    Core::main->GetDevice()->GetCopyableFootprints(&Desc, 0, 1, 0, &Footprint, &Rows, &RowSize, &TotalBytes);

    BYTE* pMappedPtr = nullptr;
    CD3DX12_RANGE writeRange(0, 0);

    _uploadResource->Map(0, &writeRange, reinterpret_cast<void**>(&pMappedPtr));

    const BYTE* pSrc = sysMemory;
    BYTE* pDest = pMappedPtr;

    int width = Desc.Width;
    int height = Desc.Height;

    for (UINT y = 0; y < height; y++)
    {
        memcpy(pDest, pSrc, width * dataSize);
        pSrc += (width * dataSize);
        pDest += Footprint.Footprint.RowPitch;
    }
    // Unmap
    _uploadResource->Unmap(0, nullptr);

}

void Texture::CopyCpuToGpu()
{
    
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT Footprint = {};
    UINT Rows = 0;
    UINT64 RowSize = 0;
    UINT64 TotalBytes = 0;

    D3D12_RESOURCE_DESC Desc = _resource->GetDesc();
    if (Desc.MipLevels != 1) // 밉맵 레벨은 1이어야 함
        __debugbreak();

    auto& device = Core::main->GetDevice();
    auto& cmdList = Core::main->GetCmdList();

    // 서브리소스 발자국 정보 가져오기
    device->GetCopyableFootprints(&Desc, 0, 1, 0, &Footprint, &Rows, &RowSize, &TotalBytes);

    // 리소스 상태를 복사 대상으로 전환
    cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
        _resource.Get(), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST));

    // 텍스처 복사 위치 설정
    D3D12_TEXTURE_COPY_LOCATION destLocation = {};
    destLocation.PlacedFootprint = Footprint;
    destLocation.pResource = _resource.Get();
    destLocation.SubresourceIndex = 0;
    destLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

    D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
    srcLocation.PlacedFootprint = Footprint;
    srcLocation.pResource = _uploadResource.Get();
    srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

    // 텍스처 복사 수행
    cmdList->CopyTextureRegion(&destLocation, 0, 0, 0, &srcLocation, nullptr);

    // 리소스 상태를 원래 상태로 전환
    cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
        _resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE));
}