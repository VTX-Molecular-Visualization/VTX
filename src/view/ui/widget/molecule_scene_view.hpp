#ifndef __VTX_VIEW_UI_WIDGET_MOLECULE_SCENE_VIEW__
#define __VTX_VIEW_UI_WIDGET_MOLECULE_SCENE_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_scene_item.hpp"
#include "model/atom.hpp"
#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include "view/base_view.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				class MoleculeSceneView : public BaseSceneItem<Model::Molecule>
				{
					VTX_MANUAL_WIDGET_DECLARATION

				  public:
					void localize() override;

				  protected:
					MoleculeSceneView( Model::Molecule * const, QTreeWidgetItem * );
					void _setupUi( const QString & ) override;
					void _setupSlots() override;

				  private:
					void refreshItem( QTreeWidgetItem * const p_itemWidget );

					void _refreshItem( QTreeWidgetItem * const p_itemWidget, const Model::Molecule & p_model ) const;
					void _refreshItem( QTreeWidgetItem * const p_itemWidget, const Model::Chain & p_model ) const;
					void _refreshItem( QTreeWidgetItem * const p_itemWidget, const Model::Residue & p_model ) const;
					void _refreshItem( QTreeWidgetItem * const p_itemWidget, const Model::Atom & p_model ) const;

					const Qt::CheckState _getCheckState( const bool p_enable ) const { return p_enable ? Qt::CheckState::Checked : Qt::CheckState::Unchecked; };
				};

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX

#endif
