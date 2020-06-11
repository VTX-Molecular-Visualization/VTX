#ifndef __VTX_VIEW_3D_MESH_TRIANGLE__
#define __VTX_VIEW_3D_MESH_TRIANGLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/mesh_triangle.hpp"
#include "view/base_view_3d.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			class Triangle : public BaseView3D<Model::MeshTriangle>
			{
			  public:
				explicit Triangle( Model::MeshTriangle * const p_model ) : BaseView3D( p_model ) {}

				virtual const std::string &		  getName() const override { return ID::View::D3_TRIANGLE; };
				virtual Renderer::GLSL::Program * createProgram() override;
				virtual void					  setUniFormLocations() override;

				virtual void render() override;

			  private:
				// Uniforms.
				GLint _uModelViewMatrixLoc = GL_INVALID_INDEX;
				GLint _uProjMatrixLoc	   = GL_INVALID_INDEX;
				GLint _uNormalMatrixLoc	   = GL_INVALID_INDEX;
			};
		} // namespace D3
	}	  // namespace View
} // namespace VTX

#endif;
