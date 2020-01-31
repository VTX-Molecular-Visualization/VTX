#ifndef __VTX_VIEW_UI_PATH_LIST__
#define __VTX_VIEW_UI_PATH_LIST__

#ifdef _MSC_VER
#pragma once
#endif

#include "../model/model_path.hpp"
#include "base_view_ui.hpp"

namespace VTX
{
	namespace View
	{
		class ViewUIPathList : public BaseViewUI<Model::ModelPath>
		{
		  public:
			explicit ViewUIPathList( Model::ModelPath * const p_model ) : BaseViewUI( p_model ) {}

			virtual std::string getName() const override { return ID::View::UI_PATH_LIST; }
			virtual std::string getComponentParentName() const override { return ID::UI::CAMERA_EDITOR; };

		  protected:
			virtual void _draw() override;

		  private:
		};
	} // namespace View
} // namespace VTX
#endif
