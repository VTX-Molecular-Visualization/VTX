#ifndef __VTX_VIEW_3D_MOLECULE_CYLINDER__
#define __VTX_VIEW_3D_MOLECULE_CYLINDER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_view_3d_molecule.hpp"

namespace VTX
{
	namespace View
	{
		class View3DMoleculeCylinder : public BaseView3DMolecule
		{
		  public:
			View3DMoleculeCylinder()  = default;
			~View3DMoleculeCylinder() = default;

			virtual VIEW_NAME getViewName() const override { return VIEW_NAME::D3_CYLINDER; };

			virtual void notify( Event::EVENT_MODEL ) override;

		  protected:
			virtual void _prepare() override;
			virtual void _draw() override;

		  private:
			// Uniforms.
			GLint _uRadius = GL_INVALID_INDEX;
		};
	} // namespace View
} // namespace VTX

#endif;
