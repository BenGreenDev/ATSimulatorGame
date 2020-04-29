//#include "GenericModel.h"
//#include "GenericOBJ.h"
//#include "Texture.h"
//#include "Surface.h"
//#include "BindableBase.h"
//#include "Sampler.h"
//
//GenericModel::GenericModel(Graphics& gfx, std::mt19937& rng,
//	std::uniform_real_distribution<float>& adist,
//	std::uniform_real_distribution<float>& ddist,
//	std::uniform_real_distribution<float>& odist,
//	std::uniform_real_distribution<float>& rdist,
//	DirectX::XMFLOAT3 material,
//	std::string fileName)
//	:
//	TestObject(gfx, rng, adist, ddist, odist, rdist)
//{
//	namespace dx = DirectX;
//
//	if (!IsStaticInitialised())
//	{
//
//		auto model = GenericOBJ::MakeOBJ<Vertex>(fileName.c_str(), false);
//		//model.SetNormalsIndependentFlat();
//
//		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));
//		/*AddStaticBind(std::make_unique<Texture>(gfx, Surface::FromFile("./Images/cube.tga")));
//		AddStaticBind(std::make_unique<Sampler>(gfx));*/
//
//		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));
//
//		auto pvs = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
//		auto pvsbc = pvs->GetBytecode();
//		AddStaticBind(std::move(pvs));
//
//		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));
//
//		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
//		{
//			{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
//			//{ "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0 },
//		};
//
//		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));
//		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
//	}
//	else
//	{
//		SetIndexFromStatic();
//	}
//
//	AddBind(std::make_unique<TransformCbuf>(gfx, *this));
//
//	struct PSMaterialConstant
//	{
//		DirectX::XMFLOAT3 color;
//		float specularIntensity = 0.6f;
//		float specularPower = 30.0f;
//		float padding[3];
//	} pmc;
//	pmc.color = material;
//	AddStaticBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));
//}
//
//
//void GenericModel::SetPos(int _xPos, int _yPos, int _zPos) noexcept
//{
//}
