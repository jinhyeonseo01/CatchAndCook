#pragma once
//[wall] <-> [player][molgogi]
enum class GameObjectTag : uint64
{
	Defualt = 1 << 0,
	Player = 1 << 1,
	Monster = 1 << 2,
	Wall = 1 << 3,
	NPC = 1 << 4,

	CookType_Bake	= 1 << 10,
	CookType_Boil	= 1 << 11,
	CookType_Wash	= 1 << 12,
	CookType_Cut	= 1 << 13,
	CookType_Table	= 1 << 14,

	NonCulling = 1ull << 25,
	NonInstancing = 1ull << 26,

};

static unordered_map<string, GameObjectTag> GameObjectTagMap =
{
	{ "Defualt", GameObjectTag::Defualt },
	{ "Player", GameObjectTag::Player },
	{ "Monster", GameObjectTag::Monster },
	{ "NPC", GameObjectTag::NPC },
	{ "Wall", GameObjectTag::Wall },
	{ "NonCulling", GameObjectTag::NonCulling },
	{ "NonInstancing", GameObjectTag::NonInstancing },

	{ "CookType_Bake",		GameObjectTag::CookType_Bake},
	{ "CookType_Boil",		GameObjectTag::CookType_Boil},
	{ "CookType_Wash",		GameObjectTag::CookType_Wash},
	{ "CookType_Cut",			GameObjectTag::CookType_Cut	 },
	{ "CookType_Table",		GameObjectTag::CookType_Table }
};

inline GameObjectTag& operator|=(GameObjectTag& a, GameObjectTag b) {
	a = static_cast<GameObjectTag>(
		static_cast<std::uint64_t>(a) | static_cast<std::uint64_t>(b));
	return a;
}

inline GameObjectTag operator&(GameObjectTag& a, GameObjectTag b) {
	return static_cast<GameObjectTag>(
		static_cast<uint64>(a) & static_cast<uint64>(b));
}

inline bool HasTag(GameObjectTag tag, GameObjectTag flag) {
	return (static_cast<uint64>(tag) & static_cast<uint64>(flag)) != 0;
}


enum class GameObjectType
{
	Dynamic = 0, // ����(�������� ����)
	Static, // ����(ù�����ӵ���, ���� ��������)
	Deactivate //��Ȱ��ȭ(����, ������ �Ѵ� X)
};