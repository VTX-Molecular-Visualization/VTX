#ifndef __VTX_VIEW_3D_RIBBON__
#define __VTX_VIEW_3D_RIBBON__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/secondary_structure.hpp"
#include "view/base_view_3d.hpp"

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
			  protected:
				void						  _init() override;
				void						  _render() override;
				Renderer::GL::Program * const _createProgram( Renderer::GL::ProgramManager & ) override;
				void						  _createUniforms() override;

			  private:
				GLint _uCamPositionLoc = GL_INVALID_INDEX;
				GLint _uMaxIndiceLoc   = GL_INVALID_INDEX;

				explicit Ribbon( Model::SecondaryStructure * const p_model ) : BaseView3D( p_model ) {}
			};
		} // namespace D3
	}	  // namespace View
} // namespace VTX

#endif;
