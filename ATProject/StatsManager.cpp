#include "StatsManager.h"
#include <cmath>
#include "imgui.h"


StatsManager::StatsManager(float startingMoney, float _startingPopulation)
{
	startingPopulation = _startingPopulation;
	currentMoney = startingMoney;
	currentPopulation = startingPopulation;
}

bool StatsManager::CanAfford(int price)
{
	if (currentMoney > price)
	{
		return true;
	}
	else
	{
		return false;
	}
}


void StatsManager::Purchase(int purchaseAmount)
{
	currentMoney = currentMoney - purchaseAmount;
}

void StatsManager::UseMoney(int useAmount)
{
	currentMoney = currentMoney - useAmount;
}

void StatsManager::MakeMoney(int money)
{
	currentMoney += money;
}

void StatsManager::AddHomes(int amountOfHomes)
{
	currentHousing += amountOfHomes;
}

void StatsManager::RemoveHomes(int amountOfHomes)
{
	currentHousing = currentHousing - amountOfHomes;
}

void StatsManager::AddJobs(int jobs)
{
	currentJobs += jobs;
}

void StatsManager::RemoveJobs(int jobs)
{
	currentJobs = currentJobs - jobs;
}

void StatsManager::CurePeople(float percentageToCure)
{
	currentSick = currentSick - (currentSick * percentageToCure);
}

int StatsManager::GetCurrentSick()
{
	return currentSick;
}

void StatsManager::RenderUI()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
	ImGui::SetNextWindowPos(ImVec2(1050, 0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(350, 275), ImGuiCond_Always);

	if (ImGui::Begin("Stats", NULL, flags))
	{
		ImGui::Text("Population");
		std::string curPop = std::to_string((int)floor(currentPopulation));
		ImGui::Text(curPop.c_str());

		ImGui::Text("Number Unemployed");
		std::string curUnemployed = std::to_string((int)floor(currentUnemployed));
		ImGui::Text(curUnemployed.c_str());

		ImGui::Text("Number Homeless");
		std::string curHomeless = std::to_string((int)floor(currentHomeless));
		ImGui::Text(curHomeless.c_str());

		ImGui::Text("Number Sick");
		std::string curSick = std::to_string((int)floor(currentSick));
		ImGui::Text(curSick.c_str());

		ImGui::Separator();

		ImGui::Text("Current Money");
		std::string curMoney = std::to_string(currentMoney);
		ImGui::Text(curMoney.c_str());

		ImGui::Separator();

		ImGui::Text("Current Day");
		std::string curDay = std::to_string(currentDay);
		ImGui::Text(curDay.c_str());

		ImGui::Text("Current Time");
		std::string curTime = IntToTime();
		ImGui::Text(curTime.c_str());
	}

	ImGui::End();
}

void StatsManager::Update(float dt)
{
	//Update current population
	currentPopulation = currentPopulation*(std::pow(1 + populationGrowthMultiplier, dt / 25.0f));

	currentHomeless = (currentPopulation - currentHousing < 0) ? 0 : currentPopulation - currentHousing;

	currentUnemployed = (currentPopulation - currentJobs < 0) ? 0 : currentPopulation - currentJobs;
	currentEmployed = (currentPopulation - currentUnemployed < 0) ? 0 : currentPopulation - currentUnemployed;

	//Update once per day
	currentTime += dt;
	currentGDPTime += dt;


	double r = ((double)rand() / (double)(RAND_MAX));
	if (r > 0.95)
	{
		double percentageSick = ((double)rand() / (double)(RAND_MAX));

		if (percentageSick < 0.02)
		{
			if (currentSick > 1)
			{
				//Assuming an r0 of 2
				currentSick += ceil(currentSick * 0.2);
			}
			else
			{
				currentSick = 2;
			}
		}
	}

	if (currentGDPTime > GDPRecalculationPeriod)
	{
		currentMoney += (currentEmployed * averageGDPIntakePerWorker) - (currentUnemployed * averageGDPRemovalPerUnemployedCitizen);
		currentGDPTime = 0.0f;
	}

	if(currentTime > dayDuration)
	{
		currentTime = 0.0f;
		currentDay++;
	}
}

std::string StatsManager::IntToTime()
{
	int currTime = (int)currentTime;
	currTime = floor(currTime);

	if (currTime < 12) 
	{
		std::string t = std::to_string(currTime);
		return t + " AM";
	}
	else
	{
		std::string t = std::to_string(currTime);
		return t + " PM";
	}
}


