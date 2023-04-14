#include "multiple_measurement_distance_to_cycle_inspector_widget.hpp"
#include "action/action_manager.hpp"
#include "action/atom.hpp"
#include "action/label.hpp"
#include <app/old_app/model/measurement/distance.hpp"
#include "old_ui/ui/widget_factory.hpp"
#include "util/measurement.hpp"

namespace VTX::UI::Widget::Inspector
{
	// MultipleMeasurmentDistanceToCycleWidget::MultipleMeasurmentDistanceToCycleWidget( QWidget * p_parent ) :
	//	MultipleModelInspectorWidget( p_parent, ID::View::UI_INSPECTOR_VIEWPOINT ) {};

	// MultipleMeasurmentDistanceToCycleWidget::~MultipleMeasurmentDistanceToCycleWidget() {}

	// void MultipleMeasurmentDistanceToCycleWidget::_setupUi( const QString & p_name )
	//{
	//	MultipleModelInspectorWidget::_setupUi( p_name );

	//	_positionSection = VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSectionVLayout>(
	//		this, "inspector_item_section" );

	//	_positionInfoWidget = UI::WidgetFactory::get().instantiateWidget<CustomWidget::Vector3Widget>(
	//		this, "inspector_measurement_distance_vector" );
	//	_positionInfoWidget->setEnabled( false );
	//	_positionSection->setBody( _positionInfoWidget );

	//	_infoSection = VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSectionVLayout>(
	//		this, "inspector_item_section" );

	//	QWidget * const fullNameWidget = new QWidget( this );

	//	QHBoxLayout * const nameLayout = new QHBoxLayout( fullNameWidget );

	//	_nameWidget		   = new CustomWidget::QLineEditMultiField( this );
	//	_setAutoNameWidget = new QPushButton( this );
	//	_setAutoNameWidget->setText( "Auto" );
	//	nameLayout->addWidget( _nameWidget );
	//	nameLayout->addWidget( _setAutoNameWidget );

	//	_distanceLabelWidget = new CustomWidget::QLabelMultiField( this );

	//	QWidget * const goToAtomButtons = new QWidget( this );
	//	QHBoxLayout *	goToAtomLayout	= new QHBoxLayout( goToAtomButtons );

	//	_goToFirstAtom	= new QPushButton( goToAtomButtons );
	//	_goToSecondAtom = new QPushButton( goToAtomButtons );

	//	goToAtomLayout->addWidget( _goToFirstAtom );
	//	goToAtomLayout->addSpacing( 10 );
	//	goToAtomLayout->addWidget( _goToSecondAtom );

	//	_infoSection->appendField( "Name", fullNameWidget );
	//	_infoSection->appendField( "Atoms", goToAtomButtons );
	//	_infoSection->appendField( "Distance", _distanceLabelWidget );

	//	_appendWidget( _positionSection );
	//	_appendWidget( _infoSection );

	//	const bool oldBlockState = blockSignals( true );
	//	refresh();
	//	blockSignals( oldBlockState );
	//}

	// void MultipleMeasurmentDistanceToCycleWidget::_setupSlots()
	//{
	//	connect( _nameWidget,
	//			 &CustomWidget::QLineEditMultiField::editingFinished,
	//			 this,
	//			 &MultipleMeasurmentDistanceToCycleWidget::_renameAction );

	//	connect( _setAutoNameWidget,
	//			 &QPushButton::clicked,
	//			 this,
	//			 &MultipleMeasurmentDistanceToCycleWidget::_setAutoNameAction );

	//	connect(
	//		_goToFirstAtom, &QPushButton::clicked, this, &MultipleMeasurmentDistanceToCycleWidget::_orientOnFirstAtom );
	//	connect( _goToSecondAtom,
	//			 &QPushButton::clicked,
	//			 this,
	//			 &MultipleMeasurmentDistanceToCycleWidget::_orientOnSecondAtom );
	//};

	// void MultipleMeasurmentDistanceToCycleWidget::_endOfFrameRefresh( const SectionFlag & p_flag )
	//{
	//	MultipleModelInspectorWidget::_endOfFrameRefresh( p_flag );

	//	_resetFieldStates( p_flag );

	//	const std::unordered_set<Model::Measurement::DistanceToCycle *> & targets = getTargets();

	//	const bool goToAtomButtonsVisible = targets.size() == 1;

	//	_goToFirstAtom->setVisible( goToAtomButtonsVisible );
	//	_goToSecondAtom->setVisible( goToAtomButtonsVisible );

	//	if ( targets.size() > 0 )
	//	{
	//		const QString headerTitle = QString::fromStdString( "Distance (" + std::to_string( targets.size() ) + ")" );
	//		_getHeader()->setHeaderTitle( headerTitle );

	//		const QPixmap * symbolPixmap
	//			= Style::IconConst::get().getModelSymbol( VTX::ID::Model::MODEL_MEASUREMENT_DISTANCE_TO_CYCLE );
	//		_getHeader()->setHeaderIcon( *symbolPixmap );

	//		for ( const Model::Measurement::DistanceToCycle * distanceModel : targets )
	//		{
	//			if ( bool( p_flag & SectionFlag::INFOS ) )
	//			{
	//				const Vec3f labelWorldPosition = distanceModel->getPosition();

	//				_positionInfoWidget->updateWithNewValue( labelWorldPosition );
	//				_nameWidget->updateWithNewValue( distanceModel->getDefaultName() );
	//				const std::string distanceStr = Util::Measurement::getDistanceString( *distanceModel );
	//				_distanceLabelWidget->updateWithNewValue( distanceStr );

	//				if ( distanceModel->getTypeId() == ID::Model::MODEL_MEASUREMENT_DISTANCE_TO_CYCLE )
	//				{
	//					if ( goToAtomButtonsVisible )
	//					{
	//						_goToFirstAtom->setText( QString::fromStdString(
	//							distanceModel->getFirstAtom().getName() + " "
	//							+ std::to_string( distanceModel->getFirstAtom().getIndex() ) ) );
	//						_goToSecondAtom->setText( QString::fromStdString(
	//							distanceModel->getSecondAtom().getName() + " "
	//							+ std::to_string( distanceModel->getFirstAtom().getIndex() ) ) );
	//					}
	//				}
	//			}
	//		}
	//	}
	//}

	// void MultipleMeasurmentDistanceToCycleWidget::_orientOnFirstAtom() const
	//{
	//	const Model::Measurement::Distance * const distance
	//		= dynamic_cast<const Model::Measurement::Distance *>( *( getTargets().begin() ) );

	//	VTX_ACTION( new Action::Atom::Orient( distance->getFirstAtom() ) );
	//}
	// void MultipleMeasurmentDistanceToCycleWidget::_orientOnSecondAtom() const
	//{
	//	const Model::Measurement::Distance * const distance
	//		= dynamic_cast<const Model::Measurement::Distance *>( *( getTargets().begin() ) );

	//	VTX_ACTION( new Action::Atom::Orient( distance->getSecondAtom() ) );
	//}

	// void MultipleMeasurmentDistanceToCycleWidget::_renameAction() const
	//{
	//	std::unordered_set<Model::Label *> labelTargets = std::unordered_set<Model::Label *>();
	//	for ( Model::Measurement::DistanceToCycle * const targetDistance : getTargets() )
	//		labelTargets.emplace( targetDistance );

	//	VTX_ACTION( new Action::Label::Rename( labelTargets, _nameWidget->text().toStdString() ) );
	//}

	// void MultipleMeasurmentDistanceToCycleWidget::_setAutoNameAction() const
	//{
	//	std::unordered_set<Model::Label *> labelTargets = std::unordered_set<Model::Label *>();
	//	for ( Model::Measurement::DistanceToCycle * const targetDistance : getTargets() )
	//		labelTargets.emplace( targetDistance );

	//	VTX_ACTION( new Action::Label::EnableAutoNaming( labelTargets, true ) );
	//}

	// void MultipleMeasurmentDistanceToCycleWidget::_resetFieldStates( const SectionFlag & p_flag )
	//{
	//	if ( bool( p_flag & SectionFlag::INFOS ) )
	//	{
	//		_positionInfoWidget->resetState();
	//		_nameWidget->resetState();
	//		_distanceLabelWidget->resetState();
	//		_goToFirstAtom->setVisible( false );
	//		_goToSecondAtom->setVisible( false );
	//	}
	//}

	// void MultipleMeasurmentDistanceToCycleWidget::localize()
	//{
	//	_positionSection->setHeaderTitle( "Position" );
	//	_infoSection->setHeaderTitle( "Informations" );
	//	_positionSection->localize();
	//	_infoSection->localize();
	//}
} // namespace VTX::UI::Widget::Inspector
