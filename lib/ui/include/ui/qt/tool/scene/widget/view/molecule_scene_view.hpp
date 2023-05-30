#ifndef __VTX_UI_QT_TOOL_SCENE_WIDGET_VIEW_MOLECULE_SCENE_VIEW__
#define __VTX_UI_QT_TOOL_SCENE_WIDGET_VIEW_MOLECULE_SCENE_VIEW__

#include "ui/qt/style.hpp"
#include "ui/qt/tool/scene/widget/scene_item_widget.hpp"
#include "ui/qt/widget/base_manual_widget.hpp"
#include <QKeyEvent>
#include <QList>
#include <QMenu>
#include <QMimeData>
#include <QString>
#include <QTreeWidgetItem>
#include <QWidget>
#include <app/application/selection/selection.hpp>
#include <app/component/chemistry/atom.hpp>
#include <app/component/chemistry/category.hpp>
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <app/component/chemistry/residue.hpp>
#include <app/core/event/base_event_receiver_vtx.hpp>
#include <app/core/view/base_view.hpp>
#include <core/chemdb/category.hpp>
#include <core/chemdb/residue.hpp>
#include <app/component/generic/base_visible.hpp>
#include <map>

namespace VTX::UI::QT::Tool::Scene::Widget::View
{
	class MoleculeSceneView :
		public VTX::App::Core::View::BaseView<App::Component::Chemistry::Molecule>,
		public SceneItemWidget
	{
		NEW_ARCHI_VTX_WIDGET
		VTX_VIEW

	  public:
		inline static const Qt::ItemDataRole CATEGORY_ROLE = CHILD_FIRST_ROLE;

	  public:
		void notify( const VTX::App::Core::Event::VTXEvent * const p_event ) override;
		void receiveEvent( const VTX::App::Core::Event::VTXEvent & p_event ) override;

		const App::Core::Model::ID &					getModelID() const override { return _model->getId(); };
		virtual const App::Core::Scene::BaseSceneItem & getBaseSceneItem() const { return *_model; };

		bool containsModel( const App::Core::Model::BaseModel & p_model ) const override;
		virtual std::vector<App::Core::Model::ID> getAllItemsFrom(
			const App::Core::Model::BaseModel & p_model ) const override;
		virtual std::vector<App::Core::Model::ID> getAllItemsTo(
			const App::Core::Model::BaseModel & p_model ) const override;

		QTreeWidgetItem * getLastVisibleItem() override;

	  protected:
		void _setupUi( const QString & ) override;
		void _setupSlots() override;

		// virtual void keyPressEvent( QKeyEvent * p_event ) override;
		void _onCustomContextMenuCalled( const QPoint & p_clicPos ) override;

		bool _canDragAtPos( const QPoint & p_position ) const override;

		void _fillItemSelection( const App::Application::Selection::SelectionModel & p_selection,
								 QItemSelection &									 p_itemSelection ) override;
		bool _itemCanBeRenamed( const QTreeWidgetItem * p_item ) override;

		void _selectAllCategoriesFrom( std::vector<App::Core::Model::ID> &		   p_selection,
									   const App::Component::Chemistry::Category & p_itemFrom ) const;
		void _selectAllChainsFrom( std::vector<App::Core::Model::ID> &		p_selection,
								   const App::Component::Chemistry::Chain & p_itemFrom ) const;
		void _selectAllResiduesFrom( std::vector<App::Core::Model::ID> &		p_selection,
									 const App::Component::Chemistry::Residue & p_itemFrom ) const;
		void _selectAllAtomsFrom( std::vector<App::Core::Model::ID> &	  p_selection,
								  const App::Component::Chemistry::Atom & p_itemFrom ) const;

		void _selectAllCategoriesTo( std::vector<App::Core::Model::ID> &		 p_selection,
									 const App::Component::Chemistry::Category & p_itemFrom ) const;
		void _selectAllChainsTo( std::vector<App::Core::Model::ID> &	  p_selection,
								 const App::Component::Chemistry::Chain & p_itemFrom ) const;
		void _selectAllResiduesTo( std::vector<App::Core::Model::ID> &		  p_selection,
								   const App::Component::Chemistry::Residue & p_itemFrom ) const;
		void _selectAllAtomsTo( std::vector<App::Core::Model::ID> &		p_selection,
								const App::Component::Chemistry::Atom & p_itemFrom ) const;

	  private:
		QMenu *													_contextMenu;
		const QTreeWidgetItem *									_lastItemClicked = nullptr;
		QTreeWidgetItem *										_lastItemVisible = nullptr;
		std::map<QTreeWidgetItem *, QList<QTreeWidgetItem *> *> _mapLoadedItems
			= std::map<QTreeWidgetItem *, QList<QTreeWidgetItem *> *>();

		MoleculeSceneView( App::Component::Chemistry::Molecule * const p_model, QWidget * const p_parent );
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

		void _applyMoleculeDataOnItem( const App::Component::Chemistry::Molecule & p_molecule,
									   QTreeWidgetItem &						   p_item ) const;
		void _applyCategoryDataOnItem( const App::Component::Chemistry::Category & p_category,
									   QTreeWidgetItem &						   p_item ) const;
		void _applyChainDataOnItem( const App::Component::Chemistry::Chain &	p_molecule,
									QTreeWidgetItem &							p_item,
									const VTX::Core::ChemDB::Category::TYPE p_category ) const;
		void _applyResidueDataOnItem( const App::Component::Chemistry::Residue & p_molecule,
									  QTreeWidgetItem &							 p_item ) const;
		void _applyAtomDataOnItem( const App::Component::Chemistry::Atom & p_molecule, QTreeWidgetItem & p_item ) const;

		void _applyResidueNameOnItem(
			const App::Component::Chemistry::Residue &					p_molecule,
			QTreeWidgetItem &											p_item,
			const VTX::Core::ChemDB::Residue::SYMBOL_DISPLAY_MODE & p_symbolDisplayMode ) const;

		void _refreshSymbolDisplay( const VTX::Core::ChemDB::Residue::SYMBOL_DISPLAY_MODE & p_displayMode );
		void _refreshSymbolDisplayRecursive(
			QTreeWidgetItem * const										p_item,
			const VTX::Core::ChemDB::Residue::SYMBOL_DISPLAY_MODE & p_displayMode );

		void _updateMoleculeStructure();
		void _updateCategoryStructure( const App::Component::Chemistry::Category & p_category,
									   QTreeWidgetItem &						   p_item );
		void _updateChainStructure( const App::Component::Chemistry::Chain & p_chain, QTreeWidgetItem & p_item );
		void _updateResidueStructure( const App::Component::Chemistry::Residue & p_residue, QTreeWidgetItem & p_item );

		QTreeWidgetItem * const _getMoleculeTreeWidgetItem() const;
		QTreeWidgetItem * const _getTreeWidgetItem( const App::Component::Chemistry::Category & p_category ) const;
		QTreeWidgetItem * const _getTreeWidgetItem( const App::Component::Chemistry::Chain & p_chain ) const;
		QTreeWidgetItem * const _getTreeWidgetItem( const App::Component::Chemistry::Residue & p_residue ) const;
		QTreeWidgetItem * const _getTreeWidgetItem( const App::Component::Chemistry::Atom & p_atom ) const;

		bool _isMoleculeExpanded() const;
		bool _isCategoryExpanded( const App::Component::Chemistry::Category & p_category ) const;
		bool _isChainExpanded( const App::Component::Chemistry::Chain & p_chain ) const;
		bool _isResidueExpanded( const App::Component::Chemistry::Residue & p_residue ) const;

		VTX::Core::ChemDB::Category::TYPE _getCategoryFromItem( const QTreeWidgetItem & p_item ) const;
	};

} // namespace VTX::UI::QT::Tool::Scene::Widget::View

#endif
