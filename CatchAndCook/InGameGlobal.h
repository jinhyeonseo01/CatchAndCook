#pragma once
#include "GUIItem.h"

class InGameGlobal
{

public:  
	static unique_ptr<InGameGlobal> main;
	void Init();
	void InitMainField();

	inline std::mt19937& GetRandomMachine()
	{
		static std::mt19937 rng(std::random_device{}());
		return rng;
	}
	
public:
	void AddItem(const wstring& name);
public: 
	float skyTime = 0;
	int gold = 0;
public: 

	static constexpr int InventoryCount = 5;
	std::array<ItemData, InventoryCount> invItems;
	vector<ItemData> boxItems; 
	std::vector<std::pair<ItemData, vector<ItemData>>> cookTable;
};

