#include "pch.h"
#include "ParticleComponent.h"
#include "ParticleManager.h"

static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
static std::uniform_real_distribution<float> dist2(-0.1f, 0.1f);
void ParticleComponent::Init()
{
}

void ParticleComponent::Start()
{



}

void ParticleComponent::Update()
{
	_currTime += Time::main->GetDeltaTime();

	float fadeProgress = _currTime / _autoDestroyTime;
	fadeProgress = std::clamp(fadeProgress, 0.0f, 1.0f);

	float alpha = 0.5f * (1.0f - (fadeProgress * fadeProgress * (3.0f - 2.0f * fadeProgress)));

	for (int i = 0; i < 4; ++i)
	{
		_blendFactor[i] = alpha;
	}

	if (_currTime >= _autoDestroyTime)
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

void ParticleComponent::SetParticle(shared_ptr<StructuredBuffer> strBuffer, float autoDestroyTime, int particleCount, float size, float speed ,const vec3& worldPos, const vec3& worldNormal, const ParticleMoveType& moveType, const ParticleColorType& colorType, shared_ptr<Texture> texture,
	 const vec4& clipingColor)
{
	assert(particleCount <= 500);

	if (particleCount > 500)
		cout << "파티클 갯수 잘못설정" << endl;

	_helperParams.paricleCount = particleCount;
	_helperParams.clipingColor = clipingColor;

	if (texture)
	{
		_helperParams.textureUse = 1;
	}

	_strBuffer = strBuffer;
	_autoDestroyTime = autoDestroyTime;


	vector<ParticleData> vecData(particleCount);

	for (int i = 0; i < particleCount; ++i)
	{

		ParticleData data;

		data.size = size;


		switch (moveType)
		{
		case ParticleMoveType::RadialSpread:
			data.worldPos = worldPos;
			data.dir = vec3(dist(gen), dist(gen), dist(gen));
			data.velocity = speed;
			break;

		case ParticleMoveType::BloodUnderwater:
		{
			data.worldPos = worldPos;

			float x = dist(gen) * 0.5f;
			float y = dist(gen) * 0.3f + 0.2f;
			float z = dist(gen) * 0.5f;

			data.dir = vec3(x, y, z);
			data.velocity = speed;
			break;
		}

		case ParticleMoveType::ScreenSpaceBubble:
		{
			data.worldPos = worldPos;
			data.worldPos.x += dist2(gen);
			data.worldPos.y += dist2(gen);
			float y = (dist(gen)) * 0.3f + 0.1f;
			data.dir = vec3(0, y, 0);
			data.velocity = speed;
		}
		break;
		default:
			break;
		}

		
		if (texture == nullptr)
		{

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
		}

		vecData[i] = data;
	}


	_strBuffer->CopyToDefaultHeap(vecData);
}

