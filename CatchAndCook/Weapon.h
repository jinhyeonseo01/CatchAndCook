#pragma once
#include "Component.h"
#include "SeaPlayerController.h"

struct weapon
{
	wstring GunName =L"NULL";
	shared_ptr<GameObject> gun{};
	shared_ptr<GameObject> weaponSlot{};
	float _speed =800.0f;
};


class Weapon 
{
public:
	Weapon();
	~Weapon();

	void Init(SeaPlayerController* contorller);
public:
	void CreateBullet();
	void SetCurrentWeapon(const wstring& weaponName);
	void AddWeapon(const wstring& gunName,const wstring& slotName ,float speed);
	void SetTargetHudSize();
	vec3 GetTaretPos();
	shared_ptr<weapon> GetCurrentWeapon() { return _currentWeapon; }

	void Shot();

	void SetTargetHudVisible(bool onOff)
	{
		if (_targetHud)
			_targetHud->SetActiveSelf(onOff);
	}

public:
	static void RecycleBullet(const shared_ptr<GameObject>& bullet);


private:
	SeaPlayerController* _controller;
	shared_ptr<weapon> _currentWeapon;
	shared_ptr<GameObject> _targetHud;
	unordered_map<wstring, shared_ptr<weapon>> _weapons;

	static std::queue<shared_ptr<GameObject>> _bulletQueue;

};

