#ifndef __VTX_BASE_VIEW_3D_MOLECULE__
#define __VTX_BASE_VIEW_3D_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "../model/model_molecule.hpp"
#include "../object3d/camera/camera_orbit.hpp"
#include "base_view_3d.hpp"
#include <GL/gl3w.h>
#include <vector>

namespace VTX
{
	namespace View
	{
		class BaseView3DMolecule : public BaseView3D<Model::ModelMolecule>
		{
		  public:
			enum class REPRESENTATION
			{
				BALL_AND_STICK
			};

			enum ATTRIBUTE_LOCATION
			{
				ATOM_POSITION = 0,
				ATOM_COLOR	  = 1,
				ATOM_RADIUS	  = 2
			};

			BaseView3DMolecule() = default;
			~BaseView3DMolecule();

			void bindVAO() const { glBindVertexArray( _vao ); }
			void unbindVAO() const { glBindVertexArray( 0 ); }

			void bindIBO() const { glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _bondsIBO ); }
			void unbindIBO() const { glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ); }

		  protected:
			// Atom positions vbo.
			GLuint _atomPositionsVBO = GL_INVALID_VALUE;
			// Bonds ibo.
			GLuint _bondsIBO = GL_INVALID_VALUE;
			// Radii vbo.
			GLuint _atomRadiusVBO = GL_INVALID_VALUE; // TODO compress with position
			// Color vbo.
			// TODO ? use SSBO ? ok for atom colors (CPK/residue/chain)
			// but for energy based coloration, useless...
			GLuint _atomColorsVBO = GL_INVALID_VALUE;
			// Vao.
			GLuint _vao = GL_INVALID_VALUE;

			virtual void _prepare() override;
		}; // namespace View
	}	   // namespace View
} // namespace VTX
#endif
