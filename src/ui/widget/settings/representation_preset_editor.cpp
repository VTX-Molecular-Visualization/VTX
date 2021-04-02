#include "representation_preset_editor.hpp"
#include "action/action_manager.hpp"
#include "action/representation.hpp"
#include "setting.hpp"
#include "ui/widget_factory.hpp"
#include <QCheckBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

namespace VTX::UI::Widget::Settings
{
	RepresentationPresetEditor::RepresentationPresetEditor( QWidget * const p_parent ) :
		VTX::UI::Widget::BaseManualWidget<QScrollArea>( p_parent ),
		EditorView( ID::View::UI_REPRESENTATION_EDITOR_PRESET )
	{
	}

	void RepresentationPresetEditor::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		QLineEdit * const nameWidget = new QLineEdit( this );

		QComboBox * const representationTypeWidget = new QComboBox( this );
		_populateRepresentationTypeComboBox( representationTypeWidget );

		CustomWidget::FloatFieldSliderWidget * const sphereRadiusWidget
			= VTX::UI::WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>(
				this, "SphereRadiusWidget" );
		CustomWidget::FloatFieldSliderWidget * const cylinderRadiusWidget
			= VTX::UI::WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>(
				this, "CylinderRadiusWidget" );

		QComboBox * const colorModeWidget = new QComboBox( this );
		for ( const std::string & colorModeStr : Generic::COLOR_MODE_STRING )
		{
			colorModeWidget->addItem( QString::fromStdString( colorModeStr ) );
		}

		QComboBox * const ssColorModeWidget = new QComboBox( this );
		for ( const std::string & colorModeStr : Generic::SECONDARY_STRUCTURE_COLOR_MODE_STRING )
		{
			ssColorModeWidget->addItem( QString::fromStdString( colorModeStr ) );
		}

		CustomWidget::ColorFieldButton * const colorButtonWidget
			= VTX::UI::WidgetFactory::get().instantiateWidget<CustomWidget::ColorFieldButton>( this,
																							   "ColorButtonWidget" );

		QCheckBox * const quickAccess = new QCheckBox( this );

		QHBoxLayout * const hboxLayout = new QHBoxLayout( this );
		QVBoxLayout * const vboxLayout = new QVBoxLayout();

		_layout = new QGridLayout();
		_layout->setMargin( 0 );
		_layout->setColumnStretch( 0, 1 );
		_layout->setColumnStretch( 1, 10 );

		_addParameter( PARAMETER::NAME, nameWidget, QString( "Name" ) );
		_addParameter( PARAMETER::QUICK_ACCESS, quickAccess, QString( "Quick Access" ) );
		_addSpace();
		_addParameter( PARAMETER::TYPE, representationTypeWidget, QString( "Type" ) );
		_addParameter( PARAMETER::SPHERE_RADIUS, sphereRadiusWidget, QString( "Sphere Radius" ) );
		_addParameter( PARAMETER::CYLINDER_RADIUS, cylinderRadiusWidget, QString( "Cylinder Radius" ) );
		_addParameter( PARAMETER::COLOR_MODE, colorModeWidget, QString( "Color Mode" ) );
		_addParameter( PARAMETER::SS_COLOR_MODE, ssColorModeWidget, QString( "SS Color Mode" ) );
		_addParameter( PARAMETER::COLOR, colorButtonWidget, QString( "Custom color" ) );

		vboxLayout->addItem( _layout );
		vboxLayout->addStretch( 1000 );
		hboxLayout->addItem( vboxLayout );
		hboxLayout->addStretch( 1000 );
	}
	void RepresentationPresetEditor::_setupSlots()
	{
		connect( _getParameter<QLineEdit>( PARAMETER::NAME ),
				 &QLineEdit::editingFinished,
				 this,
				 &RepresentationPresetEditor::_onRepresentationNameChange );

		connect( _getParameter<QCheckBox>( PARAMETER::QUICK_ACCESS ),
				 &QCheckBox::stateChanged,
				 this,
				 &RepresentationPresetEditor::_onQuickAccessChange );

		connect( _getParameter<QComboBox>( PARAMETER::TYPE ),
				 QOverload<int>::of( &QComboBox::currentIndexChanged ),
				 this,
				 &RepresentationPresetEditor::_onRepresentationTypeChange );

		connect( _getParameter<CustomWidget::FloatFieldSliderWidget>( PARAMETER::SPHERE_RADIUS ),
				 &Widget::CustomWidget::FloatFieldSliderWidget::onValueChange,
				 this,
				 &RepresentationPresetEditor::_onSphereRadiusChanged );
		connect( _getParameter<CustomWidget::FloatFieldSliderWidget>( PARAMETER::CYLINDER_RADIUS ),
				 &Widget::CustomWidget::FloatFieldSliderWidget::onValueChange,
				 this,
				 &RepresentationPresetEditor::_onCylinderRadiusChanged );

		connect( _getParameter<QComboBox>( PARAMETER::COLOR_MODE ),
				 QOverload<int>::of( &QComboBox::currentIndexChanged ),
				 this,
				 &RepresentationPresetEditor::_onColorModeChanged );

		connect( _getParameter<QComboBox>( PARAMETER::SS_COLOR_MODE ),
				 QOverload<int>::of( &QComboBox::currentIndexChanged ),
				 this,
				 &RepresentationPresetEditor::_onSSColorModeChanged );

		connect( _getParameter<CustomWidget::ColorFieldButton>( PARAMETER::COLOR ),
				 &Widget::CustomWidget::ColorFieldButton::onValueChange,
				 this,
				 &RepresentationPresetEditor::_onColorChanged );
	}

	void RepresentationPresetEditor::localize() {}

	void RepresentationPresetEditor::_catchModelEvent( const Event::VTXEvent * const p_event ) { refresh(); }

	void RepresentationPresetEditor::refresh()
	{
		_getParameter<QComboBox>( PARAMETER::TYPE )->setCurrentIndex( int( _preset->getRepresentationType() ) );
		_getParameter<QLineEdit>( PARAMETER::NAME )->setText( QString::fromStdString( _preset->getName() ) );
		_getParameter<QCheckBox>( PARAMETER::QUICK_ACCESS )
			->setCheckState( _preset->hasQuickAccess() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );

		_refreshRepresentationData();
		_getParameter<CustomWidget::ColorFieldButton>( PARAMETER::COLOR )->setColor( _preset->getColor() );
	}

	void RepresentationPresetEditor::_refreshRepresentationData()
	{
		switch ( _preset->getRepresentationType() )
		{
		case Generic::REPRESENTATION::BALL_AND_STICK: _refreshBallStickAndCartoonRepresentation(); break;
		case Generic::REPRESENTATION::BALL_AND_STICK_AND_CARTOON: _refreshBallStickAndCartoonRepresentation(); break;
		case Generic::REPRESENTATION::STICK: _refreshStickRepresentation(); break;
		case Generic::REPRESENTATION::STICK_AND_CARTOON: _refreshStickAndCartoonRepresentation(); break;
		case Generic::REPRESENTATION::VAN_DER_WAALS: _refreshVanDerWaalsRepresentation(); break;
		case Generic::REPRESENTATION::TRACE: _refreshTraceRepresentation(); break;
		case Generic::REPRESENTATION::SAS: _refreshSASRepresentation(); break;
		case Generic::REPRESENTATION::CARTOON: _refreshCartoonRepresentation(); break;
		}
	}
	void RepresentationPresetEditor::_refreshBallAndStickRepresentation()
	{
		CustomWidget::FloatFieldSliderWidget * const sphereRadiusWidget
			= _getParameter<CustomWidget::FloatFieldSliderWidget>( PARAMETER::SPHERE_RADIUS );
		sphereRadiusWidget->setValue( _preset->getData().getSphereRadius() );
		sphereRadiusWidget->setMinMax( Setting::ATOMS_RADIUS_MIN, Setting::ATOMS_RADIUS_MAX );

		CustomWidget::FloatFieldSliderWidget * const cylinderRadiusWidget
			= _getParameter<CustomWidget::FloatFieldSliderWidget>( PARAMETER::CYLINDER_RADIUS );
		cylinderRadiusWidget->setValue( _preset->getData().getCylinderRadius() );
		cylinderRadiusWidget->setMinMax( Setting::BONDS_RADIUS_MIN, Setting::BONDS_RADIUS_MAX );

		_getParameter<QComboBox>( PARAMETER::COLOR_MODE )->setCurrentIndex( int( _preset->getData().getColorMode() ) );

		_setParameterVisibility( PARAMETER::SS_COLOR_MODE, false );
	}
	void RepresentationPresetEditor::_refreshBallStickAndCartoonRepresentation()
	{
		CustomWidget::FloatFieldSliderWidget * const sphereRadiusWidget
			= _getParameter<CustomWidget::FloatFieldSliderWidget>( PARAMETER::SPHERE_RADIUS );
		sphereRadiusWidget->setValue( _preset->getData().getSphereRadius() );
		sphereRadiusWidget->setMinMax( Setting::ATOMS_RADIUS_MIN, Setting::ATOMS_RADIUS_MAX );

		CustomWidget::FloatFieldSliderWidget * const cylinderRadiusWidget
			= _getParameter<CustomWidget::FloatFieldSliderWidget>( PARAMETER::CYLINDER_RADIUS );
		cylinderRadiusWidget->setValue( _preset->getData().getCylinderRadius() );
		cylinderRadiusWidget->setMinMax( Setting::BONDS_RADIUS_MIN, Setting::BONDS_RADIUS_MAX );

		_getParameter<QComboBox>( PARAMETER::COLOR_MODE )->setCurrentIndex( int( _preset->getData().getColorMode() ) );

		_getParameter<QComboBox>( PARAMETER::SS_COLOR_MODE )
			->setCurrentIndex( int( _preset->getData().getSecondaryStructureColorMode() ) );
	}
	void RepresentationPresetEditor::_refreshStickRepresentation()
	{
		_setParameterVisibility( PARAMETER::SPHERE_RADIUS, false );

		CustomWidget::FloatFieldSliderWidget * const cylinderRadiusWidget
			= _getParameter<CustomWidget::FloatFieldSliderWidget>( PARAMETER::CYLINDER_RADIUS );
		cylinderRadiusWidget->setValue( _preset->getData().getCylinderRadius() );
		cylinderRadiusWidget->setMinMax( Setting::BONDS_RADIUS_MIN, Setting::BONDS_RADIUS_MAX );

		_getParameter<QComboBox>( PARAMETER::COLOR_MODE )->setCurrentIndex( int( _preset->getData().getColorMode() ) );
		_setParameterVisibility( PARAMETER::SS_COLOR_MODE, false );
	}
	void RepresentationPresetEditor::_refreshStickAndCartoonRepresentation()
	{
		_setParameterVisibility( PARAMETER::SPHERE_RADIUS, false );

		CustomWidget::FloatFieldSliderWidget * const cylinderRadiusWidget
			= _getParameter<CustomWidget::FloatFieldSliderWidget>( PARAMETER::CYLINDER_RADIUS );
		cylinderRadiusWidget->setValue( _preset->getData().getCylinderRadius() );
		cylinderRadiusWidget->setMinMax( Setting::BONDS_RADIUS_MIN, Setting::BONDS_RADIUS_MAX );

		_getParameter<QComboBox>( PARAMETER::COLOR_MODE )->setCurrentIndex( int( _preset->getData().getColorMode() ) );
		_getParameter<QComboBox>( PARAMETER::SS_COLOR_MODE )
			->setCurrentIndex( int( _preset->getData().getSecondaryStructureColorMode() ) );
	}
	void RepresentationPresetEditor::_refreshVanDerWaalsRepresentation()
	{
		CustomWidget::FloatFieldSliderWidget * const sphereRadiusWidget
			= _getParameter<CustomWidget::FloatFieldSliderWidget>( PARAMETER::SPHERE_RADIUS );
		sphereRadiusWidget->setValue( _preset->getData().getSphereRadius() );
		sphereRadiusWidget->setMinMax( Setting::ATOMS_RADIUS_ADD_MIN, Setting::ATOMS_RADIUS_ADD_MAX );

		_setParameterVisibility( PARAMETER::CYLINDER_RADIUS, false );
		_getParameter<QComboBox>( PARAMETER::COLOR_MODE )->setCurrentIndex( int( _preset->getData().getColorMode() ) );
		_setParameterVisibility( PARAMETER::SS_COLOR_MODE, false );
	}
	void RepresentationPresetEditor::_refreshTraceRepresentation()
	{
		_setParameterVisibility( PARAMETER::SPHERE_RADIUS, false );

		CustomWidget::FloatFieldSliderWidget * const cylinderRadiusWidget
			= _getParameter<CustomWidget::FloatFieldSliderWidget>( PARAMETER::CYLINDER_RADIUS );
		cylinderRadiusWidget->setValue( _preset->getData().getCylinderRadius() );
		cylinderRadiusWidget->setMinMax( Setting::BONDS_RADIUS_MIN, Setting::BONDS_RADIUS_MAX );

		_getParameter<QComboBox>( PARAMETER::COLOR_MODE )->setCurrentIndex( int( _preset->getData().getColorMode() ) );
		_setParameterVisibility( PARAMETER::SS_COLOR_MODE, false );
	}
	void RepresentationPresetEditor::_refreshSASRepresentation()
	{
		_setParameterVisibility( PARAMETER::SPHERE_RADIUS, false );
		_setParameterVisibility( PARAMETER::CYLINDER_RADIUS, false );
		_getParameter<QComboBox>( PARAMETER::COLOR_MODE )->setCurrentIndex( int( _preset->getData().getColorMode() ) );
		_setParameterVisibility( PARAMETER::SS_COLOR_MODE, false );
	}
	void RepresentationPresetEditor::_refreshCartoonRepresentation()
	{
		_setParameterVisibility( PARAMETER::SPHERE_RADIUS, false );
		_setParameterVisibility( PARAMETER::CYLINDER_RADIUS, false );
		_setParameterVisibility( PARAMETER::COLOR_MODE, false );

		_getParameter<QComboBox>( PARAMETER::SS_COLOR_MODE )
			->setCurrentIndex( int( _preset->getData().getSecondaryStructureColorMode() ) );
	}

	void RepresentationPresetEditor::setPreset( Model::Representation::Representation * const p_model,
												const bool									  p_updateRender )
	{
		if ( _preset == p_model )
			return;

		_preset = p_model;

		createTempView( _preset );

		blockSignals( !p_updateRender );
		refresh();
		blockSignals( false );
	}

	void RepresentationPresetEditor::_onRepresentationNameChange()
	{
		const std::string nameStr = _getParameter<QLineEdit>( PARAMETER::NAME )->text().toStdString();
		if ( !signalsBlocked() && nameStr != _preset->getName() )
			VTX_ACTION( new Action::Representation::ChangeName( _preset, nameStr ) );
	}

	void RepresentationPresetEditor::_onQuickAccessChange( const int p_state )
	{
		const bool quickAccess = p_state == Qt::CheckState::Checked;
		if ( !signalsBlocked() && quickAccess != _preset->hasQuickAccess() )
			VTX_ACTION( new Action::Representation::ChangeQuickAccess( _preset, quickAccess ) );
	}

	void RepresentationPresetEditor::_onRepresentationTypeChange( const int p_newIndex )
	{
		const Generic::REPRESENTATION representationType = Generic::REPRESENTATION( p_newIndex );

		if ( !signalsBlocked() && representationType != _preset->getRepresentationType() )
			VTX_ACTION( new Action::Representation::ChangeRepresentation( _preset, representationType ) );
	}
	void RepresentationPresetEditor::_onSphereRadiusChanged( const float p_radius )
	{
		if ( !signalsBlocked() )
			VTX_ACTION( new Action::Representation::ChangeSphereRadius( _preset, p_radius ) );
	}
	void RepresentationPresetEditor::_onCylinderRadiusChanged( const float p_radius )
	{
		if ( !signalsBlocked() )
			VTX_ACTION( new Action::Representation::ChangeCylinderRadius( _preset, p_radius ) );
	}
	void RepresentationPresetEditor::_onColorModeChanged( const int p_colorMode )
	{
		Generic::COLOR_MODE colorMode = Generic::COLOR_MODE( p_colorMode );

		if ( !signalsBlocked() && colorMode != _preset->getData().getColorMode() )
			VTX_ACTION( new Action::Representation::ChangeColorMode( _preset, colorMode ) );
	}
	void RepresentationPresetEditor::_onSSColorModeChanged( const int p_colorMode )
	{
		Generic::SECONDARY_STRUCTURE_COLOR_MODE colorMode = Generic::SECONDARY_STRUCTURE_COLOR_MODE( p_colorMode );

		if ( !signalsBlocked() && colorMode != _preset->getData().getSecondaryStructureColorMode() )
			VTX_ACTION( new Action::Representation::ChangeSecondaryStructureColorMode( _preset, colorMode ) );
	}
	void RepresentationPresetEditor::_onColorChanged( const Color::Rgb & p_color )
	{
		if ( !signalsBlocked() && p_color != _preset->getColor() )
			VTX_ACTION( new Action::Representation::ChangeColor( _preset, p_color ) );
	}

	void RepresentationPresetEditor::_applyPreset() const
	{
		// if ( !signalsBlocked() )
		//	VTX_ACTION( new Action::Representation::ApplyRepresentationPreset( *_preset ) );
	}

	void RepresentationPresetEditor::_addParameter( const PARAMETER & p_parameter,
													QWidget * const	  p_widget,
													const QString &	  p_label )
	{
		QLabel * const label = new QLabel( this );
		label->setText( p_label );

		_layout->addWidget( label, _itemCount, 0 );
		_layout->addWidget( p_widget, _itemCount, 1 );

		ParameterLine * const parameter = new ParameterLine( label, p_widget );

		_parameterWidgets[ p_parameter ] = parameter;

		_itemCount++;
	}
	void RepresentationPresetEditor::_addSpace()
	{
		_layout->addItem( new QSpacerItem( 0, 10 ), _itemCount, 0, 2, 1 );
		_itemCount++;
	}

	void RepresentationPresetEditor::_populateRepresentationTypeComboBox( QComboBox * const p_comboBox ) const
	{
		for ( const std::string & representationName : Generic::REPRESENTATION_STRING )
		{
			p_comboBox->addItem( QString::fromStdString( representationName ) );
		}
	}

	void RepresentationPresetEditor::_setParameterVisibility( const PARAMETER & p_parameter, const bool p_visible )
	{
		_parameterWidgets[ p_parameter ]->setVisible( p_visible );
	}

} // namespace VTX::UI::Widget::Settings
