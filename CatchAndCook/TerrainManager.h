#pragma once

class Terrain;

class TerrainManager
{
public:
	static unique_ptr<TerrainManager> main;

public:
	void PushTerrain(shared_ptr<Terrain> terrain)
	{
		if (std::ranges::find(_terrains, terrain) == _terrains.end())
			_terrains.push_back(terrain);
	}
	void RemoveTerrain(shared_ptr<Terrain> terrain)
	{
		auto it = std::ranges::find(_terrains, terrain);
		if (it != _terrains.end())	
			_terrains.erase(it);
	}

	vector<shared_ptr<Terrain>> & GetTerrains()
	{
		return _terrains;
	}

	vector<shared_ptr<Terrain>> _terrains;

	void CullingInstancing(Vector3 pos, Vector3 look);

private:

};

