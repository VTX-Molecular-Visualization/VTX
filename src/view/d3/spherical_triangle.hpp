#ifndef __VTX_VIEW_3D_SPHERICAL_TRIANGLE__
#define __VTX_VIEW_3D_SPHERICAL_TRIANGLE__

#include "model/contour_buildup.hpp"
#include "view/base_view_3d.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			class SphericalTriangle : public BaseView3D<Model::ContourBuildup>
			{
				VTX_VIEW

			  public:
				void render( const Object3D::Camera & p_camera ) const override;

			  protected:
				Renderer::GL::Program * const _createProgram() override;
				void						  _init() override;

			  private:
				explicit SphericalTriangle( Model::ContourBuildup * const p_model ) : BaseView3D( p_model ) {}
			};
		} // namespace D3
	}	  // namespace View
} // namespace VTX

#endif
