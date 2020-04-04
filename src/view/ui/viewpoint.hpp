#ifndef __VTX_VIEW_UI_VIEWPOINT__
#define __VTX_VIEW_UI_VIEWPOINT__

#ifdef _MSC_VER
#pragma once

#endif
#include "model/viewpoint.hpp"
#include "view/base_view_ui.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			class Viewpoint : public BaseViewUI<Model::Viewpoint>
			{
			  public:
				explicit Viewpoint( Model::Viewpoint * const p_model ) : BaseViewUI( p_model ) {}

				virtual const std::string & getName() const override { return ID::View::UI_VIEWPOINT; };
				virtual const std::string & getComponentParentName() const override { return ID::UI::INSPECTOR; };

			  protected:
				virtual void _draw() override;

			  private:
				char _action[ 64 ] = "";
			};
		} // namespace UI
	}	  // namespace View
} // namespace VTX
#endif
