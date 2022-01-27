#ifndef __VTX_VIEW_3D_TORUS__
#define __VTX_VIEW_3D_TORUS__

#include "model/contour_buildup.hpp"
#include "view/base_view_3d.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			class Torus : public BaseView3D<Model::ContourBuildup>
			{
				VTX_VIEW

			  public:
				void render( const Object3D::Camera & p_camera ) const override;

			  protected:
				Renderer::GL::Program * const _createProgram() override;
				void						  _init() override;

			  private:
				explicit Torus( Model::ContourBuildup * const p_model ) : BaseView3D( p_model ) {}
			};
		} // namespace D3
	}	  // namespace View
} // namespace VTX

#endif
