#pragma once
#include <string>


class StatsManager
{
public:
	StatsManager(float startingMoney, float _startingPopulation);

	bool CanAfford(int price);

	void Purchase(int purchaseAmount);
	void UseMoney(int useAmount);
	void MakeMoney(int money);

	//Housing
	void AddHomes(int amountOfHomes);
	void RemoveHomes(int amountOfHomes);

	//Jobs
	void AddJobs(int jobs);
	void RemoveJobs(int jobs);

	//Healthcare
	void CurePeople(float percentageToCure);
	int GetCurrentSick();

	void RenderUI();

	void Update(float dt);

private:
	//Population
	float startingPopulation = 0.0f;
	float currentPopulation = 0.0f;
	int currentHomeless = 0;
	int currentUnemployed = 0;
	int currentEmployed = 0;

	int currentHousing = 0;
	int currentJobs = 0;
	int currentSick = 0;
	
	float currentMoney = 0.0f;

	float populationGrowthMultiplier = 0.05f;
	
	float averageGDPIntakePerWorker = 0.5f;
	float averageGDPRemovalPerUnemployedCitizen = 0.35f;

	//Date
	float dayDuration = 24.0f;
	float currentTime = 0.0f;
	int currentDay = 1;

	float GDPRecalculationPeriod = 3.0f;
	float currentGDPTime = 0.0f;

private:
	std::string IntToTime();
};

