#include "ui/qt/widget/library/graphics_config/fog_section.hpp"
#include "ui/qt/helper.hpp"
#include <QLabel>
#include <QSignalBlocker>

namespace VTX::UI::QT::Widget::Library::GraphicsConfig
{
	FogSection::FogSection( QWidget * const p_parent ) : Section( "Fog", p_parent )
	{
		using namespace Renderer;
		_colorPicker = new ColorPicker( this );
		addWidget( _colorPicker );
		addWidget( new QLabel( "Near", this ) );
		_sliderNear = new EditableSlider( Qt::Orientation::Horizontal, this );
		addWidget( _sliderNear );
		_sliderNear->setMinimum( FOG_NEAR_MIN );
		_sliderNear->setMaximum( FOG_NEAR_MAX );
		addWidget( new QLabel( "Far", this ) );
		_sliderFar = new EditableSlider( Qt::Orientation::Horizontal, this );
		addWidget( _sliderFar );
		_sliderFar->setMinimum( FOG_FAR_MIN );
		_sliderFar->setMaximum( FOG_FAR_MAX );
		addWidget( new QLabel( "Density", this ) );
		_sliderDensity = new EditableSlider( Qt::Orientation::Horizontal, this );
		addWidget( _sliderDensity );
		_sliderDensity->setMinimum( FOG_DENSITY_MIN );
		_sliderDensity->setMaximum( FOG_DENSITY_MAX );

		connect(
			this,
			&HideableGroupBox::toggled,
			[ this ]( const bool p_state ) { _changeValue<E_GRAPHICS_CONFIG_VALUES::ACTIVE_FOG, bool>( p_state ); }
		);
		_colorPicker->onColorChanged += [ this ]( const QColor & p_color )
		{ _changeValue<E_GRAPHICS_CONFIG_VALUES::COLOR_FOG, Util::Color::Rgba>( Helper::fromQColor( p_color ) ); };
		connect(
			_sliderNear,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value ) { _changeValue<E_GRAPHICS_CONFIG_VALUES::FOG_NEAR, float>( p_value ); }
		);
		connect(
			_sliderFar,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value ) { _changeValue<E_GRAPHICS_CONFIG_VALUES::FOG_FAR, float>( p_value ); }
		);
		connect(
			_sliderDensity,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value ) { _changeValue<E_GRAPHICS_CONFIG_VALUES::FOG_DENSITY, float>( p_value ); }
		);
	}

	void FogSection::setConfig( const Entity p_preset, const VTX::Renderer::GraphicsConfig & p_config )
	{
		_setCurrentPreset( p_preset );
		const QSignalBlocker blockerGroup( this );
		const QSignalBlocker blockerColor( _colorPicker );
		const QSignalBlocker blockerNear( _sliderNear );
		const QSignalBlocker blockerFar( _sliderFar );
		const QSignalBlocker blockerDensity( _sliderDensity );
		const auto			 config = p_config.fog.value_or( VTX::Renderer::GraphicsConfigs::FOG_DEFAULT );
		setChecked( p_config.fog.has_value() );
		_colorPicker->setColor( Helper::toQColor( config.color ) );
		_sliderNear->setValue( config.near );
		_sliderFar->setValue( config.far );
		_sliderDensity->setValue( config.density );
	}
} // namespace VTX::UI::QT::Widget::Library::GraphicsConfig
