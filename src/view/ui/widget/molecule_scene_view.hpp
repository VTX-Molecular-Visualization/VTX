#ifndef __VTX_VIEW_UI_WIDGET_MOLECULE_SCENE_VIEW__
#define __VTX_VIEW_UI_WIDGET_MOLECULE_SCENE_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/base_event_receiver_vtx.hpp"
#include "generic/base_visible.hpp"
#include "id.hpp"
#include "model/atom.hpp"
#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include "model/selection.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/scene/scene_item_widget.hpp"
#include "view/base_view.hpp"
#include <QKeyEvent>
#include <QList>
#include <QMenu>
#include <QMimeData>
#include <QString>
#include <QTreeWidgetItem>
#include <QWidget>
#include <map>

namespace VTX::View::UI::Widget
{
	class MoleculeSceneView : public View::BaseView<Model::Molecule>, public VTX::UI::Widget::Scene::SceneItemWidget
	{
		VTX_WIDGET
		VTX_VIEW

	  protected:
		inline static const Qt::ItemDataRole MODEL_ID_ROLE	   = Qt::ItemDataRole( Qt::UserRole );
		inline static const Qt::ItemDataRole EXPAND_STATE_ROLE = Qt::ItemDataRole( Qt::UserRole + 1 );

	  public:
		void localize() override;
		void notify( const Event::VTXEvent * const p_event ) override;
		void receiveEvent( const Event::VTXEvent & p_event ) override;

		const Model::ID & getModelID() const override { return _model->getId(); };

	  protected:
		void _setupUi( const QString & ) override;
		void _setupSlots() override;

		virtual void keyPressEvent( QKeyEvent * p_event ) override;
		void		 mouseMoveEvent( QMouseEvent * p_event ) override;

		bool		_canDragObjectAtPos( const QPoint & p_position ) override;
		QMimeData * _getDataForDrag() override;

	  private:
		QMenu *											_contextMenu;
		const QTreeWidgetItem *							_lastItemClicked = nullptr;
		std::map<Model::ID, QList<QTreeWidgetItem *> *> _mapLoadedItems
			= std::map<Model::ID, QList<QTreeWidgetItem *> *>();
		int _enableSignalCounter = 0;

		MoleculeSceneView( Model::Molecule * const p_model, QWidget * const p_parent );
		~MoleculeSceneView();

		void _rebuildTree();
		void _enableSignals( const bool p_enable );

		void _onItemChanged( const QTreeWidgetItem * const, const int ) const;
		void _onItemDoubleClicked( const QTreeWidgetItem * const, const int ) const;
		void _onItemExpanded( QTreeWidgetItem * const );
		void _onItemCollapsed( QTreeWidgetItem * const );

		void _deleteAction() override;
		void _doEnableStateChangeAction( const QTreeWidgetItem * const p_item ) const;

		void _expandAll( QTreeWidgetItem * const p_from );
		void _expandMolecule( QTreeWidgetItem * const, const bool p_forceExpandChildren = false );
		void _expandChain( QTreeWidgetItem * const, const bool p_forceExpandChildren = false );
		void _expandResidue( QTreeWidgetItem * const, const bool p_forceExpandChildren = false );

		void _collapseItem( QTreeWidgetItem & p_item );

		void _fillListWithItemChildren( const QTreeWidgetItem & p_parent, QList<QTreeWidgetItem *> & p_list ) const;

		void _applyMoleculeDataOnItem( const Model::Molecule & p_molecule, QTreeWidgetItem & p_item ) const;
		void _applyChainDataOnItem( const Model::Chain & p_molecule, QTreeWidgetItem & p_item ) const;
		void _applyResidueDataOnItem( const Model::Residue & p_molecule, QTreeWidgetItem & p_item ) const;
		void _applyAtomDataOnItem( const Model::Atom & p_molecule, QTreeWidgetItem & p_item ) const;

		void _refreshItemVisibility( QTreeWidgetItem * const p_itemWidget, const Generic::BaseVisible & p_baseVisible );
		void _refreshSelection( const Model::Selection & p_selection );

		Model::ID _getModelIDFromItem( const QTreeWidgetItem & p_item ) const;
		bool	  _getItemExpandState( const QTreeWidgetItem & p_item ) const;
	};

} // namespace VTX::View::UI::Widget

#endif
