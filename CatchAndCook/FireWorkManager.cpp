#include "pch.h"
#include "FireWorkManager.h"
#include "FireWork.h"
#include "Dobby.h"
 unique_ptr<FireWorkManager> FireWorkManager::main=nullptr;

 void FireWorkManager::LateInit()
 {

	 {
		 auto object = SceneManager::main->GetCurrentScene()->Find(L"redball");
		 if (object)
		 {
			 _fireBalls[fireballColor::red] = object;
		 }
	 }

	 {
		 auto object = SceneManager::main->GetCurrentScene()->Find(L"yellowball");
		 if (object)
		 {
			 _fireBalls[fireballColor::yellow] = object;
		 }
	 }

	 {
		 auto object = SceneManager::main->GetCurrentScene()->Find(L"greenball");
		 if (object)
		 {
			 _fireBalls[fireballColor::green] = object;
		 }
	 }

	 for (auto& ele : _fireBalls)
	 {
		 ele.second->SetActiveSelf(false);
	 }

 };

 void FireWorkManager::SetFire()
 {
	 
		 for (auto& firework : _fireworks)
		 {
			 auto fireworkComponent = firework->GetComponent<FireWork>();

			 if (fireworkComponent)
			 {
				 fireworkComponent->Shot();
			 }
		 }

		 Sound::main->Play("firework", 0.3f);
		 _dobby->SetAnimation("magic");
	 

 }

 shared_ptr<GameObject> FireWorkManager::GetFireBall(fireballColor color)
 {
	 {
		 auto it = _fireBalls.find(color);
		 if (it != _fireBalls.end())
		 {
			 return it->second;
		 }
		 return nullptr;
	 }
 }

 bool FireWorkManager::CheckAllFireWorkDone()
 {
	 for (auto& firework : _fireworks)
	 {
		 auto fireworkComponent = firework->GetComponent<FireWork>();

		 if (fireworkComponent)
		 {
			 if (fireworkComponent->IsDone()== false)
				 return false;
		 }
	 }

	 return true;
 }
