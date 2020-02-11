#ifndef __VTX_VIEW_UI_CHECKPOINT__
#define __VTX_VIEW_UI_CHECKPOINT__

#ifdef _MSC_VER
#pragma once

#endif
#include "base_view_ui.hpp"
#include "model/checkpoint.hpp"

namespace VTX
{
	namespace View
	{
		class ViewUICheckpoint : public BaseViewUI<Model::Checkpoint>
		{
		  public:
			explicit ViewUICheckpoint( Model::Checkpoint * const p_model ) : BaseViewUI( p_model ) {}

			virtual std::string getName() const override { return ID::View::UI_CHECKPOINT; };
			virtual std::string getComponentParentName() const override { return ID::UI::INSPECTOR; };

		  protected:
			virtual void _draw() override;

		  private:
		};
	} // namespace View
} // namespace VTX
#endif
