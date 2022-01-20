#include "multiple_measurement_dihedral_angle_inspector_widget.hpp"
#include "action/action_manager.hpp"
#include "action/atom.hpp"
#include "action/label.hpp"
#include "model/measurement/distance.hpp"
#include "ui/widget_factory.hpp"
#include "util/measurement.hpp"

namespace VTX::UI::Widget::Inspector
{
	// MultipleMeasurmentDihedralAngleWidget::MultipleMeasurmentDihedralAngleWidget( QWidget * p_parent ) :
	//	MultipleModelInspectorWidget( p_parent, ID::View::UI_INSPECTOR_VIEWPOINT ) {};

	// MultipleMeasurmentDihedralAngleWidget::~MultipleMeasurmentDihedralAngleWidget() {}

	// void MultipleMeasurmentDihedralAngleWidget::_setupUi( const QString & p_name )
	//{
	//	MultipleModelInspectorWidget::_setupUi( p_name );

	//	_positionSection = VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSectionVLayout>(
	//		this, "inspector_item_section" );

	//	_positionInfoWidget = UI::WidgetFactory::get().instantiateWidget<CustomWidget::Vector3Widget>(
	//		this, "inspector_label_position" );
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

	//	_dihedralAngleLabelWidget = new CustomWidget::QLabelMultiField( this );

	//	QWidget * const goToAtomButtons = new QWidget( this );
	//	QHBoxLayout *	goToAtomLayout	= new QHBoxLayout( goToAtomButtons );

	//	_goToFirstAtom	= new QPushButton( goToAtomButtons );
	//	_goToSecondAtom = new QPushButton( goToAtomButtons );

	//	goToAtomLayout->addWidget( _goToFirstAtom );
	//	goToAtomLayout->addSpacing( 10 );
	//	goToAtomLayout->addWidget( _goToSecondAtom );

	//	_infoSection->appendField( "Name", fullNameWidget );
	//	_infoSection->appendField( "Atoms", goToAtomButtons );
	//	_infoSection->appendField( "Dihedral angle", _dihedralAngleLabelWidget );

	//	_appendWidget( _positionSection );
	//	_appendWidget( _infoSection );

	//	const bool oldBlockState = blockSignals( true );
	//	refresh();
	//	blockSignals( oldBlockState );
	//}

	// void MultipleMeasurmentDihedralAngleWidget::_setupSlots()
	//{
	//	connect( _nameWidget,
	//			 &CustomWidget::QLineEditMultiField::editingFinished,
	//			 this,
	//			 &MultipleMeasurmentDihedralAngleWidget::_renameAction );

	//	connect( _setAutoNameWidget,
	//			 &QPushButton::clicked,
	//			 this,
	//			 &MultipleMeasurmentDihedralAngleWidget::_setAutoNameAction );

	//	connect(
	//		_goToFirstAtom, &QPushButton::clicked, this, &MultipleMeasurmentDihedralAngleWidget::_orientOnFirstAtom );
	//	connect(
	//		_goToSecondAtom, &QPushButton::clicked, this, &MultipleMeasurmentDihedralAngleWidget::_orientOnSecondAtom );
	//};

	// void MultipleMeasurmentDihedralAngleWidget::_endOfFrameRefresh( const SectionFlag & p_flag )
	//{
	//	MultipleModelInspectorWidget::_endOfFrameRefresh( p_flag );

	//	_resetFieldStates( p_flag );

	//	const std::unordered_set<Model::Measurement::DihedralAngle *> & targets = getTargets();

	//	const bool goToAtomButtonsVisible = targets.size() == 1;

	//	_goToFirstAtom->setVisible( goToAtomButtonsVisible );
	//	_goToSecondAtom->setVisible( goToAtomButtonsVisible );

	//	if ( targets.size() > 0 )
	//	{
	//		const QString headerTitle
	//			= QString::fromStdString( "Dihedral angle (" + std::to_string( targets.size() ) + ")" );
	//		_getHeader()->setHeaderTitle( headerTitle );

	//		const QPixmap * symbolPixmap
	//			= Style::IconConst::get().getModelSymbol( VTX::ID::Model::MODEL_MEASUREMENT_DIHEDRAL_ANGLE );
	//		_getHeader()->setHeaderIcon( *symbolPixmap );

	//		for ( const Model::Measurement::DihedralAngle * dihedralAngleModel : targets )
	//		{
	//			if ( bool( p_flag & SectionFlag::INFOS ) )
	//			{
	//				const Vec3f labelWorldPosition = dihedralAngleModel->getPosition();

	//				_positionInfoWidget->updateWithNewValue( labelWorldPosition );
	//				_nameWidget->updateWithNewValue( dihedralAngleModel->getDefaultName() );
	//				const std::string dihedralAngleStr
	//					= Util::Measurement::getDihedralAngleString( *dihedralAngleModel );
	//				_dihedralAngleLabelWidget->updateWithNewValue( dihedralAngleStr );

	//				if ( dihedralAngleModel->getTypeId() == ID::Model::MODEL_MEASUREMENT_DIHEDRAL_ANGLE )
	//				{
	//					if ( goToAtomButtonsVisible )
	//					{
	//						_goToFirstAtom->setText( QString::fromStdString(
	//							dihedralAngleModel->getFirstAtom().getName() + " "
	//							+ std::to_string( dihedralAngleModel->getFirstAtom().getIndex() ) ) );
	//						_goToSecondAtom->setText( QString::fromStdString(
	//							dihedralAngleModel->getSecondAtom().getName() + " "
	//							+ std::to_string( dihedralAngleModel->getFirstAtom().getIndex() ) ) );
	//					}
	//				}
	//			}
	//		}
	//	}
	//}

	// void MultipleMeasurmentDihedralAngleWidget::_orientOnFirstAtom() const
	//{
	//	const Model::Measurement::DihedralAngle * const dihedralAngleModel
	//		= dynamic_cast<const Model::Measurement::DihedralAngle *>( *( getTargets().begin() ) );

	//	VTX_ACTION( new Action::Atom::Orient( dihedralAngleModel->getFirstAtom() ) );
	//}
	// void MultipleMeasurmentDihedralAngleWidget::_orientOnSecondAtom() const
	//{
	//	const Model::Measurement::DihedralAngle * const dihedralAngleModel
	//		= dynamic_cast<const Model::Measurement::DihedralAngle *>( *( getTargets().begin() ) );

	//	VTX_ACTION( new Action::Atom::Orient( dihedralAngleModel->getSecondAtom() ) );
	//}

	// void MultipleMeasurmentDihedralAngleWidget::_renameAction() const
	//{
	//	std::unordered_set<Model::Label *> labelTargets = std::unordered_set<Model::Label *>();
	//	for ( Model::Measurement::DihedralAngle * const dihedralAngleModel : getTargets() )
	//		labelTargets.emplace( dihedralAngleModel );

	//	VTX_ACTION( new Action::Label::Rename( labelTargets, _nameWidget->text().toStdString() ) );
	//}

	// void MultipleMeasurmentDihedralAngleWidget::_setAutoNameAction() const
	//{
	//	std::unordered_set<Model::Label *> labelTargets = std::unordered_set<Model::Label *>();
	//	for ( Model::Measurement::DihedralAngle * const dihedralAngleModel : getTargets() )
	//		labelTargets.emplace( dihedralAngleModel );

	//	VTX_ACTION( new Action::Label::EnableAutoNaming( labelTargets, true ) );
	//}

	// void MultipleMeasurmentDihedralAngleWidget::_resetFieldStates( const SectionFlag & p_flag )
	//{
	//	if ( bool( p_flag & SectionFlag::INFOS ) )
	//	{
	//		_positionInfoWidget->resetState();
	//		_nameWidget->resetState();
	//		_dihedralAngleLabelWidget->resetState();
	//		_goToFirstAtom->setVisible( false );
	//		_goToSecondAtom->setVisible( false );
	//	}
	//}

	// void MultipleMeasurmentDihedralAngleWidget::localize()
	//{
	//	_positionSection->setHeaderTitle( "Position" );
	//	_infoSection->setHeaderTitle( "Informations" );
	//	_positionSection->localize();
	//	_infoSection->localize();
	//}
} // namespace VTX::UI::Widget::Inspector
