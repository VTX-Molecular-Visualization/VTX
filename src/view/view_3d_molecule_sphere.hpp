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
			~View3DMoleculeSphere();

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

			// Buffers.
			GLuint _atomPositionsVBO = GL_INVALID_VALUE; // Atom positions vbo.
			GLuint _atomRadiusVBO	 = GL_INVALID_VALUE; // Radii vbo. TODO: compress with position.
														 // TODO ? use SSBO ? ok for atom colors (CPK/residue/chain)
														 // but for energy based coloration, useless...
			GLuint _atomColorsVBO = GL_INVALID_VALUE;	 // Color vbo.

			// Uniforms.
			GLint _uRadiusScale = GL_INVALID_INDEX;
		};
	} // namespace View
} // namespace VTX

#endif;
