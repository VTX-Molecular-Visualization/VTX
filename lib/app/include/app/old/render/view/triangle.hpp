#ifndef __VTX_RENDER_VIEW_MESH_TRIANGLE__
#define __VTX_RENDER_VIEW_MESH_TRIANGLE__

#include "app/old/component/chemistry/solvent_excluded_surface.hpp"
#include "app/old/component/object3d/mesh_triangle.hpp"
#include "app/old/core/view/base_view_3d.hpp"

namespace VTX::App::Old::Render::View
{
	class Triangle : public App::Old::Core::View::BaseView3D<App::Old::Component::Object3D::MeshTriangle>
	{
		VTX_VIEW

	  public:
		void render( const App::Old::Component::Render::Camera & p_camera ) const override;

	  protected:
		Renderer::GL::Program * const _createProgram() override;

	  private:
		explicit Triangle( App::Old::Component::Object3D::MeshTriangle * const p_model ) : BaseView3D( p_model ) {}
	};

	class TriangleSES : public App::Old::Core::View::BaseView3D<App::Old::Component::Chemistry::SolventExcludedSurface>
	{
		VTX_VIEW

	  public:
		void render( const App::Old::Component::Render::Camera & p_camera ) const override;

	  protected:
		Renderer::GL::Program * const _createProgram() override;

	  private:
		explicit TriangleSES( App::Old::Component::Chemistry::SolventExcludedSurface * const p_model ) :
			BaseView3D( p_model )
		{
		}
	};
} // namespace VTX::App::Old::Render::View

#endif
