#pragma once
#include "GUIItem.h"

class InGameGlobal
{
public:
	static unique_ptr<InGameGlobal> main;
	void Init();
	void InitMainField();

public: // Main Field
	float skyTime = 3;

	static constexpr int InventoryCount = 5;
	std::array<ItemData, InventoryCount> invItems;
	vector<ItemData> boxItems;

	std::vector<std::pair<ItemData, vector<ItemData>>> cookTable;
};

