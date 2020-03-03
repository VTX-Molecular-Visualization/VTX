#ifndef __VTX_VIEW_UI_PATH__
#define __VTX_VIEW_UI_PATH__

#ifdef _MSC_VER
#pragma once

#endif
#include "model/path.hpp"
#include "view/base_view_ui.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			class Path : public BaseViewUI<Model::Path>
			{
			  public:
				explicit Path( Model::Path * const p_model ) : BaseViewUI( p_model ) {}

				virtual const std::string & getName() const override { return ID::View::UI_PATH; };
				virtual const std::string & getComponentParentName() const override { return ID::UI::INSPECTOR; };

			  protected:
				virtual void _draw() override;

			  private:
			};
		} // namespace UI
	}	  // namespace View
} // namespace VTX
#endif
