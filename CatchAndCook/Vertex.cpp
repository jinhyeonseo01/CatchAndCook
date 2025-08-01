#include "pch.h"
#include "Vertex.h"

SelectorInfo SelectorInfo::GetInfo(const std::vector<VertexProp>& props)
{
    SelectorInfo info;
    VertexPropInfo propInfo;
    info.props = props;

    int offset = 0;
    int byteOffset = 0;
    for (int i = 0; i < props.size(); i++)
    {
        auto& select = props[i];
        int elementTypeSize = sizeof(float);
        int byteSize = 0;
        int index = 0;
        bool isFloat = true;
        switch (select)
        {
        case VertexProp::pos:
            byteSize = sizeof(Vector3);
            break;
        case VertexProp::normal:
            byteSize = sizeof(Vector3);
            break;
        case VertexProp::tangent:
            byteSize = sizeof(Vector3);
            break;
        case VertexProp::bitangent:
            byteSize = sizeof(Vector3);
            break;
        case VertexProp::color:
            byteSize = sizeof(Vector4);
            break;
        case VertexProp::uv:
        case VertexProp::uv0:
            byteSize = sizeof(Vector2);
            index = 0;
            break;
        case VertexProp::uv1:
            byteSize = sizeof(Vector2);
            index = 1;
            break;
        case VertexProp::uv2:
            byteSize = sizeof(Vector2);
            index = 2;
            break;
        case VertexProp::uv3:
            byteSize = sizeof(Vector2);
            index = 3;
            break;
        case VertexProp::uv4:
            byteSize = sizeof(Vector2);
            index = 4;
            break;
        case VertexProp::uv5:
            byteSize = sizeof(Vector2);
            index = 5;
            break;
        case VertexProp::uv6:
            byteSize = sizeof(Vector2);
            index = 6;
            break;
        case VertexProp::uv7:
            byteSize = sizeof(Vector2);
            index = 7;
            break;
        case VertexProp::bone_id0:
            byteSize = sizeof(float);
            index = 0;
            break;
        case VertexProp::bone_id1:
            byteSize = sizeof(float);
            index = 1;
            break;
        case VertexProp::bone_id2:
            byteSize = sizeof(float);
            index = 2;
            break;
        case VertexProp::bone_id3:
            byteSize = sizeof(float);
            index = 3;
            break;
        case VertexProp::bone_ids:
            byteSize = sizeof(Vector4);
            index = 0;
            break;
        case VertexProp::bone_w0:
            byteSize = sizeof(float);
            index = 0;
            break;
        case VertexProp::bone_w1:
            byteSize = sizeof(float);
            index = 1;
            break;
        case VertexProp::bone_w2:
            byteSize = sizeof(float);
            index = 2;
            break;
        case VertexProp::bone_w3:
            byteSize = sizeof(float);
            index = 3;
            break;
        case VertexProp::bone_weights:
            byteSize = sizeof(Vector4);
            index = 0;
            break;
        default:
	    {
            index = 0;
            if(select >= VertexProp::int_0 && select <= VertexProp::int_31)
            {
                byteSize = sizeof(int);
                index = static_cast<int>(select) - static_cast<int>(VertexProp::int_0);
                isFloat = false;
            }
            if(select >= VertexProp::float_0 && select <= VertexProp::float_31)
            {
                byteSize = sizeof(float);
                index = static_cast<int>(select) - static_cast<int>(VertexProp::float_0);
            }
            if(select >= VertexProp::vec2_0 && select <= VertexProp::vec2_31)
            {
                byteSize = sizeof(Vector2);
                index = static_cast<int>(select) - static_cast<int>(VertexProp::vec2_0);
            }
            if(select >= VertexProp::vec3_0 && select <= VertexProp::vec3_31)
            {
                byteSize = sizeof(Vector3);
                index = static_cast<int>(select) - static_cast<int>(VertexProp::vec3_0);
            }
            if(select >= VertexProp::vec4_0 && select <= VertexProp::vec4_31)
            {
                byteSize = sizeof(Vector4);
                index = static_cast<int>(select) - static_cast<int>(VertexProp::vec4_0);
            }
            if(select >= VertexProp::matrix_0 && select <= VertexProp::matrix_31)
            {
                byteSize = sizeof(Matrix);
                index = 0;
            }

	    }
        break;
        }
        if(isFloat)
            elementTypeSize = sizeof(float);
        else
            elementTypeSize = sizeof(int);
        int size = byteSize / elementTypeSize;

        propInfo.offset = offset;
        propInfo.byteOffset = byteOffset;
        propInfo.size = size;
        propInfo.byteSize = byteSize;
        propInfo.prop = select;
        propInfo.index = index;
        propInfo.isFloat = isFloat;

        info.propInfos.push_back(propInfo);

        offset += size;
        byteOffset += byteSize;
    }
    info.totalByteSize = byteOffset;
    info.totalSize = offset;
    info.propCount = static_cast<int>(props.size());
    return info;
}

D3D12_INPUT_ELEMENT_DESC SelectorInfo::GetDesc(const VertexPropInfo& propInfo)
{
    D3D12_INPUT_ELEMENT_DESC elementDesc = {};
    elementDesc.SemanticName = PropNameStrings[static_cast<int>(propInfo.prop)].c_str();
    elementDesc.SemanticIndex = propInfo.index;
    elementDesc.InputSlot = 0;
    elementDesc.AlignedByteOffset = propInfo.byteOffset;
    elementDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
    elementDesc.InstanceDataStepRate = 0;
    elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
    if(propInfo.isFloat){
        if(propInfo.size == 1) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
        else if(propInfo.size == 2) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
        else if(propInfo.size == 3) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
        else elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    }
    else
    {
        if(propInfo.size == 1) elementDesc.Format = DXGI_FORMAT_R32_SINT;
        else if(propInfo.size == 2) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
        else if(propInfo.size == 3) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
        else elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
    }
    return elementDesc;
}

std::vector<D3D12_INPUT_ELEMENT_DESC> SelectorInfo::GetInstanceDescs() const
{
    auto descList = GetRawDescs();
    for(auto& desc : descList)
    {
        desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
        desc.InputSlot = 1;
        desc.InstanceDataStepRate = 1;
    }
    return descList;
}
std::vector<D3D12_INPUT_ELEMENT_DESC> SelectorInfo::GetVertexDescs() const
{
    auto descList = GetRawDescs();
    for(auto& desc : descList)
    {
        desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        desc.InputSlot = 0;
        desc.InstanceDataStepRate = 0;
    }
    return descList;
}

std::vector<D3D12_INPUT_ELEMENT_DESC> SelectorInfo::GetRawDescs() const
{
    std::vector<D3D12_INPUT_ELEMENT_DESC> descList;
    for(int i=0;i<propInfos.size();i++)
    {
        auto& propInfo = propInfos[i];
        auto desc = GetDesc(propInfo);
        int splitCount = 0;
        if(propInfo.size > 4)
            splitCount = propInfo.size / 4;
        else
			descList.push_back(desc);

        int byteOffset = propInfo.byteOffset;
        for(int i=0;i<splitCount;i++)
        {
            desc.SemanticIndex = i;
            desc.AlignedByteOffset = byteOffset + (propInfo.byteSize / splitCount) * i;
            descList.push_back(desc);
        }
    }
    return descList;
}
