#pragma once
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "LevelLoader.h"
#include "PointLight.h"
#include "StatsManager.h"

class App
{
public:
	App();
	int Go();
	~App();
private:
	void DoFrame();
	void DrawUI();

	void RenderSpecificSpawnableGUI(std::string identifier, LevelLoader::Spawnables spawnable, std::string name);

	void FindMousePosWorld(DirectX::XMINT2 sp, DirectX::XMINT2 screenDims);

	void ClickCheck(DirectX::XMINT2 sp, DirectX::XMINT2 screenDims);

private:
	ImguiManager imgui;
	Window wnd;
	Timer timer;
	float speed_factor = 1.0f;
	std::vector<std::unique_ptr<class Drawable>> drawables;
	std::vector<std::unique_ptr<Drawable>> buildings;

	static constexpr size_t nDrawables = 20;
	const float PI = 3.14159265f;
	Camera cam{ {0.0f,80.0f,0.0f},1.55334f,PI / 2.0f };
	//PointLight pointLight;
	LevelLoader levelLoader;
	int mouseScrollSpeed = 2.0f;

	float startingPopulation = 100.0f;
	float startingMoney = 10000.0f;

	std::shared_ptr<StatsManager> statsManager = std::make_shared<StatsManager>(startingMoney, startingPopulation);

	DirectX::XMFLOAT3 currentMousePosWorld;

	float timeBetweenRaycasts = 1.0f;
	float currentRaycastTime = 0.0f;

	Drawable* buildingBeingPlaced = nullptr;
	Drawable* selectedBuilding = nullptr;

};

