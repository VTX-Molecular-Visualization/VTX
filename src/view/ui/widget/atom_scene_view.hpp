#ifndef __VTX_VIEW_UI_WIDGET_ATOM_SCENE_VIEW__
#define __VTX_VIEW_UI_WIDGET_ATOM_SCENE_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_scene_item.hpp"
#include "model/atom.hpp"
#include "view/base_view.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				class AtomSceneView : public BaseSceneItem<Model::Atom>
				{
					VTX_MANUAL_WIDGET_DECLARATION

				  public:
					void localize() override;

				  protected:
					AtomSceneView( Model::Atom * const p_model, QTreeWidgetItem * p_parent ) : BaseSceneItem<Model::Atom>( p_model, p_parent ) {};
					void _setupSlots() override;

				  private:
				};

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX

#endif
