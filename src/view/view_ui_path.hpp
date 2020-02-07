#ifndef __VTX_VIEW_UI_PATH__
#define __VTX_VIEW_UI_PATH__

#ifdef _MSC_VER
#pragma once

#endif
#include "base_view_ui.hpp"
#include "model/model_path.hpp"

namespace VTX
{
	namespace View
	{
		class ViewUIPath : public BaseViewUI<Model::ModelPath>
		{
		  public:
			explicit ViewUIPath( Model::ModelPath * const p_model ) : BaseViewUI( p_model ) {}

			virtual std::string getName() const override { return ID::View::UI_PATH; };
			virtual std::string getComponentParentName() const override { return ID::UI::INSPECTOR; };

		  protected:
			virtual void _draw() override;

		  private:
		};
	} // namespace View
} // namespace VTX
#endif
