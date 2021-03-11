#include "base_representation_widget.hpp"
#include "action/action_manager.hpp"
#include "action/instantiated_representation.hpp"
#include "action/molecule.hpp"
#include "generic/base_representable.hpp"
#include "id.hpp"
#include "model/molecule.hpp"
#include "tool/logger.hpp"
#include "view/ui/widget/representation/instantiated_representation_view.hpp"
#include <QLabel>
#include <string>

namespace VTX::UI::Widget::Representation
{
	BaseRepresentationWidget::BaseRepresentationWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {};

	BaseRepresentationWidget::~BaseRepresentationWidget()
	{
		if ( _representationView != nullptr )
		{
			MVC::MvcManager::get().deleteView( _instantiatedRepresentation,
											   ID::View::UI_INSPECTOR_INSTANTIATED_REPRESENTATION );
			_representationView = nullptr;
		}
	}

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
		blockSignals( true );
		_refresh();
		blockSignals( false );
	}

	void BaseRepresentationWidget::setRepresentation(
		Model::Representation::InstantiatedRepresentation * const p_representation )
	{
		if ( _representationView != nullptr )
		{
			MVC::MvcManager::get().deleteView( _instantiatedRepresentation,
											   ID::View::UI_INSPECTOR_INSTANTIATED_REPRESENTATION );
			_representationView = nullptr;
		}

		_instantiatedRepresentation = p_representation;

		if ( _instantiatedRepresentation != nullptr )
		{
			_representationView
				= MVC::MvcManager::get()
					  .instantiateView<View::UI::Widget::Representation::InstantiatedRepresentationView>(
						  _instantiatedRepresentation, ID::View::UI_INSPECTOR_INSTANTIATED_REPRESENTATION );

			_representationView->setTarget( this );

			refresh();
		}
	}

	void BaseRepresentationWidget::_addSphereWidgetInLayout( const QString & p_label,
															 const float	 p_min,
															 const float	 p_max )
	{
		QLabel * const sphereLabel = new QLabel( this );
		sphereLabel->setText( p_label );

		_sphereWidget = VTX::UI::WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>(
			this, "sphereWidget" );
		_sphereWidget->setMinMax( p_min, p_max );

		const int row = _layout->rowCount();
		_layout->addWidget( sphereLabel, row, 0 );
		_layout->addWidget( _sphereWidget, row, 1 );
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
	void BaseRepresentationWidget::_setCylinderValue( const float p_value ) { _cylinderWidget->setValue( p_value ); }

	void BaseRepresentationWidget::_refreshColorModeWidget()
	{
		const Generic::COLOR_MODE & colorMode = _instantiatedRepresentation->getColorMode();

		if ( colorMode != _colorModeWidget->getColorMode() )
			_colorModeWidget->setColorMode( colorMode );

		if ( colorMode == Generic::COLOR_MODE::ATOM_CUSTOM || colorMode == Generic::COLOR_MODE::CUSTOM )
		{
			_colorModeWidget->setColor( _instantiatedRepresentation->getColor() );
		}
		else if ( colorMode == Generic::COLOR_MODE::ATOM_PROTEIN || colorMode == Generic::COLOR_MODE::PROTEIN )
		{
			_colorModeWidget->setColor( _instantiatedRepresentation->getTarget()->getMolecule()->getColor() );
		}
	}
	void BaseRepresentationWidget::_refreshSSColorModeWidget()
	{
		const Generic::SECONDARY_STRUCTURE_COLOR_MODE & colorMode
			= _instantiatedRepresentation->getSecondaryStructureColorMode();
		_ssColorModeWidget->setColorMode( colorMode );

		if ( colorMode == Generic::SECONDARY_STRUCTURE_COLOR_MODE::CUSTOM )
		{
			_ssColorModeWidget->setColor( _instantiatedRepresentation->getColor() );
		}
		else if ( colorMode == Generic::SECONDARY_STRUCTURE_COLOR_MODE::PROTEIN )
		{
			_ssColorModeWidget->setColor( _instantiatedRepresentation->getTarget()->getMolecule()->getColor() );
		}
	}

	void BaseRepresentationWidget::_onSphereRadiusChange( const float p_newRadius )
	{
		if ( signalsBlocked() )
			return;

		VTX_ACTION(
			new Action::InstantiatedRepresentation::ChangeSphereRadius( _instantiatedRepresentation, p_newRadius ) );

		emit onDataChange();
	}
	void BaseRepresentationWidget::_onCylinderRadiusChange( const float p_newRadius )
	{
		if ( signalsBlocked() )
			return;

		VTX_ACTION(
			new Action::InstantiatedRepresentation::ChangeCylinderRadius( _instantiatedRepresentation, p_newRadius ) );

		emit onDataChange();
	}
	void BaseRepresentationWidget::_colorModeChanged( const Generic::COLOR_MODE & p_colorMode )
	{
		if ( signalsBlocked() )
			return;

		VTX_ACTION(
			new Action::InstantiatedRepresentation::ChangeColorMode( _instantiatedRepresentation, p_colorMode ) );

		emit onDataChange();
	}
	void BaseRepresentationWidget::_ssColorModeChanged( const Generic::SECONDARY_STRUCTURE_COLOR_MODE & p_colorMode )
	{
		if ( signalsBlocked() )
			return;

		VTX_ACTION( new Action::InstantiatedRepresentation::ChangeSecondaryStructureColorMode(
			_instantiatedRepresentation, p_colorMode ) );

		emit onDataChange();
	}
	void BaseRepresentationWidget::_colorChanged( const Color::Rgb & p_color )
	{
		if ( signalsBlocked() )
			return;

		bool actionSend = false;
		switch ( _instantiatedRepresentation->getColorMode() )
		{
		case Generic::COLOR_MODE::ATOM_CUSTOM:
		case Generic::COLOR_MODE::CUSTOM:
			VTX_ACTION( new Action::InstantiatedRepresentation::ChangeColor( _instantiatedRepresentation, p_color ) );
			actionSend = true;
			break;

		case Generic::COLOR_MODE::ATOM_PROTEIN:
		case Generic::COLOR_MODE::PROTEIN:
			VTX_ACTION( new Action::Molecule::ChangeColor( *_instantiatedRepresentation->getTarget()->getMolecule(),
														   p_color ) );
			actionSend = true;
			break;

		case Generic::COLOR_MODE::ATOM_CHAIN:
		case Generic::COLOR_MODE::CHAIN:
		case Generic::COLOR_MODE::RESIDUE: break;

		default:
			VTX_WARNING( "COLOR_MODE " + std::to_string( int( _instantiatedRepresentation->getColorMode() ) )
						 + " not managed in BaseRepresentationWidget::_colorChanged." );
			break;
		}

		if ( actionSend )
			emit onDataChange();

		_refreshColorModeWidget();
	}
	void BaseRepresentationWidget::_ssColorChanged( const Color::Rgb & p_color )
	{
		if ( signalsBlocked() )
			return;

		bool actionSend = false;
		switch ( _instantiatedRepresentation->getSecondaryStructureColorMode() )
		{
		case Generic::SECONDARY_STRUCTURE_COLOR_MODE::CUSTOM:
			VTX_ACTION( new Action::InstantiatedRepresentation::ChangeColor( _instantiatedRepresentation, p_color ) );
			actionSend = true;
			break;

		case Generic::SECONDARY_STRUCTURE_COLOR_MODE::PROTEIN:
			VTX_ACTION( new Action::Molecule::ChangeColor( *_instantiatedRepresentation->getTarget()->getMolecule(),
														   p_color ) );
			actionSend = true;
			break;

		case Generic::SECONDARY_STRUCTURE_COLOR_MODE::JMOL:
		case Generic::SECONDARY_STRUCTURE_COLOR_MODE::CHAIN:
		case Generic::SECONDARY_STRUCTURE_COLOR_MODE::RESIDUE: break;

		default:
			VTX_WARNING( "SECONDARY_STRUCTURE_COLOR_MODE "
						 + std::to_string( int( _instantiatedRepresentation->getSecondaryStructureColorMode() ) )
						 + " not managed in BaseRepresentationWidget::_ssColorChanged." );
			break;
		}

		if ( actionSend )
			emit onDataChange();

		_refreshSSColorModeWidget();
	}
} // namespace VTX::UI::Widget::Representation
