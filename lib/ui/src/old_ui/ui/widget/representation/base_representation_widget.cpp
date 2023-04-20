#include "ui/old_ui/ui/widget/representation/base_representation_widget.hpp"
#include "ui/old_ui/style.hpp"
#include "ui/old_ui/util/ui.hpp"
#include <QLabel>
#include <app/core/action/action_manager.hpp>
#include <app/action/instantiated_representation.hpp>
#include <app/action/molecule.hpp>
#include <app/old_app/generic/base_representable.hpp>
#include <app/old_app/id.hpp>
#include <app/model/molecule.hpp>
#include <string>
#include <util/logger.hpp>

namespace VTX::UI::Widget::Representation
{
	BaseRepresentationWidget::BaseRepresentationWidget( QWidget * p_parent ) :
		BaseManualWidget( p_parent ), TMultiDataField()
	{
	}

	void BaseRepresentationWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );
		_layout = new QGridLayout( this );
	}

	void BaseRepresentationWidget::_setupSlots()
	{
		if ( _sphereWidget != nullptr )
		{
			connect( _sphereWidget,
					 &View::SphereWidget::onRadiusChange,
					 this,
					 &BaseRepresentationWidget::_onSphereRadiusChange );

			connect( _sphereWidget,
					 &View::SphereWidget::onRadiusOffsetChange,
					 this,
					 &BaseRepresentationWidget::_onSphereRadiusOffsetChange );
		}

		if ( _cylinderWidget != nullptr )
		{
			connect( _cylinderWidget,
					 &View::CylinderWidget::onRadiusChange,
					 this,
					 &BaseRepresentationWidget::_onCylinderRadiusChange );

			connect( _cylinderWidget,
					 &View::CylinderWidget::onColorBlendingModeChange,
					 this,
					 &BaseRepresentationWidget::_onCylinderColorBlendingModeChange );
		}

		if ( _ribbonWidget != nullptr )
		{
			connect( _ribbonWidget,
					 &View::RibbonWidget::onColorChange,
					 this,
					 &BaseRepresentationWidget::_onRibbonColorChange );

			connect( _ribbonWidget,
					 &View::RibbonWidget::onColorModeChange,
					 this,
					 &BaseRepresentationWidget::_onRibbonColorModeChange );

			connect( _ribbonWidget,
					 &View::RibbonWidget::onColorBlendingModeChange,
					 this,
					 &BaseRepresentationWidget::_onRibbonColorBlendingModeChange );
		}

		if ( _colorModeWidget != nullptr )
		{
			connect( _colorModeWidget,
					 &CustomWidget::ColorModeFieldWidget::colorModeChanged,
					 this,
					 &BaseRepresentationWidget::_onColorModeChange );
			connect( _colorModeWidget,
					 &CustomWidget::ColorModeFieldWidget::colorChanged,
					 this,
					 &BaseRepresentationWidget::_onColorChange );
		}
	}

	void BaseRepresentationWidget::refresh()
	{
		const bool oldBlockState = blockSignals( true );

		if ( _sphereWidget != nullptr )
		{
			_sphereWidget->refresh( *_instantiatedRepresentation, _targets );
		}

		if ( _cylinderWidget != nullptr )
		{
			_cylinderWidget->refresh( *_instantiatedRepresentation, _targets );
		}

		if ( _ribbonWidget != nullptr )
		{
			_ribbonWidget->refresh( *_instantiatedRepresentation, _targets );
		}

		if ( _colorModeWidget != nullptr )
		{
			_refreshColorModeWidget();
		}

		blockSignals( oldBlockState );
	}

	void BaseRepresentationWidget::setRepresentation( InstantiatedRepresentation * const p_representation )
	{
		_instantiatedRepresentation = p_representation;
		refresh();
	}

	void BaseRepresentationWidget::updateWithNewValue(
		const Model::Representation::InstantiatedRepresentation & p_value )
	{
		if ( _sphereWidget != nullptr )
		{
			_sphereWidget->updateWithNewValue( p_value, _targets );
		}

		if ( _cylinderWidget != nullptr )
		{
			_cylinderWidget->updateWithNewValue( p_value, _targets );
		}

		if ( _ribbonWidget != nullptr )
		{
			_ribbonWidget->updateWithNewValue( p_value, _targets );
		}

		if ( _colorModeWidget != nullptr )
		{
			_addColorModeValue( p_value );
		}
	}

	void BaseRepresentationWidget::resetState()
	{
		if ( _sphereWidget != nullptr )
		{
			_sphereWidget->resetState();
		}
		if ( _cylinderWidget != nullptr )
		{
			_cylinderWidget->resetState();
		}
		if ( _ribbonWidget != nullptr )
		{
			_ribbonWidget->resetState();
		}
		if ( _colorModeWidget != nullptr )
		{
			Util::UI::setDynamicProperty( _colorModeLabel, Style::WidgetProperty::OVERIDDEN_PARAMETER, true );
			_colorModeWidget->resetState();
		}

		_targets.clear();
	}

	void BaseRepresentationWidget::_addSphereWidgetInLayout()
	{
		_sphereWidget = VTX::UI::WidgetFactory::get().instantiateWidget<View::SphereWidget>( this, "sphereWidget" );
	}

	void BaseRepresentationWidget::_addCylinderWidgetInLayout()
	{
		_cylinderWidget
			= VTX::UI::WidgetFactory::get().instantiateWidget<View::CylinderWidget>( this, "cylinderWidget" );
	}

	void BaseRepresentationWidget::_addRibbonWidgetInLayout()
	{
		_ribbonWidget = VTX::UI::WidgetFactory::get().instantiateWidget<View::RibbonWidget>( this, "ribbonWidget" );
	}

	void BaseRepresentationWidget::_addColorModeWidgetInLayout()
	{
		_colorModeLabel = new QLabel( this );
		_colorModeLabel->setText( "Color mode" );
		_colorModeWidget = VTX::UI::WidgetFactory::get().instantiateWidget<CustomWidget::ColorModeFieldWidget>(
			this, "colorModeWidget" );
		_appendWidgetInLayout( _colorModeLabel, _colorModeWidget );
	}

	void BaseRepresentationWidget::_appendWidgetInLayout( QWidget * const p_label, QWidget * const p_widget )
	{
		const int row = _layout->rowCount();
		_layout->addWidget( p_label, row, 0 );
		_layout->addWidget( p_widget, row, 1 );
	}

	void BaseRepresentationWidget::_refreshColorModeWidget()
	{
		_colorModeWidget->resetState();

		const bool overriden = _instantiatedRepresentation->isMemberOverrided( MEMBER_FLAG::COLOR_MODE );

		Util::UI::setDynamicProperty( _colorModeLabel, Style::WidgetProperty::OVERIDDEN_PARAMETER, overriden );

		const Generic::COLOR_MODE & colorMode = _instantiatedRepresentation->getColorMode();

		if ( colorMode != _colorModeWidget->getColorMode() )
			_colorModeWidget->setColorMode( colorMode );

		if ( colorMode == Generic::COLOR_MODE::ATOM_CUSTOM || colorMode == Generic::COLOR_MODE::CUSTOM )
		{
			_colorModeWidget->setColor( _instantiatedRepresentation->getColor() );
		}
		else if ( colorMode == Generic::COLOR_MODE::ATOM_PROTEIN || colorMode == Generic::COLOR_MODE::PROTEIN )
		{
			_colorModeWidget->resetState();
			for ( const Generic::BaseRepresentable * const target : _targets )
			{
				_colorModeWidget->updateWithNewValue( std::pair( colorMode, target->getMolecule()->getColor() ) );
			}
		}
	}

	void BaseRepresentationWidget::_addColorModeValue( const InstantiatedRepresentation & p_representation )
	{
		std::pair<Generic::COLOR_MODE, Color::Rgba> pair = std::pair<Generic::COLOR_MODE, Color::Rgba>();

		Generic::COLOR_MODE colorMode = p_representation.getColorMode();
		pair.first					  = colorMode;

		if ( colorMode == Generic::COLOR_MODE::ATOM_CUSTOM || colorMode == Generic::COLOR_MODE::CUSTOM )
		{
			pair.second = p_representation.getColor();
		}
		else if ( colorMode == Generic::COLOR_MODE::ATOM_PROTEIN || colorMode == Generic::COLOR_MODE::PROTEIN )
		{
			pair.second = p_representation.getTarget()->getMolecule()->getColor();
		}

		_colorModeWidget->updateWithNewValue( pair );
	}

	void BaseRepresentationWidget::_onSphereRadiusChange( const float p_newRadius )
	{
		if ( signalsBlocked() )
			return;

		_instantiatedRepresentation->setSphereRadius( p_newRadius );

		emit onDataChange( Model::Representation::MEMBER_FLAG::SPHERE_RADIUS_FIXED );
	}

	void BaseRepresentationWidget::_onSphereRadiusOffsetChange( const float p_newRadius )
	{
		if ( signalsBlocked() )
			return;

		_instantiatedRepresentation->setSphereRadius( p_newRadius );

		emit onDataChange( Model::Representation::MEMBER_FLAG::SPHERE_RADIUS_ADD );
	}

	void BaseRepresentationWidget::_onCylinderRadiusChange( const float p_newRadius )
	{
		if ( signalsBlocked() )
			return;

		_instantiatedRepresentation->setCylinderRadius( p_newRadius );

		emit onDataChange( Model::Representation::MEMBER_FLAG::CYLINDER_RADIUS );
	}

	void BaseRepresentationWidget::_onCylinderColorBlendingModeChange(
		const Generic::COLOR_BLENDING_MODE & p_colorBlendindrMode )
	{
		if ( signalsBlocked() )
			return;

		_instantiatedRepresentation->setCylinderColorBlendingMode( p_colorBlendindrMode );

		emit onDataChange( Model::Representation::MEMBER_FLAG::CYLINDER_COLOR_BLENDING_MODE );
	}

	void BaseRepresentationWidget::_onRibbonColorChange( const Color::Rgba & p_color )
	{
		if ( signalsBlocked() )
			return;

		emit onColorChange( p_color, true );
	}

	void BaseRepresentationWidget::_onRibbonColorModeChange(
		const Generic::SECONDARY_STRUCTURE_COLOR_MODE & p_colorMode )
	{
		if ( signalsBlocked() )
			return;

		_instantiatedRepresentation->setRibbonColorMode( p_colorMode, false );

		emit onDataChange( Model::Representation::MEMBER_FLAG::RIBBON_COLOR_MODE );
	}

	void BaseRepresentationWidget::_onRibbonColorBlendingModeChange( const Generic::COLOR_BLENDING_MODE & p_mode )
	{
		if ( signalsBlocked() )
			return;

		_instantiatedRepresentation->setRibbonColorBlendingMode( p_mode, false );

		emit onDataChange( Model::Representation::MEMBER_FLAG::RIBBON_COLOR_BLENDING_MODE );
	}

	void BaseRepresentationWidget::_onColorChange( const Color::Rgba & p_color )
	{
		if ( signalsBlocked() )
			return;

		emit onColorChange( p_color, false );
	}

	void BaseRepresentationWidget::_onColorModeChange( const Generic::COLOR_MODE & p_colorMode )
	{
		if ( signalsBlocked() )
			return;

		_instantiatedRepresentation->setColorMode( p_colorMode, false );

		emit onDataChange( Model::Representation::MEMBER_FLAG::COLOR_MODE );
	}

	void BaseRepresentationWidget::_displayDifferentsDataFeedback() {}

} // namespace VTX::UI::Widget::Representation
