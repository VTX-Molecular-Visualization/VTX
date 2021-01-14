#ifndef __VTX_VIEW_3D_MOLECULE_BOX__
#define __VTX_VIEW_3D_MOLECULE_BOX__

#ifdef _MSC_VER
#pragma once
#endif

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
				void render() override;

			  private:
				// Uniforms.
				GLint _uModelViewMatrixLoc = GL_INVALID_INDEX;
				GLint _uProjMatrixLoc	   = GL_INVALID_INDEX;

				explicit Box( Model::BaseModel3D<Buffer::BaseBufferOpenGL> * const );
			};
		} // namespace D3
	}	  // namespace View
} // namespace VTX

#endif;
