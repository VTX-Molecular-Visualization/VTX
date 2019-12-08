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

			BaseView3DMolecule() {};
			~BaseView3DMolecule() = default;

			void							 setCameraMatrices( const Object3D::Camera::BaseCamera & ) const;
			virtual std::string				 getProgramName() const		  = 0;
			virtual std::vector<std::string> getShaderNames() const		  = 0;
			virtual void					 setupUniforms( const GLint ) = 0;

		  protected:
			GLint _uViewMatrix	   = GL_INVALID_INDEX;
			GLint _uProjMatrix	   = GL_INVALID_INDEX;
			GLint _uCylinderRadius = GL_INVALID_INDEX;
		}; // namespace View
	}	   // namespace View
} // namespace VTX
#endif
