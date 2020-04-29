#pragma once
#include "DrawableBase.h"
#include "GenericOBJ.h"
#include "StatsManager.h"

class Hospital : public DrawableBase<Hospital>
{
public:
	Hospital(Graphics& gfx);

	void init(std::string name, int level, int price);
	void SetPos(int _xPos, int _yPos, int _zPos) noexcept override;
	void SetScaling(float xScaling, float yScaling, float zScaling);
	void SetRotation(float rotation);

	virtual void Update(float dt) noexcept override;

	DirectX::XMMATRIX GetTransformXM() const noexcept;

	bool CheckRayIntersection(DirectX::XMVECTOR rayOrigin, DirectX::XMVECTOR rayDirection) override;

	void RenderUI() override;

	virtual DirectX::XMFLOAT3 GetPos() override;

	std::shared_ptr<StatsManager> statsManager;
private:
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;

		struct {
			float u;
			float v;
		}tex;
	};

	float xPos = 0.0f;
	float yPos = 0.0f;
	float zPos = 0.0f;

	float xScale = 0.0f;
	float yScale = 0.0f;
	float zScale = 0.0f;

	float not_Radians_Rotation = 0.0f;

	// positional
	float r;
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float theta;
	float phi;
	float chi;

	// speed (delta/s)
	float droll;
	float dpitch;
	float dyaw;
	float dtheta;
	float dphi;
	float dchi;

	IndexedTriangleList<Vertex> model = GenericOBJ::MakeOBJ<Vertex>("./Models/Hospital/hospital.obj", false);

	// model transform
	DirectX::XMFLOAT3X3 mt;

private:
	std::string buildingName;
	int level = 1;
	int price = 0;

	int timePerUpdate = 3.0f;
	float currentTime = 0.0f;
	int pricePerUpdate = 35.0f;
	int baseNumberOfJobs = 10;
};