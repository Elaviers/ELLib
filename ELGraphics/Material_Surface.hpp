#pragma once
#include "Material.hpp"
#include "Texture.hpp"
#include <ELCore/SharedPointer.hpp>

class MaterialSurface : public Material
{
private:
	SharedPointer<const Texture> _diffuse, _normal, _specular, _reflection;
	
	void CMD_tex(const Buffer<String>& args, const Context&);
public:
	MaterialSurface(
		const SharedPointer<const Texture>& diffuse = SharedPointer<const Texture>(),
		const SharedPointer<const Texture>& normal = SharedPointer<const Texture>(),
		const SharedPointer<const Texture>& specular = SharedPointer<const Texture>(),
		const SharedPointer<const Texture>& reflection = SharedPointer<const Texture>());
	
	virtual ~MaterialSurface() {}

	virtual const PropertyCollection& GetProperties() override;

	const SharedPointer<const Texture>& GetDiffuse() const { return _diffuse; }
	const SharedPointer<const Texture>& GetNormal() const { return _normal; }
	const SharedPointer<const Texture>& GetSpecular() const { return _specular; }
	const SharedPointer<const Texture>& GetReflection() const { return _reflection; }

	void SetDiffuse(const SharedPointer<const Texture>& texture) { _diffuse = texture; }
	void SetNormal(const SharedPointer<const Texture>& texture) { _normal = texture; }
	void SetSpecular(const SharedPointer<const Texture>& texture) { _specular = texture; }
	void SetReflection(const SharedPointer<const Texture>& texture) { _reflection = texture; }

	virtual void Apply(RenderEntry&, const MaterialParam* param = nullptr) const override;
	
	virtual bool SetDefaultTexture(const SharedPointer<const Texture>& texture) override { SetDiffuse(texture); return true; }
	virtual const SharedPointer<const Texture>& GetDefaultTexture() const override { return _diffuse; }

	bool operator==(const MaterialSurface &other) { return _diffuse == other._diffuse && _normal == other._normal && _specular == other._specular; }
};
