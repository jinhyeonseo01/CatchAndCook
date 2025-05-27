#include "pch.h"
#include "Material.h"
#include "Core.h"
#include "BufferManager.h"
#include "BufferPool.h"
#include "Texture.h"
#include "Shader.h"

uint32 Material::_instanceIDGenator =0;
TableContainer Material::_defualtTableContainer;

Material::Material()
{
	_instanceID  = _instanceIDGenator++;
}

Material::~Material()
{

}
//void Material::SetHandle(std::string name, D3D12_CPU_DESCRIPTOR_HANDLE& field)
//{
//	_propertyHandle[name] = field;
//}

void Material::SetTexture(std::string name, std::shared_ptr<Texture> texture)
{
	_propertyHandle[name] = texture->GetSRVCpuHandle();
	_propertyTexture[name] = texture;

}

void Material::AllocDefualtTextureHandle()
{
	auto& NoneTexture = ResourceManager::main->GetNoneTexture()->GetSRVCpuHandle();


	auto& table = Core::main->GetBufferManager()->GetTable();
	_defualtTableContainer = Core::main->GetBufferManager()->GetTable()->Alloc(SRV_TABLE_REGISTER_COUNT);
	for (int i=0;i< SRV_TABLE_REGISTER_COUNT;i++)
		table->CopyHandle(_defualtTableContainer.CPUHandle, NoneTexture, i);
}


void Material::AllocTextureTable()
{
	_tableContainer = Core::main->GetBufferManager()->GetTable()->Alloc(SRV_TABLE_REGISTER_COUNT);
}

void Material::AllocTextureLongTable()
{
	_tableLongContainer = Core::main->GetBufferManager()->GetTable()->Alloc(SRV_LONG_TABLE_REGISTER_COUNT);
}

void Material::PushData()
{

	for (auto& injector : _shaderInjectors)
		injector->Inject(GetCast<Material>());
	for (auto& injector : _customInjectors)
		injector->Inject(GetCast<Material>());
}

void Material::SetData()
{
	if (_setDataOff)
		return;
	//텍스쳐바인딩

	if (CheckLongHandle())
	{
		AllocTextureLongTable();
		PushLongHandle();
		Core::main->GetCmdList()->SetGraphicsRootDescriptorTable(SRV_LONG_TABLE_INDEX, _tableLongContainer.GPUHandle);
	}
	if (CheckHandle())
	{
		AllocTextureTable();
		PushHandle();
		Core::main->GetCmdList()->SetGraphicsRootDescriptorTable(SRV_TABLE_INDEX, _tableContainer.GPUHandle);
	}
	else
		Core::main->GetCmdList()->SetGraphicsRootDescriptorTable(SRV_TABLE_INDEX, _defualtTableContainer.GPUHandle);

	PushData();

	//if (_useMaterialParams)
	//	Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(6, _cbufferContainer->GPUAdress);

	for (auto& injector : _shaderInjectors)
		injector->SetData(_shader);

	for (auto& injector : _customInjectors)
		injector->SetData(_shader);
}

void Material::PushHandle()
{
	array<bool, SRV_TABLE_REGISTER_COUNT> copyCheckList;
	copyCheckList.fill(false);

	auto& table =Core::main->GetBufferManager()->GetTable();

	for (auto& [name, handle] : _propertyHandle)
	{
		int index = _shader->GetRegisterIndex(name);

		if (index != -1)
		{
			if (index < SRV_TABLE_REGISTER_COUNT)
			{
				copyCheckList[index] = true;
				table->CopyHandle(_tableContainer.CPUHandle, handle, index);
			}
		}
	}

	auto& noneTexture = ResourceManager::main->GetNoneTexture()->GetSRVCpuHandle();

	auto& tTable = _shader->GetTRegisterIndexs();
	for (auto& tIndex : tTable)
		if (tIndex < SRV_TABLE_REGISTER_COUNT && (!copyCheckList[tIndex]))
			table->CopyHandle(_tableContainer.CPUHandle,
				noneTexture, tIndex);

}

void Material::PushLongHandle()
{
	auto& table = Core::main->GetBufferManager()->GetTable();

	for (auto& [name, handle] : _propertyHandle)
	{
		int index = _shader->GetRegisterIndex(name);

		if (index != -1)
		{
			if (index >= SRV_LONG_TABLE_REGISTER_OFFSET && (index < (SRV_LONG_TABLE_REGISTER_OFFSET + SRV_LONG_TABLE_REGISTER_COUNT))) {
				table->CopyHandle(_tableLongContainer.CPUHandle, handle, index - SRV_LONG_TABLE_REGISTER_OFFSET);
			}
		}
	}
}

bool Material::CheckHandle()
{
	bool tf = false;
	for (auto& [name, handle] : _propertyHandle) {
		int index = _shader->GetRegisterIndex(name);

		if (index != -1) {
			if (index < SRV_TABLE_REGISTER_COUNT) {
				tf = true;
				break;
			}
		}
	}
	return tf;
}

bool Material::CheckLongHandle()
{
	return _shader->_profileInfo.maxTRegister >= SRV_LONG_TABLE_REGISTER_OFFSET;
}

vec4 Material::GetPropertyVector(const std::string& name)
{
	if (!_propertyVectors.contains(name))
		return Vector4::One;
	auto& color = _propertyVectors[name];
	return color;
}

bool Material::HasPropertyInt(const std::string& name)
{
	return _propertyInts.contains(name);
}

bool Material::HasPropertyFloat(const std::string& name)
{
	return _propertyFloats.contains(name);
}

bool Material::HasPropertyVector(const std::string& name)
{
	return _propertyVectors.contains(name);
}

bool Material::HasPropertyMatrix(const std::string& name)
{
	return _propertyMatrixs.contains(name);
}

std::shared_ptr<Material> Material::Clone()
{
	return make_shared<Material>(*this);

	//std::shared_ptr<Material> material = make_shared<Material>();
	//material->_propertyInts = _propertyInts;
	//material->_propertyFloats = _propertyFloats;
	//material->_propertyVectors = _propertyVectors;
	//material->_propertyMatrixs = _propertyMatrixs;
	//material->_propertyHandle = _propertyHandle;
	//m aterial->_propertyTexture = _propertyTexture;
	//material->_shaderInjectors = _shaderInjectors;
	//material->_customInjectors = _customInjectors;
	//material->_shader = _shader;
	//material->_pass = _pass;
	//material->_stencilIndex = _stencilIndex;
	//material->_useMaterialParams = _useMaterialParams;
	//material->_shadowCasting = _shadowCasting;
	//material->_setDataOff = _setDataOff;
	//material->_preDepthNormal = _preDepthNormal;
	//return material;

}

void Material::CopyProperties(std::shared_ptr<Material>& dest)
{
	for (auto& property : _propertyInts)
		dest->_propertyInts.insert_or_assign(property.first, property.second);
	for (auto& property : _propertyFloats)
		dest->_propertyFloats.insert_or_assign(property.first, property.second);
	for (auto& property : _propertyVectors)
		dest->_propertyVectors.insert_or_assign(property.first, property.second);
	for (auto& property : _propertyMatrixs)
		dest->_propertyMatrixs.insert_or_assign(property.first, property.second);
	for (auto& property : _propertyHandle)
		dest->_propertyHandle.insert_or_assign(property.first, property.second);
	for (auto& property : _propertyTexture)
		dest->_propertyTexture.insert_or_assign(property.first, property.second);

	for (auto& property : _shaderInjectors)
		if (std::ranges::find(dest->_shaderInjectors, property) == dest->_shaderInjectors.end())
			dest->_shaderInjectors.push_back(property);
	for (auto& property : _customInjectors)
		if (std::ranges::find(dest->_customInjectors, property) == dest->_customInjectors.end())
			dest->_customInjectors.push_back(property);

}
