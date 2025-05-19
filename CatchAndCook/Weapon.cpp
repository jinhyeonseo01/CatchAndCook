#include "pch.h"
#include "Weapon.h"
#include "Transform.h"
#include "Gizmo.h"
#include "MeshRenderer.h"
#include "CameraManager.h"
#include "Camera.h"
#include "Collider.h"
Weapon::Weapon()
{
}

Weapon::~Weapon()
{
}

void Weapon::SetDestroy()
{

}

void Weapon::Init()
{
	_targetHud = SceneManager::main->GetCurrentScene()->CreateGameObject(L"Sprite1");
	auto& renderer = _targetHud->AddComponent<MeshRenderer>();
	auto& sprite = _targetHud->AddComponent<Sprite>();
	sprite->SetLocalPos(vec3(300, 0, 0));
	sprite->SetSize(vec2(10, 10));

	sprite->SetTexture(ResourceManager::main->Load<Texture>(L"targetHud", L"Textures/targetHud.png"));

	shared_ptr<Material> material = make_shared<Material>();
	material->SetShader(ResourceManager::main->Get<Shader>(L"SpriteShader"));
	material->SetPass(RENDER_PASS::UI);
	renderer->AddMaterials({ material });
};

void Weapon::Start()
{
	controller = GetOwner()->GetComponent<SeaPlayerController>().get();
}

void Weapon::Update()
{

}

void Weapon::Update2()
{
}

void Weapon::Enable()
{
}

void Weapon::Disable()
{
}

void Weapon::Destroy()
{
}

void Weapon::RenderBegin()
{
}

void Weapon::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	other->GetOwner()->SetDestroy();
	cout << "Collision Begin" << endl;
}

void Weapon::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

bool Weapon::IsExecuteAble()
{
    return false;
}



void Weapon::SetTargetHudPos()
{
	if (_currentWeapon == nullptr)
		return;

	auto& cameraParams = CameraManager::main->GetCamera(CameraType::SeaCamera)->GetCameraParam();

	vec2 screenPos{ 0, 0 };

	vec4 worldPos = _currentWeapon->weaponSlot->_transform->GetWorldPosition() + _currentWeapon->hook->_transform->GetForward() * _currentWeapon->_range;
	worldPos.w = 1.0f;

	Matrix vp = cameraParams.VPMatrix;   

	vec4 clipPos = vec4::Transform(worldPos, vp);

	vec3 ndc = vec3(clipPos) / clipPos.w;

	float w = static_cast<float>(WINDOW_WIDTH);
	float h = static_cast<float>(WINDOW_HEIGHT);

	screenPos.x = (ndc.x * 0.5f + 0.5f) * w;
	screenPos.y = (1.0f - (ndc.y * 0.5f + 0.5f)) * h;

	_targetHud->GetComponent<Sprite>()->SetLocalPos(vec3(screenPos.x-5.0f, screenPos.y-5.0f, 0.0f));
}
void Weapon::SetCurrentWeapon(const wstring& weaponName)
{
	if (_currentWeapon && _currentWeapon->GunName == weaponName)
		return;

	auto it = _weapons.find(weaponName);

	if (it == _weapons.end())
	{
		cout << "Weapon Not Found" << endl;
		return;
	}

	if (_currentWeapon)
	{
		_currentWeapon->body->SetActiveSelf(false);
		_currentWeapon->hook->SetActiveSelf(false);
	}

	_currentWeapon = it->second;

	_currentWeapon->body->SetActiveSelf(true);
	_currentWeapon->hook->SetActiveSelf(true);

}

void Weapon::AddWeapon(const wstring& weaponName, const wstring& bodyName, const wstring& hookName, const wstring& weaponSlot)
{
	shared_ptr<Gun> gun = make_shared<Gun>();
	gun->body = SceneManager::main->GetCurrentScene()->Find(bodyName);
	gun->hook = SceneManager::main->GetCurrentScene()->Find(hookName);
	gun->weaponSlot = SceneManager::main->GetCurrentScene()->Find(weaponSlot);
	if (gun->body == nullptr || gun->hook == nullptr || gun->weaponSlot ==nullptr )
	{
		cout << "Weapon Not Found" << endl;
		return;
	}

	gun->_backToPos = gun->hook->_transform->GetLocalPosition();
	gun->GunName = weaponName;
	_currentWeapon = gun;
	_weapons[weaponName] = gun;

}

void Weapon::AddWeapon(shared_ptr<Gun> gun)
{
	auto it = _weapons.find(gun->GunName);

	if (it != _weapons.end())
	{
		cout << "Weapon Already Exist" << endl;
		return;
	}

	_weapons[gun->GunName] = gun;
}

void Weapon::Shot()
{
	const float speed = _currentWeapon->_speed;
	vec3 currentPos = _currentWeapon->hook->_transform->GetWorldPosition();
	vec3 forward = _currentWeapon->hook->_transform->GetForward();
	_currentWeapon->hook->_transform->SetWorldPosition(currentPos + forward * _currentWeapon->_speed * Time::main->GetDeltaTime());
	_moveDist += speed * Time::main->GetDeltaTime();

	if (_moveDist >= _currentWeapon->_range)
	{
		_moveDist = 0;
		controller->SetState(SeaPlayerState::Reload);
	}

	SetTargetHudPos();

	Gizmo::main->Width(0.2f);
	Gizmo::main->Line(_currentWeapon->weaponSlot->_transform->GetWorldPosition(), _currentWeapon->hook->_transform->GetWorldPosition(), vec4(0.545f, 0.27f, 0.075f, 1));
}

void Weapon::Reload()
{
	const float speed = _currentWeapon->_speed;
	vec3 slotPos = _currentWeapon->weaponSlot->_transform->GetWorldPosition();
	vec3 currentHookPos = _currentWeapon->hook->_transform->GetWorldPosition();
	vec3 dir = slotPos - currentHookPos;
	dir.Normalize();

	_currentWeapon->hook->_transform->SetWorldPosition(currentHookPos + dir * speed * Time::main->GetDeltaTime());
	_moveDist += speed * Time::main->GetDeltaTime();

	SetTargetHudPos();

	Gizmo::main->Width(0.2f);
	Gizmo::main->Line(_currentWeapon->weaponSlot->_transform->GetWorldPosition(), _currentWeapon->hook->_transform->GetWorldPosition(), vec4(0.545f, 0.27f, 0.075f, 1));

	if (_moveDist >= _currentWeapon->_range)
	{
		_moveDist = 0;
		_currentWeapon->hook->_transform->SetLocalPosition(_currentWeapon->_backToPos);
		controller->SetState(SeaPlayerState::Aiming);
	}
}




