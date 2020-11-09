#ifndef __VTX_VIEW_UI_WIDGET_MOLECULE_SCENE_VIEW__
#define __VTX_VIEW_UI_WIDGET_MOLECULE_SCENE_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/atom.hpp"
#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include "ui/widget/base_manual_widget_initializer.hpp"
#include "view/base_view.hpp"
#include <QTreeWidgetItem>

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				class MoleculeSceneView : public View::BaseView<Model::Molecule>, public QTreeWidgetItem, public VTX::UI::Widget::BaseManualWidgetInitializer
				{
					VTX_MANUAL_WIDGET_DECLARATION

				  public:
					MoleculeSceneView( Model::Molecule * const p_model, QTreeWidgetItem * const p_parent ) :
						View::BaseView<Model::Molecule>( p_model ), QTreeWidgetItem( p_parent ), BaseManualWidgetInitializer()
					{
					}

					void localize() override;
					void notify( const Event::VTX_EVENT_MODEL & p_event, const Event::VTXEventModelData * const p_eventData = nullptr ) override;

				  protected:
					void _setupUi( const QString & ) override;
					void _setupSlots() override;

				  private:
					void _refreshItem( QTreeWidgetItem * const p_itemWidget );
					void _refreshItem( const Model::ID & p_id );

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
