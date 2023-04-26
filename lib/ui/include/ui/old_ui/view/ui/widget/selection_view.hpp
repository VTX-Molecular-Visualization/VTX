#ifndef __VTX_VIEW_UI_WIDGET_SELECTION_VIEW__
#define __VTX_VIEW_UI_WIDGET_SELECTION_VIEW__

#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include "ui/old_ui/ui/widget/scene/scene_item_widget.hpp"
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>
#include <QTreeWidgetItem>
#include <app/component/chemistry/atom.hpp>
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <app/component/chemistry/residue.hpp>
#include <app/model/selection.hpp>
#include <app/core/view/base_view.hpp>

namespace VTX::View::UI::Widget
{
	class SelectionView : public App::Core::View::BaseView<Model::Selection>, public VTX::UI::Widget::BaseManualWidget<QTreeWidget>
	{
		class SelectionStyleItemDelegate : public QStyledItemDelegate
		{
		  public:
			void  paint( QPainter *					  painter,
						 const QStyleOptionViewItem & option,
						 const QModelIndex &		  index ) const override;
			QSize sizeHint( const QStyleOptionViewItem & option, const QModelIndex & index ) const override;

		  protected:
			int _getSize() const;
		};

		VTX_WIDGET
		VTX_VIEW

		using SceneItemWidget = VTX::UI::Widget::Scene::SceneItemWidget;

	  protected:
		inline static const int NAME_COLUMN_INDEX	= 0;
		inline static const int REMOVE_COLUMN_INDEX = 1;

	  public:
		void localize() override;

	  protected:
		SelectionView( Model::Selection * const p_model, QWidget * const p_parent ) :
			App::Core::View::BaseView<Model::Selection>( p_model ), BaseManualWidget( p_parent )
		{
		}
		~SelectionView();

		void _setupUi( const QString & ) override;
		void _setupSlots() override;
		void _refreshView() override; // Debug only.

	  private:
		std::map<App::Core::Model::ID, QList<QTreeWidgetItem *> *> _mapLoadedItems
			= std::map<App::Core::Model::ID, QList<QTreeWidgetItem *> *>();

		int							 _enableSignalCounter = 0;
		SelectionStyleItemDelegate * _styleItemDelegate	  = nullptr;

		void _onItemClicked( const QTreeWidgetItem * const, const int ) const;
		void _onItemDoubleClicked( const QTreeWidgetItem * const, const int ) const;
		void _onItemExpanded( QTreeWidgetItem * const );
		void _onItemCollapsed( QTreeWidgetItem * const );

		void _expandAll( QTreeWidgetItem * const p_from );
		void _expandMolecule( QTreeWidgetItem * const, const bool p_forceExpandChildren = false );
		void _expandChain( QTreeWidgetItem * const, const bool p_forceExpandChildren = false );
		void _expandResidue( QTreeWidgetItem * const, const bool p_forceExpandChildren = false );

		void _collapseItem( QTreeWidgetItem & p_item );

		void _applyMoleculeDataOnItem( const App::Component::Chemistry::Molecule &				 p_molecule,
									   QTreeWidgetItem &					 p_item,
									   const Model::Selection::MapChainIds & p_children ) const;
		void _applyChainDataOnItem( const App::Component::Chemistry::Chain &					p_chain,
									QTreeWidgetItem &						p_item,
									const Model::Selection::MapResidueIds & p_children ) const;
		void _applyResidueDataOnItem( const App::Component::Chemistry::Residue &			   p_residue,
									  QTreeWidgetItem &					   p_item,
									  const Model::Selection::VecAtomIds & p_children ) const;
		void _applyAtomDataOnItem( const App::Component::Chemistry::Atom & p_molecule, QTreeWidgetItem & p_item ) const;

		QTreeWidgetItem * _extractItemFromList( const App::Core::Model::ID & p_id, QList<QTreeWidgetItem *> & p_list ) const;
		void			  _clearList( QList<QTreeWidgetItem *> & p_list ) const;

		void _enableSignals( const bool p_enable );

		App::Core::Model::ID _getModelID( const QTreeWidgetItem & p_item ) const;
		bool	  _getItemExpandState( const QTreeWidgetItem & p_item ) const;
	};

} // namespace VTX::View::UI::Widget

#endif
