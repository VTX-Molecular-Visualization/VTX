#ifndef __VTX_VIEW_3D_MOLECULE_SPHERE__
#define __VTX_VIEW_3D_MOLECULE_SPHERE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_view_3d_molecule.hpp"

namespace VTX
{
	namespace View
	{
		class View3DMoleculeSphere : public BaseView3DMolecule
		{
		  public:
			View3DMoleculeSphere()	= default;
			~View3DMoleculeSphere() = default;

			virtual std::string getName() const override { return ID::View::D3_SPHERE; };
			virtual void		initItem() override;
			virtual void		notify( Event::EVENT_MODEL ) override;

		  protected:
			virtual void _draw() override;

		  private:
			float _radiusScale = 0.0f;

			// Uniforms.
			GLint _uRadiusScale = GL_INVALID_INDEX;
		};
	} // namespace View
} // namespace VTX

#endif;
