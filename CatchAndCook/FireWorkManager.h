#pragma once

#include "FireBall.h"

class FireWorkManager
{
public:
	static unique_ptr<FireWorkManager> main;

public:
	void LateInit();
	void PushFireWork(shared_ptr<GameObject>& firework){_fireworks.push_back(firework);}
	void SetFire();
	shared_ptr<GameObject> GetFireBall(fireballColor color);
	bool CheckAllFireWorkDone();

private:
	unordered_map<fireballColor, shared_ptr<GameObject>> _fireBalls;
	vector<shared_ptr<GameObject>> _fireworks;
};

