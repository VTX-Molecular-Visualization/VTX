#include "base_representation_widget.hpp"
#include "action/action_manager.hpp"
#include "action/instantiated_representation.hpp"
#include "action/molecule.hpp"
#include "generic/base_representable.hpp"
#include "id.hpp"
#include "model/molecule.hpp"
#include "tool/logger.hpp"
#include <QLabel>
#include <string>

namespace VTX::UI::Widget::Representation
{
	BaseRepresentationWidget::BaseRepresentationWidget( QWidget * p_parent ) :
		BaseManualWidget( p_parent ), TMultiDataField() {};

	BaseRepresentationWidget::~BaseRepresentationWidget() {}

	void BaseRepresentationWidget::notifyInstantiatedRepresentationViewDeleted() { _representationView = nullptr; }

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
					 &CustomWidget::FloatFieldSliderWidget::onValueChange,
					 this,
					 &BaseRepresentationWidget::_onSphereRadiusChange );
		}

		if ( _cylinderWidget != nullptr )
		{
			connect( _cylinderWidget,
					 &CustomWidget::FloatFieldSliderWidget::onValueChange,
					 this,
					 &BaseRepresentationWidget::_onCylinderRadiusChange );
		}

		if ( _colorModeWidget != nullptr )
		{
			connect( _colorModeWidget,
					 &CustomWidget::ColorModeFieldWidget::colorModeChanged,
					 this,
					 &BaseRepresentationWidget::_colorModeChanged );
			connect( _colorModeWidget,
					 &CustomWidget::ColorModeFieldWidget::colorChanged,
					 this,
					 &BaseRepresentationWidget::_colorChanged );
		}

		if ( _ssColorModeWidget != nullptr )
		{
			connect( _ssColorModeWidget,
					 &CustomWidget::SecondaryStructureColorModeFieldWidget::colorModeChanged,
					 this,
					 &BaseRepresentationWidget::_ssColorModeChanged );
			connect( _ssColorModeWidget,
					 &CustomWidget::SecondaryStructureColorModeFieldWidget::colorChanged,
					 this,
					 &BaseRepresentationWidget::_ssColorChanged );
		}
	}

	void BaseRepresentationWidget::refresh()
	{
		const bool oldBlockState = blockSignals( true );
		_refresh();
		blockSignals( oldBlockState );
	}

	void BaseRepresentationWidget::setRepresentation( InstantiatedRepresentation * const p_representation )
	{
		_instantiatedRepresentation = p_representation;
		refresh();
	}

	void BaseRepresentationWidget::_addSphereWidgetInLayout( const QString &						  p_label,
															 const float							  p_min,
															 const float							  p_max,
															 const Model::Representation::MEMBER_FLAG p_sphereFlag )
	{
		QLabel * const sphereLabel = new QLabel( this );
		sphereLabel->setText( p_label );

		_sphereWidget = VTX::UI::WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>(
			this, "sphereWidget" );
		_sphereWidget->setMinMax( p_min, p_max );

		const int row = _layout->rowCount();
		_layout->addWidget( sphereLabel, row, 0 );
		_layout->addWidget( _sphereWidget, row, 1 );

		_sphereFlag = p_sphereFlag;
	}
	void BaseRepresentationWidget::_addCylinderWidgetInLayout( const QString & p_label,
															   const float	   p_min,
															   const float	   p_max )
	{
		QLabel * const cylinderLabel = new QLabel( this );
		cylinderLabel->setText( p_label );

		_cylinderWidget = VTX::UI::WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>(
			this, "cylinderWidget" );
		_cylinderWidget->setMinMax( p_min, p_max );

		const int row = _layout->rowCount();
		_layout->addWidget( cylinderLabel, row, 0 );
		_layout->addWidget( _cylinderWidget, row, 1 );
	}
	void BaseRepresentationWidget::_addColorModeInLayout( const QString & p_label )
	{
		QLabel * const colorModeLabel = new QLabel( this );
		colorModeLabel->setText( p_label );

		_colorModeWidget = VTX::UI::WidgetFactory::get().instantiateWidget<CustomWidget::ColorModeFieldWidget>(
			this, "colorModeWidget" );

		const int row = _layout->rowCount();
		_layout->addWidget( colorModeLabel, row, 0 );
		_layout->addWidget( _colorModeWidget, row, 1 );
	}
	void BaseRepresentationWidget::_addSSColorModeInLayout( const QString & p_label )
	{
		QLabel * const ssColorModeLabel = new QLabel( this );
		ssColorModeLabel->setText( p_label );

		_ssColorModeWidget
			= VTX::UI::WidgetFactory::get().instantiateWidget<CustomWidget::SecondaryStructureColorModeFieldWidget>(
				this, "ssColorModeWidget" );

		const int row = _layout->rowCount();
		_layout->addWidget( ssColorModeLabel, row, 0 );
		_layout->addWidget( _ssColorModeWidget, row, 1 );
	}

	void BaseRepresentationWidget::_setSphereValue( const float p_value ) { _sphereWidget->setValue( p_value ); }
	void BaseRepresentationWidget::_addSphereValue( const float p_value )
	{
		_sphereWidget->updateWithNewValue( p_value );
	}
	void BaseRepresentationWidget::_setCylinderValue( const float p_value ) { _cylinderWidget->setValue( p_value ); }
	void BaseRepresentationWidget::_addCylinderValue( const float p_value )
	{
		_cylinderWidget->updateWithNewValue( p_value );
	}

	void BaseRepresentationWidget::_refreshColorModeWidget()
	{
		_colorModeWidget->resetState();

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
	void BaseRepresentationWidget::_refreshColorModeWidget( const InstantiatedRepresentation & p_representation )
	{
		const Generic::COLOR_MODE & colorMode = p_representation.getColorMode();

		if ( colorMode != _colorModeWidget->getColorMode() )
			_colorModeWidget->setColorMode( colorMode );

		if ( colorMode == Generic::COLOR_MODE::ATOM_CUSTOM || colorMode == Generic::COLOR_MODE::CUSTOM )
		{
			_colorModeWidget->setColor( p_representation.getColor() );
		}
		else if ( colorMode == Generic::COLOR_MODE::ATOM_PROTEIN || colorMode == Generic::COLOR_MODE::PROTEIN )
		{
			_colorModeWidget->resetState();
			for ( const Generic::BaseRepresentable * const target : _targets )
				_colorModeWidget->updateWithNewValue( std::pair( colorMode, target->getMolecule()->getColor() ) );
		}
	}
	void BaseRepresentationWidget::_refreshSSColorModeWidget()
	{
		_ssColorModeWidget->resetState();

		const Generic::SECONDARY_STRUCTURE_COLOR_MODE & colorMode
			= _instantiatedRepresentation->getSecondaryStructureColorMode();

		_ssColorModeWidget->setColorMode( colorMode );

		if ( colorMode == Generic::SECONDARY_STRUCTURE_COLOR_MODE::CUSTOM )
		{
			_ssColorModeWidget->setColor( _instantiatedRepresentation->getColor() );
		}
		else if ( colorMode == Generic::SECONDARY_STRUCTURE_COLOR_MODE::PROTEIN )
		{
			_ssColorModeWidget->resetState();
			for ( const Generic::BaseRepresentable * const target : _targets )
			{
				_ssColorModeWidget->updateWithNewValue( std::pair( colorMode, target->getMolecule()->getColor() ) );
			}
		}
	}
	void BaseRepresentationWidget::_refreshSSColorModeWidget( const InstantiatedRepresentation & p_representation )
	{
		const Generic::SECONDARY_STRUCTURE_COLOR_MODE & colorMode = p_representation.getSecondaryStructureColorMode();
		_ssColorModeWidget->setColorMode( colorMode );

		if ( colorMode == Generic::SECONDARY_STRUCTURE_COLOR_MODE::CUSTOM )
		{
			_ssColorModeWidget->setColor( p_representation.getColor() );
		}
		else if ( colorMode == Generic::SECONDARY_STRUCTURE_COLOR_MODE::PROTEIN )
		{
			_ssColorModeWidget->resetState();
			for ( const Generic::BaseRepresentable * const target : _targets )
			{
				_ssColorModeWidget->updateWithNewValue( std::pair( colorMode, target->getMolecule()->getColor() ) );
			}
		}
	}

	void BaseRepresentationWidget::_addColorModeValue( const InstantiatedRepresentation & p_representation )
	{
		std::pair<Generic::COLOR_MODE, Color::Rgb> pair = std::pair<Generic::COLOR_MODE, Color::Rgb>();

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
	void BaseRepresentationWidget::_addSSColorModeValue( const InstantiatedRepresentation & p_representation )
	{
		std::pair<Generic::SECONDARY_STRUCTURE_COLOR_MODE, Color::Rgb> pair
			= std::pair<Generic::SECONDARY_STRUCTURE_COLOR_MODE, Color::Rgb>();

		const Generic::SECONDARY_STRUCTURE_COLOR_MODE & colorMode
			= _instantiatedRepresentation->getSecondaryStructureColorMode();

		pair.first = colorMode;

		if ( colorMode == Generic::SECONDARY_STRUCTURE_COLOR_MODE::CUSTOM )
		{
			pair.second = _instantiatedRepresentation->getColor();
		}
		else if ( colorMode == Generic::SECONDARY_STRUCTURE_COLOR_MODE::PROTEIN )
		{
			pair.second = _instantiatedRepresentation->getTarget()->getMolecule()->getColor();
		}

		_ssColorModeWidget->updateWithNewValue( pair );
	}

	void BaseRepresentationWidget::_onSphereRadiusChange( const float p_newRadius )
	{
		if ( signalsBlocked() )
			return;

		_instantiatedRepresentation->setSphereRadius( p_newRadius );

		emit onDataChange( _sphereFlag );
	}
	void BaseRepresentationWidget::_onCylinderRadiusChange( const float p_newRadius )
	{
		if ( signalsBlocked() )
			return;

		_instantiatedRepresentation->setCylinderRadius( p_newRadius );

		emit onDataChange( Model::Representation::MEMBER_FLAG::CYLINDER_RADIUS );
	}
	void BaseRepresentationWidget::_colorModeChanged( const Generic::COLOR_MODE & p_colorMode )
	{
		if ( signalsBlocked() )
			return;

		_instantiatedRepresentation->setColorMode( p_colorMode, false );

		emit onDataChange( Model::Representation::MEMBER_FLAG::COLOR_MODE );
	}
	void BaseRepresentationWidget::_ssColorModeChanged( const Generic::SECONDARY_STRUCTURE_COLOR_MODE & p_colorMode )
	{
		if ( signalsBlocked() )
			return;

		_instantiatedRepresentation->setSecondaryStructureColorMode( p_colorMode, false );

		emit onDataChange( Model::Representation::MEMBER_FLAG::SS_COLOR_MODE );
	}
	void BaseRepresentationWidget::_colorChanged( const Color::Rgb & p_color )
	{
		if ( signalsBlocked() )
			return;

		emit onColorChange( p_color, false );
	}
	void BaseRepresentationWidget::_ssColorChanged( const Color::Rgb & p_color )
	{
		if ( signalsBlocked() )
			return;

		emit onColorChange( p_color, true );
	}

	void BaseRepresentationWidget::resetState()
	{
		if ( _sphereWidget != nullptr )
			_sphereWidget->resetState();
		if ( _cylinderWidget != nullptr )
			_cylinderWidget->resetState();
		if ( _colorModeWidget != nullptr )
			_colorModeWidget->resetState();
		if ( _ssColorModeWidget != nullptr )
			_ssColorModeWidget->resetState();

		_targets.clear();
	}

	void BaseRepresentationWidget::updateWithNewValue(
		const Model::Representation::InstantiatedRepresentation & p_value )
	{
		_targets.emplace( p_value.getTarget() );
	};

	void BaseRepresentationWidget::_displayDifferentsDataFeedback() {}

} // namespace VTX::UI::Widget::Representation