#include "ui/old_ui/ui/widget/inspector/inspector_widget.hpp"
#include "ui/old_ui/style.hpp"
#include "ui/old_ui/ui/widget/inspector/multiple_atom_inspector_widget.hpp"
#include "ui/old_ui/ui/widget/inspector/multiple_chain_inspector_widget.hpp"
// #include "ui/old_ui/ui/widget/inspector/multiple_measurement_angle_inspector_widget.hpp"
// #include "ui/old_ui/ui/widget/inspector/multiple_measurement_dihedral_angle_inspector_widget.hpp"
// #include "ui/old_ui/ui/widget/inspector/multiple_measurement_distance_inspector_widget.hpp"
// #include "ui/old_ui/ui/widget/inspector/multiple_measurement_distance_to_cycle_inspector_widget.hpp"
#include "ui/old_ui/ui/widget/inspector/multiple_molecule_inspector_widget.hpp"
#include "ui/old_ui/ui/widget/inspector/multiple_residue_inspector_widget.hpp"
#include "ui/old_ui/ui/widget/inspector/multiple_viewpoint_inspector_widget.hpp"
#include <QFrame>
#include <QHBoxLayout>
#include <app/old/application/representation/representation_library.hpp>
#include <app/old/application/representation/representation_manager.hpp>
#include <app/old/application/representation/representation_preset.hpp>
#include <app/old/application/selection/selection.hpp>
#include <app/old/application/selection/selection_manager.hpp>
#include <app/old/component/chemistry/atom.hpp>
#include <app/old/component/chemistry/chain.hpp>
#include <app/old/component/chemistry/molecule.hpp>
#include <app/old/component/chemistry/residue.hpp>
#include <app/old/component/object3d/label.hpp>
#include <app/old/component/object3d/viewpoint.hpp>
#include <app/old/event/global.hpp>
// #include <tool/old_tool/model/measurement/angle.hpp>
// #include <tool/old_tool/model/measurement/dihedral_angle.hpp>
// #include <tool/old_tool/model/measurement/distance.hpp>
// #include <tool/old_tool/model/measurement/distance_to_cycle.hpp>
#include <unordered_set>

namespace VTX::UI::Widget::Inspector
{
	InspectorWidget::InspectorWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
	{
		_registerEvent( VTX::App::Old::Event::Global::SELECTION_CHANGE );
		_registerEvent( VTX::App::Old::Event::Global::MOLECULE_REMOVED );
		_registerEvent( VTX::App::Old::Event::Global::CHAIN_REMOVED );
		_registerEvent( VTX::App::Old::Event::Global::RESIDUE_REMOVED );
		_registerEvent( VTX::App::Old::Event::Global::ATOM_REMOVED );
		_registerEvent( VTX::App::Old::Event::Global::VIEWPOINT_REMOVED );
		_registerEvent( VTX::App::Old::Event::Global::LABEL_REMOVED );
	}

	InspectorWidget::~InspectorWidget() {}

	void InspectorWidget::receiveEvent( const VTX::App::Old::Core::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::App::Old::Event::Global::SELECTION_CHANGE )
		{
			refresh();
		}
		else if ( p_event.name == VTX::App::Old::Event::Global::MOLECULE_REMOVED )
		{
			const VTX::App::Old::Core::Event::VTXEventArg<App::Old::Component::Chemistry::Molecule *> & castedEvent
				= dynamic_cast<const VTX::App::Old::Core::Event::VTXEventArg<App::Old::Component::Chemistry::Molecule *> &>(
					p_event );

			_removeTargetToInspector<MultipleMoleculeWidget>( INSPECTOR_TYPE::MOLECULE, castedEvent.get() );
		}
		else if ( p_event.name == VTX::App::Old::Event::Global::CHAIN_REMOVED )
		{
			const VTX::App::Old::Core::Event::VTXEventArg<App::Old::Component::Chemistry::Chain *> & castedEvent
				= dynamic_cast<const VTX::App::Old::Core::Event::VTXEventArg<App::Old::Component::Chemistry::Chain *> &>(
					p_event );

			_removeTargetToInspector<MultipleChainWidget>( INSPECTOR_TYPE::CHAIN, castedEvent.get() );
		}
		else if ( p_event.name == VTX::App::Old::Event::Global::RESIDUE_REMOVED )
		{
			const VTX::App::Old::Core::Event::VTXEventArg<App::Old::Component::Chemistry::Residue *> & castedEvent
				= dynamic_cast<const VTX::App::Old::Core::Event::VTXEventArg<App::Old::Component::Chemistry::Residue *> &>(
					p_event );

			_removeTargetToInspector<MultipleResidueWidget>( INSPECTOR_TYPE::RESIDUE, castedEvent.get() );
		}
		else if ( p_event.name == VTX::App::Old::Event::Global::ATOM_REMOVED )
		{
			const VTX::App::Old::Core::Event::VTXEventArg<App::Old::Component::Chemistry::Atom *> & castedEvent
				= dynamic_cast<const VTX::App::Old::Core::Event::VTXEventArg<App::Old::Component::Chemistry::Atom *> &>(
					p_event );

			_removeTargetToInspector<MultipleAtomWidget>( INSPECTOR_TYPE::ATOM, castedEvent.get() );
		}
		else if ( p_event.name == VTX::App::Old::Event::Global::VIEWPOINT_REMOVED )
		{
			const VTX::App::Old::Core::Event::VTXEventArg<App::Old::Component::Object3D::Viewpoint *> & castedEvent
				= dynamic_cast<const VTX::App::Old::Core::Event::VTXEventArg<App::Old::Component::Object3D::Viewpoint *> &>(
					p_event );

			_removeTargetToInspector<MultipleViewpointWidget>( INSPECTOR_TYPE::VIEWPOINT, castedEvent.get() );
		}
		else if ( p_event.name == VTX::App::Old::Event::Global::LABEL_REMOVED )
		{
			// const App::Old::Core::Event::VTXEventArg<App::Old::Component::Object3D::Label*> & castedEvent
			//	= dynamic_cast<const App::Old::Core::Event::VTXEventArg<App::Old::Component::Object3D::Label*> &>( p_event );

			// const App::Old::VTX_ID & labelTypeID = castedEvent.ptr->getTypeId();
			// if ( labelTypeID == ID::Model::MODEL_MEASUREMENT_DISTANCE )
			//{
			//	Model::Measurement::Distance * const distanceModel
			//		= dynamic_cast<Model::Measurement::Distance *>( castedEvent.ptr );
			//	_removeTargetToInspector<MultipleMeasurmentDistanceWidget>( INSPECTOR_TYPE::MEASURE_DISTANCE,
			//																distanceModel );
			// }
			//// else if ( labelTypeID == ID::Model::MODEL_MEASUREMENT_DISTANCE_TO_CYCLE )
			////{
			////	Model::Measurement::DistanceToCycle * const angleModel
			////		= dynamic_cast<Model::Measurement::DistanceToCycle *>( castedEvent.ptr );
			////	_removeTargetToInspector<MultipleMeasurmentDistanceToCycleWidget>(
			////		INSPECTOR_TYPE::MEASURE_DISTANCE_TO_CYCLE, angleModel );
			////}
			// else if ( labelTypeID == ID::Model::MODEL_MEASUREMENT_ANGLE )
			//{
			//	Model::Measurement::Angle * const angleModel
			//		= dynamic_cast<Model::Measurement::Angle *>( castedEvent.ptr );
			//	_removeTargetToInspector<MultipleMeasurmentAngleWidget>( INSPECTOR_TYPE::MEASURE_ANGLE, angleModel );
			// }
			// else if ( labelTypeID == ID::Model::MODEL_MEASUREMENT_DIHEDRAL_ANGLE )
			//{
			//	Model::Measurement::DihedralAngle * const angleModel
			//		= dynamic_cast<Model::Measurement::DihedralAngle *>( castedEvent.ptr );
			//	_removeTargetToInspector<MultipleMeasurmentDihedralAngleWidget>( INSPECTOR_TYPE::MEASURE_DIHEDRAL_ANGLE,
			//																	 angleModel );
			// }
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

		_inspectors.resize( int( INSPECTOR_TYPE::COUNT ) );

		_inspectors[ int( INSPECTOR_TYPE::MOLECULE ) ]
			= WidgetFactory::get().instantiateWidget<MultipleMoleculeWidget>( this, "multipleMoleculeInspector" );
		_inspectors[ int( INSPECTOR_TYPE::CHAIN ) ]
			= WidgetFactory::get().instantiateWidget<MultipleChainWidget>( this, "multipleChainInspector" );
		_inspectors[ int( INSPECTOR_TYPE::RESIDUE ) ]
			= WidgetFactory::get().instantiateWidget<MultipleResidueWidget>( this, "multipleResidueInspector" );
		_inspectors[ int( INSPECTOR_TYPE::ATOM ) ]
			= WidgetFactory::get().instantiateWidget<MultipleAtomWidget>( this, "multipleAtomInspector" );
		_inspectors[ int( INSPECTOR_TYPE::VIEWPOINT ) ]
			= WidgetFactory::get().instantiateWidget<MultipleViewpointWidget>( this, "multipleViewpointInspector" );
		//_inspectors[ int( INSPECTOR_TYPE::MEASURE_DISTANCE ) ]
		//	= WidgetFactory::get().instantiateWidget<MultipleMeasurmentDistanceWidget>(
		//		this, "multipleMeasurementDistanceInspector" );
		//_inspectors[ int( INSPECTOR_TYPE::MEASURE_DISTANCE_TO_CYCLE ) ]
		//	= WidgetFactory::get().instantiateWidget<MultipleMeasurmentDistanceWidget>(
		//		this, "multipleMeasurementAngleInspector" );
		//_inspectors[ int( INSPECTOR_TYPE::MEASURE_ANGLE ) ]
		//	= WidgetFactory::get().instantiateWidget<MultipleMeasurmentAngleWidget>(
		//		this, "multipleMeasurementAngleInspector" );
		//_inspectors[ int( INSPECTOR_TYPE::MEASURE_DIHEDRAL_ANGLE ) ]
		//	= WidgetFactory::get().instantiateWidget<MultipleMeasurmentDihedralAngleWidget>(
		//		this, "multipleMeasurementAngleInspector" );

		for ( InspectorItemWidget * const inspector : _inspectors )
		{
			_verticalLayout->addWidget( inspector );
		}
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

			const App::Old::Application::Selection::SelectionModel & selectionModel
				= VTX::App::Old::Application::Selection::SelectionManager::get().getSelectionModel();

			for ( const App::Old::Core::Model::ID & modelID : selectionModel.getItems() )
			{
				const App::Old::VTX_ID & modelTypeID = VTX::MVC_MANAGER().getModelTypeID( modelID );

				if ( modelTypeID == App::Old::ID::Model::MODEL_MOLECULE )
				{
					App::Old::Component::Chemistry::Molecule & molecule
						= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( modelID );
					const App::Old::Application::Selection::SelectionModel::MapChainIds & moleculeSelection
						= selectionModel.getMoleculesMap().at( modelID );

					if ( moleculeSelection.getFullySelectedChildCount() == molecule.getRealChainCount() )
					{
						_addTargetToInspector<MultipleMoleculeWidget>( INSPECTOR_TYPE::MOLECULE, &molecule );
					}
					else
					{
						for ( const App::Old::Application::Selection::SelectionModel::PairChainIds & chainData :
							  moleculeSelection )
						{
							App::Old::Component::Chemistry::Chain * const chain = molecule.getChain( chainData.first );

							if ( chainData.second.getFullySelectedChildCount() == chain->getRealResidueCount() )
							{
								_addTargetToInspector<MultipleChainWidget>( INSPECTOR_TYPE::CHAIN, chain );
							}
							else
							{
								for ( const App::Old::Application::Selection::SelectionModel::PairResidueIds & residueData :
									  chainData.second )
								{
									App::Old::Component::Chemistry::Residue * const residue
										= molecule.getResidue( residueData.first );

									if ( residueData.second.getFullySelectedChildCount()
										 == residue->getRealAtomCount() )
									{
										_addTargetToInspector<MultipleChainWidget>( INSPECTOR_TYPE::RESIDUE, residue );
									}
									else
									{
										for ( const uint & atomID : residueData.second )
										{
											App::Old::Component::Chemistry::Atom * const atom = molecule.getAtom( atomID );
											_addTargetToInspector<MultipleAtomWidget>( INSPECTOR_TYPE::ATOM, atom );
										}
									}
								}
							}
						}
					}
				}
				else if ( modelTypeID == App::Old::ID::Model::MODEL_VIEWPOINT )
				{
					App::Old::Component::Object3D::Viewpoint & viewpoint
						= VTX::MVC_MANAGER().getModel<App::Old::Component::Object3D::Viewpoint>( modelID );
					_addTargetToInspector<MultipleViewpointWidget>( INSPECTOR_TYPE::VIEWPOINT, &viewpoint );
				}
				// else if ( modelTypeID == App::Old::ID::Model::MODEL_MEASUREMENT_DISTANCE )
				//{
				//	Model::Measurement::Distance & distanceModel
				//		= VTX::MVC_MANAGER().getModel<Model::Measurement::Distance>( modelID );
				//	_addTargetToInspector<MultipleMeasurmentDistanceWidget>( INSPECTOR_TYPE::MEASURE_DISTANCE,
				//															 &distanceModel );
				// }
				// else if ( modelTypeID == App::Old::ID::Model::MODEL_MEASUREMENT_ANGLE )
				//{
				//	Model::Measurement::Angle & angleModel
				//		= VTX::MVC_MANAGER().getModel<Model::Measurement::Angle>( modelID );
				//	_addTargetToInspector<MultipleMeasurmentAngleWidget>( INSPECTOR_TYPE::MEASURE_ANGLE, &angleModel );
				// }
				// else if ( modelTypeID == App::Old::ID::Model::MODEL_MEASUREMENT_DIHEDRAL_ANGLE )
				//{
				//	Model::Measurement::DihedralAngle & dihedralAngleModel
				//		= VTX::MVC_MANAGER().getModel<Model::Measurement::DihedralAngle>( modelID );
				//	_addTargetToInspector<MultipleMeasurmentDihedralAngleWidget>(
				//		INSPECTOR_TYPE::MEASURE_DIHEDRAL_ANGLE, &dihedralAngleModel );
				// }
				// else if ( modelTypeID == App::Old::ID::Model::MODEL_MEASUREMENT_DISTANCE_TO_CYCLE )
				//{
				//	Model::Measurement::Distance & distanceModel
				//		= VTX::MVC_MANAGER().getModel<Model::Measurement::Distance>( modelID );
				//	_addTargetToInspector<MultipleMeasurmentDistanceWidget>( INSPECTOR_TYPE::MEASURE_DISTANCE,
				//															 &distanceModel );
				// }
			}
		}

		bool allInspectorHidden = true;
		for ( InspectorItemWidget * inspector : _inspectors )
		{
			const bool inspectorIsVisible = inspector->isVisible();
			allInspectorHidden			  = allInspectorHidden && !inspectorIsVisible;

			if ( inspectorIsVisible )
				inspector->refresh();
		}

		// Unfreeze Inspector when every frozen items are destroyed
		if ( allInspectorHidden )
			setFreezed( false );
	}

	void InspectorWidget::clear()
	{
		if ( !isFreezed() )
		{
			for ( InspectorItemWidget * inspector : _inspectors )
			{
				inspector->clearTargets();
				inspector->setVisible( false );
			}

			_inspectorViewsData.clear();
		}
	}

	void InspectorWidget::forceInspector( const INSPECTOR_TYPE & p_type )
	{
		clear();
		const App::Old::Application::Selection::SelectionModel & selectionModel
			= VTX::App::Old::Application::Selection::SelectionManager::get().getSelectionModel();
		switch ( p_type )
		{
		case INSPECTOR_TYPE::MOLECULE:
		{
			for ( const App::Old::Core::Model::ID & modelID : selectionModel.getItems() )
			{
				const App::Old::VTX_ID & modelTypeID = VTX::MVC_MANAGER().getModelTypeID( modelID );
				if ( modelTypeID == App::Old::ID::Model::MODEL_MOLECULE )
				{
					App::Old::Component::Chemistry::Molecule & molecule
						= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( modelID );
					_addTargetToInspector<MultipleMoleculeWidget>( p_type, &molecule );
				}
			}
		}
		break;
		case INSPECTOR_TYPE::CHAIN:
		{
			for ( const App::Old::Core::Model::ID & modelID : selectionModel.getItems() )
			{
				App::Old::Component::Chemistry::Molecule & molecule
					= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( modelID );
				const App::Old::Application::Selection::SelectionModel::MapChainIds & moleculeSelection
					= selectionModel.getMoleculesMap().at( modelID );

				for ( const App::Old::Application::Selection::SelectionModel::PairChainIds & chainData : moleculeSelection )
				{
					App::Old::Component::Chemistry::Chain * const chain = molecule.getChain( chainData.first );
					_addTargetToInspector<MultipleChainWidget>( INSPECTOR_TYPE::CHAIN, chain );
				}
			}
		}
		break;
		case INSPECTOR_TYPE::RESIDUE:
		{
			for ( const App::Old::Core::Model::ID & modelID : selectionModel.getItems() )
			{
				App::Old::Component::Chemistry::Molecule & molecule
					= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( modelID );
				const App::Old::Application::Selection::SelectionModel::MapChainIds & moleculeSelection
					= selectionModel.getMoleculesMap().at( modelID );

				for ( const App::Old::Application::Selection::SelectionModel::PairChainIds & chainData : moleculeSelection )
				{
					const App::Old::Component::Chemistry::Chain * const chain = molecule.getChain( chainData.first );

					for ( const App::Old::Application::Selection::SelectionModel::PairResidueIds & residueData :
						  chainData.second )
					{
						App::Old::Component::Chemistry::Residue * const residue = molecule.getResidue( residueData.first );
						_addTargetToInspector<MultipleResidueWidget>( INSPECTOR_TYPE::RESIDUE, residue );
					}
				}
			}
		}
		break;
		case INSPECTOR_TYPE::ATOM:
		{
			for ( const App::Old::Core::Model::ID & modelID : selectionModel.getItems() )
			{
				App::Old::Component::Chemistry::Molecule & molecule
					= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( modelID );
				const App::Old::Application::Selection::SelectionModel::MapChainIds & moleculeSelection
					= selectionModel.getMoleculesMap().at( modelID );

				for ( const App::Old::Application::Selection::SelectionModel::PairChainIds & chainData : moleculeSelection )
				{
					const App::Old::Component::Chemistry::Chain * const chain = molecule.getChain( chainData.first );

					for ( const App::Old::Application::Selection::SelectionModel::PairResidueIds & residueData :
						  chainData.second )
					{
						const App::Old::Component::Chemistry::Residue * const residue
							= molecule.getResidue( residueData.first );

						for ( const uint & atomID : residueData.second )
						{
							App::Old::Component::Chemistry::Atom * const atom = molecule.getAtom( atomID );
							_addTargetToInspector<MultipleAtomWidget>( INSPECTOR_TYPE::ATOM, atom );
						}
					}
				}
			}
		}
		break;
		case INSPECTOR_TYPE::VIEWPOINT:
		{
			for ( const App::Old::Core::Model::ID & modelID : selectionModel.getItems() )
			{
				App::Old::Component::Object3D::Viewpoint & viewpoint
					= VTX::MVC_MANAGER().getModel<App::Old::Component::Object3D::Viewpoint>( modelID );
				_addTargetToInspector<MultipleViewpointWidget>( INSPECTOR_TYPE::VIEWPOINT, &viewpoint );
			}
		}
		break;
			// case INSPECTOR_TYPE::MEASURE_DISTANCE:
			//{
			//	for ( const App::Old::Core::Model::ID & modelID : selectionModel.getItems() )
			//	{
			//		Model::Measurement::Distance & distanceModel
			//			= VTX::MVC_MANAGER().getModel<Model::Measurement::Distance>( modelID );
			//		_addTargetToInspector<MultipleMeasurmentDistanceWidget>( INSPECTOR_TYPE::MEASURE_DISTANCE,
			//																 &distanceModel );
			//	}
			// }
			// break;
			// case INSPECTOR_TYPE::MEASURE_ANGLE:
			//{
			//	for ( const App::Old::Core::Model::ID & modelID : selectionModel.getItems() )
			//	{
			//		Model::Measurement::Angle & angleModel
			//			= VTX::MVC_MANAGER().getModel<Model::Measurement::Angle>( modelID );
			//		_addTargetToInspector<MultipleMeasurmentAngleWidget>( INSPECTOR_TYPE::MEASURE_ANGLE, &angleModel );
			//	}
			// }
			// break;
			// case INSPECTOR_TYPE::MEASURE_DIHEDRAL_ANGLE:
			//{
			//	for ( const App::Old::Core::Model::ID & modelID : selectionModel.getItems() )
			//	{
			//		Model::Measurement::DihedralAngle & dihedralAngleModel
			//			= VTX::MVC_MANAGER().getModel<Model::Measurement::DihedralAngle>( modelID );
			//		_addTargetToInspector<MultipleMeasurmentDihedralAngleWidget>(
			// INSPECTOR_TYPE::MEASURE_DIHEDRAL_ANGLE,
			// &dihedralAngleModel
			// );
			//	}
			// }
			// break;
			// case INSPECTOR_TYPE::MEASURE_DISTANCE_TO_CYCLE:
			//{
			//	for ( const App::Old::Core::Model::ID & modelID : selectionModel.getItems() )
			//	{
			//		Model::Measurement::Distance & distanceModel
			//			= VTX::MVC_MANAGER().getModel<Model::Measurement::Distance>( modelID );
			//		_addTargetToInspector<MultipleMeasurmentDistanceWidget>( INSPECTOR_TYPE::MEASURE_DISTANCE,
			//																 &distanceModel );
			//	}
			// }
			// break;
		}

		for ( InspectorItemWidget * inspector : _inspectors )
		{
			if ( inspector->isVisible() )
				inspector->refresh();
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
