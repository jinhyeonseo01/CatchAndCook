#include "pch.h"
#include "Weapon.h"
#include "Transform.h"
#include "Gizmo.h"
#include "MeshRenderer.h"
#include "CameraManager.h"
#include "Camera.h"
#include "Collider.h"
#include "Mesh.h"
Weapon::Weapon()
{
}

Weapon::~Weapon()
{

}

void Weapon::Init(SeaPlayerController* contorller)
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

	_controller = contorller;
};




void Weapon::SetTargetHudPos() 
{
	if (_currentWeapon == nullptr)
		return;

	/*auto& cameraParams = CameraManager::main->GetCamera(CameraType::SeaCamera)->GetCameraParam();

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

	_targetHud->GetComponent<Sprite>()->SetLocalPos(vec3(screenPos.x-5.0f, screenPos.y-5.0f, 0.0f));*/
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
		_currentWeapon->gun->SetActiveSelf(false);
		_currentWeapon->gun->SetActiveSelf(false);
	}

	_currentWeapon = it->second;

	_currentWeapon->gun->SetActiveSelf(true);
	_currentWeapon->gun->SetActiveSelf(true);

}

void Weapon::AddWeapon(const wstring& gunName, const wstring& slotName , float speed)
{
	shared_ptr<weapon> weaPon = make_shared<weapon>();
	weaPon->_speed = speed;
	weaPon->gun = SceneManager::main->GetCurrentScene()->Find(gunName);
	weaPon->weaponSlot = SceneManager::main->GetCurrentScene()->Find(slotName);

	if (weaPon->gun == nullptr  || weaPon->weaponSlot == nullptr)
	{
		cout << "Weapon Not Found" << endl;
		return;
	}

	weaPon->GunName = gunName;
	_currentWeapon = weaPon;
	_weapons[gunName] = weaPon;
}


void Weapon::Shot()
{

}




