#include "LevelLoader.h"
#include "SkinnedBox.h"
#include "MathHelperFunctions.h"
#include <iostream>
#include <fstream>
#include <string>
#include "Texture.h"
#include "Surface.h"
#include "GenericModel.h"
#include <ctime>
#include "imgui.h"
#include "Hospital.h"
#include "WaterTile.h"
#include "Mountain.h"
#include "Appartment.h"
#include "House.h"
#include "Shop.h"
#include "bank.h"

bool LevelLoader::Load(std::vector<std::unique_ptr<class Drawable>>& drawables, Graphics& graphics, std::shared_ptr<StatsManager> _statsManager) noexcept
{

	//drawables.emplace_back(std::make_unique<SkinnedBox>(graphics, "cube.tga"));
	statsManager = _statsManager;

	InitialiseMap();

	for (int i = 0; i < numberOfSimulationSteps; i++)
	{
		DoSimulationStep();
	}

	for (int x = 0; x < levelHeight; x++)
	{
		for (int y = 0; y < levelWidth; y++)
		{
			if (cellmap[x][y] == true)
			{
				//Spawn water
				std::unique_ptr newTile = std::make_unique<WaterTile>(graphics, "water.png");
				newTile->SetPos(x * 40.0f, -90.0f, y * 20.0f);
				newTile->SetScaling(40.0f, 3.0f, 20.0f);
				drawables.emplace_back(std::move(newTile));

			}
			else
			{
				std::unique_ptr newTile = std::make_unique<SkinnedBox>(graphics, "grass2.png");
				newTile->SetPos(x * 40.0f, -90.0f, y * 20.0f);
				newTile->SetScaling(40.0f, 3.0f, 20.0f);
				newTile->canBeClicked = true;
				drawables.emplace_back(std::move(newTile));

			}
		}
	}
	hasLevelLoaded = true;
	return true;
}

void LevelLoader::RenderUI()
{

}

Drawable* LevelLoader::SpawnObject(Spawnables spawnable, DirectX::XMFLOAT3 position, Graphics& graphics, std::vector<std::unique_ptr<class Drawable>>& drawables, std::shared_ptr<StatsManager> statsManager, std::string name, int level, int price) noexcept
{
	Drawable* newBuilding = nullptr;

	statsManager->Purchase(price);

	if (spawnable == Spawnables::HOSPITAL)
	{
		std::unique_ptr newHospital = std::make_unique<Hospital>(graphics);
		newHospital->statsManager = statsManager;
		newHospital->init(name, level, price);
		newHospital->SetPos(1.0f, -80.0f, 1.0f);
		newHospital->SetScaling(0.05f, 0.085f, 0.05f);
		newHospital->SetRotation((180 * std::_Pi) / 180);
		newBuilding = newHospital.get();
		drawables.emplace_back(std::move(newHospital));
	}
	else if (spawnable == Spawnables::APPARTMENT)
	{
		std::unique_ptr newAppartment = std::make_unique<Appartment>(graphics);
		newAppartment->statsManager = statsManager;
		newAppartment->init(name, level, price);
		newAppartment->SetPos(1.0f, -89.0f, 1.0f);
		newAppartment->SetScaling(2.0f, 2.0f, 4.0f);
		newAppartment->SetRotation((270 * std::_Pi) / 180);
		newBuilding = newAppartment.get();
		drawables.emplace_back(std::move(newAppartment));
	}
	else if (spawnable == Spawnables::HOUSE)
	{
		std::unique_ptr newHouse = std::make_unique<House>(graphics);
		newHouse->statsManager = statsManager;
		newHouse->init(name, level, price);
		newHouse->SetPos(1.0f, -89.0f, 1.0f);
		newHouse->SetScaling(0.5f, 1.0f, 1.0f);
		newHouse->SetRotation((270 * std::_Pi) / 180);
		newBuilding = newHouse.get();
		drawables.emplace_back(std::move(newHouse));
	}
	else if (spawnable == Spawnables::SHOP)
	{
		std::unique_ptr newShop = std::make_unique<Shop>(graphics);
		newShop->statsManager = statsManager;
		newShop->init(name, level, price);
		newShop->SetPos(1.0f, -86.0f, 1.0f);
		newShop->SetScaling(0.75f, 1.0f, 2.0f);
		newShop->SetRotation((270 * std::_Pi) / 180);
		newBuilding = newShop.get();
		drawables.emplace_back(std::move(newShop));
	}
	else if (spawnable == Spawnables::BANK)
	{
		std::unique_ptr newBank = std::make_unique<Bank>(graphics);
		newBank->statsManager = statsManager;
		newBank->init(name, level, price);
		newBank->SetPos(1.0f, -87.0f, 1.0f);
		newBank->SetScaling(0.1f, 0.06f, 0.045f);
		newBank->SetRotation((180 * std::_Pi) / 180);
		newBuilding = newBank.get();
		drawables.emplace_back(std::move(newBank));
	}
	

	return newBuilding;

}


void LevelLoader::InitialiseMap()
{
	srand(time(NULL));

	for (int x = 0; x < levelHeight; x++) {
		for (int y = 0; y < levelWidth; y++) {
			double r = ((double)rand() / (double)(RAND_MAX));
			if (r < chanceToStartAlive) {
				cellmap[x][y] = true;
			}
		}
	}
}

void LevelLoader::DoSimulationStep()
{
	bool newMap[levelWidth][levelHeight];

	for (int x = 0; x < levelHeight; x++) {
		for (int y = 0; y < levelWidth; y++) {
			int nbs = CountAliveNeighbours(x, y);
			if (cellmap[x][y]) {
				if (nbs < deathLimit) {
					cellmap[x][y] = false;
				}
				else {
					cellmap[x][y] = true;
				}
			}
			else {
				if (nbs > birthLimit) {
					cellmap[x][y] = true;
				}
				else {
					cellmap[x][y] = false;
				}
			}
		}
	}
}

int LevelLoader::CountAliveNeighbours(int x, int y)
{
	int count = 0;
	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			int neighbour_x = x + i;
			int neighbour_y = y + j;
			if (i == 0 && j == 0) {
			}
			else if (neighbour_x < 0 || neighbour_y < 0 || neighbour_x >= levelWidth * levelHeight || neighbour_y >= levelWidth) {
				count = count + 1;
			}
			else if (cellmap[neighbour_x][neighbour_y]) {
				count = count + 1;
			}
		}
	}

	return count;
}
