#include "ui/old_ui/ui/widget/representation/view/ribbon_widget.hpp"
#include <app/old/component/chemistry/molecule.hpp>

namespace VTX::UI::Widget::Representation::View
{
	void RibbonWidget::_setupUi( const QString & p_name )
	{
		BaseRepresentationViewWidget::_setupUi( p_name );

		_colorModeLabel = new QLabel( this );
		_colorModeLabel->setText( "Ribbon color mode" );
		_colorModeWidget = WidgetFactory::get().instantiateWidget<CustomWidget::RibbonColorModeFieldWidget>(
			this, "ribbonColorModeWidget" );
		_appendWidgetInLayout( _colorModeLabel, _colorModeWidget );

		_colorBlendingModeLabel = new QLabel( this );
		_colorBlendingModeLabel->setText( "Ribbon color blending mode" );
		_colorBlendingModeWidget = _addQComboBoxMultiFieldWidgetInLayout(
			"ribonColorBLendingModeWidget", _colorBlendingModeLabel, VTX::Core::ChemDB::Color::COLOR_BLENDING_MODE_STRING );
	}

	void RibbonWidget::_setupSlots()
	{
		connect( _colorModeWidget,
				 &CustomWidget::RibbonColorModeFieldWidget::colorChanged,
				 this,
				 &RibbonWidget::_onColorChange );
		connect( _colorModeWidget,
				 &CustomWidget::RibbonColorModeFieldWidget::colorModeChanged,
				 this,
				 &RibbonWidget::_onColorModeChange );
		connect( _colorBlendingModeWidget,
				 QOverload<int>::of( &QComboBox::currentIndexChanged ),
				 this,
				 &RibbonWidget::_onColorBlendingModeChange );
	}

	void RibbonWidget::_onColorChange( const Util::Color::Rgba & p_color ) { emit onColorChange( p_color ); }
	void RibbonWidget::_onColorModeChange( const VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE & p_newMode )
	{
		emit onColorModeChange( VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE( p_newMode ) );
	}
	void RibbonWidget::_onColorBlendingModeChange( const int p_newMode )
	{
		emit onColorBlendingModeChange( VTX::Core::ChemDB::Color::COLOR_BLENDING_MODE( p_newMode ) );
	}

	void RibbonWidget::refresh(
		const App::Old::Application::Representation::InstantiatedRepresentation &					p_representation,
		const std::unordered_set<const App::Old::Application::Representation::BaseRepresentable *> & p_targets )
	{
		Util::UI::setDynamicProperty(
			_colorBlendingModeLabel,
			Style::WidgetProperty::OVERIDDEN_PARAMETER,
			p_representation.isMemberOverrided(
				App::Old::Application::Representation::MEMBER_FLAG::ENUM::RIBBON_COLOR_BLENDING_MODE ) );

		_colorBlendingModeWidget->setCurrentIndex( int( p_representation.getRibbonData().colorBlendingMode ) );

		// Color mode widget.
		_colorModeWidget->resetState();

		const bool overriden = p_representation.isMemberOverrided(
			App::Old::Application::Representation::MEMBER_FLAG::ENUM::RIBBON_COLOR_MODE );

		Util::UI::setDynamicProperty( _colorModeLabel, Style::WidgetProperty::OVERIDDEN_PARAMETER, overriden );

		const VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE & colorMode = p_representation.getRibbonData().colorMode;

		_colorModeWidget->setColorMode( colorMode );

		if ( colorMode == VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE::CUSTOM )
		{
			_colorModeWidget->setColor( p_representation.getColor() );
		}
		else if ( colorMode == VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE::PROTEIN )
		{
			_colorModeWidget->resetState();
			for ( const App::Old::Application::Representation::BaseRepresentable * const target : p_targets )
			{
				_colorModeWidget->updateWithNewValue( std::pair( colorMode, target->getMolecule()->getColor() ) );
			}
		}
	}

	void RibbonWidget::updateWithNewValue(
		const App::Old::Application::Representation::InstantiatedRepresentation &					p_representation,
		const std::unordered_set<const App::Old::Application::Representation::BaseRepresentable *> & p_targets )
	{
		_updateLabelOverriddenProperty(
			_colorBlendingModeLabel,
			p_representation.isMemberOverrided(
				App::Old::Application::Representation::MEMBER_FLAG::ENUM::RIBBON_COLOR_BLENDING_MODE ) );

		_colorBlendingModeWidget->updateWithNewValue( int( p_representation.getRibbonData().colorBlendingMode ) );

		// Color mode widget.
		std::pair<VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE, Util::Color::Rgba> pair
			= std::pair<VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE, Util::Color::Rgba>();

		const VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE & colorMode = p_representation.getRibbonData().colorMode;

		pair.first = colorMode;

		if ( colorMode == VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE::CUSTOM )
		{
			pair.second = p_representation.getColor();
		}
		else if ( colorMode == VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE::PROTEIN )
		{
			pair.second = p_representation.getConstTarget()->getMolecule()->getColor();
		}

		_colorModeWidget->updateWithNewValue( pair );
	}

	void RibbonWidget::resetState()
	{
		Util::UI::setDynamicProperty( _colorModeLabel, Style::WidgetProperty::OVERIDDEN_PARAMETER, true );
		Util::UI::setDynamicProperty( _colorBlendingModeLabel, Style::WidgetProperty::OVERIDDEN_PARAMETER, true );

		_colorModeWidget->resetState();
		_colorBlendingModeWidget->resetState();
	}

} // namespace VTX::UI::Widget::Representation::View
