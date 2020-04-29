#pragma once
#include "DrawableBase.h"

class LightingSphere : public DrawableBase<LightingSphere>
{
public:
	LightingSphere(Graphics& gfx, float radius);
	void Update(float dt) noexcept override;
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void SetPos(int _xPos, int _yPos, int _zPos) noexcept override;
	virtual DirectX::XMFLOAT3 GetPos() override;
	virtual bool CheckRayIntersection(DirectX::XMVECTOR rayOrigin, DirectX::XMVECTOR rayDirection) override;
	virtual void RenderUI() override;
private:
	DirectX::XMFLOAT3 pos = { 1.0f,1.0f,1.0f };
};