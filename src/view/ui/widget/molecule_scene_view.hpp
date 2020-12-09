#ifndef __VTX_VIEW_UI_WIDGET_MOLECULE_SCENE_VIEW__
#define __VTX_VIEW_UI_WIDGET_MOLECULE_SCENE_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/base_event_receiver_vtx.hpp"
#include "model/atom.hpp"
#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "view/base_view.hpp"
#include <QMenu>
#include <QMouseEvent>
#include <QPoint>
#include <QTreeWidget>

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				class MoleculeSceneView : public View::BaseView<Model::Molecule>, public VTX::UI::Widget::BaseManualWidget<QTreeWidget>
				{
					VTX_WIDGET
					VTX_VIEW

				  public:
					void localize() override;
					void notify( const Event::VTXEvent * const p_event ) override;
					void receiveEvent( const Event::VTXEvent & p_event ) override;

				  protected:
					void _setupUi( const QString & ) override;
					void _setupSlots() override;

					void mousePressEvent( QMouseEvent * p_event ) override;
					void mouseMoveEvent( QMouseEvent * p_event ) override;

				  private:
					QMenu * _contextMenu;
					QPoint	_dragStartPosition;

					MoleculeSceneView( Model::Molecule * const p_model, QWidget * const p_parent );

					void _onItemChanged( QTreeWidgetItem * item, int column );
					void _onItemClicked( QTreeWidgetItem *, int );
					void _onItemExpanded( QTreeWidgetItem * );
					void _onItemCollapsed( QTreeWidgetItem * );
					void _onCustomContextMenuCalled( const QPoint & p_clicPos );

					void _deleteAction();

					void _refreshItem( QTreeWidgetItem * const p_itemWidget, const Model::Molecule & p_model ) const;
					void _refreshItem( QTreeWidgetItem * const p_itemWidget, const Model::Chain & p_model ) const;
					void _refreshItem( QTreeWidgetItem * const p_itemWidget, const Model::Residue & p_model ) const;

					void _sendEnableStateChangeAction( const Model::ID & p_modelID, const bool modelEnabled ) const;

					inline Model::ID _getModelID( const QTreeWidgetItem & p_item ) const
					{
						const QVariant & dataID = p_item.data( 0, Qt::UserRole );
						return dataID.value<VTX::Model::ID>();
					}

					const Qt::CheckState _getCheckState( const bool p_enable ) const { return p_enable ? Qt::CheckState::Checked : Qt::CheckState::Unchecked; };
				};

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX

#endif
