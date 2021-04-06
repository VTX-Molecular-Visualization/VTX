#include "inspector_widget.hpp"
#include "model/representation/representation.hpp"
#include "model/representation/representation_library.hpp"
#include "model/selection.hpp"
#include "representation/representation_manager.hpp"
#include "selection/selection_manager.hpp"
#include "style.hpp"
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

	void InspectorWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		_scrollWidget = new QWidget( this );
		_scrollWidget->setSizePolicy( QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum );

		_verticalLayout = new QVBoxLayout( _scrollWidget );
		_verticalLayout->setSizeConstraint( QLayout::SizeConstraint::SetMinAndMaxSize );
		_verticalLayout->setSpacing( 4 );
		_verticalLayout->setContentsMargins( 0, 0, 0, 0 );

		_scrollArea = new CustomWidget::DockWindowMainWidget<QScrollArea>( this );
		_scrollArea->setFrameShape( QFrame::Shape::NoFrame );
		_scrollArea->setWidget( _scrollWidget );
		_scrollArea->setWidgetResizable( true );
		_scrollArea->setSizeAdjustPolicy( QAbstractScrollArea::SizeAdjustPolicy::AdjustIgnored );
		_scrollArea->setSizeHint( Style::INSPECTOR_PREFERED_SIZE );
		_scrollArea->setMinimumSizeHint( Style::INSPECTOR_MINIMUM_SIZE );

		_moleculesInspector
			= WidgetFactory::get().instantiateWidget<MultipleMoleculeWidget>( this, "multipleMoleculeInspector" );
		_chainsInspector
			= WidgetFactory::get().instantiateWidget<MultipleChainWidget>( this, "multipleChainInspector" );
		_residuesInspector
			= WidgetFactory::get().instantiateWidget<MultipleResidueWidget>( this, "multipleResidueInspector" );
		_atomsInspector = WidgetFactory::get().instantiateWidget<MultipleAtomWidget>( this, "multipleAtomInspector" );

		_verticalLayout->addWidget( _moleculesInspector );
		_verticalLayout->addWidget( _chainsInspector );
		_verticalLayout->addWidget( _residuesInspector );
		_verticalLayout->addWidget( _atomsInspector );
		_verticalLayout->addStretch( 1000 );

		setWidget( _scrollArea );
		clear();
	}

	void InspectorWidget::refresh()
	{
		clear();

		for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & molData :
			  VTX::Selection::SelectionManager::get().getSelectionModel().getItems() )
		{
			Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( molData.first );

			if ( molData.second.getFullySelectedChildCount() == molecule.getRealChainCount() )
			{
				_moleculesInspector->addTarget( &molecule, false );
				_moleculesInspector->setVisible( true );
			}
			else
			{
				for ( const std::pair<Model::ID, Model::Selection::MapResidueIds> & chainData : molData.second )
				{
					Model::Chain * const chain = molecule.getChain( chainData.first );

					if ( chainData.second.getFullySelectedChildCount() == chain->getRealResidueCount() )
					{
						_chainsInspector->addTarget( chain, false );
						_chainsInspector->setVisible( true );
					}
					else
					{
						for ( const std::pair<Model::ID, Model::Selection::VecAtomIds> & residueData :
							  chainData.second )
						{
							Model::Residue * const residue = molecule.getResidue( residueData.first );

							if ( residueData.second.getFullySelectedChildCount() == residue->getRealAtomCount() )
							{
								_residuesInspector->addTarget( residue, false );
								_residuesInspector->setVisible( true );
							}
							else
							{
								for ( const uint & atomID : residueData.second )
								{
									Model::Atom * const atom = molecule.getAtom( atomID );
									_atomsInspector->addTarget( atom, false );
									_atomsInspector->setVisible( true );
								}
							}
						}
					}
				}
			}
		}

		if ( _moleculesInspector->isVisible() )
			_moleculesInspector->refresh();
		if ( _chainsInspector->isVisible() )
			_chainsInspector->refresh();
		if ( _residuesInspector->isVisible() )
			_residuesInspector->refresh();
		if ( _atomsInspector->isVisible() )
			_atomsInspector->refresh();
	}

	void InspectorWidget::clear()
	{
		_moleculesInspector->clearTargets();
		_moleculesInspector->setVisible( false );
		_chainsInspector->clearTargets();
		_chainsInspector->setVisible( false );
		_residuesInspector->clearTargets();
		_residuesInspector->setVisible( false );
		_atomsInspector->clearTargets();
		_atomsInspector->setVisible( false );

		_inspectorViewsData.clear();
	}

	void InspectorWidget::_setupSlots() {}
	void InspectorWidget::localize()
	{
		this->setWindowTitle( "Inspector" );
		// this->setWindowTitle( QCoreApplication::translate( "InspectorWidget", "Inspector", nullptr ) );
	}
} // namespace VTX::UI::Widget::Inspector
