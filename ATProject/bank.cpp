#include "bank.h"
#include "BindableBase.h"
#include "imgui.h"
#include "GenericOBJ.h"
#include "Surface.h"
#include "Texture.h"
#include "Sampler.h"
#include "Prices.h"

Bank::Bank(Graphics& gfx)
{
	if (!IsStaticInitialised())
	{
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));
		AddStaticBind(std::make_unique<Texture>(gfx, Surface::FromFile("Images\\bank.png")));
		AddStaticBind(std::make_unique<Sampler>(gfx));

		auto pvs = std::make_unique<VertexShader>(gfx, L"TextureVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"TexturePS.cso"));
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};

		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<TransformCbuf>(gfx, *this));

	// model deformation transform (per instance, not stored as bind)
	DirectX::XMStoreFloat3x3(
		&mt,
		DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f)
		);
}

void Bank::init(std::string name, int _level, int _price)
{
	canBeClicked = true;
	buildingName = name;
	level = _level;
	price = _price;
	statsManager->AddJobs(numberOfPeopleEmployed);
}

void Bank::SetPos(int _xPos, int _yPos, int _zPos) noexcept
{
	xPos = _xPos;
	yPos = _yPos;
	zPos = _zPos;

	DirectX::XMStoreFloat3x3(&mt, DirectX::XMMatrixTranslation(xPos, yPos, zPos));
	DirectX::XMStoreFloat3x3(
		&mt,
		DirectX::XMMatrixScaling(xScale, yScale, zScale));
}

void Bank::SetScaling(float xScaling, float yScaling, float zScaling)
{
	xScale = xScaling;
	yScale = yScaling;
	zScale = zScaling;

	DirectX::XMStoreFloat3x3(
		&mt,
		DirectX::XMMatrixScaling(xScaling, yScaling, zScaling));
}

void Bank::SetRotation(float rotation)
{
	yaw = rotation;
	not_Radians_Rotation = yaw * (180.0 / std::_Pi);

	DirectX::XMStoreFloat3x3(
		&mt,
		DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll));
}


void Bank::Update(float dt) noexcept
{
	currentTime += dt;

	if (currentTime > timePerUpdate)
	{
		statsManager->MakeMoney(incomePerUpdate * level);
		currentTime = 0.0f;
	}
}

DirectX::XMMATRIX Bank::GetTransformXM() const noexcept
{
	{
		namespace dx = DirectX;
		return dx::XMLoadFloat3x3(&mt) *
			dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
			dx::XMMatrixTranslation(xPos, yPos, zPos) *
			dx::XMMatrixRotationRollPitchYaw(theta, phi, chi);
	}
}

bool Bank::CheckRayIntersection(DirectX::XMVECTOR rayOrigin, DirectX::XMVECTOR rayDirection)
{
	for (int i = 0; i < model.indices.size() / 3; ++i)
	{
		DirectX::XMVECTOR v0 = DirectX::XMVector3Transform(DirectX::XMVECTOR{ model.vertices[model.indices[i]].pos.x,  model.vertices[model.indices[i]].pos.y, model.vertices[model.indices[i]].pos.z }, GetTransformXM());
		DirectX::XMVECTOR v1 = DirectX::XMVector3Transform(DirectX::XMVECTOR{ model.vertices[model.indices[i + 1]].pos.x,  model.vertices[model.indices[i + 1]].pos.y, model.vertices[model.indices[i + 1]].pos.z }, GetTransformXM());
		DirectX::XMVECTOR v2 = DirectX::XMVector3Transform(DirectX::XMVECTOR{ model.vertices[model.indices[i + 2]].pos.x,  model.vertices[model.indices[i + 2]].pos.y, model.vertices[model.indices[i + 2]].pos.z }, GetTransformXM());

		DirectX::XMVECTOR v0v1 = DirectX::XMVectorSubtract(v1, v0);
		DirectX::XMVECTOR v0v2 = DirectX::XMVectorSubtract(v2, v0);

		DirectX::XMVECTOR pvec = DirectX::XMVector3Cross(rayDirection, v0v2);
		float det = DirectX::XMVectorGetX(DirectX::XMVector3Dot(v0v1, pvec));

		if (fabs(det) < DBL_EPSILON)
		{
			continue;
		}

		float invDet = 1 / det;

		DirectX::XMVECTOR tvec = DirectX::XMVectorSubtract(rayOrigin, v0);

		float u = DirectX::XMVectorGetX(DirectX::XMVectorScale(DirectX::XMVector3Dot(tvec, pvec), invDet));

		if (u < 0 || u > 1)
		{
			continue;
		}

		DirectX::XMVECTOR qvec = DirectX::XMVector3Cross(tvec, v0v1);

		float v = DirectX::XMVectorGetX(DirectX::XMVectorScale(DirectX::XMVector3Dot(rayDirection, qvec), invDet));

		if (v < 0 || u + v > 1)
		{
			continue;
		}

		float t = DirectX::XMVectorGetX(DirectX::XMVectorScale(DirectX::XMVector3Dot(v0v2, qvec), invDet));

		return true;
	}

	return false;

}

void Bank::RenderUI()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(350, 180), ImGuiCond_Always);

	if (ImGui::Begin(buildingName.c_str()))
	{
		std::string UIlevel = "Current Level : " + std::to_string(level);
		ImGui::Text(UIlevel.c_str());
		ImGui::Text("Updgrade");
		if (level != 3)
		{
			int newPrice = Prices[buildingName + " " + std::to_string(level + 1)];
			newPrice = newPrice - price;

			if (ImGui::Button(std::to_string(newPrice).c_str()))
			{
				level = level + 1;
				price = newPrice;
				statsManager->UseMoney(newPrice);
			}
		}
		else
		{
			ImGui::Text("Max Level");
		}


		ImGui::Separator();

		std::string pricePerUp = "Current Income Per Update - " + std::to_string(incomePerUpdate * level);
		ImGui::Text(pricePerUp.c_str());

		ImGui::Separator();
		std::string sellButton = "Sell - " + std::to_string(price / 2);
		ImGui::Text(sellButton.c_str());
		if (ImGui::Button("Sell"))
		{
			statsManager->RemoveJobs(numberOfPeopleEmployed);
			statsManager->MakeMoney(price / 2);
			SetToBeDeleted = true;
		}
	}

	ImGui::End();
}

DirectX::XMFLOAT3 Bank::GetPos()
{
	DirectX::XMFLOAT3 temp{ xPos, yPos, zPos };
	return temp;
}
