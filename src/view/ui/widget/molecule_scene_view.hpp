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
#include "ui/widget/scene/scene_item_widget.hpp"
#include "view/base_view.hpp"
#include <QMenu>
#include <QMouseEvent>
#include <QPoint>
#include <QTreeWidget>
#include <QWidget>

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				class MoleculeSceneView : public View::BaseView<Model::Molecule>, public VTX::UI::Widget::Scene::SceneItemWidget
				{
					VTX_WIDGET
					VTX_VIEW

				  public:
					void			  localize() override;
					void			  notify( const Event::VTXEvent * const p_event ) override;
					void			  receiveEvent( const Event::VTXEvent & p_event ) override;
					const Model::ID & getModelID() const override { return _model->getId(); };

				  protected:
					void _setupUi( const QString & ) override;
					void _setupSlots() override;

					QMimeData * _getDataForDrag() override;

				  private:
					QMenu *			  _contextMenu;
					QTreeWidgetItem * _lastItemClicked = nullptr;

					MoleculeSceneView( Model::Molecule * const p_model, QWidget * const p_parent );

					void _buildTree();

					void _onItemChanged( QTreeWidgetItem * item, int column );
					void _onItemClicked( QTreeWidgetItem *, int );
					void _onItemExpanded( QTreeWidgetItem * );
					void _onItemCollapsed( QTreeWidgetItem * );

					void _deleteAction() override;

					void _refreshItem( QTreeWidgetItem * const p_itemWidget, const Model::Molecule & p_model ) const;
					void _refreshItem( QTreeWidgetItem * const p_itemWidget, const Model::Chain & p_model ) const;
					void _refreshItem( QTreeWidgetItem * const p_itemWidget, const Model::Residue & p_model ) const;

					void _selectModelAction( const Model::ID & p_modelId, const bool p_appendToSelection ) const;
					void _selectModelAction( const std::vector<Model::ID> & p_modelId, const bool p_appendToSelection ) const;

					void _doEnableStateChangeAction( const QTreeWidgetItem * const p_item ) const;

					inline Model::ID _getModelID( const QTreeWidgetItem & p_item ) const
					{
						const QVariant & dataID = p_item.data( 0, Qt::UserRole );
						return dataID.value<VTX::Model::ID>();
					}
				};

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX

#endif
