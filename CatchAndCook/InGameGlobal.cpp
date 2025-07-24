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

	itemData.itemCode = 6;
	itemData.itemCookType = -1;
	boxItems.push_back(itemData);

	itemData.itemCode = 4;
	itemData.itemCookType = -1;
	boxItems.push_back(itemData);
	itemData.itemCode = 3;
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


	cookTable.push_back(std::make_pair(
		ItemData(11, -1), // 생선탕 레시피
		vector{
			ItemData{3,3},
			ItemData{0,3},
			ItemData{5,3}
		}));
	cookTable.push_back(std::make_pair(
		ItemData(11, -1), // 생선탕 레시피
		vector{
			ItemData{4,3},
			ItemData{0,3},
			ItemData{5,3}
		}));

	cookTable.push_back(std::make_pair(
		ItemData(11, -1), // 생선탕 레시피
		vector{
			ItemData{3,3},
			ItemData{1,3},
			ItemData{5,3}
		}));
	cookTable.push_back(std::make_pair(
		ItemData(11, -1), // 생선탕 레시피
		vector{
			ItemData{4,3},
			ItemData{1,3},
			ItemData{5,3}
		}));

	cookTable.push_back(std::make_pair(
		ItemData(11, -1), // 생선탕 레시피
		vector{
			ItemData{3,3},
			ItemData{2,3},
			ItemData{5,3}
		}));
	cookTable.push_back(std::make_pair(
		ItemData(11, -1), // 생선탕 레시피
		vector{
			ItemData{4,3},
			ItemData{2,3},
			ItemData{5,3}
		}));

		cookTable.push_back(std::make_pair(
		ItemData(11, -1), // 생선탕 레시피
		vector{
			ItemData{3,3},
			ItemData{0,3},
			ItemData{5,3}
		}));
	cookTable.push_back(std::make_pair(
		ItemData(11, -1), // 생선탕 레시피
		vector{
			ItemData{4,3},
			ItemData{0,3},
			ItemData{5,3}
		}));

	cookTable.push_back(std::make_pair(
		ItemData(11, -1), // 생선탕 레시피
		vector{
			ItemData{3,3},
			ItemData{1,3},
			ItemData{5,3}
		}));
	cookTable.push_back(std::make_pair(
		ItemData(11, -1), // 생선탕 레시피
		vector{
			ItemData{4,3},
			ItemData{1,3},
			ItemData{5,3}
		}));

	cookTable.push_back(std::make_pair(
		ItemData(11, -1), // 생선탕 레시피
		vector{
			ItemData{3,3},
			ItemData{2,3},
			ItemData{5,3}
		}));
	cookTable.push_back(std::make_pair(
		ItemData(11, -1), // 생선탕 레시피
		vector{
			ItemData{4,3},
			ItemData{2,3},
			ItemData{5,3}
		}));




	cookTable.push_back(std::make_pair(
		ItemData(11, -1), // 생선탕 레시피
		vector{
			ItemData{3,3},
			ItemData{0,3},
			ItemData{6,3}
		}));
	cookTable.push_back(std::make_pair(
		ItemData(11, -1), // 생선탕 레시피
		vector{
			ItemData{4,3},
			ItemData{0,3},
			ItemData{6,3}
		}));

	cookTable.push_back(std::make_pair(
		ItemData(11, -1), // 생선탕 레시피
		vector{
			ItemData{3,3},
			ItemData{1,3},
			ItemData{6,3}
		}));
	cookTable.push_back(std::make_pair(
		ItemData(11, -1), // 생선탕 레시피
		vector{
			ItemData{4,3},
			ItemData{1,3},
			ItemData{6,3}
		}));

	cookTable.push_back(std::make_pair(
		ItemData(11, -1), // 생선탕 레시피
		vector{
			ItemData{3,3},
			ItemData{2,3},
			ItemData{6,3}
		}));
	cookTable.push_back(std::make_pair(
		ItemData(11, -1), // 생선탕 레시피
		vector{
			ItemData{4,3},
			ItemData{2,3},
			ItemData{6,3}
		}));


}

void InGameGlobal::InitMainField()
{

}

void InGameGlobal::AddItem(const wstring& name)
{

	if (name == L"ray")
		boxItems.push_back(ItemData(3, -1));
	if (name == L"redFish")
		boxItems.push_back(ItemData(4, -1));
	if (name == L"주황버섯")
		boxItems.push_back(ItemData(6, -1));
	if (name == L"갈색버섯")
		boxItems.push_back(ItemData(5, -1));
    //auto it = _inventory.find(name);
	/*
    if (it != _inventory.end())
    {
        it->second.count += 1;
    }
    else
    {
        _inventory.emplace(name, Item(name, 1));
    }


    for (const auto& [itemName, item] : _inventory)
    {
        wcout << itemName << "," << item.count << endl;
    }
	*/
}

