#include "Panel.hpp"
#include <ELGraphics/Material_Grid.hpp>
#include <ELGraphics/MaterialParam.hpp>
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderQueue.hpp>

void UIPanel::Render(RenderQueue& q) const
{
	RenderEntry& e = q.CreateEntry(ERenderChannels::UNLIT);
	_colour.Apply(e);

	if (!_material)
	{
		e.AddSetUVScale();
		e.AddSetUVOffset();
		e.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);
		e.AddSetTransform(_transforms[4].GetMatrix());
		e.AddCommand(RCMDRenderMesh::PLANE);
		return;
	}

	const MaterialGrid* grid = dynamic_cast<const MaterialGrid*>(_material.Ptr());
	if (grid)
	{
		MaterialParam param;
		param.type = MaterialParam::EType::GRID_PARAM;

		int rc = grid->GetRowCount();
		int cc = grid->GetColumnCount();

		if (cc >= 3 && rc == 1)
		{
			float h = _transforms[0].GetScale().y + _transforms[3].GetScale().y + _transforms[6].GetScale().y;

			param.gridData.uvScale = Vector2(1.f, 1.f);
			param.gridData.row = param.gridData.column = 0;
			_material->Apply(e, &param);
			Transform t = _transforms[3];
			t.SetScale(Vector3(t.GetScale().x, h, t.GetScale().z));
			e.AddSetTransform(t.GetMatrix());
			e.AddCommand(RCMDRenderMesh::PLANE);

			param.gridData.column = 2;
			_material->Apply(e, &param);
			t = _transforms[5];
			t.SetScale(Vector3(t.GetScale().x, h, t.GetScale().z));
			e.AddSetTransform(t.GetMatrix());
			e.AddCommand(RCMDRenderMesh::PLANE);

			param.gridData.uvScale = Vector2(_uvScaling.x, 1.f);
			param.gridData.column = 1;
			_material->Apply(e, &param);
			t = _transforms[4];
			t.SetScale(Vector3(t.GetScale().x, h, t.GetScale().z));
			e.AddSetTransform(t.GetMatrix());
			e.AddCommand(RCMDRenderMesh::PLANE);
		}
		else if (rc >= 3 && cc == 1)
		{
			float w = _transforms[0].GetScale().x + _transforms[1].GetScale().x + _transforms[2].GetScale().x;

			param.gridData.uvScale = Vector2(1.f, 1.f);
			param.gridData.row = param.gridData.column = 0;
			_material->Apply(e, &param);
			Transform t = _transforms[1];
			t.SetScale(Vector3(w, t.GetScale().y, t.GetScale().z));
			e.AddSetTransform(t.GetMatrix());
			e.AddCommand(RCMDRenderMesh::PLANE);

			param.gridData.row = 2;
			_material->Apply(e, &param);
			t = _transforms[7];
			t.SetScale(Vector3(w, t.GetScale().y, t.GetScale().z));
			e.AddSetTransform(t.GetMatrix());
			e.AddCommand(RCMDRenderMesh::PLANE);

			param.gridData.uvScale = Vector2(1.f, _uvScaling.y);
			param.gridData.row = 1;
			_material->Apply(e, &param);
			t = _transforms[4];
			t.SetScale(Vector3(w, t.GetScale().y, t.GetScale().z));
			e.AddSetTransform(t.GetMatrix());
			e.AddCommand(RCMDRenderMesh::PLANE);
		}
		
		else if (rc >= 3 && cc >= 3)
		{
			//Full bordered panel
			//centre
			param.gridData.uvScale = Vector2(1.f, 1.f);
			param.gridData.row = param.gridData.column = 1;
			_material->Apply(e, &param);
			e.AddSetTransform(_transforms[4].GetMatrix());
			e.AddCommand(RCMDRenderMesh::PLANE);

			//Corners
			param.gridData.row = param.gridData.column = 0;
			_material->Apply(e, &param);
			e.AddSetTransform(_transforms[0].GetMatrix());
			e.AddCommand(RCMDRenderMesh::PLANE);

			param.gridData.column = 2;
			_material->Apply(e, &param);
			e.AddSetTransform(_transforms[2].GetMatrix());
			e.AddCommand(RCMDRenderMesh::PLANE);

			param.gridData.row = 2;
			param.gridData.column = 0;
			_material->Apply(e, &param);
			e.AddSetTransform(_transforms[6].GetMatrix());
			e.AddCommand(RCMDRenderMesh::PLANE);

			param.gridData.column = 2;
			_material->Apply(e, &param);
			e.AddSetTransform(_transforms[8].GetMatrix());
			e.AddCommand(RCMDRenderMesh::PLANE);

			//X edges
			param.gridData.uvScale = Vector2(_uvScaling.x, 1.f);

			param.gridData.row = 0;
			param.gridData.column = 1;
			_material->Apply(e, &param);
			e.AddSetTransform(_transforms[1].GetMatrix());
			e.AddCommand(RCMDRenderMesh::PLANE);

			param.gridData.row = 2;
			param.gridData.column = 1;
			_material->Apply(e, &param);
			e.AddSetTransform(_transforms[7].GetMatrix());
			e.AddCommand(RCMDRenderMesh::PLANE);

			//Y edges
			param.gridData.uvScale = Vector2(1.f, _uvScaling.y);

			param.gridData.row = 1;
			param.gridData.column = 0;
			_material->Apply(e, &param);
			e.AddSetTransform(_transforms[3].GetMatrix());
			e.AddCommand(RCMDRenderMesh::PLANE);

			param.gridData.column = 2;
			_material->Apply(e, &param);
			e.AddSetTransform(_transforms[5].GetMatrix());
			e.AddCommand(RCMDRenderMesh::PLANE);
		}
		else
		{
			Debug::Message(CSTR("Invalid row/column grid material setup for UIPanel (", rc, " rows, ", cc, " columns)"), "Go and fix this");
		}
	}
	else
	{
		e.AddSetUVScale();
		e.AddSetUVOffset();
		_material->Apply(e);
		e.AddSetTransform(_transforms[4].GetMatrix());
		e.AddCommand(RCMDRenderMesh::PLANE);
	}
}

void UIPanel::UpdateBounds()
{
	UIElement::UpdateBounds();

	float halfBordersize = (float)_borderSize / 2.f;
	float x0 = _absoluteBounds.x + halfBordersize;
	float y0 = _absoluteBounds.y + halfBordersize;
	float centreX = _absoluteBounds.x + _absoluteBounds.w / 2.f;
	float centreY = _absoluteBounds.y + _absoluteBounds.h / 2.f;
	float x2 = _absoluteBounds.x + _absoluteBounds.w - halfBordersize;
	float y2 = _absoluteBounds.y + _absoluteBounds.h - halfBordersize;

	float centreW = _absoluteBounds.w;
	float centreH = _absoluteBounds.h;
	if (_borderSize > 0.f)
	{
		centreW -= _borderSize * 2.f;
		centreH -= _borderSize * 2.f;
	}

	float absBorderSize = Maths::Abs(_borderSize);

	_transforms[0].SetScale(Vector3(absBorderSize, absBorderSize, 1.f));
	_transforms[2].SetScale(Vector3(absBorderSize, absBorderSize, 1.f));
	_transforms[6].SetScale(Vector3(absBorderSize, absBorderSize, 1.f));
	_transforms[8].SetScale(Vector3(absBorderSize, absBorderSize, 1.f));
	_transforms[1].SetScale(Vector3(centreW, absBorderSize, 1.f));
	_transforms[7].SetScale(Vector3(centreW, absBorderSize, 1.f));
	_transforms[3].SetScale(Vector3(absBorderSize, centreH, 1.f));
	_transforms[5].SetScale(Vector3(absBorderSize, centreH, 1.f));
	_transforms[4].SetScale(Vector3(centreW, centreH, 1.f));

	_transforms[0].SetPosition(Vector3(x0,		y2,			_z));
	_transforms[1].SetPosition(Vector3(centreX, y2,			_z));
	_transforms[2].SetPosition(Vector3(x2,		y2,			_z));
	_transforms[3].SetPosition(Vector3(x0,		centreY,	_z));
	_transforms[4].SetPosition(Vector3(centreX, centreY,	_z));
	_transforms[5].SetPosition(Vector3(x2,		centreY,	_z));
	_transforms[6].SetPosition(Vector3(x0,		y0,			_z));
	_transforms[7].SetPosition(Vector3(centreX, y0,			_z));
	_transforms[8].SetPosition(Vector3(x2,		y0,			_z));

	_uvScaling.x = centreW / absBorderSize;
	_uvScaling.y = centreH / absBorderSize;
}
