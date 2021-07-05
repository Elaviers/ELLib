#pragma once
#include "Material_Sprite.hpp"

class MaterialSpritesheet : public MaterialSprite
{
protected:
	Vector2 _frameSize;
	int _frameCount;
	float _frameRate;

	//UV scale of one frame
	Vector2 _frameUV;
	int _framesPerRow;
	void _UpdateFrameData();

	const Vector2& _GetFrameSize() const { return _frameSize; }
	void _SetFrameSize(const Vector2& frameSize) { _frameSize = frameSize; _UpdateFrameData(); }

	void _CMD_tex(const Array<String>& args, const Context& ctx)
	{
		MaterialSprite::_CMD_tex(args, ctx);
		_UpdateFrameData();
	}

public:
	MaterialSpritesheet(const SharedPointer<const Texture>& diffuse = SharedPointer<const Texture>()) : 
		MaterialSprite(diffuse), _frameCount(1), _frameRate(24.f), _framesPerRow(1) 
	{}
	virtual ~MaterialSpritesheet() {}

	virtual const PropertyCollection& GetProperties() override;

	virtual void Apply(RenderEntry&, const MaterialParam* param = nullptr) const override;
};
