#ifndef __VTX_VIEW_3D_MESH_TRIANGLE__
#define __VTX_VIEW_3D_MESH_TRIANGLE__

#include "app/core/view/base_view_3d.hpp"
#include "app/model/mesh_triangle.hpp"
#include "app/component/chemistry/solvent_excluded_surface.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			class Triangle : public App::Core::View::BaseView3D<Model::MeshTriangle>
			{
				VTX_VIEW

			  public:
				void render( const Object3D::Camera & p_camera ) const override;

			  protected:
				Renderer::GL::Program * const _createProgram() override;

			  private:
				explicit Triangle( Model::MeshTriangle * const p_model ) : BaseView3D( p_model ) {}
			};

			class TriangleSES : public App::Core::View::BaseView3D<App::Component::Chemistry::SolventExcludedSurface>
			{
				VTX_VIEW

			  public:
				void render( const Object3D::Camera & p_camera ) const override;

			  protected:
				Renderer::GL::Program * const _createProgram() override;

			  private:
				explicit TriangleSES( App::Component::Chemistry::SolventExcludedSurface * const p_model ) : BaseView3D( p_model ) {}
			};
		} // namespace D3
	}	  // namespace View
} // namespace VTX

#endif
