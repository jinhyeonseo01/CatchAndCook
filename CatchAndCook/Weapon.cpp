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

	sprite->SetLocalPos(vec3(0.5f, 0.5f, 0.0f));
	sprite->SetClipingColor(vec4(0, 0, 0, 0));
	sprite->SetTexture(ResourceManager::main->Load<Texture>(L"targetHud", L"Textures/targetHud.png"));
	shared_ptr<Material> material = make_shared<Material>();
	material->SetShader(ResourceManager::main->Get<Shader>(L"SpriteShader"));
	material->SetBlendFactor({ 0.7f,0.7f,0.7f,0.7f });
	material->SetPass(RENDER_PASS::UI);
	renderer->AddMaterials({ material });

	_controller = contorller;
};



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

void Weapon::SetTargetHudSize()
{
	float aspect = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT);
	float height = 0.05f; 
	vec2 size(height / aspect, height); 
	auto& sprite = _targetHud->GetComponent<Sprite>();
	sprite->SetSize(size);
	sprite->SetLocalPos(vec3(0.5f-size.x*0.5f,0.5f-size.y*0.5f, 0.0f));
}

vec3 Weapon::GetTaretPos()
{
	auto& sprite = _targetHud->GetComponent<Sprite>();
	return sprite->GetTargetPos();
}


void Weapon::Shot()
{

}




