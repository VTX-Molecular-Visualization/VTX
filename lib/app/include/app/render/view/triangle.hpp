#ifndef __VTX_RENDER_VIEW_MESH_TRIANGLE__
#define __VTX_RENDER_VIEW_MESH_TRIANGLE__

#include "app/component/chemistry/solvent_excluded_surface.hpp"
#include "app/component/object3d/mesh_triangle.hpp"
#include "app/core/view/base_view_3d.hpp"

namespace VTX::App::Render::View
{
	class Triangle : public App::Core::View::BaseView3D<App::Component::Object3D::MeshTriangle>
	{
		VTX_VIEW

	  public:
		void render( const App::Component::Render::Camera & p_camera ) const override;

	  protected:
		Renderer::GL::Program * const _createProgram() override;

	  private:
		explicit Triangle( App::Component::Object3D::MeshTriangle * const p_model ) : BaseView3D( p_model ) {}
	};

	class TriangleSES : public App::Core::View::BaseView3D<App::Component::Chemistry::SolventExcludedSurface>
	{
		VTX_VIEW

	  public:
		void render( const App::Component::Render::Camera & p_camera ) const override;

	  protected:
		Renderer::GL::Program * const _createProgram() override;

	  private:
		explicit TriangleSES( App::Component::Chemistry::SolventExcludedSurface * const p_model ) :
			BaseView3D( p_model )
		{
		}
	};
} // namespace VTX::App::Render::View

#endif