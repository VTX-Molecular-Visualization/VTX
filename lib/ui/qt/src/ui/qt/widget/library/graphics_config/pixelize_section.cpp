#include "ui/qt/widget/library/graphics_config/pixelize_section.hpp"
#include <QLabel>
#include <QSignalBlocker>

namespace VTX::UI::QT::Widget::Library::GraphicsConfig
{
	PixelizeSection::PixelizeSection( QWidget * const p_parent ) : Section( "Pixelize", p_parent )
	{
		using namespace Renderer;
		addWidget( new QLabel( "Size", this ) );
		_sliderSize = new EditableSlider( Qt::Orientation::Horizontal, this );
		addWidget( _sliderSize );
		_sliderSize->setMinimum( PIXELIZE_SIZE_MIN );
		_sliderSize->setMaximum( PIXELIZE_SIZE_MAX );
		_sliderSize->setStep( 1 );
		_sliderSize->setDecimals( 0 );
		_checkBoxBackground = new QCheckBox( "Background", this );
		addWidget( _checkBoxBackground );

		connect(
			this,
			&HideableGroupBox::toggled,
			[ this ]( const bool p_state ) { _changeValue<E_GRAPHICS_CONFIG_VALUES::ACTIVE_PIXELIZE, bool>( p_state ); }
		);
		connect(
			_sliderSize,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::PIXELIZE_SIZE, uint>( static_cast<uint>( p_value ) ); }
		);
		connect(
			_checkBoxBackground,
			&QCheckBox::toggled,
			[ this ]( const bool p_state )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::PIXELIZE_BACKGROUND, bool>( p_state ); }
		);
	}

	void PixelizeSection::setConfig( const Entity p_preset, const VTX::Renderer::GraphicsConfig & p_config )
	{
		_setCurrentPreset( p_preset );
		const QSignalBlocker blockerGroup( this );
		const QSignalBlocker blockerSize( _sliderSize );
		const QSignalBlocker blockerBackground( _checkBoxBackground );
		const auto			 config = p_config.pixelize.value_or( VTX::Renderer::GraphicsConfigs::PIXELIZE_DEFAULT );
		setChecked( p_config.pixelize.has_value() );
		_sliderSize->setValue( config.size );
		_checkBoxBackground->setChecked( config.background );
	}
} // namespace VTX::UI::QT::Widget::Library::GraphicsConfig
