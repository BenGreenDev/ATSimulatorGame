#pragma once
#include "Window.h"
#include "StatsManager.h"

enum TileTypes
{
	DIRT = 0,
	GRASS = 1,
	WATER = 2,
	BUILDING = 3
};

struct Tile
{
	TileTypes TileType = TileTypes::DIRT;
	int posX = 0;
	int posY = 0;
	int posz = 0;

};

const int levelWidth = 40;
const int levelHeight = 40;

class LevelLoader
{
public:
	enum Spawnables
	{
		BANK,
		HOUSE,
		SHOP,
		APPARTMENT,
		RESTURANT,
		HOSPITAL,
	};

	bool Load(std::vector<std::unique_ptr<class Drawable>>& drawables, Graphics& gfx, std::shared_ptr<StatsManager> _statsManager) noexcept;

	bool hasLevelLoaded = false;

	void RenderUI();

	Drawable* SpawnObject(Spawnables spawnable, DirectX::XMFLOAT3 position, Graphics& graphics, std::vector<std::unique_ptr<class Drawable>>& drawables, std::shared_ptr<StatsManager> statsManager, std::string name, int level, int price) noexcept;

private:
	std::shared_ptr<StatsManager> statsManager;
	int levelStartXPos = -10;
	int levelEndXPos = 10;

	int levelStartYPos = -10;
	int levelEndYPos = 10;
	float chanceToStartAlive = 0.4f;
	int numberOfSimulationSteps = 4;
	int deathLimit = 3;
	int birthLimit = 4;

	bool cellmap[levelWidth][levelHeight] = { {false} };;

	void InitialiseMap();
	void DoSimulationStep();
	int CountAliveNeighbours(int x, int y);
};

