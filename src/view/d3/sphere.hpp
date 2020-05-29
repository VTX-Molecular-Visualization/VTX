#ifndef __VTX_VIEW_3D_MOLECULE_SPHERE__
#define __VTX_VIEW_3D_MOLECULE_SPHERE__

#ifdef _MSC_VER
#pragma once
#endif

#include "view/base_view_3d_molecule.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			class Sphere : public BaseView3DMolecule
			{
			  public:
				explicit Sphere( Model::Molecule * const p_model ) : BaseView3DMolecule( p_model ) {}

				virtual const std::string & getName() const override { return ID::View::D3_SPHERE; };
				virtual void				init() override;
				virtual void				notify( const Event::VTX_EVENT_MODEL & ) override;

				virtual void render() override;

			  private:
				float _radiusFixed	 = 1.f;
				float _radiusAdd	 = 0.f;
				bool  _isRadiusFixed = false;

				// Uniforms.
				GLint _uIsRadiusFixedLoc = GL_INVALID_INDEX;
				GLint _uRadiusAddLoc	 = GL_INVALID_INDEX;
				GLint _uRadiusFixedLoc	 = GL_INVALID_INDEX;
				GLint _uZNearLoc		 = GL_INVALID_INDEX;
				GLint _uZFarLoc			 = GL_INVALID_INDEX;
			};
		} // namespace D3
	}	  // namespace View
} // namespace VTX

#endif;
