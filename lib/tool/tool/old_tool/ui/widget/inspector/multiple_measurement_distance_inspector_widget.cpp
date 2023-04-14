#include "multiple_measurement_distance_inspector_widget.hpp"
#include "old_tool/model/measurement/distance.hpp"
#include "old_tool/util/measurement.hpp"
#include "old_ui/ui/widget_factory.hpp"
#include <app/old_app/action/action_manager.hpp>
#include <app/old_app/action/atom.hpp>
#include <app/old_app/action/label.hpp>
#include <ui/qt/action/atom.hpp>

namespace VTX::UI::Widget::Inspector
{
	MultipleMeasurmentDistanceWidget::MultipleMeasurmentDistanceWidget( QWidget * p_parent ) :
		MultipleModelInspectorWidget( p_parent, ID::View::UI_INSPECTOR_VIEWPOINT ) {};

	MultipleMeasurmentDistanceWidget::~MultipleMeasurmentDistanceWidget() {}

	void MultipleMeasurmentDistanceWidget::_setupUi( const QString & p_name )
	{
		MultipleModelInspectorWidget::_setupUi( p_name );

		_positionSection = VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSectionVLayout>(
			this, "inspector_item_section" );

		_positionInfoWidget = UI::WidgetFactory::get().instantiateWidget<CustomWidget::Vector3Widget>(
			this, "inspector_label_position" );
		_positionInfoWidget->setEnabled( false );
		_positionSection->setBody( _positionInfoWidget );

		_infoSection = VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSectionVLayout>(
			this, "inspector_item_section" );

		QWidget * const fullNameWidget = new QWidget( this );

		QHBoxLayout * const nameLayout = new QHBoxLayout( fullNameWidget );

		_nameWidget		   = new CustomWidget::QLineEditMultiField( this );
		_setAutoNameWidget = new QPushButton( this );
		_setAutoNameWidget->setText( "Auto" );
		nameLayout->addWidget( _nameWidget );
		nameLayout->addWidget( _setAutoNameWidget );

		_distanceLabelWidget = new CustomWidget::QLabelMultiField( this );

		QWidget * const goToAtomButtons = new QWidget( this );
		QHBoxLayout *	goToAtomLayout	= new QHBoxLayout( goToAtomButtons );

		_goToFirstAtom	= new QPushButton( goToAtomButtons );
		_goToSecondAtom = new QPushButton( goToAtomButtons );

		_displaySection = VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSectionVLayout>(
			this, "inspector_item_section" );

		_colorWidget
			= VTX::UI::WidgetFactory::get().instantiateWidget<CustomWidget::ColorFieldButton>( this, "colorWidget" );

		goToAtomLayout->addWidget( _goToFirstAtom );
		goToAtomLayout->addSpacing( 10 );
		goToAtomLayout->addWidget( _goToSecondAtom );

		_infoSection->appendField( "Name", fullNameWidget );
		_infoSection->appendField( "Atoms", goToAtomButtons );
		_infoSection->appendField( "Distance", _distanceLabelWidget );

		_displaySection->appendField( "Color", _colorWidget );

		_appendWidget( _positionSection );
		_appendWidget( _displaySection );
		_appendWidget( _infoSection );

		const bool oldBlockState = blockSignals( true );
		refresh();
		blockSignals( oldBlockState );
	}

	void MultipleMeasurmentDistanceWidget::_setupSlots()
	{
		connect( _nameWidget,
				 &CustomWidget::QLineEditMultiField::editingFinished,
				 this,
				 &MultipleMeasurmentDistanceWidget::_renameAction );

		connect(
			_setAutoNameWidget, &QPushButton::clicked, this, &MultipleMeasurmentDistanceWidget::_setAutoNameAction );

		connect( _goToFirstAtom, &QPushButton::clicked, this, &MultipleMeasurmentDistanceWidget::_orientOnFirstAtom );
		connect( _goToSecondAtom, &QPushButton::clicked, this, &MultipleMeasurmentDistanceWidget::_orientOnSecondAtom );

		connect( _colorWidget,
				 &CustomWidget::ColorFieldButton::onValueChange,
				 this,
				 &MultipleMeasurmentDistanceWidget::_setLabelColor );
	};

	void MultipleMeasurmentDistanceWidget::_endOfFrameRefresh( const SectionFlag & p_flag )
	{
		MultipleModelInspectorWidget::_endOfFrameRefresh( p_flag );

		_resetFieldStates( p_flag );

		const std::unordered_set<Model::Measurement::Distance *> & targets = getTargets();

		const bool goToAtomButtonsVisible = targets.size() == 1;

		_goToFirstAtom->setVisible( goToAtomButtonsVisible );
		_goToSecondAtom->setVisible( goToAtomButtonsVisible );

		if ( targets.size() > 0 )
		{
			const QString headerTitle = QString::fromStdString( "Distance (" + std::to_string( targets.size() ) + ")" );
			_getHeader()->setHeaderTitle( headerTitle );

			const QPixmap * symbolPixmap
				= Style::IconConst::get().getModelSymbol( VTX::ID::Model::MODEL_MEASUREMENT_DISTANCE );
			_getHeader()->setHeaderIcon( *symbolPixmap );

			for ( const Model::Measurement::Distance * distanceModel : targets )
			{
				if ( bool( p_flag & SectionFlag::INFOS ) )
				{
					const Vec3f labelWorldPosition = distanceModel->getPosition();

					_positionInfoWidget->updateWithNewValue( labelWorldPosition );
					_nameWidget->updateWithNewValue( distanceModel->getDefaultName() );
					const std::string distanceStr = Util::Measurement::getDistanceString( *distanceModel );
					_distanceLabelWidget->updateWithNewValue( distanceStr );
					_colorWidget->updateWithNewValue( distanceModel->getColor() );

					if ( distanceModel->getTypeId() == ID::Model::MODEL_MEASUREMENT_DISTANCE )
					{
						if ( goToAtomButtonsVisible )
						{
							_goToFirstAtom->setText( QString::fromStdString(
								distanceModel->getFirstAtom().getName() + " "
								+ std::to_string( distanceModel->getFirstAtom().getIndex() ) ) );
							_goToSecondAtom->setText( QString::fromStdString(
								distanceModel->getSecondAtom().getName() + " "
								+ std::to_string( distanceModel->getFirstAtom().getIndex() ) ) );
						}
					}
				}
			}
		}
	}

	void MultipleMeasurmentDistanceWidget::_orientOnFirstAtom() const
	{
		const Model::Measurement::Distance * const distance
			= dynamic_cast<const Model::Measurement::Distance *>( *( getTargets().begin() ) );

		VTX_ACTION( new QT::Action::Atom::Orient( distance->getFirstAtom() ) );
	}
	void MultipleMeasurmentDistanceWidget::_orientOnSecondAtom() const
	{
		const Model::Measurement::Distance * const distance
			= dynamic_cast<const Model::Measurement::Distance *>( *( getTargets().begin() ) );

		VTX_ACTION( new QT::Action::Atom::Orient( distance->getSecondAtom() ) );
	}

	void MultipleMeasurmentDistanceWidget::_renameAction() const
	{
		std::unordered_set<Model::Label *> labelTargets = std::unordered_set<Model::Label *>();
		for ( Model::Measurement::Distance * const targetDistance : getTargets() )
			labelTargets.emplace( targetDistance );

		VTX_ACTION( new Action::Label::Rename( labelTargets, _nameWidget->text().toStdString() ) );
	}

	void MultipleMeasurmentDistanceWidget::_setLabelColor( const Color::Rgba & p_color ) const
	{
		std::unordered_set<Model::Label *> labelTargets = std::unordered_set<Model::Label *>();
		for ( Model::Measurement::Distance * const targetDistance : getTargets() )
			labelTargets.emplace( targetDistance );

		VTX_ACTION( new Action::Label::ChangeColor( labelTargets, p_color ) );
	}

	void MultipleMeasurmentDistanceWidget::_setAutoNameAction() const
	{
		std::unordered_set<Model::Label *> labelTargets = std::unordered_set<Model::Label *>();
		for ( Model::Measurement::Distance * const targetDistance : getTargets() )
			labelTargets.emplace( targetDistance );

		VTX_ACTION( new Action::Label::EnableAutoNaming( labelTargets, true ) );
	}

	void MultipleMeasurmentDistanceWidget::_resetFieldStates( const SectionFlag & p_flag )
	{
		if ( bool( p_flag & SectionFlag::INFOS ) )
		{
			_positionInfoWidget->resetState();
			_nameWidget->resetState();
			_distanceLabelWidget->resetState();
			_colorWidget->resetState();

			_goToFirstAtom->setVisible( false );
			_goToSecondAtom->setVisible( false );
		}
	}

	void MultipleMeasurmentDistanceWidget::localize()
	{
		_positionSection->setHeaderTitle( "Position" );
		_displaySection->setHeaderTitle( "Display" );
		_infoSection->setHeaderTitle( "Informations" );
		_positionSection->localize();
		_infoSection->localize();
		_displaySection->localize();
	}
} // namespace VTX::UI::Widget::Inspector
