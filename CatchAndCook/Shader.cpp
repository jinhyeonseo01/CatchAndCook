#include "pch.h"
#include "Shader.h"
#include "RenderTarget.h"
#include "Vertex.h"

#include "Core.h"
#include "RootSignature.h"
#include "Texture.h"
#include "Vertex.h"


ShaderRegisterInfo ShaderProfileInfo::GetRegisterByName(const std::string& name)
{
    if (_nameToRegisterTable.contains(name))
        return _nameToRegisterTable[name];
    return ShaderRegisterInfo{};
}

ShaderCBufferInfo ShaderProfileInfo::GetCBufferByName(const std::string& name)
{
    if (_nameToCBufferTable.contains(name))
        return _nameToCBufferTable[name];
    return ShaderCBufferInfo{};
}

ShaderInfo::ShaderInfo() = default;
ShaderInfo::~ShaderInfo() = default;

void ShaderInfo::SetRenderTargets(const std::vector<std::shared_ptr<Texture>>& renderTargets)
{
    renderTargetCount = std::min(static_cast<unsigned int>(renderTargets.size()), 8u);

    if (renderTargetCount == 0)
    {
        renderTargetCount = 1;
        RTVForamts[0] = DXGI_FORMAT_UNKNOWN;
        return;
    }

    for (uint32 i = 0; i < renderTargetCount; i++)
        RTVForamts[i] = renderTargets[i]->GetFormat();
}

void ShaderInfo::SetDSTexture(const std::shared_ptr<Texture>& DSTexture)
{
    if (DSTexture == nullptr)
    {
        DSVFormat = DXGI_FORMAT_UNKNOWN;
        _zTest = false;
        _stencilTest = false;
    }

    DSVFormat = DSTexture->GetFormat();
}

void Shader::InitPipeLine(const std::vector<VertexProp>& vertexProp)
{
    auto device = Core::main->GetDevice();

    SelectorInfo vertexSetInfo = SelectorInfo::GetInfo(vertexProp);

    std::vector<D3D12_INPUT_ELEMENT_DESC> _inputElementDesc = vertexSetInfo.GetVertexDescs();
    if(!_instanceProp.empty())
    {
        SelectorInfo instanceSetInfo = SelectorInfo::GetInfo(_instanceProp);
        std::vector<D3D12_INPUT_ELEMENT_DESC> descs = instanceSetInfo.GetInstanceDescs();
        for(auto& instanceDesc : descs)
            _inputElementDesc.push_back(instanceDesc);
    }

    _pipelineDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    if (_info._computeShader)
    {
        _cspipelineDesc.pRootSignature = Core::main->GetRootSignature()->GetComputeRootSignature().Get();
    }
    else
    {
        _pipelineDesc.pRootSignature = Core::main->GetRootSignature()->GetGraphicsRootSignature().Get();
    }
  
    _pipelineDesc.InputLayout = { _inputElementDesc.data(), static_cast<unsigned int>(_inputElementDesc.size()) };

    if (!_info._depthOnly)
    {
        _pipelineDesc.RTVFormats[0] = SWAP_CHAIN_FORMAT;
        _pipelineDesc.NumRenderTargets = _info.renderTargetCount;
        for (int i = 0; i < _pipelineDesc.NumRenderTargets; i++)
            _pipelineDesc.RTVFormats[i] = _info.RTVForamts[i];
    }

    else
    {
        _pipelineDesc.NumRenderTargets = 0;
        _pipelineDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
    }

    _pipelineDesc.PrimitiveTopologyType = _info._primitiveType;
    _pipelineDesc.SampleMask = UINT_MAX;
    _pipelineDesc.SampleDesc.Count = 1;
    _pipelineDesc.SampleDesc.Quality = 0;
    _pipelineDesc.DSVFormat = _info.DSVFormat;

    CD3DX12_RASTERIZER_DESC resterizerDesc{ D3D12_DEFAULT };

    resterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
    resterizerDesc.CullMode = D3D12_CULL_MODE_NONE;

    switch (_info.cullingType)
    {
    case CullingType::BACK:
        resterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
        resterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
        break;
    case CullingType::FRONT:
        resterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
        resterizerDesc.CullMode = D3D12_CULL_MODE_FRONT;
        break;
    case CullingType::NONE:
        resterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
        resterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
        break;
    case CullingType::WIREFRAME:
        resterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
        resterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
        break;
    }

    _pipelineDesc.RasterizerState = resterizerDesc;
    _pipelineDesc.RasterizerState.FrontCounterClockwise = _info._wise == FrontWise::CCW ? TRUE : FALSE;
    _pipelineDesc.RasterizerState.DepthClipEnable = TRUE; // 깊이값 범위 밖 짤라냄.
    _pipelineDesc.RasterizerState.AntialiasedLineEnable = _info.cullingType == CullingType::WIREFRAME ? TRUE : FALSE;
    _pipelineDesc.RasterizerState.DepthBias = _info.depthBias;
    _pipelineDesc.RasterizerState.SlopeScaledDepthBias = _info.slopeScaledDepthBias;
    _pipelineDesc.RasterizerState.DepthBiasClamp = _info.depthBiasClamp;

    CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc{ D3D12_DEFAULT };
    depthStencilDesc.DepthEnable = _info._zTest ? TRUE : FALSE;
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

    switch (_info._zComp)
    {
    case CompOper::Always: depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        break;
    case CompOper::Less: depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
        break;
    case CompOper::LEqual: depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
        break;
    case CompOper::Equal: depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_EQUAL;
        break;
    case CompOper::GEqual: depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
        break;
    case CompOper::Greater: depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
        break;
    case CompOper::NotEqual: depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_NOT_EQUAL;
        break;
    case CompOper::Never: depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_NEVER;
        break;
    default: depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
        break;
    }

    depthStencilDesc.DepthWriteMask = _info._zWrite ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.StencilEnable = _info._stencilTest ? TRUE : FALSE;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;
    depthStencilDesc.FrontFace.StencilFailOp = _info._stencilFailOp;
    depthStencilDesc.FrontFace.StencilDepthFailOp = _info._stencilDepthFailOp;
    depthStencilDesc.FrontFace.StencilPassOp = _info._stencilPassOp;
    depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

    switch (_info._stencilComp)
    {
    case CompOper::Always: depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        break;
    case CompOper::Less: depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_LESS;
        break;
    case CompOper::LEqual: depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
        break;
    case CompOper::Equal: depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
        break;
    case CompOper::GEqual: depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
        break;
    case CompOper::Greater: depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_GREATER;
        break;
    case CompOper::NotEqual: depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NOT_EQUAL;
        break;
    case CompOper::Never: depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
        break;
    default: depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
        break;
    }

    depthStencilDesc.BackFace = depthStencilDesc.FrontFace;
    _pipelineDesc.DepthStencilState = depthStencilDesc;
    _pipelineDesc.BlendState.AlphaToCoverageEnable = FALSE; // 필요에 따라 TRUE로 설정
    _pipelineDesc.BlendState.IndependentBlendEnable = FALSE; // 여러 렌더 타겟에 대해 다른 블렌딩 설정을 원할 경우 TRUE로 설정

    for (int i = 0; i < 8; i++)
    {
        D3D12_RENDER_TARGET_BLEND_DESC& blendDesc = _pipelineDesc.BlendState.RenderTarget[i];
        blendDesc.BlendEnable = _info._blendEnable;
        blendDesc.LogicOpEnable = FALSE;
        blendDesc.SrcBlend = D3D12_BLEND_ONE;
        blendDesc.DestBlend = D3D12_BLEND_ZERO;
        blendDesc.LogicOpEnable = FALSE;
        blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
        blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
        blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;

        if (blendDesc.BlendEnable)
        {
            switch (_info._blendType[i])
            {
            case BlendType::AlphaBlend:
                blendDesc.BlendEnable = TRUE;
                blendDesc.LogicOpEnable = FALSE;
                blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
                blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
                blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
                blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
                blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
                blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
                break;
            case BlendType::PremultipliedAlpha:
     /*           Out.rgb = = Src.rgb + Dst.rgb * (1 - Src.a)*/
                blendDesc.BlendEnable = TRUE;
                blendDesc.SrcBlend = D3D12_BLEND_ONE;
                blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
                blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
                blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
                blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
				blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
				break;
            case BlendType::Add:
                blendDesc.BlendEnable = TRUE;
                blendDesc.LogicOpEnable = FALSE;
                blendDesc.SrcBlend = D3D12_BLEND_ONE;
                blendDesc.DestBlend = D3D12_BLEND_ONE;
                blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
                break;
            case BlendType::Multiple:
                blendDesc.BlendEnable = TRUE;
                blendDesc.LogicOpEnable = FALSE;
                blendDesc.SrcBlend = D3D12_BLEND_DEST_COLOR;
                blendDesc.DestBlend = D3D12_BLEND_ZERO;
                blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
                break;
            case BlendType::ColorDodge:
                blendDesc.BlendEnable = TRUE;
                blendDesc.LogicOpEnable = FALSE;
                blendDesc.SrcBlend = D3D12_BLEND_ONE;
                blendDesc.DestBlend = D3D12_BLEND_INV_SRC_COLOR;
                blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
                break;
            case BlendType::Subtract:
                blendDesc.BlendEnable = TRUE;
                blendDesc.LogicOpEnable = FALSE;
                blendDesc.SrcBlend = D3D12_BLEND_ONE;
                blendDesc.DestBlend = D3D12_BLEND_ONE;
                blendDesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
                break;
            case BlendType::Screen:
                blendDesc.BlendEnable = TRUE;
                blendDesc.LogicOpEnable = FALSE;
                blendDesc.SrcBlend = D3D12_BLEND_ONE;
                blendDesc.DestBlend = D3D12_BLEND_INV_DEST_COLOR;
                blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
                break;
            }
        }
        else
        {
            blendDesc.BlendEnable = FALSE;
            blendDesc.LogicOpEnable = FALSE;
            blendDesc.LogicOp = D3D12_LOGIC_OP_COPY;
            blendDesc.SrcBlend = D3D12_BLEND_ONE;
            blendDesc.DestBlend = D3D12_BLEND_ZERO;
        }
    }


    for (auto& shaderCode : _shaderCodeTable)
    {
        if (shaderCode.first == "vs")
            _pipelineDesc.VS = _shaderCodeTable[shaderCode.first]->_shaderByteCode;
        if (shaderCode.first == "ps")
            _pipelineDesc.PS = _shaderCodeTable[shaderCode.first]->_shaderByteCode;
        if (shaderCode.first == "gs")
            _pipelineDesc.GS = _shaderCodeTable[shaderCode.first]->_shaderByteCode;
        if (shaderCode.first == "hs")
            _pipelineDesc.HS = _shaderCodeTable[shaderCode.first]->_shaderByteCode;
        if (shaderCode.first == "ds")
            _pipelineDesc.DS = _shaderCodeTable[shaderCode.first]->_shaderByteCode;
        if (shaderCode.first == "cs")
            _cspipelineDesc.CS = _shaderCodeTable[shaderCode.first]->_shaderByteCode;
    }

    if (_info._computeShader)
    {
        ThrowIfFailed(device->CreateComputePipelineState(&_cspipelineDesc, IID_PPV_ARGS(&_pipelineState)));
    }
    else 
    {
        ThrowIfFailed(device->CreateGraphicsPipelineState(&_pipelineDesc, IID_PPV_ARGS(&_pipelineState)));
    }
   
}

void Shader::SetMacro(const std::vector<D3D_SHADER_MACRO>& macros)
{
    _macros = macros;
    if(_macros.size() == 0 || _macros[_macros.size() - 1].Name != nullptr)
        _macros.push_back(D3D_SHADER_MACRO(nullptr, nullptr));
}

void Shader::SetInjector(const std::vector<BufferType>& injectors)
{
    _cbufferInjectorTypes = injectors;
}

void Shader::SetInstanceProp(const std::vector<VertexProp>& props)
{
    _instanceProp = props;
}

void Shader::SetInfo(const ShaderInfo& info)
{
    this->_info = info;
}

void Shader::Profile()
{
    for (auto& codePair : _shaderCodeTable)
    {
        auto& code = codePair.second;
        ComPtr<ID3D12ShaderReflection> pReflector = nullptr;
        HRESULT hr = D3DReflect(code->_shaderByteCode.pShaderBytecode, code->_shaderByteCode.BytecodeLength,
            IID_ID3D12ShaderReflection, reinterpret_cast<void**>(pReflector.GetAddressOf()));
        ThrowIfFailed(hr);
        
        {
            D3D12_SHADER_DESC shaderDesc;
            pReflector->GetDesc(&shaderDesc);

            ShaderCBufferInfo cbufferInfo;
            ShaderCBufferPropertyInfo cbufferPropertyInfo;

            for (UINT i = 0; i < shaderDesc.ConstantBuffers; i++)
            {
                ID3D12ShaderReflectionConstantBuffer* pConstantBuffer = pReflector->GetConstantBufferByIndex(i);
                D3D12_SHADER_BUFFER_DESC bufferDesc;
                pConstantBuffer->GetDesc(&bufferDesc);
                if (bufferDesc.Type != D3D_CT_CBUFFER)
                    continue;

                cbufferInfo.name = std::string(bufferDesc.Name);
                cbufferInfo.index = bufferDesc.Variables;
                cbufferInfo.bufferByteSize = static_cast<int>(bufferDesc.Size);

                for (UINT j = 0; j < bufferDesc.Variables; j++)
                {
                    ID3D12ShaderReflectionVariable* pVar = pConstantBuffer->GetVariableByIndex(j);

                    D3D12_SHADER_VARIABLE_DESC varDesc;
                    pVar->GetDesc(&varDesc);
                    ID3D12ShaderReflectionType* pType = pVar->GetType();
                    D3D12_SHADER_TYPE_DESC typeDesc;
                    pType->GetDesc(&typeDesc);

                    switch (typeDesc.Class)
                    {
                    case D3D_SVC_SCALAR:
                        cbufferPropertyInfo._className = "scalar";
                        break;
                    case D3D_SVC_VECTOR:
                        cbufferPropertyInfo._className = "vector";
                        break;
                    case D3D_SVC_MATRIX_ROWS:
                        cbufferPropertyInfo._className = "matrix";
                        break;
                    case D3D_SVC_MATRIX_COLUMNS:
                        cbufferPropertyInfo._className = "matrix";
                        break;
                    case D3D_SVC_OBJECT:
                        cbufferPropertyInfo._className = "object";
                        break;
                    case D3D_SVC_STRUCT:
                        cbufferPropertyInfo._className = "struct";
                        break;
                    }
                    cbufferPropertyInfo.elementType = "none";
                    switch (typeDesc.Type)
                    {
                    case D3D_SVT_FLOAT:
                        cbufferPropertyInfo.elementType = "float";
                        break;
                    case D3D_SVT_INT:
                        cbufferPropertyInfo.elementType = "int";
                        break;
                    case D3D_SVT_UINT:
                        cbufferPropertyInfo.elementType = "uint";
                        break;
                    }
                    cbufferPropertyInfo.index = static_cast<int>(i);
                    cbufferPropertyInfo._name = std::string(varDesc.Name);
                    cbufferPropertyInfo.byteOffset = varDesc.StartOffset;
                    cbufferPropertyInfo.byteSize = varDesc.Size;
                    cbufferPropertyInfo.elementCount = static_cast<int>(typeDesc.Elements);
                    cbufferPropertyInfo.rowCount = static_cast<int>(typeDesc.Rows);
                    cbufferPropertyInfo.colCount = static_cast<int>(typeDesc.Columns);

                    cbufferInfo.propertys.push_back(cbufferPropertyInfo);
                }

                //cbufferInfo.propertys.push_back();

                if (!_profileInfo._nameToCBufferTable.contains(cbufferInfo.name))
                {
                    _profileInfo._nameToCBufferTable.emplace(cbufferInfo.name, cbufferInfo);
                    _profileInfo.cbuffers.push_back(cbufferInfo);
                    //CBufferPool::CBufferRegister(cbufferInfo, 1);
                }
            }


            ShaderStructPropertyInfo structPropertyInfo;
            if (!_profileInfo._typeToStructTable.contains(codePair.first))
                _profileInfo._typeToStructTable.emplace(codePair.first, ShaderStructInfo{});
            _profileInfo._typeToStructTable[codePair.first].count = shaderDesc.InputParameters;

            for (UINT i = 0; i < shaderDesc.InputParameters; i++)
            {
                D3D12_SIGNATURE_PARAMETER_DESC desc;
                pReflector->GetInputParameterDesc(i, &desc);

                structPropertyInfo.elementType = "float";
                switch (desc.ComponentType)
                {
                case D3D_REGISTER_COMPONENT_UINT32:
                    structPropertyInfo.elementType = "uint";
                    break;
                case D3D_REGISTER_COMPONENT_SINT32:
                    structPropertyInfo.elementType = "int";
                    break;
                case D3D_REGISTER_COMPONENT_FLOAT32:
                    structPropertyInfo.elementType = "float";
                    break;
                    break;
                default:
                    std::cout << "Unknown" << "\n";
                    break;
                }
                structPropertyInfo.elementTypeRange = 0;
                for (int j = 0; j < 8; j++)
                    structPropertyInfo.elementTypeRange += (desc.Mask & (1 << j)) ? 1 : 0;
                structPropertyInfo.semantic = std::string(desc.SemanticName);
                structPropertyInfo.semanticIndex = static_cast<int>(desc.SemanticIndex);
                structPropertyInfo.registerIndex = static_cast<int>(desc.Register);

                _profileInfo._typeToStructTable[codePair.first].propertys.push_back(structPropertyInfo);
            }


            ShaderRegisterInfo registerInfo;

            D3D12_SHADER_INPUT_BIND_DESC bindDesc;
            for (UINT i = 0; i < shaderDesc.BoundResources; i++)
            {
                pReflector->GetResourceBindingDesc(i, &bindDesc);
                registerInfo.registerIndex = static_cast<int>(bindDesc.BindPoint);
                registerInfo.registerCount = static_cast<int>(bindDesc.BindCount);
                registerInfo.name = std::string(bindDesc.Name);
                registerInfo.elementType = "float";
                switch (bindDesc.ReturnType)
                {
                case D3D_RETURN_TYPE_UINT:
                    registerInfo.elementType = "uint";
                    break;
                case D3D_RETURN_TYPE_SINT:
                    registerInfo.elementType = "int";
                    break;
                case D3D_RETURN_TYPE_FLOAT:
                    registerInfo.elementType = "float";
                    break;
                case D3D_RETURN_TYPE_UNORM:
                    registerInfo.elementType = "float";
                    break;
                case D3D_RETURN_TYPE_SNORM:
                    registerInfo.elementType = "float";
                    break;
                case D3D_RETURN_TYPE_DOUBLE:
                    registerInfo.elementType = "double";
                    break;
                case D3D_RETURN_TYPE_MIXED:
                    registerInfo.elementType = "struct";
                    break;
                case 0:
                    // SRV가 아닐경우
                    break;
                default:
                    std::cout << "Unknown Return Type: " << bindDesc.ReturnType << "\n";
                    break;
                }
                registerInfo.space = static_cast<int>(bindDesc.Space);
                registerInfo.numSample = static_cast<int>(bindDesc.NumSamples);
                registerInfo.registerType = 'b';
                switch (bindDesc.Type)
                {
                case D3D_SIT_CBUFFER:
                    registerInfo.registerType = 'b';
                    break;
                case D3D_SIT_BYTEADDRESS:
                case D3D_SIT_STRUCTURED:
                case D3D_SIT_TEXTURE:
                case D3D_SIT_TBUFFER:
                    registerInfo.registerType = 't';
                    break;
                case D3D_SIT_UAV_RWSTRUCTURED:
                case D3D_SIT_UAV_RWBYTEADDRESS:
                case D3D_SIT_UAV_APPEND_STRUCTURED:
                case D3D_SIT_UAV_CONSUME_STRUCTURED:
                case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
                case D3D_SIT_UAV_RWTYPED:
                    registerInfo.registerType = 'u';
                    break;
                case D3D_SIT_SAMPLER:
                    registerInfo.registerType = 's';
                    break;
                default:
                    std::cout << "Unknown Type : " << bindDesc.Type << "\n";
                    break;
                }
                registerInfo.bufferType = "none";
                switch (bindDesc.Dimension)
                {
                case D3D_SRV_DIMENSION_BUFFER:
                    registerInfo.bufferType = "buffer";
                    break;
                case D3D_SRV_DIMENSION_TEXTURE1D:
                    registerInfo.bufferType = "texture1D";
                    break;
                case D3D_SRV_DIMENSION_TEXTURE1DARRAY:
                    registerInfo.bufferType = "texture1DArray";
                    break;
                case D3D_SRV_DIMENSION_TEXTURE2D:
                    registerInfo.bufferType = "texture2D";
                    break;
                case D3D_SRV_DIMENSION_TEXTURE2DARRAY:
                    registerInfo.bufferType = "texture2DArray";
                    break;
                case D3D_SRV_DIMENSION_TEXTURE2DMS:
                    registerInfo.bufferType = "texture2DMS";
                    break;
                case D3D_SRV_DIMENSION_TEXTURE2DMSARRAY:
                    registerInfo.bufferType = "texture2DMSArray";
                    break;
                case D3D_SRV_DIMENSION_TEXTURE3D:
                    registerInfo.bufferType = "texture3D";
                    break;
                case D3D_SRV_DIMENSION_TEXTURECUBE:
                    registerInfo.bufferType = "textureCube";
                    break;
                case D3D_SRV_DIMENSION_TEXTURECUBEARRAY:
                    registerInfo.bufferType = "textureCubeArray";
                    break;
                }
                if(registerInfo.registerType == 't')
                {
                    _profileInfo.tRegisterTable.push_back(registerInfo.registerIndex);
                    _profileInfo.maxTRegister = std::max(_profileInfo.maxTRegister,registerInfo.registerIndex);
                }

                registerInfo.registerTypeString = registerInfo.registerType +
                    std::to_string(registerInfo.registerIndex);
                str::trim(registerInfo.registerTypeString);

                if (!_profileInfo._nameToRegisterTable.contains(registerInfo.name))
                {
                    _profileInfo._nameToRegisterTable.emplace(registerInfo.name, registerInfo);
                    _profileInfo.registers.push_back(registerInfo);
                }
                if(registerInfo.registerType == 't' && registerInfo.bufferType == "buffer")
                {
                    _profileInfo.structuredBuffers.push_back(registerInfo);
                }
            }
        }
    }
}

int Shader::GetRegisterIndex(const std::string& name)
{
    auto iter = _profileInfo._nameToRegisterTable.find(name);

    if (iter != _profileInfo._nameToRegisterTable.end())
        return iter->second.registerIndex;

    return -1;
}
std::vector<ShaderRegisterInfo>& Shader::GetTRegisterStructured()
{
    return _profileInfo.structuredBuffers;
}

std::vector<int>& Shader::GetTRegisterIndexs()
{
    return _profileInfo.tRegisterTable;
}

std::shared_ptr<ShaderCode> Shader::LoadBlob(std::wstring path, std::string endPointName, std::string shaderType)
{
    std::wstring originPath = L"../Resources/Shaders/";

    std::wstring shaderPath = originPath+ path;

    std::wstring ext = std::filesystem::path(shaderPath).extension();

    auto shaderCode = std::make_shared<ShaderCode>();
    shaderCode->shaderType = shaderType;
    shaderCode->endPointName = endPointName;

    if (ext == L".hlsl" || ext == L".HLSL")
    {
        uint32_t compileFlag = 0;
#ifdef _DEBUG
        compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        //compileFlag = D3DCOMPILE_OPTIMIZATION_LEVEL2;
        compileFlag = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

        std::string functionName = shaderCode->endPointName;
        std::string version = shaderCode->shaderType;
        if (str::split(version, "_").size() <= 1)
            version += "_5_0";

        ComPtr<ID3DBlob> _shaderBlob;

        HRESULT resurt = ::D3DCompileFromFile(shaderPath.c_str(),_macros.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE
            , functionName.c_str(), version.c_str(), compileFlag, 0,
            _shaderBlob.GetAddressOf(), shaderCode->_errorBlob.GetAddressOf());

        if (resurt < 0)
            std::cout << "Shader Compile Failed\n" << std::string{
                (char*)shaderCode->_errorBlob->GetBufferPointer(), shaderCode->_errorBlob->GetBufferSize() } << "\n";

    	ThrowIfFailed(resurt);
        
        shaderCode->_shaderByteCode.pShaderBytecode = _shaderBlob->GetBufferPointer();
        shaderCode->_shaderByteCode.BytecodeLength = _shaderBlob->GetBufferSize();
        shaderCode->_blob = _shaderBlob;
        
    }

    if (ext == L".cso" || ext == L".CSO")
    {
        std::ifstream shaderFile(shaderPath, std::ios::binary | std::ios::ate);
        if (!shaderFile.is_open())
            std::cout << "Shader Compile Failed\n" << "미리 컴파일 된 쉐이더 경로 실패\n";
        std::streamsize size = shaderFile.tellg();
        shaderFile.seekg(0, std::ios::beg);
        shaderCode->_shaderPrecompiledBuffer.resize(size);
        if (!shaderFile.read(shaderCode->_shaderPrecompiledBuffer.data(), size))
            std::cout << "Shader Compile Failed\n" << "미리 컴파일 된 쉐이더 읽기 실패\n";

        shaderCode->_shaderByteCode.pShaderBytecode = shaderCode->_shaderPrecompiledBuffer.data();
        shaderCode->_shaderByteCode.BytecodeLength = shaderCode->_shaderPrecompiledBuffer.size();
    }

    return shaderCode;
}

void Shader::Init(const std::wstring& path, const std::vector<VertexProp>& prop, const ShaderArg& shaderParams, const ShaderInfo& info)
{
	_name = to_string(path);
    for (auto& pair : shaderParams.shaderParams)
    {
        auto shaderCodeData = LoadBlob(path, pair.first, pair.second);
        _shaderCodeTable.emplace(shaderCodeData->shaderType, shaderCodeData);
    }

    Profile();
    SetInfo(info);
    InitPipeLine(prop);
}
