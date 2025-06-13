#include "pch.h"
#include "ParticleRenderer.h"
#include "ParticleComponent.h"

shared_ptr<Shader> ParticleRenderer::_particleComputeShader = nullptr;
shared_ptr<Shader> ParticleRenderer::_particleRenderingShader = nullptr;
shared_ptr<Texture> ParticleRenderer::_particleTexture = nullptr;
bool ParticleRenderer::IsExecuteAble()
{
    return false;
}

void ParticleRenderer::Init()
{
    SetCulling(false);
    SetInstancing(false);

	if(_particleComputeShader ==nullptr)
	{
		_particleComputeShader = make_shared<Shader>();
		ShaderInfo info;
		info._computeShader = true;
		_particleComputeShader->Init(L"ParticleComputeShader.hlsl", {}, ShaderArg{ {{"CS_Main","cs"}} }, info);
	}

	if (_particleRenderingShader == nullptr)
	{
		ShaderInfo info;
		info.renderTargetCount = 1;
		info._primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		info.RTVForamts[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

		info.cullingType = CullingType::BACK;

		_particleRenderingShader = make_shared<Shader>();

		_particleRenderingShader->Init(L"ParticleRenderingShader.hlsl", {}, ShaderArg{ {{"PS_Main","ps"},{"VS_Main","vs"},{"GS_Main","gs"}} }, info);
		_particleRenderingShader->SetPass(RENDER_PASS::NoEffectPostProcessing);

	}

	if (_particleTexture == nullptr)
	{
		_particleTexture = make_shared<Texture>();
		_particleTexture->Init(L"../Resources/Textures/particle.png");
	}

}

void ParticleRenderer::Start()
{
	 _particleComponent = GetOwner()->GetComponent<ParticleComponent>();

	if (_particleComponent ==nullptr)
	{
		assert(false);
		cout << "파티클 컴포넌트 없음" << endl;
	}

}

void ParticleRenderer::Update()
{
}

void ParticleRenderer::Update2()
{
}

void ParticleRenderer::Enable()
{
}

void ParticleRenderer::Disable()
{
}

void ParticleRenderer::Destroy()
{
}

void ParticleRenderer::RenderBegin()
{
    SceneManager::main->GetCurrentScene()->AddRenderer(nullptr, this, RENDER_PASS::ParticlePass);
}

void ParticleRenderer::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void ParticleRenderer::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void ParticleRenderer::SetDestroy()
{
}

void ParticleRenderer::Rendering(Material* material, Mesh* mesh, int instanceCount)
{
    auto& cmdList = Core::main->GetCmdList();

	//파티클 움직임 연산
	{

		cmdList->SetPipelineState(_particleComputeShader->_pipelineState.Get());

		auto& table = Core::main->GetBufferManager()->GetTable();
		_tableContainer = table->Alloc(10);
		table->CopyHandle(_tableContainer.CPUHandle, _particleComponent->GetStructuredBuffer()->GetUAVHandle(), 5);

		cmdList->SetComputeRootDescriptorTable(10, _tableContainer.GPUHandle);

		uint32 groupCount = (_particleComponent->GetParicleCount() + 255) / 256;
		cmdList->Dispatch(groupCount, 1, 1);

		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_particleComponent->GetStructuredBuffer()->GetDefaultBuffer().Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ));
	}

	//파티클 렌더링
	{
		cmdList->SetPipelineState(_particleRenderingShader->_pipelineState.Get());
		auto& table = Core::main->GetBufferManager()->GetTable(); 
		_tableContainer = table->Alloc(1);



		table->CopyHandle(_tableContainer.CPUHandle, _particleComponent->GetStructuredBuffer()->GetSRVHandle(), 0);
		table->CopyHandle(_tableContainer.CPUHandle, _particleTexture->GetSRVCpuHandle(), 1);

		cmdList->SetGraphicsRootDescriptorTable(SRV_TABLE_INDEX, _tableContainer.GPUHandle);
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		cmdList->DrawInstanced(1, _particleComponent->GetParicleCount(), 0, 0);
	}

	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_particleComponent->GetStructuredBuffer()->GetDefaultBuffer().Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));
}

void ParticleRenderer::DebugRendering()
{
}
