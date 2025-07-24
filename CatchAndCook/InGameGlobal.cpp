#include "pch.h"
#include "InGameGlobal.h"


unique_ptr<InGameGlobal> InGameGlobal::main = nullptr;

void InGameGlobal::Init()
{
	InitMainField();



	ItemData itemData;

	itemData.itemCode = 7;
	itemData.itemCookType = -1;
	boxItems.push_back(itemData);

	itemData.itemCode = 7;
	itemData.itemCookType = -1;
	boxItems.push_back(itemData);

	itemData.itemCode = 7;
	itemData.itemCookType = -1;
	boxItems.push_back(itemData);

	itemData.itemCode = 7;
	itemData.itemCookType = -1;
	boxItems.push_back(itemData);

	itemData.itemCode = 5;
	itemData.itemCookType = -1;
	boxItems.push_back(itemData);

	itemData.itemCode = 5;
	itemData.itemCookType = -1;
	boxItems.push_back(itemData);

	itemData.itemCode = 4;
	itemData.itemCookType = -1;
	boxItems.push_back(itemData);





	cookTable.push_back(std::make_pair(
		ItemData(10, -1), // 생선구이 레시피
		vector{
			ItemData{0,0},
			ItemData{8,-1}
		}));
	cookTable.push_back(std::make_pair(
		ItemData(10, -1), // 생선구이 레시피
		vector{
			ItemData{1,0},
			ItemData{8,-1}
		}));
	cookTable.push_back(std::make_pair(
		ItemData(10, -1), // 생선구이 레시피
		vector{
			ItemData{2,0},
			ItemData{8,-1}
		}));
}

void InGameGlobal::InitMainField()
{

}
