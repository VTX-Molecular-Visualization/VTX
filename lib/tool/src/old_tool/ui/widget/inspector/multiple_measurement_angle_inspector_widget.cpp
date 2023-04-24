#include "tool/old_tool/ui/widget/inspector/multiple_measurement_angle_inspector_widget.hpp"
#include "tool/old_tool/model/measurement/angle.hpp"
#include "tool/old_tool/util/measurement.hpp"

#include <app/action/atom.hpp>
#include <app/action/label.hpp>
#include <ui/old_ui/ui/widget_factory.hpp>
#include <ui/qt/action/atom.hpp>
#include <ui/qt/action/label.hpp>

namespace VTX::UI::Widget::Inspector
{
	MultipleMeasurmentAngleWidget::MultipleMeasurmentAngleWidget( QWidget * p_parent ) :
		MultipleModelInspectorWidget( p_parent, ID::View::UI_INSPECTOR_VIEWPOINT ) {};

	MultipleMeasurmentAngleWidget::~MultipleMeasurmentAngleWidget() {}

	void MultipleMeasurmentAngleWidget::_setupUi( const QString & p_name )
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

		_angleLabelWidget = new CustomWidget::QLabelMultiField( this );

		_displaySection = VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSectionVLayout>(
			this, "inspector_item_section" );

		_colorWidget
			= VTX::UI::WidgetFactory::get().instantiateWidget<CustomWidget::ColorFieldButton>( this, "colorWidget" );

		QWidget * const goToAtomButtons = new QWidget( this );
		QVBoxLayout *	goToAtomLayout	= new QVBoxLayout( goToAtomButtons );

		_goToFirstAtom	= new QPushButton( goToAtomButtons );
		_goToSecondAtom = new QPushButton( goToAtomButtons );
		_goToThirdAtom	= new QPushButton( goToAtomButtons );

		goToAtomLayout->addWidget( _goToFirstAtom );
		goToAtomLayout->addWidget( _goToSecondAtom );
		goToAtomLayout->addWidget( _goToThirdAtom );

		_infoSection->appendField( "Name", fullNameWidget );
		_infoSection->appendField( "Atoms", goToAtomButtons );
		_infoSection->appendField( "Angle", _angleLabelWidget );

		_displaySection->appendField( "Color", _colorWidget );

		_appendWidget( _positionSection );
		_appendWidget( _displaySection );
		_appendWidget( _infoSection );

		const bool oldBlockState = blockSignals( true );
		refresh();
		blockSignals( oldBlockState );
	}

	void MultipleMeasurmentAngleWidget::_setupSlots()
	{
		connect( _nameWidget,
				 &CustomWidget::QLineEditMultiField::editingFinished,
				 this,
				 &MultipleMeasurmentAngleWidget::_renameAction );

		connect( _setAutoNameWidget, &QPushButton::clicked, this, &MultipleMeasurmentAngleWidget::_setAutoNameAction );

		connect( _goToFirstAtom, &QPushButton::clicked, this, &MultipleMeasurmentAngleWidget::_orientOnFirstAtom );
		connect( _goToSecondAtom, &QPushButton::clicked, this, &MultipleMeasurmentAngleWidget::_orientOnSecondAtom );
		connect( _goToThirdAtom, &QPushButton::clicked, this, &MultipleMeasurmentAngleWidget::_orientOnThirdAtom );

		connect( _colorWidget,
				 &CustomWidget::ColorFieldButton::onValueChange,
				 this,
				 &MultipleMeasurmentAngleWidget::_setLabelColor );
	};

	void MultipleMeasurmentAngleWidget::_endOfFrameRefresh( const SectionFlag & p_flag )
	{
		MultipleModelInspectorWidget::_endOfFrameRefresh( p_flag );

		_resetFieldStates( p_flag );

		const std::unordered_set<Model::Measurement::Angle *> & targets = getTargets();

		const bool goToAtomButtonsVisible = targets.size() == 1;

		_goToFirstAtom->setVisible( goToAtomButtonsVisible );
		_goToSecondAtom->setVisible( goToAtomButtonsVisible );
		_goToThirdAtom->setVisible( goToAtomButtonsVisible );

		if ( targets.size() > 0 )
		{
			const QString headerTitle = QString::fromStdString( "Angle (" + std::to_string( targets.size() ) + ")" );
			_getHeader()->setHeaderTitle( headerTitle );

			const QPixmap * symbolPixmap
				= Style::IconConst::get().getModelSymbol( VTX::ID::Model::MODEL_MEASUREMENT_ANGLE );
			_getHeader()->setHeaderIcon( *symbolPixmap );

			for ( const Model::Measurement::Angle * angleModel : targets )
			{
				if ( bool( p_flag & SectionFlag::INFOS ) )
				{
					const Vec3f labelWorldPosition = angleModel->getPosition();

					_positionInfoWidget->updateWithNewValue( labelWorldPosition );
					_nameWidget->updateWithNewValue( angleModel->getDefaultName() );
					const std::string angleStr = Util::Measurement::getAngleString( *angleModel );
					_angleLabelWidget->updateWithNewValue( angleStr );
					_colorWidget->updateWithNewValue( angleModel->getColor() );

					if ( angleModel->getTypeId() == ID::Model::MODEL_MEASUREMENT_ANGLE )
					{
						if ( goToAtomButtonsVisible )
						{
							_goToFirstAtom->setText(
								QString::fromStdString( angleModel->getFirstAtom().getName() + " "
														+ std::to_string( angleModel->getFirstAtom().getIndex() ) ) );
							_goToSecondAtom->setText(
								QString::fromStdString( angleModel->getSecondAtom().getName() + " "
														+ std::to_string( angleModel->getSecondAtom().getIndex() ) ) );
							_goToThirdAtom->setText(
								QString::fromStdString( angleModel->getThirdAtom().getName() + " "
														+ std::to_string( angleModel->getThirdAtom().getIndex() ) ) );
						}
					}
				}
			}
		}
	}

	void MultipleMeasurmentAngleWidget::_orientOnFirstAtom() const
	{
		const Model::Measurement::Angle * const angleModel
			= dynamic_cast<const Model::Measurement::Angle *>( *( getTargets().begin() ) );

		VTX_ACTION( new QT::Action::Atom::Orient( angleModel->getFirstAtom() ) );
	}
	void MultipleMeasurmentAngleWidget::_orientOnSecondAtom() const
	{
		const Model::Measurement::Angle * const angleModel
			= dynamic_cast<const Model::Measurement::Angle *>( *( getTargets().begin() ) );

		VTX_ACTION( new QT::Action::Atom::Orient( angleModel->getSecondAtom() ) );
	}
	void MultipleMeasurmentAngleWidget::_orientOnThirdAtom() const
	{
		const Model::Measurement::Angle * const angleModel
			= dynamic_cast<const Model::Measurement::Angle *>( *( getTargets().begin() ) );

		VTX_ACTION( new QT::Action::Atom::Orient( angleModel->getThirdAtom() ) );
	}

	void MultipleMeasurmentAngleWidget::_renameAction() const
	{
		std::unordered_set<Model::Label *> labelTargets = std::unordered_set<Model::Label *>();
		for ( Model::Measurement::Angle * const angleModel : getTargets() )
			labelTargets.emplace( angleModel );

		VTX_ACTION( new App::Action::Label::Rename( labelTargets, _nameWidget->text().toStdString() ) );
	}

	void MultipleMeasurmentAngleWidget::_setLabelColor( const Color::Rgba & p_color ) const
	{
		std::unordered_set<Model::Label *> labelTargets = std::unordered_set<Model::Label *>();
		for ( Model::Measurement::Angle * const targetDistance : getTargets() )
			labelTargets.emplace( targetDistance );

		VTX_ACTION( new App::Action::Label::ChangeColor( labelTargets, p_color ) );
	}

	void MultipleMeasurmentAngleWidget::_setAutoNameAction() const
	{
		std::unordered_set<Model::Label *> labelTargets = std::unordered_set<Model::Label *>();
		for ( Model::Measurement::Angle * const angleModel : getTargets() )
			labelTargets.emplace( angleModel );

		VTX_ACTION( new App::Action::Label::EnableAutoNaming( labelTargets, true ) );
	}

	void MultipleMeasurmentAngleWidget::_resetFieldStates( const SectionFlag & p_flag )
	{
		if ( bool( p_flag & SectionFlag::INFOS ) )
		{
			_positionInfoWidget->resetState();
			_nameWidget->resetState();
			_angleLabelWidget->resetState();
			_colorWidget->resetState();
			_goToFirstAtom->setVisible( false );
			_goToSecondAtom->setVisible( false );
			_goToThirdAtom->setVisible( false );
		}
	}

	void MultipleMeasurmentAngleWidget::localize()
	{
		_positionSection->setHeaderTitle( "Position" );
		_displaySection->setHeaderTitle( "Display" );
		_infoSection->setHeaderTitle( "Informations" );
		_positionSection->localize();
		_infoSection->localize();
	}
} // namespace VTX::UI::Widget::Inspector
