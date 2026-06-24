#include "ui/qt/widget/library/graphics_config/ssao_section.hpp"
#include <QLabel>
#include <QSignalBlocker>
#include <app/arguments.hpp>
#include <app/services.hpp>
#include <string_view>

namespace VTX::UI::QT::Widget::Library::GraphicsConfig
{
	namespace
	{
		float _scaleFromUi( const int p_index ) { return p_index == 1 ? 2.f : 1.f; }

		int _scaleToUi( const float p_value ) { return p_value == 2.f ? 1 : 0; }
	} // namespace

	SSAOSection::SSAOSection( QWidget * const p_parent ) : Section( "Shadows", p_parent )
	{
		using namespace Renderer;
		_comboBoxMethod = new QComboBox( this );
		addWidget( _comboBoxMethod );
		constexpr std::string_view METHOD_STR[ int( E_SSAO_METHOD::COUNT ) ]
			= { "SSAO", "SSAO line", "SAO", "BMGTAO", "HBAO" };
		for ( int i = 0; i < int( E_SSAO_METHOD::COUNT ); ++i )
		{
			_comboBoxMethod->addItem( METHOD_STR[ i ].data() );
		}

		_comboBoxScale = new QComboBox( this );
		addWidget( _comboBoxScale );
		_comboBoxScale->addItem( "Normal" );
		_comboBoxScale->addItem( "Downscale" );
		_comboBoxMethod->setVisible( App::ARGS().debug );
		_comboBoxScale->setVisible( App::ARGS().debug );

		addWidget( new QLabel( "Intensity", this ) );
		_sliderIntensity = new EditableSlider( Qt::Orientation::Horizontal, this );
		addWidget( _sliderIntensity );
		_sliderIntensity->setMinimum( SSAO_INTENSITY_MIN );
		_sliderIntensity->setMaximum( SSAO_INTENSITY_MAX );
		addWidget( new QLabel( "Blur size", this ) );
		_sliderBlurSize = new EditableSlider( Qt::Orientation::Horizontal, this );
		addWidget( _sliderBlurSize );
		_sliderBlurSize->setMinimum( BLUR_SIZE_MIN );
		_sliderBlurSize->setMaximum( BLUR_SIZE_MAX );

		connect(
			this,
			&HideableGroupBox::toggled,
			[ this ]( const bool p_state ) { _changeValue<E_GRAPHICS_CONFIG_VALUES::ACTIVE_SSAO, bool>( p_state ); }
		);
		connect(
			_comboBoxMethod,
			QOverload<int>::of( &QComboBox::currentIndexChanged ),
			[ this ]( const int p_index )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::SSAO_METHOD, E_SSAO_METHOD>( E_SSAO_METHOD( p_index ) ); }
		);
		connect(
			_comboBoxScale,
			QOverload<int>::of( &QComboBox::currentIndexChanged ),
			[ this ]( const int p_index )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::SSAO_SCALE, float>( _scaleFromUi( p_index ) ); }
		);
		connect(
			_sliderIntensity,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::SSAO_INTENSITY, float>( p_value ); }
		);
		connect(
			_sliderBlurSize,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value ) { _changeValue<E_GRAPHICS_CONFIG_VALUES::BLUR_SIZE, float>( p_value ); }
		);
	}

	void SSAOSection::setConfig( const Entity p_preset, const VTX::Renderer::GraphicsConfig & p_config )
	{
		_setCurrentPreset( p_preset );
		const QSignalBlocker blockerGroup( this );
		const QSignalBlocker blockerMethod( _comboBoxMethod );
		const QSignalBlocker blockerScale( _comboBoxScale );
		const QSignalBlocker blockerIntensity( _sliderIntensity );
		const QSignalBlocker blockerBlur( _sliderBlurSize );
		const auto			 config = p_config.ssao.value_or( VTX::Renderer::GraphicsConfigs::SSAO_DEFAULT );
		setChecked( p_config.ssao.has_value() );
		_comboBoxMethod->setCurrentIndex( int( config.method ) );
		_comboBoxScale->setCurrentIndex( _scaleToUi( config.scale ) );
		_sliderIntensity->setValue( config.intensity );
		_sliderBlurSize->setValue( config.blurSize );
	}
} // namespace VTX::UI::QT::Widget::Library::GraphicsConfig
