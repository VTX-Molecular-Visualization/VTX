#ifndef __VTX_VIEW_3D_MOLECULE_BOX__
#define __VTX_VIEW_3D_MOLECULE_BOX__

#include "model/base_model_3d.hpp"
#include "view/base_view_3d.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			class Box : public BaseView3D<Model::BaseModel3D<Buffer::BaseBufferOpenGL>>
			{
				VTX_VIEW

			  public:
				void render( const Object3D::Camera & p_camera ) const override;

			  protected:
				Renderer::GL::Program * const _createProgram() override;
				void						  _init() override;

			  private:
				explicit Box( Model::BaseModel3D<Buffer::BaseBufferOpenGL> * const );
			};
		} // namespace D3
	}	  // namespace View
} // namespace VTX

#endif;
