#pragma once

struct Item
{
	std::wstring name; // ResourceManager 에서 알맞는 이미지 찾아적용
	int count;
	Item() {};

	Item(const wstring& name, int count)
		: name(name), count(count) {
	}
};

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
	std::unordered_map<wstring, Item> _inventory; // temp
	float skyTime = 0;
};

