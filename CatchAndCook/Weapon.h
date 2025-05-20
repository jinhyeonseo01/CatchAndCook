#pragma once
#include "Component.h"
#include "SeaPlayerController.h"

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


class Weapon 
{

public:
	Weapon();
	~Weapon();
	void SetTargetHudPos();
	void Init(SeaPlayerController* contorller);
public:
	
	void SetCurrentWeapon(const wstring& weaponName);
	void AddWeapon(const wstring& weaponName, const wstring& bodyName, const wstring& hookName, const wstring& weaponSlot );
	void AddWeapon(shared_ptr<Gun> gun);
	shared_ptr<Gun> GetCurrentWeapon() { return _currentWeapon; }

	void Shot();
	void Reload();

	void SetTargetHudVisible(bool onOff)
	{
		if (_targetHud)
			_targetHud->SetActiveSelf(onOff);
	}

private:
	SeaPlayerController* _controller;
	float _moveDist = 0;
	shared_ptr<Gun> _currentWeapon;
	shared_ptr<GameObject> _targetHud;
	unordered_map<wstring, shared_ptr<Gun>> _weapons;

};

