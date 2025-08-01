#include "pch.h"
#include "InstancingManager.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
unique_ptr<InstancingManager> InstancingManager::main;

void InstancingManager::RenderNoInstancing(RenderObjectStrucutre& RoS)
{
	auto& cmdList = Core::main->GetCmdList();
	auto& bufferManager = Core::main->GetBufferManager();
	auto& tableManager = bufferManager->GetTable();

	auto& material = RoS.material;
	auto& renderer = RoS.renderer;
	auto& mesh = RoS.mesh;
	auto& shader = material->GetShader();
	auto& structuredInfo = shader->GetTRegisterStructured();

	InstanceOffsetParam param = {};

	for (const auto& infos : structuredInfo)
	{
		const std::string& name = infos.name;

		if (name == "g_lights")
			continue;

		auto& bufferType = bufferManager->GetStructuredNameToBufferType(name);
		auto& pool = bufferManager->GetStructuredBufferPool(bufferType);
		int offset = pool->GetOffset();

		auto table = tableManager->Alloc(1);

		auto& setter = RoS.renderer->FindStructuredSetter(bufferType);
		setter->SetData(pool.get());

		tableManager->CopyHandle(table.CPUHandle, pool->GetSRVHandle(), 0);

		int ROOT_OFFSET = infos.registerIndex - SRV_STRUCTURED_TABLE_REGISTER_OFFSET;
		assert(ROOT_OFFSET >= 0);

		cmdList->SetGraphicsRootDescriptorTable(SRV_STRUCTURED_TABLE_INDEX + ROOT_OFFSET, table.GPUHandle);
		param.offset[ROOT_OFFSET].x = offset;
	}

	auto cbufferContainer = bufferManager->GetBufferPool(BufferType::InstanceOffsetParam)->Alloc(1);
	memcpy(cbufferContainer->ptr, &param, sizeof(InstanceOffsetParam));
	cmdList->SetGraphicsRootConstantBufferView(4, cbufferContainer->GPUAdress);

	g_debug_draw_call++;
	renderer->Rendering(material, mesh, 1);
};

void InstancingManager::Render()
{
	auto& cmdList = Core::main->GetCmdList();
	auto& bufferManager = Core::main->GetBufferManager();
	auto& tableManager = bufferManager->GetTable();

	for (auto& [id, RenderObjectStrutures] : _objectMap)
	{
		if (RenderObjectStrutures.empty()) continue;

		auto& material = RenderObjectStrutures[0].material;
		auto& renderer = RenderObjectStrutures[0].renderer;
		auto& mesh = RenderObjectStrutures[0].mesh;
		auto& shader = material->GetShader();
		auto& structuredInfo = shader->GetTRegisterStructured();
		InstanceOffsetParam param = {};

		for (const auto& infos : structuredInfo)
		{
			const std::string& name = infos.name;

			if (name == "g_lights")
				continue;

			auto& bufferType = bufferManager->GetStructuredNameToBufferType(name);
			auto& struturedBufferPool = bufferManager->GetStructuredBufferPool(bufferType);
			int offset = struturedBufferPool->GetOffset();

			auto table = tableManager->Alloc(1);

			for (auto& renderobjectStruture : RenderObjectStrutures)
			{
				auto& setter = renderobjectStruture.renderer->FindStructuredSetter(bufferType);
				assert(setter != nullptr);
				if (setter != nullptr)
					setter->SetData(struturedBufferPool.get(),renderobjectStruture.material);
			}

			tableManager->CopyHandle(table.CPUHandle, struturedBufferPool->GetSRVHandle(), 0);

			int ROOT_OFFSET = infos.registerIndex - SRV_STRUCTURED_TABLE_REGISTER_OFFSET;
			assert(ROOT_OFFSET >= 0);
			cmdList->SetGraphicsRootDescriptorTable(SRV_STRUCTURED_TABLE_INDEX + ROOT_OFFSET, table.GPUHandle);
			param.offset[ROOT_OFFSET].x = offset;
		}

		auto cbufferContainer = bufferManager->GetBufferPool(BufferType::InstanceOffsetParam)->Alloc(1);
		memcpy(cbufferContainer->ptr, &param, sizeof(InstanceOffsetParam));
		cmdList->SetGraphicsRootConstantBufferView(4, cbufferContainer->GPUAdress);

		g_debug_draw_call++;
		renderer->Rendering(material, mesh, RenderObjectStrutures.size());
	}

	_objectMap.clear();
}

void InstancingManager::Clear()
{

}


void InstancingManager::AddObject(RenderObjectStrucutre & obj)
{
	unsigned int materialId = 0;
	if (obj.material != nullptr)
		materialId = obj.material->GetID();
	InstanceID id = {obj.mesh->GetID(), obj.material->GetID() };
	_objectMap[id.id].push_back(obj);

}
