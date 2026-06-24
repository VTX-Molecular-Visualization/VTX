#include "ui/qt/widget/library/graphics_config/outline_section.hpp"
#include "ui/qt/helper.hpp"
#include <QLabel>
#include <QSignalBlocker>

namespace VTX::UI::QT::Widget::Library::GraphicsConfig
{
	OutlineSection::OutlineSection( QWidget * const p_parent ) : Section( "Outline", p_parent )
	{
		using namespace Renderer;
		_colorPicker = new ColorPicker( this );
		addWidget( _colorPicker );
		addWidget( new QLabel( "Sensitivity", this ) );
		_sliderSensitivity = new EditableSlider( Qt::Orientation::Horizontal, this );
		addWidget( _sliderSensitivity );
		_sliderSensitivity->setMinimum( OUTLINE_SENSITIVITY_MIN );
		_sliderSensitivity->setMaximum( OUTLINE_SENSITIVITY_MAX );
		addWidget( new QLabel( "Thickness", this ) );
		_sliderThickness = new EditableSlider( Qt::Orientation::Horizontal, this );
		addWidget( _sliderThickness );
		_sliderThickness->setMinimum( OUTLINE_THICKNESS_MIN );
		_sliderThickness->setMaximum( OUTLINE_THICKNESS_MAX );
		_sliderThickness->setStep( 1 );
		_sliderThickness->setDecimals( 0 );

		connect(
			this,
			&HideableGroupBox::toggled,
			[ this ]( const bool p_state ) { _changeValue<E_GRAPHICS_CONFIG_VALUES::ACTIVE_OUTLINE, bool>( p_state ); }
		);
		_colorPicker->onColorChanged += [ this ]( const QColor & p_color )
		{ _changeValue<E_GRAPHICS_CONFIG_VALUES::COLOR_OUTLINE, Util::Color::Rgba>( Helper::fromQColor( p_color ) ); };
		connect(
			_sliderSensitivity,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::OUTLINE_SENSITIVITY, float>( p_value ); }
		);
		connect(
			_sliderThickness,
			&EditableSlider::valueChanged,
			[ this ]( const uint p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::OUTLINE_THICKNESS, uint>( p_value ); }
		);
	}

	void OutlineSection::setConfig( const Entity p_preset, const VTX::Renderer::GraphicsConfig & p_config )
	{
		_setCurrentPreset( p_preset );
		const QSignalBlocker blockerGroup( this );
		const QSignalBlocker blockerColor( _colorPicker );
		const QSignalBlocker blockerSensitivity( _sliderSensitivity );
		const QSignalBlocker blockerThickness( _sliderThickness );
		const auto			 config = p_config.outline.value_or( VTX::Renderer::GraphicsConfigs::OUTLINE_DEFAULT );
		setChecked( p_config.outline.has_value() );
		_colorPicker->setColor( Helper::toQColor( config.color ) );
		_sliderSensitivity->setValue( config.sensitivity );
		_sliderThickness->setValue( config.thickness );
	}
} // namespace VTX::UI::QT::Widget::Library::GraphicsConfig
