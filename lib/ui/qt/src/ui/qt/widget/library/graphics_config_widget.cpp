#include "ui/qt/widget/library/graphics_config_widget.hpp"
#include <app/services.hpp>

namespace VTX::UI::QT::Widget::Library
{
	GraphicsConfigWidget::GraphicsConfigWidget( QWidget * const p_parent ) : BasePresetWidget( p_parent )
	{
		setTitle( "Edit render settings" );

		_backgroundSection = new GraphicsConfig::BackgroundSection( presetGroupBox() );
		addWidget( _backgroundSection );
		_lightingSection = new GraphicsConfig::LightingSection( presetGroupBox() );
		addWidget( _lightingSection );
		_ssaoSection = new GraphicsConfig::SSAOSection( presetGroupBox() );
		addWidget( _ssaoSection );
		_selectionSection = new GraphicsConfig::SelectionSection( presetGroupBox() );
		addWidget( _selectionSection );
		_outlineSection = new GraphicsConfig::OutlineSection( presetGroupBox() );
		addWidget( _outlineSection );
		_fogSection = new GraphicsConfig::FogSection( presetGroupBox() );
		addWidget( _fogSection );
		_chromaticAberrationSection = new GraphicsConfig::ChromaticAberrationSection( presetGroupBox() );
		addWidget( _chromaticAberrationSection );
		_pixelizeSection = new GraphicsConfig::PixelizeSection( presetGroupBox() );
		addWidget( _pixelizeSection );
		_crtSection = new GraphicsConfig::CRTSection( presetGroupBox() );
		addWidget( _crtSection );
	}

	void GraphicsConfigWidget::_update( const Entity p_preset )
	{
		const auto & config = App::REG().get<Renderer::GraphicsConfig>( p_preset );
		_backgroundSection->setConfig( p_preset, config );
		_lightingSection->setConfig( p_preset, config );
		_ssaoSection->setConfig( p_preset, config );
		_selectionSection->setConfig( p_preset, config );
		_outlineSection->setConfig( p_preset, config );
		_fogSection->setConfig( p_preset, config );
		_chromaticAberrationSection->setConfig( p_preset, config );
		_pixelizeSection->setConfig( p_preset, config );
		_crtSection->setConfig( p_preset, config );
	}
} // namespace VTX::UI::QT::Widget::Library
