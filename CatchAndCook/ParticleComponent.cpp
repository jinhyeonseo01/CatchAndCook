#include "pch.h"
#include "ParticleComponent.h"
#include "ParticleManager.h"

static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_real_distribution<float> dist(0.0f, 1.0f);

void ParticleComponent::Init()
{
}

void ParticleComponent::Start()
{
}

void ParticleComponent::Update()
{
	_currTime += Time::main->GetDeltaTime();

	if (_autoDestroyTime <= _currTime)
	{
		SceneManager::main->GetCurrentScene()->AddDestroyQueue(GetOwner());
	}
}

void ParticleComponent::Update2()
{
}

void ParticleComponent::Enable()
{
}

void ParticleComponent::Disable()
{
}

void ParticleComponent::RenderBegin()
{
}

void ParticleComponent::RenderEnd()
{
}

void ParticleComponent::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void ParticleComponent::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void ParticleComponent::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
}

void ParticleComponent::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
}

void ParticleComponent::SetDestroy()
{
}

void ParticleComponent::Destroy()
{
	ParticleManager::main->RecycleParticleBuffer(_strBuffer);
}

void ParticleComponent::SetParticle(shared_ptr<StructuredBuffer> strBuffer, float autoDestroyTime, int particleCount, vec3 worldPos, ParticleMoveType moveType, ParticleColorType colorType)
{
	assert(particleCount <= 500);

	_strBuffer = strBuffer;
	_autoDestroyTime = autoDestroyTime;

	if (particleCount > 500)
		cout << "파티클 갯수 잘못설정" << endl;

	_particleCount = particleCount;
	vector<ParticleData> vecData(particleCount);
	
	for (int i = 0; i < _particleCount; ++i)
	{

		ParticleData data;

		switch (moveType)
		{
		case ParticleMoveType::RadialSpread:
			data.worldPos = worldPos;
			data.dir = vec3(dist(gen), dist(gen), dist(gen));

			data.velocity = 1.0f;
			break;
		default:
			break;
		}

		switch (colorType)
		{
		case ParticleColorType::Random:
			data.color = vec3(dist(gen), dist(gen), dist(gen));
			break;
		case ParticleColorType::Black:
			data.color = vec3(0, 0, 0);
			break;
		case ParticleColorType::White:
			data.color = vec3(1, 1, 1);
			break;
		case ParticleColorType::Red:
			data.color = vec3(1, 0, 0);
			break;
		default:
			break;
		}

		vecData[i] = data;
	}



	_strBuffer->CopyToDefaultHeap(vecData);

}

