#ifndef __VTX_VIEW_UI_WIDGET_CHAIN_SCENE_VIEW__
#define __VTX_VIEW_UI_WIDGET_CHAIN_SCENE_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_scene_item.hpp"
#include "model/chain.hpp"
#include "view/base_view.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				class ChainSceneView : public BaseSceneItem<Model::Chain>
				{
					VTX_MANUAL_WIDGET_DECLARATION

				  public:
					void localize() override;

				  protected:
					ChainSceneView( Model::Chain * const p_model, QTreeWidgetItem * p_parent ) : BaseSceneItem<Model::Chain>( p_model, p_parent ) {};
					void _setupSlots() override;

				  private:
				};

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX

#endif
