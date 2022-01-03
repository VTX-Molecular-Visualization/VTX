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
#include <QFrame>
#include <QHBoxLayout>
#include <unordered_set>

namespace VTX::UI::Widget::Inspector
{
	InspectorWidget::InspectorWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
	{
		_registerEvent( Event::Global::SELECTION_CHANGE );
		_registerEvent( Event::Global::MOLECULE_REMOVED );
		_registerEvent( Event::Global::CHAIN_REMOVED );
		_registerEvent( Event::Global::RESIDUE_REMOVED );
		_registerEvent( Event::Global::ATOM_REMOVED );
		_registerEvent( Event::Global::VIEWPOINT_REMOVED );
	}

	InspectorWidget::~InspectorWidget() {}

	void InspectorWidget::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::SELECTION_CHANGE )
		{
			refresh();
		}
		else if ( p_event.name == Event::Global::MOLECULE_REMOVED )
		{
			const Event::VTXEventPtr<Model::Molecule> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );

			if ( _moleculesInspector->hasTarget( castedEvent.ptr ) )
			{
				_moleculesInspector->removeTarget( castedEvent.ptr );

				if ( _moleculesInspector->getTargets().size() <= 0 )
					_moleculesInspector->setVisible( false );

				refresh();
			}
		}
		else if ( p_event.name == Event::Global::CHAIN_REMOVED )
		{
			const Event::VTXEventPtr<Model::Chain> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Chain> &>( p_event );

			if ( _chainsInspector->hasTarget( castedEvent.ptr ) )
			{
				_chainsInspector->removeTarget( castedEvent.ptr );

				if ( _chainsInspector->getTargets().size() <= 0 )
					_chainsInspector->setVisible( false );

				refresh();
			}
		}
		else if ( p_event.name == Event::Global::RESIDUE_REMOVED )
		{
			const Event::VTXEventPtr<Model::Residue> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Residue> &>( p_event );

			if ( _residuesInspector->hasTarget( castedEvent.ptr ) )
			{
				_residuesInspector->removeTarget( castedEvent.ptr );

				if ( _residuesInspector->getTargets().size() <= 0 )
					_residuesInspector->setVisible( false );

				refresh();
			}
		}
		else if ( p_event.name == Event::Global::ATOM_REMOVED )
		{
			const Event::VTXEventPtr<Model::Atom> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Atom> &>( p_event );

			if ( _atomsInspector->hasTarget( castedEvent.ptr ) )
			{
				_atomsInspector->removeTarget( castedEvent.ptr );

				if ( _atomsInspector->getTargets().size() <= 0 )
					_atomsInspector->setVisible( false );

				refresh();
			}
		}
		else if ( p_event.name == Event::Global::VIEWPOINT_REMOVED )
		{
			const Event::VTXEventPtr<Model::Viewpoint> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Viewpoint> &>( p_event );

			if ( _viewpointsInspector->hasTarget( castedEvent.ptr ) )
			{
				_viewpointsInspector->removeTarget( castedEvent.ptr );

				if ( _viewpointsInspector->getTargets().size() <= 0 )
					_viewpointsInspector->setVisible( false );

				refresh();
			}
		}
	}

	void InspectorWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		CustomWidget::DockWindowMainWidget<QWidget> * body = new CustomWidget::DockWindowMainWidget<QWidget>(
			Style::INSPECTOR_PREFERRED_SIZE, Style::INSPECTOR_MINIMUM_SIZE, this );

		QVBoxLayout * const mainLayout = new QVBoxLayout( body );

		QFrame * const toolbar = new QFrame( this );
		toolbar->setContentsMargins( 0, 0, 0, 0 );
		toolbar->setFixedHeight( 30 );
		toolbar->setObjectName( "Inspector_Toolbar" );
		toolbar->setFrameShadow( QFrame::Shadow::Raised );
		toolbar->setFrameShape( QFrame::Shape::Box );

		QHBoxLayout * const toolbarLayout = new QHBoxLayout( toolbar );
		toolbarLayout->setContentsMargins( 5, 1, 5, 1 );
		_freezeButton = new QPushButton( toolbar );
		_freezeButton->setFlat( true );
		_updateFreezedFeedback();

		toolbarLayout->addStretch( 1000 );
		toolbarLayout->addWidget( _freezeButton );

		_scrollWidget = new QWidget( this );

		_verticalLayout = new QVBoxLayout( _scrollWidget );
		_verticalLayout->setSizeConstraint( QLayout::SizeConstraint::SetMinAndMaxSize );
		_verticalLayout->setSpacing( 4 );
		_verticalLayout->setContentsMargins( 0, 0, 0, 0 );

		_scrollArea = new QScrollArea( body );

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

		mainLayout->addWidget( _scrollArea, 1000 );
		mainLayout->addWidget( toolbar );

		setWidget( body );
		clear();
	}
	void InspectorWidget::_setupSlots()
	{
		connect( _freezeButton, &QPushButton::clicked, this, &InspectorWidget::_toggleFreezeState );
	}

	void InspectorWidget::refresh()
	{
		if ( !isFreezed() )
		{
			clear();

			const Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();

			for ( const Model::ID & modelID : selectionModel.getItems() )
			{
				const ID::VTX_ID & modelTypeID = MVC::MvcManager::get().getModelTypeID( modelID );

				if ( modelTypeID == VTX::ID::Model::MODEL_MOLECULE )
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
						for ( const Model::Selection::PairChainIds & chainData : moleculeSelection )
						{
							Model::Chain * const chain = molecule.getChain( chainData.first );

							if ( chainData.second.getFullySelectedChildCount() == chain->getRealResidueCount() )
							{
								_chainsInspector->addTarget( chain );
								_chainsInspector->setVisible( true );
							}
							else
							{
								for ( const Model::Selection::PairResidueIds & residueData : chainData.second )
								{
									Model::Residue * const residue = molecule.getResidue( residueData.first );

									if ( residueData.second.getFullySelectedChildCount()
										 == residue->getRealAtomCount() )
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
				else if ( modelTypeID == VTX::ID::Model::MODEL_VIEWPOINT )
				{
					Model::Viewpoint & viewpoint = MVC::MvcManager::get().getModel<Model::Viewpoint>( modelID );
					_viewpointsInspector->addTarget( &viewpoint );
					_viewpointsInspector->setVisible( true );
				}
			}
		}

		bool allInspectorHidden = true;
		if ( _moleculesInspector->isVisible() )
		{
			allInspectorHidden = false;
			_moleculesInspector->refresh();
		}
		if ( _chainsInspector->isVisible() )
		{
			allInspectorHidden = false;
			_chainsInspector->refresh();
		}
		if ( _residuesInspector->isVisible() )
		{
			allInspectorHidden = false;
			_residuesInspector->refresh();
		}
		if ( _atomsInspector->isVisible() )
		{
			allInspectorHidden = false;
			_atomsInspector->refresh();
		}
		if ( _viewpointsInspector->isVisible() )
		{
			allInspectorHidden = false;
			_viewpointsInspector->refresh();
		}

		// Unfreeze Inspector when every frozen items are destroyed
		if ( allInspectorHidden )
			setFreezed( false );
	}

	void InspectorWidget::clear()
	{
		if ( !isFreezed() )
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
	}
	void InspectorWidget::localize()
	{
		this->setWindowTitle( "Inspector" );
		// this->setWindowTitle( QCoreApplication::translate( "InspectorWidget", "Inspector", nullptr ) );
	}

	void InspectorWidget::setFreezed( const bool p_freezed )
	{
		if ( _isFreezed != p_freezed )
		{
			_isFreezed = p_freezed;
			_updateFreezedFeedback();

			if ( !_isFreezed )
			{
				refresh();
			}
		}
	}

	void InspectorWidget::_toggleFreezeState() { setFreezed( !_isFreezed ); }

	void InspectorWidget::_updateFreezedFeedback()
	{
		const QIcon & freezedIcon
			= _isFreezed ? QIcon( ":/sprite/locked_icon.png" ) : QIcon( ":/sprite/unlocked_icon.png" );
		_freezeButton->setIcon( freezedIcon );
	}

} // namespace VTX::UI::Widget::Inspector
