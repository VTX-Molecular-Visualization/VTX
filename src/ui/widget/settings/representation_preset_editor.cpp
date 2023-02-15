#include "representation_preset_editor.hpp"
#include "action/action_manager.hpp"
#include "action/representation.hpp"
#include "setting.hpp"
#include "ui/widget_factory.hpp"
#include "util/ui.hpp"
#include <QHBoxLayout>
#include <QLineEdit>
#include <QVBoxLayout>

namespace VTX::UI::Widget::Settings
{
	const std::vector<std::vector<RepresentationPresetEditor::OPTIONAL_PARAMETER>>
		RepresentationPresetEditor::_MAP_REPRESENTATION_PARAMETERS = {
			// BALL_AND_STICK
			{ OPTIONAL_PARAMETER::SPHERE_RADIUS,
			  OPTIONAL_PARAMETER::CYLINDER_RADIUS,
			  OPTIONAL_PARAMETER::CYLINDER_COLOR_BLENDING_MODE,
			  OPTIONAL_PARAMETER::COLOR_MODE },

			// VAN_DER_WAALS
			{ OPTIONAL_PARAMETER::SPHERE_RADIUS, OPTIONAL_PARAMETER::COLOR_MODE },

			// STICK
			{ OPTIONAL_PARAMETER::CYLINDER_RADIUS,
			  OPTIONAL_PARAMETER::CYLINDER_COLOR_BLENDING_MODE,
			  OPTIONAL_PARAMETER::COLOR_MODE },

			// SAS
			{ OPTIONAL_PARAMETER::COLOR_MODE },

			// CARTOON
			{ OPTIONAL_PARAMETER::RIBBON_COLOR_MODE, OPTIONAL_PARAMETER::RIBBON_COLOR_BLENDING_MODE },

			// BALL_AND_STICK_AND_CARTOON
			{ OPTIONAL_PARAMETER::SPHERE_RADIUS,
			  OPTIONAL_PARAMETER::CYLINDER_RADIUS,
			  OPTIONAL_PARAMETER::COLOR_MODE,
			  OPTIONAL_PARAMETER::CYLINDER_COLOR_BLENDING_MODE,
			  OPTIONAL_PARAMETER::RIBBON_COLOR_MODE,
			  OPTIONAL_PARAMETER::RIBBON_COLOR_BLENDING_MODE },

			// STICK_AND_CARTOON
			{ OPTIONAL_PARAMETER::CYLINDER_RADIUS,
			  OPTIONAL_PARAMETER::CYLINDER_COLOR_BLENDING_MODE,
			  OPTIONAL_PARAMETER::COLOR_MODE,
			  OPTIONAL_PARAMETER::RIBBON_COLOR_MODE,
			  OPTIONAL_PARAMETER::RIBBON_COLOR_BLENDING_MODE },

			// SES
			{ OPTIONAL_PARAMETER::COLOR_MODE },
		};

	RepresentationPresetEditor::RepresentationPresetEditor( QWidget * const p_parent ) :
		VTX::UI::Widget::BaseManualWidget<QScrollArea>( p_parent ),
		EditorView( ID::View::UI_REPRESENTATION_EDITOR_PRESET )
	{
	}

	RepresentationPresetEditor::~RepresentationPresetEditor() {}

	void RepresentationPresetEditor::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		QWidget * const viewport = new QWidget( this );

		_nameWidget
			= WidgetFactory::get().instantiateWidget<CustomWidget::FilenameFieldWidget>( viewport, "nameWidget" );

		_representationTypeWidget = new QComboBox( viewport );
		Util::UI::fillComboBox( _representationTypeWidget, Generic::REPRESENTATION_STRING );

		_quickAccess	   = new QCheckBox( viewport );
		_colorButtonWidget = VTX::UI::WidgetFactory::get().instantiateWidget<CustomWidget::ColorFieldButton>(
			viewport, "ColorButtonWidget" );

		_optionalParameterWidgets.resize( int( OPTIONAL_PARAMETER::COUNT ) );

		_sphereRadiusWidget = VTX::UI::WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>(
			viewport, "SphereRadiusWidget" );
		_optionalParameterWidgets[ int( OPTIONAL_PARAMETER::SPHERE_RADIUS ) ] = _sphereRadiusWidget;

		_cylinderRadiusWidget = VTX::UI::WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>(
			viewport, "CylinderRadiusWidget" );
		_optionalParameterWidgets[ int( OPTIONAL_PARAMETER::CYLINDER_RADIUS ) ] = _cylinderRadiusWidget;

		_cylinderColorBlendingMode = new QComboBox( viewport );
		Util::UI::fillComboBox( _cylinderColorBlendingMode, Generic::COLOR_BLENDING_MODE_STRING );
		_optionalParameterWidgets[ int( OPTIONAL_PARAMETER::CYLINDER_COLOR_BLENDING_MODE ) ]
			= _cylinderColorBlendingMode;

		_colorModeWidget = new QComboBox( viewport );
		Util::UI::fillComboBox( _colorModeWidget, Generic::COLOR_MODE_STRING );
		_optionalParameterWidgets[ int( OPTIONAL_PARAMETER::COLOR_MODE ) ] = _colorModeWidget;

		_ribbonColorModeWidget = new QComboBox( viewport );
		Util::UI::fillComboBox( _ribbonColorModeWidget, Generic::SECONDARY_STRUCTURE_COLOR_MODE_STRING );
		_optionalParameterWidgets[ int( OPTIONAL_PARAMETER::RIBBON_COLOR_MODE ) ] = _ribbonColorModeWidget;

		_ribbonColorBlendingMode = new QComboBox( viewport );
		Util::UI::fillComboBox( _ribbonColorBlendingMode, Generic::COLOR_BLENDING_MODE_STRING );
		_optionalParameterWidgets[ int( OPTIONAL_PARAMETER::RIBBON_COLOR_BLENDING_MODE ) ] = _ribbonColorBlendingMode;

		_setDefaultButton = new QPushButton( viewport );
		_setDefaultButton->setIcon( Style::IconConst::get().DEFAULT_ITEM );
		_setDefaultButton->setText( "Set As Default" );

		_attributeLayout = new Layout::AttributeListLayout( viewport );
		_attributeLayout->setEndSectionSpacerSize( Style::ATTRIBUTE_LIST_LAYOUT_SMALL_SECTION_SPACER );

		_attributeLayout->startAttributeSection( "Description" );
		_attributeLayout->addAttribute( _nameWidget, QString( "Name" ) );
		_attributeLayout->addAttribute( _quickAccess, QString( "Quick access" ) );
		_attributeLayout->addAttribute( _representationTypeWidget, QString( "Type" ) );
		_attributeLayout->finishAttributeSection();

		_attributeLayout->startAttributeSection( "Parameters" );
		_attributeLayout->addAttribute( _sphereRadiusWidget, QString( "Sphere radius" ) );
		_attributeLayout->addAttribute( _cylinderRadiusWidget, QString( "Cylinder radius" ) );
		_attributeLayout->addAttribute( _cylinderColorBlendingMode, QString( "Cylinder color blending mode" ) );
		_attributeLayout->addAttribute( _ribbonColorModeWidget, QString( "Ribbon color mode" ) );
		_attributeLayout->addAttribute( _ribbonColorBlendingMode, QString( "Ribbon color blending mode" ) );
		_attributeLayout->addAttribute( _colorModeWidget, QString( "Color mode" ) );
		_attributeLayout->addAttribute( _colorButtonWidget, QString( "Custom color" ) );
		_attributeLayout->finishAttributeSection();

		_attributeLayout->addAttribute( _setDefaultButton, Qt::AlignCenter );

		setWidget( viewport );
	}
	void RepresentationPresetEditor::_setupSlots()
	{
		connect(
			_nameWidget, &QLineEdit::editingFinished, this, &RepresentationPresetEditor::_onRepresentationNameChange );

		connect( _quickAccess, &QCheckBox::stateChanged, this, &RepresentationPresetEditor::_onQuickAccessChange );

		connect( _representationTypeWidget,
				 QOverload<int>::of( &QComboBox::currentIndexChanged ),
				 this,
				 &RepresentationPresetEditor::_onRepresentationTypeChange );

		connect( _sphereRadiusWidget,
				 &Widget::CustomWidget::FloatFieldSliderWidget::onValueChange,
				 this,
				 &RepresentationPresetEditor::_onSphereRadiusChanged );

		connect( _cylinderRadiusWidget,
				 &Widget::CustomWidget::FloatFieldSliderWidget::onValueChange,
				 this,
				 &RepresentationPresetEditor::_onCylinderRadiusChanged );

		connect( _cylinderColorBlendingMode,
				 QOverload<int>::of( &QComboBox::currentIndexChanged ),
				 this,
				 &RepresentationPresetEditor::_onCylinderColorBendingModeChanged );

		connect( _colorModeWidget,
				 QOverload<int>::of( &QComboBox::currentIndexChanged ),
				 this,
				 &RepresentationPresetEditor::_onColorModeChanged );

		connect( _ribbonColorModeWidget,
				 QOverload<int>::of( &QComboBox::currentIndexChanged ),
				 this,
				 &RepresentationPresetEditor::_onRibbonColorModeChanged );

		connect( _ribbonColorBlendingMode,
				 QOverload<int>::of( &QComboBox::currentIndexChanged ),
				 this,
				 &RepresentationPresetEditor::_onRibbonColorBlendingModeChanged );

		connect( _colorButtonWidget,
				 &Widget::CustomWidget::ColorFieldButton::onValueChange,
				 this,
				 &RepresentationPresetEditor::_onColorChanged );

		connect( _setDefaultButton, &QPushButton::clicked, this, &RepresentationPresetEditor::_onSetDefault );
	}

	void RepresentationPresetEditor::localize() {}

	void RepresentationPresetEditor::_catchModelEvent( const Event::VTXEvent * const p_event ) { refresh(); }

	void RepresentationPresetEditor::refresh()
	{
		_representationTypeWidget->setCurrentIndex( int( _preset->getRepresentationType() ) );
		_nameWidget->setText( QString::fromStdString( _preset->getName() ) );
		_quickAccess->setCheckState( _preset->hasQuickAccess() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );

		_refreshOptionalParametersVisibility( _preset->getRepresentationType() );
		_refreshRepresentationData();

		_colorButtonWidget->setColor( _preset->getColor() );
	}

	void RepresentationPresetEditor::_refreshOptionalParametersVisibility(
		const Generic::REPRESENTATION & p_representationEnum )
	{
		for ( int i = 0; i < _optionalParameterWidgets.size(); i++ )
		{
			_optionalParameterWidgets[ i ]->setVisible( false );
		}

		const std::vector<OPTIONAL_PARAMETER> & visibleParameters
			= _MAP_REPRESENTATION_PARAMETERS[ int( p_representationEnum ) ];
		for ( int i = 0; i < visibleParameters.size(); i++ )
		{
			const int parameterIndex = int( visibleParameters[ i ] );
			_optionalParameterWidgets[ parameterIndex ]->setVisible( true );
		}
	}

	void RepresentationPresetEditor::_refreshRepresentationData()
	{
		switch ( _preset->getRepresentationType() )
		{
		case Generic::REPRESENTATION::BALL_AND_STICK: _refreshBallAndStickRepresentation(); break;
		case Generic::REPRESENTATION::BALL_AND_STICK_AND_CARTOON: _refreshBallStickAndCartoonRepresentation(); break;
		case Generic::REPRESENTATION::STICK: _refreshStickRepresentation(); break;
		case Generic::REPRESENTATION::STICK_AND_CARTOON: _refreshStickAndCartoonRepresentation(); break;
		case Generic::REPRESENTATION::VAN_DER_WAALS:
			_refreshVanDerWaalsRepresentation();
			break;
			// !V0.1
		// case Generic::REPRESENTATION::TRACE: _refreshTraceRepresentation(); break;
		case Generic::REPRESENTATION::SAS: _refreshSASRepresentation(); break;
		case Generic::REPRESENTATION::CARTOON: _refreshCartoonRepresentation(); break;
		case Generic::REPRESENTATION::SES: _refreshSESRepresentation(); break;
		}
	}
	void RepresentationPresetEditor::_refreshBallAndStickRepresentation()
	{
		_sphereRadiusWidget->setValue( _preset->getData().getSphereRadius() );
		_sphereRadiusWidget->setMinMax( Setting::ATOMS_RADIUS_MIN, Setting::ATOMS_RADIUS_MAX );

		_cylinderRadiusWidget->setValue( _preset->getData().getCylinderRadius() );
		_cylinderRadiusWidget->setMinMax( Setting::BONDS_RADIUS_MIN, Setting::BONDS_RADIUS_MAX );

		_cylinderColorBlendingMode->setCurrentIndex( int( _preset->getData().getCylinderColorBlendingMode() ) );

		_colorModeWidget->setCurrentIndex( int( _preset->getData().getColorMode() ) );
	}
	void RepresentationPresetEditor::_refreshBallStickAndCartoonRepresentation()
	{
		_sphereRadiusWidget->setValue( _preset->getData().getSphereRadius() );
		_sphereRadiusWidget->setMinMax( Setting::ATOMS_RADIUS_MIN, Setting::ATOMS_RADIUS_MAX );

		_cylinderRadiusWidget->setValue( _preset->getData().getCylinderRadius() );
		_cylinderRadiusWidget->setMinMax( Setting::BONDS_RADIUS_MIN, Setting::BONDS_RADIUS_MAX );

		_cylinderColorBlendingMode->setCurrentIndex( int( _preset->getData().getCylinderColorBlendingMode() ) );
		_colorModeWidget->setCurrentIndex( int( _preset->getData().getColorMode() ) );
		_ribbonColorModeWidget->setCurrentIndex( int( _preset->getData().getRibbonColorMode() ) );
		_ribbonColorBlendingMode->setCurrentIndex( int( _preset->getData().getRibbonColorBlendingMode() ) );
	}
	void RepresentationPresetEditor::_refreshStickRepresentation()
	{
		_cylinderRadiusWidget->setValue( _preset->getData().getCylinderRadius() );
		_cylinderRadiusWidget->setMinMax( Setting::BONDS_RADIUS_MIN, Setting::BONDS_RADIUS_MAX );
		_cylinderColorBlendingMode->setCurrentIndex( int( _preset->getData().getCylinderColorBlendingMode() ) );
		_colorModeWidget->setCurrentIndex( int( _preset->getData().getColorMode() ) );
	}
	void RepresentationPresetEditor::_refreshStickAndCartoonRepresentation()
	{
		_cylinderRadiusWidget->setValue( _preset->getData().getCylinderRadius() );
		_cylinderRadiusWidget->setMinMax( Setting::BONDS_RADIUS_MIN, Setting::BONDS_RADIUS_MAX );

		_cylinderColorBlendingMode->setCurrentIndex( int( _preset->getData().getCylinderColorBlendingMode() ) );
		_colorModeWidget->setCurrentIndex( int( _preset->getData().getColorMode() ) );
		_ribbonColorModeWidget->setCurrentIndex( int( _preset->getData().getRibbonColorMode() ) );
		_ribbonColorBlendingMode->setCurrentIndex( int( _preset->getData().getRibbonColorBlendingMode() ) );
	}
	void RepresentationPresetEditor::_refreshVanDerWaalsRepresentation()
	{
		_sphereRadiusWidget->setValue( _preset->getData().getSphereRadius() );
		_sphereRadiusWidget->setMinMax( Setting::ATOMS_RADIUS_ADD_MIN, Setting::ATOMS_RADIUS_ADD_MAX );
		_colorModeWidget->setCurrentIndex( int( _preset->getData().getColorMode() ) );
	}
	void RepresentationPresetEditor::_refreshTraceRepresentation()
	{
		_cylinderRadiusWidget->setValue( _preset->getData().getCylinderRadius() );
		_cylinderRadiusWidget->setMinMax( Setting::BONDS_RADIUS_MIN, Setting::BONDS_RADIUS_MAX );
		_colorModeWidget->setCurrentIndex( int( _preset->getData().getColorMode() ) );
	}
	void RepresentationPresetEditor::_refreshSASRepresentation()
	{
		_colorModeWidget->setCurrentIndex( int( _preset->getData().getColorMode() ) );
	}
	void RepresentationPresetEditor::_refreshSESRepresentation()
	{
		_colorModeWidget->setCurrentIndex( int( _preset->getData().getColorMode() ) );
	}
	void RepresentationPresetEditor::_refreshCartoonRepresentation()
	{
		_ribbonColorModeWidget->setCurrentIndex( int( _preset->getData().getRibbonColorMode() ) );
		_ribbonColorBlendingMode->setCurrentIndex( int( _preset->getData().getRibbonColorBlendingMode() ) );
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
		const std::string nameStr = _nameWidget->text().toStdString();

		if ( nameStr == "" )
		{
			_nameWidget->setText( QString::fromStdString( _preset->getName() ) );
			return;
		}

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
	void RepresentationPresetEditor::_onCylinderColorBendingModeChanged( const int p_colorMode )
	{
		Generic::COLOR_BLENDING_MODE colorMode = Generic::COLOR_BLENDING_MODE( p_colorMode );

		if ( !signalsBlocked() && colorMode != _preset->getData().getCylinderColorBlendingMode() )
			VTX_ACTION( new Action::Representation::ChangeCylinderColorBendingMode( _preset, colorMode ) );
	}
	void RepresentationPresetEditor::_onColorModeChanged( const int p_colorMode )
	{
		Generic::COLOR_MODE colorMode = Generic::COLOR_MODE( p_colorMode );

		if ( !signalsBlocked() && colorMode != _preset->getData().getColorMode() )
			VTX_ACTION( new Action::Representation::ChangeColorMode( _preset, colorMode ) );
	}
	void RepresentationPresetEditor::_onRibbonColorModeChanged( const int p_colorMode )
	{
		Generic::SECONDARY_STRUCTURE_COLOR_MODE colorMode = Generic::SECONDARY_STRUCTURE_COLOR_MODE( p_colorMode );

		if ( !signalsBlocked() && colorMode != _preset->getData().getRibbonColorMode() )
			VTX_ACTION( new Action::Representation::ChangeRibbonColorMode( _preset, colorMode ) );
	}
	void RepresentationPresetEditor::_onRibbonColorBlendingModeChanged( const int p_colorMode )
	{
		Generic::COLOR_BLENDING_MODE colorMode = Generic::COLOR_BLENDING_MODE( p_colorMode );

		if ( !signalsBlocked() && colorMode != _preset->getData().getRibbonColorBlendingMode() )
			VTX_ACTION( new Action::Representation::ChangeRibbonColorBendingMode( _preset, colorMode ) );
	}
	void RepresentationPresetEditor::_onColorChanged( const Color::Rgba & p_color )
	{
		if ( !signalsBlocked() && p_color != _preset->getColor() )
			VTX_ACTION( new Action::Representation::ChangeColor( _preset, p_color ) );
	}
	void RepresentationPresetEditor::_onSetDefault()
	{
		if ( !signalsBlocked()
			 && _preset != Model::Representation::RepresentationLibrary::get().getDefaultRepresentation() )
		{
			const int presetIndex
				= Model::Representation::RepresentationLibrary::get().getRepresentationIndex( _preset );
			VTX_ACTION( new Action::Representation::SetAsDefaultRepresentation( presetIndex ) );
		}
	}

} // namespace VTX::UI::Widget::Settings
