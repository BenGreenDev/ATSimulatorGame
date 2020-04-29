#pragma once
#include "DrawableBase.h"
#include "Cube.h"

class WaterTile : public DrawableBase<WaterTile>
{
public:
	WaterTile(Graphics& gfx,
		std::string fileName);

	void SetPos(int _xPos, int _yPos, int _zPos) noexcept override;
	void Update(float dt) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void SetScaling(float xScaling, float yScaling, float zScaling);
	virtual DirectX::XMFLOAT3 GetPos() override;

	virtual bool CheckRayIntersection(DirectX::XMVECTOR rayOrigin, DirectX::XMVECTOR rayDirection) override;
	virtual void RenderUI() override;

private:
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;

		struct {
			float u;
			float v;
		}tex;
	};

	IndexedTriangleList<Vertex> model = Cube::MakeSkinned<Vertex>();

	float xPos = 0.0f;
	float yPos = 0.0f;
	float zPos = 0.0f;

	float xScale = 0.0f;
	float yScale = 0.0f;
	float zScale = 0.0f;

	float r;
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float theta;
	float phi;
	float chi;

	float droll;
	float dpitch;
	float dyaw;
	float dtheta;
	float dphi;
	float dchi;

	// model transform
	DirectX::XMFLOAT3X3 mt;
};