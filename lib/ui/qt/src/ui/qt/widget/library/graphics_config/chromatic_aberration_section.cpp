#include "ui/qt/widget/library/graphics_config/chromatic_aberration_section.hpp"
#include <QLabel>
#include <QSignalBlocker>

namespace VTX::UI::QT::Widget::Library::GraphicsConfig
{
	namespace
	{
		float _toUi( const float p_value ) { return -p_value * 100.f; }

		float _fromUi( const float p_value ) { return -p_value / 100.f; }
	} // namespace

	ChromaticAberrationSection::ChromaticAberrationSection( QWidget * const p_parent ) :
		Section( "Chromatic aberration", p_parent )
	{
		using namespace Renderer;
		const float minimum = _toUi( CHROMAB_RGB_MAX );
		const float maximum = _toUi( CHROMAB_RGB_MIN );
		addWidget( new QLabel( "Red", this ) );
		_sliderRed = new EditableSlider( Qt::Orientation::Horizontal, this );
		addWidget( _sliderRed );
		_sliderRed->setMinimum( minimum );
		_sliderRed->setMaximum( maximum );
		addWidget( new QLabel( "Green", this ) );
		_sliderGreen = new EditableSlider( Qt::Orientation::Horizontal, this );
		addWidget( _sliderGreen );
		_sliderGreen->setMinimum( minimum );
		_sliderGreen->setMaximum( maximum );
		addWidget( new QLabel( "Blue", this ) );
		_sliderBlue = new EditableSlider( Qt::Orientation::Horizontal, this );
		addWidget( _sliderBlue );
		_sliderBlue->setMinimum( minimum );
		_sliderBlue->setMaximum( maximum );

		connect(
			this,
			&HideableGroupBox::toggled,
			[ this ]( const bool p_state ) { _changeValue<E_GRAPHICS_CONFIG_VALUES::ACTIVE_CHROMAB, bool>( p_state ); }
		);
		connect(
			_sliderRed,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::CHROMAB_RED, float>( _fromUi( p_value ) ); }
		);
		connect(
			_sliderGreen,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::CHROMAB_GREEN, float>( _fromUi( p_value ) ); }
		);
		connect(
			_sliderBlue,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::CHROMAB_BLUE, float>( _fromUi( p_value ) ); }
		);
	}

	void ChromaticAberrationSection::setConfig( const Entity p_preset, const VTX::Renderer::GraphicsConfig & p_config )
	{
		_setCurrentPreset( p_preset );
		const QSignalBlocker blockerGroup( this );
		const QSignalBlocker blockerRed( _sliderRed );
		const QSignalBlocker blockerGreen( _sliderGreen );
		const QSignalBlocker blockerBlue( _sliderBlue );
		const auto			 config
			= p_config.chromaticAberration.value_or( VTX::Renderer::GraphicsConfigs::CHROMATIC_ABERRATION_DEFAULT );
		setChecked( p_config.chromaticAberration.has_value() );
		_sliderRed->setValue( _toUi( config.red ) );
		_sliderGreen->setValue( _toUi( config.green ) );
		_sliderBlue->setValue( _toUi( config.blue ) );
	}
} // namespace VTX::UI::QT::Widget::Library::GraphicsConfig
