#pragma once

#include "FireBall.h"

class Dobby;

class FireWorkManager
{
public:
	static unique_ptr<FireWorkManager> main;

public:
	void LateInit();
	void PushFireWork(shared_ptr<GameObject>& firework){_fireworks.push_back(firework);}
	void PushDobby(shared_ptr<Dobby>& dobby) { _dobby = dobby; }

	shared_ptr<GameObject> GetFireBall(fireballColor color);

	bool CheckAllFireWorkDone();
	void SetFire();
private:
	unordered_map<fireballColor, shared_ptr<GameObject>> _fireBalls;
	vector<shared_ptr<GameObject>> _fireworks;
	shared_ptr<Dobby> _dobby;
};

