#ifndef __VTX_VIEW_3D_MESH_TRIANGLE__
#define __VTX_VIEW_3D_MESH_TRIANGLE__

#include "model/mesh_triangle.hpp"
#include "model/solvent_excluded_surface.hpp"
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
				void render( const Object3D::Camera & p_camera ) const override;

			  protected:
				Renderer::GL::Program * const _createProgram() override;

			  private:
				explicit Triangle( Model::MeshTriangle * const p_model ) : BaseView3D( p_model ) {}
			};

			class TriangleSES : public BaseView3D<Model::SolventExcludedSurface>
			{
				VTX_VIEW

			  public:
				void render( const Object3D::Camera & p_camera ) const override;

			  protected:
				Renderer::GL::Program * const _createProgram() override;

			  private:
				explicit TriangleSES( Model::SolventExcludedSurface * const p_model ) : BaseView3D( p_model ) {}
			};
		} // namespace D3
	}	  // namespace View
} // namespace VTX

#endif
