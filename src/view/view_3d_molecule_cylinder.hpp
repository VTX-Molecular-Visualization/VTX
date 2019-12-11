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

			virtual void bind() const override
			{
				glBindVertexArray( _bondsIBO );
				glBindVertexArray( _vao );
			}
			virtual void unbind() const override
			{
				glBindVertexArray( 0 );
				glBindVertexArray( 0 );
			}
			virtual void		draw() override;
			virtual void		setupShaders( Shader::GLSLProgramManager & ) override;
			virtual void		useShaders( Shader::GLSLProgramManager & ) override;
			virtual std::string getNameStr() const override
			{
				return std::string( magic_enum::enum_name( VIEW_3D_MOLECULE::CYLINDER ) );
			};

		  protected:
			virtual void _prepare() override;

		  private:
			// Uniforms.
			GLint _uRadius = GL_INVALID_INDEX;
		};
	} // namespace View
} // namespace VTX

#endif;
