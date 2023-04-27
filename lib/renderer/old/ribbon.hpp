#ifndef __VTX_VIEW_3D_RIBBON__
#define __VTX_VIEW_3D_RIBBON__

#include "app/old_app/model/secondary_structure.hpp"
#include "app/old_app/view/base_view_3d.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			class Ribbon : public BaseView3D<Model::SecondaryStructure>
			{
				VTX_VIEW

			  public:
				void render( const Object3D::Camera & p_camera ) const override;

			  protected:
				Renderer::GL::Program * const _createProgram() override;
				void						  _init() override;

			  private:
				explicit Ribbon( Model::SecondaryStructure * const p_model ) : BaseView3D( p_model ) {}
			};
		} // namespace D3
	}	  // namespace View
} // namespace VTX

#endif
