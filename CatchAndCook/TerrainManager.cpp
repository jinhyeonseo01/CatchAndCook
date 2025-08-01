#include "pch.h"
#include "TerrainManager.h"

#include "Terrain.h"

unique_ptr<TerrainManager> TerrainManager::main;

void TerrainManager::CullingInstancing(Vector3 pos, Vector3 look)
{
	for (auto& terrain : _terrains)
		terrain->CullingInstancing(pos, look);
}
