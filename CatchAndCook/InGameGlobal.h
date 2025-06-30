#pragma once

class InGameGlobal
{

public:  
	static unique_ptr<InGameGlobal> main;
	void Init();
	void InitMainField();

public: 
	float skyTime = 0;
};

