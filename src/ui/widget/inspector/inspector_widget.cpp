#include "inspector_widget.hpp"
#include "model/atom.hpp"
#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/representation/representation.hpp"
#include "model/representation/representation_library.hpp"
#include "model/residue.hpp"
#include "model/selection.hpp"
#include "model/viewpoint.hpp"
#include "representation/representation_manager.hpp"
#include "selection/selection_manager.hpp"
#include "style.hpp"
#include "ui/widget/inspector/multiple_atom_inspector_widget.hpp"
#include "ui/widget/inspector/multiple_chain_inspector_widget.hpp"
#include "ui/widget/inspector/multiple_molecule_inspector_widget.hpp"
#include "ui/widget/inspector/multiple_residue_inspector_widget.hpp"
#include "ui/widget/inspector/multiple_viewpoint_inspector_widget.hpp"
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

		_verticalLayout = new QVBoxLayout( _scrollWidget );
		_verticalLayout->setSizeConstraint( QLayout::SizeConstraint::SetMinAndMaxSize );
		_verticalLayout->setSpacing( 4 );
		_verticalLayout->setContentsMargins( 0, 0, 0, 0 );

		_scrollArea = new CustomWidget::DockWindowMainWidget<QScrollArea>(
			Style::INSPECTOR_PREFERRED_SIZE, Style::INSPECTOR_MINIMUM_SIZE, this );

		QSizePolicy sizePolicy = QSizePolicy( QSizePolicy::Policy::MinimumExpanding,
											  QSizePolicy::Policy::MinimumExpanding,
											  QSizePolicy::ControlType::Frame );
		sizePolicy.setHorizontalStretch( 10 );
		sizePolicy.setVerticalStretch( 10 );
		_scrollArea->setSizePolicy( sizePolicy );

		_scrollArea->setFrameShape( QFrame::Shape::NoFrame );
		_scrollArea->setWidget( _scrollWidget );
		_scrollArea->setWidgetResizable( true );
		_scrollArea->setSizeAdjustPolicy( QAbstractScrollArea::SizeAdjustPolicy::AdjustIgnored );

		_moleculesInspector
			= WidgetFactory::get().instantiateWidget<MultipleMoleculeWidget>( this, "multipleMoleculeInspector" );
		_chainsInspector
			= WidgetFactory::get().instantiateWidget<MultipleChainWidget>( this, "multipleChainInspector" );
		_residuesInspector
			= WidgetFactory::get().instantiateWidget<MultipleResidueWidget>( this, "multipleResidueInspector" );
		_atomsInspector = WidgetFactory::get().instantiateWidget<MultipleAtomWidget>( this, "multipleAtomInspector" );
		_viewpointsInspector
			= WidgetFactory::get().instantiateWidget<MultipleViewpointWidget>( this, "multipleViewpointInspector" );

		_verticalLayout->addWidget( _moleculesInspector );
		_verticalLayout->addWidget( _chainsInspector );
		_verticalLayout->addWidget( _residuesInspector );
		_verticalLayout->addWidget( _atomsInspector );
		_verticalLayout->addWidget( _viewpointsInspector );

		_verticalLayout->addStretch( 1000 );

		setWidget( _scrollArea );
		clear();
	}

	void InspectorWidget::refresh()
	{
		clear();

		const Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();

		for ( const Model::ID & modelID : selectionModel.getItems() )
		{
			const ID::VTX_ID & modelTypeID = MVC::MvcManager::get().getModelTypeID( modelID );

			if ( modelTypeID == ID::Model::MODEL_MOLECULE )
			{
				Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( modelID );
				const Model::Selection::MapChainIds & moleculeSelection
					= selectionModel.getMoleculesMap().at( modelID );

				if ( moleculeSelection.getFullySelectedChildCount() == molecule.getRealChainCount() )
				{
					_moleculesInspector->addTarget( &molecule );
					_moleculesInspector->setVisible( true );
				}
				else
				{
					for ( const std::pair<Model::ID, Model::Selection::MapResidueIds> & chainData : moleculeSelection )
					{
						Model::Chain * const chain = molecule.getChain( chainData.first );

						if ( chainData.second.getFullySelectedChildCount() == chain->getRealResidueCount() )
						{
							_chainsInspector->addTarget( chain );
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
									_residuesInspector->addTarget( residue );
									_residuesInspector->setVisible( true );
								}
								else
								{
									for ( const uint & atomID : residueData.second )
									{
										Model::Atom * const atom = molecule.getAtom( atomID );
										_atomsInspector->addTarget( atom );
										_atomsInspector->setVisible( true );
									}
								}
							}
						}
					}
				}
			}
			else if ( modelTypeID == ID::Model::MODEL_VIEWPOINT )
			{
				Model::Viewpoint & viewpoint = MVC::MvcManager::get().getModel<Model::Viewpoint>( modelID );
				_viewpointsInspector->addTarget( &viewpoint );
				_viewpointsInspector->setVisible( true );
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
		if ( _viewpointsInspector->isVisible() )
			_viewpointsInspector->refresh();
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
		_viewpointsInspector->clearTargets();
		_viewpointsInspector->setVisible( false );

		_inspectorViewsData.clear();
	}

	void InspectorWidget::_setupSlots() {}
	void InspectorWidget::localize()
	{
		this->setWindowTitle( "Inspector" );
		// this->setWindowTitle( QCoreApplication::translate( "InspectorWidget", "Inspector", nullptr ) );
	}
} // namespace VTX::UI::Widget::Inspector
