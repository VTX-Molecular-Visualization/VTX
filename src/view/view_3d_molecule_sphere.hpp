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
			enum ATTRIBUTE_LOCATION
			{
				ATOM_POSITION = 0,
				ATOM_COLOR	  = 1,
				ATOM_RADIUS	  = 2
			};

			View3DMoleculeSphere( const float p_radiusScale ) : _radiusScale( p_radiusScale ) {}
			~View3DMoleculeSphere() = default;

			virtual void		bind() const override { glBindVertexArray( _model->getVAO() ); }
			virtual void		unbind() const override { glBindVertexArray( 0 ); }
			virtual void		draw() override;
			virtual void		setupShaders( Shader::GLSLProgramManager & ) override;
			virtual void		useShaders( Shader::GLSLProgramManager & ) override;
			virtual std::string getNameStr() const override
			{
				return std::string( magic_enum::enum_name( VIEW_3D_MOLECULE::SPHERE ) );
			};

		  protected:
			virtual void _prepare() override;

		  private:
			float _radiusScale;

			// Uniforms.
			GLint _uRadiusScale = GL_INVALID_INDEX;
		};
	} // namespace View
} // namespace VTX

#endif;
