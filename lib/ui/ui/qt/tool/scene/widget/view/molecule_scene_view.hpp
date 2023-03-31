#ifndef __VTX_UI_QT_TOOL_SCENE_WIDGET_VIEW_MOLECULE_SCENE_VIEW__
#define __VTX_UI_QT_TOOL_SCENE_WIDGET_VIEW_MOLECULE_SCENE_VIEW__

#include "qt/style.hpp"
#include "qt/tool/scene/widget/scene_item_widget.hpp"
#include "qt/widget/base_manual_widget.hpp"
#include <QKeyEvent>
#include <QList>
#include <QMenu>
#include <QMimeData>
#include <QString>
#include <QTreeWidgetItem>
#include <QWidget>
#include <map>
#include <old/event/base_event_receiver_vtx.hpp>
#include <old/generic/base_visible.hpp>
#include <old/model/atom.hpp>
#include <old/model/category.hpp>
#include <old/model/category_enum.hpp>
#include <old/model/chain.hpp>
#include <old/model/molecule.hpp>
#include <old/model/residue.hpp>
#include <old/model/selection.hpp>
#include <old/style.hpp>
#include <old/view/base_view.hpp>

namespace VTX::UI::QT::Tool::Scene::Widget::View
{
	class MoleculeSceneView : public VTX::View::BaseView<Model::Molecule>, public SceneItemWidget
	{
		NEW_ARCHI_VTX_WIDGET
		VTX_VIEW

	  public:
		inline static const Qt::ItemDataRole CATEGORY_ROLE = CHILD_FIRST_ROLE;

	  public:
		void notify( const Event::VTXEvent * const p_event ) override;
		void receiveEvent( const Event::VTXEvent & p_event ) override;

		const Model::ID &					   getModelID() const override { return _model->getId(); };
		virtual const Generic::BaseSceneItem & getBaseSceneItem() const { return *_model; };

		bool						   containsModel( const Model::BaseModel & p_model ) const override;
		virtual std::vector<Model::ID> getAllItemsFrom( const Model::BaseModel & p_model ) const override;
		virtual std::vector<Model::ID> getAllItemsTo( const Model::BaseModel & p_model ) const override;

		QTreeWidgetItem * getLastVisibleItem() override;

	  protected:
		void _setupUi( const QString & ) override;
		void _setupSlots() override;

		// virtual void keyPressEvent( QKeyEvent * p_event ) override;
		void _onCustomContextMenuCalled( const QPoint & p_clicPos ) override;

		bool _canDragAtPos( const QPoint & p_position ) const override;

		void _fillItemSelection( const Model::Selection & p_selection, QItemSelection & p_itemSelection ) override;
		bool _itemCanBeRenamed( const QTreeWidgetItem * p_item ) override;

		void _selectAllCategoriesFrom( std::vector<Model::ID> & p_selection, const Model::Category & p_itemFrom ) const;
		void _selectAllChainsFrom( std::vector<Model::ID> & p_selection, const Model::Chain & p_itemFrom ) const;
		void _selectAllResiduesFrom( std::vector<Model::ID> & p_selection, const Model::Residue & p_itemFrom ) const;
		void _selectAllAtomsFrom( std::vector<Model::ID> & p_selection, const Model::Atom & p_itemFrom ) const;

		void _selectAllCategoriesTo( std::vector<Model::ID> & p_selection, const Model::Category & p_itemFrom ) const;
		void _selectAllChainsTo( std::vector<Model::ID> & p_selection, const Model::Chain & p_itemFrom ) const;
		void _selectAllResiduesTo( std::vector<Model::ID> & p_selection, const Model::Residue & p_itemFrom ) const;
		void _selectAllAtomsTo( std::vector<Model::ID> & p_selection, const Model::Atom & p_itemFrom ) const;

	  private:
		QMenu *													_contextMenu;
		const QTreeWidgetItem *									_lastItemClicked = nullptr;
		QTreeWidgetItem *										_lastItemVisible = nullptr;
		std::map<QTreeWidgetItem *, QList<QTreeWidgetItem *> *> _mapLoadedItems
			= std::map<QTreeWidgetItem *, QList<QTreeWidgetItem *> *>();

		MoleculeSceneView( Model::Molecule * const p_model, QWidget * const p_parent );
		~MoleculeSceneView();

		void _clearLoadedItems();
		void _createTopLevelObject() override;

		void _onItemChanged( QTreeWidgetItem * const, const int );
		void _onItemDoubleClicked( const QTreeWidgetItem * const, const int ) const;
		void _onItemExpanded( QTreeWidgetItem * const ) override;
		void _onItemCollapsed( QTreeWidgetItem * const ) override;

		void _reformatMoleculeName( std::string & p_moleculeName ) const;
		void _doEnableStateChangeAction( const QTreeWidgetItem * const p_item ) const;

		void _expandAll( QTreeWidgetItem * const p_from );
		void _expandMolecule( QTreeWidgetItem * const, const bool p_forceExpandChildren = false );
		void _expandCategory( QTreeWidgetItem * const p_from, const bool p_forceExpandChildren = false );
		void _expandChain( QTreeWidgetItem * const, const bool p_forceExpandChildren = false );
		void _expandResidue( QTreeWidgetItem * const, const bool p_forceExpandChildren = false );

		void _collapseItem( QTreeWidgetItem & p_item );

		void _fillListWithItemChildren( const QTreeWidgetItem & p_parent, QList<QTreeWidgetItem *> & p_list ) const;

		void _applyMoleculeDataOnItem( const Model::Molecule & p_molecule, QTreeWidgetItem & p_item ) const;
		void _applyCategoryDataOnItem( const Model::Category & p_category, QTreeWidgetItem & p_item ) const;
		void _applyChainDataOnItem( const Model::Chain & p_molecule,
									QTreeWidgetItem &	 p_item,
									const CATEGORY_ENUM	 p_category ) const;
		void _applyResidueDataOnItem( const Model::Residue & p_molecule, QTreeWidgetItem & p_item ) const;
		void _applyAtomDataOnItem( const Model::Atom & p_molecule, QTreeWidgetItem & p_item ) const;

		void _applyResidueNameOnItem( const Model::Residue &				  p_molecule,
									  QTreeWidgetItem &						  p_item,
									  const VTX::Style::SYMBOL_DISPLAY_MODE & p_symbolDisplayMode ) const;

		void _refreshSymbolDisplay( const VTX::Style::SYMBOL_DISPLAY_MODE & p_displayMode );
		void _refreshSymbolDisplayRecursive( QTreeWidgetItem * const				 p_item,
											 const VTX::Style::SYMBOL_DISPLAY_MODE & p_displayMode );

		void _updateMoleculeStructure();
		void _updateCategoryStructure( const Model::Category & p_category, QTreeWidgetItem & p_item );
		void _updateChainStructure( const Model::Chain & p_chain, QTreeWidgetItem & p_item );
		void _updateResidueStructure( const Model::Residue & p_residue, QTreeWidgetItem & p_item );

		QTreeWidgetItem * const _getMoleculeTreeWidgetItem() const;
		QTreeWidgetItem * const _getTreeWidgetItem( const Model::Category & p_category ) const;
		QTreeWidgetItem * const _getTreeWidgetItem( const Model::Chain & p_chain ) const;
		QTreeWidgetItem * const _getTreeWidgetItem( const Model::Residue & p_residue ) const;
		QTreeWidgetItem * const _getTreeWidgetItem( const Model::Atom & p_atom ) const;

		bool _isMoleculeExpanded() const;
		bool _isCategoryExpanded( const Model::Category & p_category ) const;
		bool _isChainExpanded( const Model::Chain & p_chain ) const;
		bool _isResidueExpanded( const Model::Residue & p_residue ) const;

		CATEGORY_ENUM _getCategoryFromItem( const QTreeWidgetItem & p_item ) const;
	};

} // namespace VTX::UI::QT::Tool::Scene::Widget::View

#endif
