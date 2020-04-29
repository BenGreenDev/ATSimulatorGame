#include "WaterTile.h"
#include "Texture.h"
#include "Surface.h"
#include "BindableBase.h"
#include "Sampler.h"
#include "GDIPlusManager.h"

WaterTile::WaterTile(Graphics& gfx,
	std::string fileName)
{
	namespace dx = DirectX;

	if (!IsStaticInitialised())
	{
		AddStaticBind(std::make_unique<Texture>(gfx, Surface::FromFile("Images\\" + fileName)));
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));
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
void WaterTile::SetPos(int _xPos, int _yPos, int _zPos) noexcept
{
	xPos = _xPos;
	yPos = _yPos;
	zPos = _zPos;

	DirectX::XMStoreFloat3x3(&mt, DirectX::XMMatrixTranslation(xPos, yPos, zPos));
}
void WaterTile::Update(float dt) noexcept
{

}
DirectX::XMMATRIX WaterTile::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return dx::XMLoadFloat3x3(&mt) *
		dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		dx::XMMatrixTranslation(xPos, yPos, zPos) *
		dx::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}
void WaterTile::SetScaling(float xScaling, float yScaling, float zScaling)
{
	xScale = xScaling;
	yScale = yScaling;
	zScale = zScaling;

	DirectX::XMStoreFloat3x3(
		&mt,
		DirectX::XMMatrixScaling(xScaling, yScaling, zScaling));
}
DirectX::XMFLOAT3 WaterTile::GetPos()
{
	DirectX::XMFLOAT3 temp{ xPos, yPos, zPos };
	return temp;
};


bool WaterTile::CheckRayIntersection(DirectX::XMVECTOR rayOrigin, DirectX::XMVECTOR rayDirection)
{
	for (int i = 0; i < model.indices.size() / 3; ++i)
	{
		DirectX::XMVECTOR v0 = DirectX::XMVector3Transform(DirectX::XMVECTOR{ model.vertices[model.indices[i]].pos.x,
																			  model.vertices[model.indices[i]].pos.y,
																			  model.vertices[model.indices[i]].pos.z }, GetTransformXM());
		DirectX::XMVECTOR v1 = DirectX::XMVector3Transform(DirectX::XMVECTOR{ model.vertices[model.indices[i + 1]].pos.x,
																			  model.vertices[model.indices[i + 1]].pos.y,
																			  model.vertices[model.indices[i + 1]].pos.z }, GetTransformXM());
		DirectX::XMVECTOR v2 = DirectX::XMVector3Transform(DirectX::XMVECTOR{ model.vertices[model.indices[i + 2]].pos.x,
																			  model.vertices[model.indices[i + 2]].pos.y,
																			  model.vertices[model.indices[i + 2]].pos.z }, GetTransformXM());

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

void WaterTile::RenderUI()
{
}
