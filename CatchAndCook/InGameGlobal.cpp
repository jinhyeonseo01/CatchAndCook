#include "pch.h"
#include "InGameGlobal.h"


unique_ptr<InGameGlobal> InGameGlobal::main = nullptr;

void InGameGlobal::Init()
{
	InitMainField();
}

void InGameGlobal::InitMainField()
{

}

void InGameGlobal::AddItem(const wstring& name)
{

    auto it = _inventory.find(name);

    if (it != _inventory.end())
    {
        it->second.count += 1;
    }
    else
    {
        _inventory.emplace(name, Item(name, 1));
    }


    //for (const auto& [itemName, item] : _inventory)
    //{
    //    wcout << itemName << "," << item.count << endl;
    //}
}

