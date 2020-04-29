#pragma once
#include "Graphics.h"
#include <DirectXMath.h>

class Bindable;

class Drawable
{
	template<class T>
	friend class DrawableBase;

public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw(Graphics& gfx) const noexcept;
	virtual void Update(float dt) noexcept = 0;
	virtual ~Drawable() = default;
	virtual void SetPos(int _xPos, int _yPos, int _zPos) = 0;
	virtual DirectX::XMFLOAT3 GetPos() = 0;
	virtual bool CheckRayIntersection(DirectX::XMVECTOR rayOrigin, DirectX::XMVECTOR rayDirection) {
		return false;
	};
	virtual void RenderUI() = 0;
	bool canBeClicked = false;

	bool ShouldBeDeleted() {
		if (SetToBeDeleted)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
protected:
	void AddBind(std::unique_ptr<Bindable> bind) noexcept;
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept;
	bool SetToBeDeleted = false;
private:
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept = 0;

private:
	const class IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bindable>> binds;
};