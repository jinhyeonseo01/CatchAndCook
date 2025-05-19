#pragma once
#include "Component.h"

struct Gun
{
	wstring GunName =L"NULL";
	shared_ptr<GameObject> body{};
	shared_ptr<GameObject> hook{};
	shared_ptr<GameObject> weaponSlot{};

	vec3 _backToPos;
	float _power{};
	float _range =300.0f;
	float _speed =800.0f;
};

enum class WeaponState
{
	Idle,
	Shot,
	Reload,
};

class Weapon :public Component
{

public:
	Weapon();
	~Weapon();
	void SetDestroy() override;
	void Init() override;
	void Start() override;
	void Update() override;
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void Destroy() override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	bool IsExecuteAble() override;

	void ChangeState(const WeaponState& state);

public:
	WeaponState GetState() { return  _state; }
	void SetCurrentWeapon(const wstring& weaponName);
	void AddWeapon(const wstring& weaponName, const wstring& bodyName, const wstring& hookName, const wstring& weaponSlot );
	void AddWeapon(shared_ptr<Gun> gun);
	shared_ptr<Gun> GetCurrentWeapon() { return _currentWeapon; }

private:
	WeaponState _state = WeaponState::Idle;
	float _moveDist = 0;
	shared_ptr<Gun> _currentWeapon;
	unordered_map<wstring, shared_ptr<Gun>> _weapons;

};

