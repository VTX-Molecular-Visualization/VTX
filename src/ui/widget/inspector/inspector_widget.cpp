#include "inspector_widget.hpp"
#include "model/representation/representation.hpp"
#include "model/representation/representation_library.hpp"
#include "model/selection.hpp"
#include "representation/representation_manager.hpp"
#include "selection/selection_manager.hpp"
#include "style.hpp"
#include "view/ui/widget/atom_inspector_view.hpp"
#include "view/ui/widget/chain_inspector_view.hpp"
#include "view/ui/widget/molecule_inspector_view.hpp"
#include "view/ui/widget/residue_inspector_view.hpp"
#include <unordered_set>

namespace VTX::UI::Widget::Inspector
{
	InspectorWidget::InspectorWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
	{
		_registerEvent( Event::Global::SELECTION_CHANGE );
	}

	InspectorWidget::~InspectorWidget() {}

	void InspectorWidget::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::SELECTION_CHANGE )
			refresh();
	}

	void InspectorWidget::refresh()
	{
		clear();

		for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & molData :
			  VTX::Selection::SelectionManager::get().getSelectionModel().getItems() )
		{
			Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( molData.first );
			if ( molData.second.getFullySelectedChildCount() == molecule.getChainCount() )
			{
				_addMolecule( &molecule );
				continue;
			}

			for ( const std::pair<Model::ID, Model::Selection::MapResidueIds> & chainData : molData.second )
			{
				Model::Chain * const chain = molecule.getChain( chainData.first );
				if ( chainData.second.getFullySelectedChildCount() == chain->getResidueCount() )
				{
					_addChain( chain );
					continue;
				}

				for ( const std::pair<Model::ID, Model::Selection::VecAtomIds> & residueData : chainData.second )
				{
					Model::Residue * const residue = molecule.getResidue( residueData.first );
					if ( residueData.second.getFullySelectedChildCount() == residue->getAtomCount() )
					{
						_addResidue( residue );
						continue;
					}

					for ( const uint & atomID : residueData.second )
					{
						Model::Atom * const atom = molecule.getAtom( atomID );
						_addAtom( atom );
					}
				}
			}
		}
	}

	void InspectorWidget::clear()
	{
		for ( const ViewData viewData : _inspectorViewsData )
		{
			if ( MVC::MvcManager::get().doesModelExists( viewData._modelID ) )
			{
				Model::BaseModel & model = MVC::MvcManager::get().getModel<Model::BaseModel>( viewData._modelID );
				_verticalLayout->removeWidget( viewData._widget );
				MVC::MvcManager::get().deleteView( &model, viewData._viewID );
			}
		}

		_inspectorViewsData.clear();
	}

	void InspectorWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		_scrollWidget = new QWidget( this );
		_scrollWidget->setSizePolicy( QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum );

		_verticalLayout = new QVBoxLayout( _scrollWidget );
		_verticalLayout->setSizeConstraint( QLayout::SizeConstraint::SetMinAndMaxSize );
		_verticalLayout->setSpacing( 4 );
		_verticalLayout->addStretch( 1000 );
		_verticalLayout->setContentsMargins( 0, 0, 0, 0 );

		_scrollArea = new CustomWidget::DockWindowMainWidget<QScrollArea>( this );
		_scrollArea->setFrameShape( QFrame::Shape::NoFrame );
		_scrollArea->setWidget( _scrollWidget );
		_scrollArea->setWidgetResizable( true );
		_scrollArea->setSizeAdjustPolicy( QAbstractScrollArea::SizeAdjustPolicy::AdjustIgnored );
		_scrollArea->setSizeHint( Style::INSPECTOR_PREFERED_SIZE );
		_scrollArea->setMinimumSizeHint( Style::INSPECTOR_MINIMUM_SIZE );

		setWidget( _scrollArea );
	}

	void InspectorWidget::_addMolecule( Model::Molecule * const p_molecule )
	{
		_addInspectorView<Model::Molecule, View::UI::Widget::MoleculeInspectorView>(
			p_molecule, ID::View::UI_INSPECTOR_MOLECULE_STRUCTURE, "MoleculeInspector" );
	}
	void InspectorWidget::_addChain( Model::Chain * const p_molecule )
	{
		_addInspectorView<Model::Chain, View::UI::Widget::ChainInspectorView>(
			p_molecule, ID::View::UI_INSPECTOR_MOLECULE_STRUCTURE, "ChainInspector" );
	}
	void InspectorWidget::_addResidue( Model::Residue * const p_molecule )
	{
		_addInspectorView<Model::Residue, View::UI::Widget::ResidueInspectorView>(
			p_molecule, ID::View::UI_INSPECTOR_MOLECULE_STRUCTURE, "ResidueInspector" );
	}
	void InspectorWidget::_addAtom( Model::Atom * const p_atom )
	{
		_addInspectorView<Model::Atom, View::UI::Widget::AtomInspectorView>(
			p_atom, ID::View::UI_INSPECTOR_MOLECULE_STRUCTURE, "AtomInspector" );
	}

	void InspectorWidget::_setupSlots() {}
	void InspectorWidget::localize()
	{
		this->setWindowTitle( "Inspector" );
		// this->setWindowTitle( QCoreApplication::translate( "InspectorWidget", "Inspector", nullptr ) );
	}
} // namespace VTX::UI::Widget::Inspector
