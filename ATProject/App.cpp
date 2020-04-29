#include "App.h"
#include <memory>
#include "imgui.h"
#include "Box.h"
#include "MathHelperFunctions.h"
#include "Surface.h"
#include "Sheet.h"
#include "SkinnedBox.h"
#include <iostream>
#include "Prices.h"
#include "GDIPlusManager.h"

GDIPlusManager gdipm;

App::App() : wnd(1280,720,"ATProject")/*, pointLight(wnd.Gfx())*/
{
	cam.Reset();

	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 1280.0f / 720.0f, 0.5f, 3000.0f));

	levelLoader.Load(drawables, wnd.Gfx(), statsManager);
}

App::~App()
{}

int App::Go()
{
	while (true)
	{
		// process all messages pending, but to not block for new messages
		if (const auto ecode = Window::ProcessMessages())
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
		DoFrame();
	}
}

void App::DoFrame()
{
	if (levelLoader.hasLevelLoaded)
	{
		const auto dt = timer.Mark() * speed_factor;
		wnd.Gfx().BeginFrame(0.3f, 0.3f, 0.4f);
		wnd.Gfx().SetCamera(cam.GetMatrix());

		//pointLight.Bind(wnd.Gfx(), cam.GetMatrix());

		//W
		if (wnd.keyboard.KeyIsPressed(0x57))
		{
			cam.Translate({ 0.0f,1.0f * dt,0.0f });
		}
		//S
		if (wnd.keyboard.KeyIsPressed(0x53))
		{
			cam.Translate({ 0.0f,-1.0f * dt,0.0f });
		}
		//D
		if (wnd.keyboard.KeyIsPressed(0x44))
		{
			cam.Translate({ 1.0f * dt,0.0f,0.0f });
		}
		//A
		if (wnd.keyboard.KeyIsPressed(0x41))
		{
			cam.Translate({ -1.0f * dt,0.0f,0.0f });
		}

		wnd.Gfx().SetCamera(cam.GetMatrix());

		ImGuiIO& io = ImGui::GetIO();

		if (io.MouseWheel > 0.0f || io.MouseWheel < 0.0f)
		{
			cam.Translate({ 0.0f, 0.0f, io.MouseWheel * dt * mouseScrollSpeed });
		}
		
		currentRaycastTime += dt;
		if (currentRaycastTime > timeBetweenRaycasts)
		{
			FindMousePosWorld({ wnd.mouse.GetPosX(), wnd.mouse.GetPosY() }, { 1280, 720 });

			if (buildingBeingPlaced != nullptr)
			{
				buildingBeingPlaced->SetPos(currentMousePosWorld.x - 7.0f, buildingBeingPlaced->GetPos().y, currentMousePosWorld.z);

			}
			currentRaycastTime = 0.0f;
		}

		if (wnd.mouse.LeftIsPressed() && buildingBeingPlaced != nullptr)
		{
			buildingBeingPlaced = nullptr;
		}
		else if (wnd.mouse.LeftIsPressed() && buildingBeingPlaced == nullptr)
		{
			ClickCheck({ wnd.mouse.GetPosX(), wnd.mouse.GetPosY() }, { 1280, 720 });
		}

		if (wnd.mouse.RightIsPressed() && selectedBuilding != nullptr)
		{
			selectedBuilding = nullptr;
		}

		statsManager->Update(dt);

		DrawUI();

		for (auto& d : drawables)
		{
			d->Update(wnd.keyboard.KeyIsPressed(VK_SPACE) ? 0.0f : dt);
			d->Draw(wnd.Gfx());
		}

		for (auto& b : buildings)
		{
			if (!b->ShouldBeDeleted())
			{
				b->Update(dt);
				b->Draw(wnd.Gfx());
			}
		}

		if (selectedBuilding != nullptr)
		{
			if (selectedBuilding->ShouldBeDeleted())
			{
				selectedBuilding = nullptr;
			}
		}

		//pointLight.Draw(wnd.Gfx());
		//pointLight.SpawnControlWindow();

		// present
		wnd.Gfx().EndFrame();
	}
}

void App::DrawUI()
{
	statsManager->RenderUI();
	
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
	ImGui::SetNextWindowPos(ImVec2(1050, 275), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(350, 440), ImGuiCond_Always);

	if (ImGui::Begin("Purchase Options", NULL, flags))
	{
		ImGui::Text("Money Makers");
		RenderSpecificSpawnableGUI("Shop", LevelLoader::Spawnables::SHOP, "Buy Shop - ");
		RenderSpecificSpawnableGUI("Bank", LevelLoader::Spawnables::BANK, "Buy Bank - ");

		ImGui::Text("Homelessness Reducers");

		RenderSpecificSpawnableGUI("House", LevelLoader::Spawnables::HOUSE, "Buy House - ");
		RenderSpecificSpawnableGUI("Appartment", LevelLoader::Spawnables::APPARTMENT, "Buy Appartment - ");


		ImGui::Text("Healthcare");
		RenderSpecificSpawnableGUI("Hospital", LevelLoader::Spawnables::HOSPITAL, "Buy Hospital - ");

	}

	ImGui::End();

	ImGuiWindowFlags simFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
	ImGui::SetNextWindowPos(ImVec2(0, 650), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(350, 80), ImGuiCond_Always);
	if (ImGui::Begin("Simulation Speed", NULL, flags))
	{
		ImGui::SliderFloat("Speed Factor", &speed_factor, 0.0f, 6.0f, "%.4f", 3.2f);
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

	ImGui::End();

	/*if (ImGui::Begin("Mouse Pos"))
	{
		ImGui::InputFloat("Mouse X", &currentMousePosWorld.x, 1.0f, 5.0f);
		ImGui::InputFloat("Mouse Y", &currentMousePosWorld.z, 1.0f, 5.0f);
	}*/


	if (selectedBuilding != nullptr)
	{
		selectedBuilding->RenderUI();
	}
}

void App::RenderSpecificSpawnableGUI(std::string identifier, LevelLoader::Spawnables spawnable, std::string name)
{
	for (int i = 0; i < 3; i++)
	{
		int level = i + 1;
		int price = Prices[identifier + " " + std::to_string(level)];
		std::string ButtonText = name + std::to_string(price);
		if (ImGui::Button(ButtonText.c_str()))
		{
			if (buildingBeingPlaced == nullptr && statsManager->CanAfford(price))
			{
				buildingBeingPlaced = levelLoader.SpawnObject(spawnable, currentMousePosWorld, wnd.Gfx(), buildings, statsManager, identifier, level, price);
			}
		}
	}
}

void App::FindMousePosWorld(DirectX::XMINT2 sp, DirectX::XMINT2 screenDims)
{
	DirectX::XMMATRIX camM = cam.GetMatrix();
	DirectX::XMFLOAT4X4 camMatrix;
	DirectX::XMStoreFloat4x4(&camMatrix, camM);

	DirectX::XMMATRIX camI = DirectX::XMMatrixInverse(NULL, camM);
	DirectX::XMFLOAT4X4 camInverse;
	DirectX::XMStoreFloat4x4(&camInverse, camI);

	//Get position of click on screen
	float px = ((2.0f * (float)sp.x) / (float)screenDims.x) - 1.0f;
	float py = (((2.0f * (float)sp.y) / (float)screenDims.y) - 1.0f) * -1.0f;

	DirectX::XMMATRIX projM = wnd.Gfx().GetProjection();
	DirectX::XMFLOAT4X4 projMatrix;
	DirectX::XMStoreFloat4x4(&projMatrix, projM);

	px = px / projMatrix._11;
	py = py / projMatrix._22;

	//Get direction ray will go
	DirectX::XMFLOAT3 direction;
	direction.x = (px * camInverse._11) + (py * camInverse._21) + camInverse._31;
	direction.y = (px * camInverse._12) + (py * camInverse._22) + camInverse._32;
	direction.z = (px * camInverse._13) + (py * camInverse._23) + camInverse._33;

	DirectX::XMFLOAT3 vRayOrig;

	//Working ray origin
	vRayOrig = cam.GetPos();

	DirectX::XMVECTOR rayorigin;
	rayorigin = DirectX::XMLoadFloat3(&vRayOrig);

	DirectX::XMVECTOR raydirection;
	raydirection = DirectX::XMLoadFloat3(&direction);

	//Testing variables
	/*DirectX::XMFLOAT3 benOrigin;
	DirectX::XMStoreFloat3(&benOrigin, rayori);

	DirectX::XMFLOAT3 benDirect;
	DirectX::XMStoreFloat3(&benDirect, raydirection);*/

	int trueCount = 0;
	for (auto& drawable : drawables)
	{
		if (drawable->canBeClicked)
		{
			if (drawable->CheckRayIntersection(rayorigin, raydirection))
			{
				trueCount = trueCount + 1;
				currentMousePosWorld = drawable->GetPos();
			}
		}
	}
}

void App::ClickCheck(DirectX::XMINT2 sp, DirectX::XMINT2 screenDims)
{
	DirectX::XMMATRIX camM = cam.GetMatrix();
	DirectX::XMFLOAT4X4 camMatrix;
	DirectX::XMStoreFloat4x4(&camMatrix, camM);

	DirectX::XMMATRIX camI = DirectX::XMMatrixInverse(NULL, camM);
	DirectX::XMFLOAT4X4 camInverse;
	DirectX::XMStoreFloat4x4(&camInverse, camI);

	//Get position of click on screen
	float px = ((2.0f * (float)sp.x) / (float)screenDims.x) - 1.0f;
	float py = (((2.0f * (float)sp.y) / (float)screenDims.y) - 1.0f) * -1.0f;

	DirectX::XMMATRIX projM = wnd.Gfx().GetProjection();
	DirectX::XMFLOAT4X4 projMatrix;
	DirectX::XMStoreFloat4x4(&projMatrix, projM);

	px = px / projMatrix._11;
	py = py / projMatrix._22;

	//Get direction ray will go
	DirectX::XMFLOAT3 direction;
	direction.x = (px * camInverse._11) + (py * camInverse._21) + camInverse._31;
	direction.y = (px * camInverse._12) + (py * camInverse._22) + camInverse._32;
	direction.z = (px * camInverse._13) + (py * camInverse._23) + camInverse._33;

	DirectX::XMFLOAT3 vRayOrig;

	//Working ray origin
	vRayOrig = cam.GetPos();

	DirectX::XMVECTOR rayorigin;
	rayorigin = DirectX::XMLoadFloat3(&vRayOrig);

	DirectX::XMVECTOR raydirection;
	raydirection = DirectX::XMLoadFloat3(&direction);

	//Testing variables
	/*DirectX::XMFLOAT3 benOrigin;
	DirectX::XMStoreFloat3(&benOrigin, rayori);

	DirectX::XMFLOAT3 benDirect;
	DirectX::XMStoreFloat3(&benDirect, raydirection);*/

	int trueCount = 0;
	for (auto& drawable : buildings)
	{
		if (drawable->canBeClicked)
		{
			if (drawable->CheckRayIntersection(rayorigin, raydirection))
			{
				trueCount = trueCount + 1;
				selectedBuilding = drawable.get();
			}
		}
	}
}