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
				VTX_VIEW

			  public:
				void render( const Object3D::Camera & p_camera ) override;

			  protected:
				Renderer::GL::Program * const _createProgram() override;

			  private:
				explicit Triangle( Model::MeshTriangle * const p_model ) : BaseView3D( p_model ) {}
			};
		} // namespace D3
	}	  // namespace View
} // namespace VTX

#endif;
