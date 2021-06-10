#ifndef __VTX_VIEW_3D_TRIANGLE_RIBBON__
#define __VTX_VIEW_3D_TRIANGLE_RIBBON__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/ribbon.hpp"
#include "view/base_view_3d.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			class TriangleRibbon : public BaseView3D<Model::Ribbon>
			{
			  public:
				explicit TriangleRibbon( Model::Ribbon * const p_model ) : BaseView3D( p_model ) {}

				virtual const std::string & getName() const override { return ID::View::D3_TRIANGLE_RIBBON; };

				virtual void createProgram() override;
				virtual void setUniFormLocations() override;

				virtual void render() override;

			  private:
				GLint _uModelViewMatrixLoc = GL_INVALID_INDEX;
				GLint _uProjMatrixLoc	   = GL_INVALID_INDEX;
				GLint _uNormalMatrixLoc	   = GL_INVALID_INDEX;
			};
		} // namespace D3
	}	  // namespace View
} // namespace VTX

#endif;