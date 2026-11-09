#include "ui/qt/widget/library/graphics_config/ssao_section.hpp"
#include "ui/qt/helper.hpp"
#include <QLabel>
#include <QSignalBlocker>
#include <app/arguments.hpp>
#include <app/services.hpp>

namespace VTX::UI::QT::Widget::Library::GraphicsConfig
{
	namespace
	{
		constexpr float _scaleFromUi( const int p_index ) { return p_index == 1 ? 2.f : 1.f; }

		constexpr int _scaleToUi( const float p_value ) { return p_value == 2.f ? 1 : 0; }
	} // namespace

	SSAOSection::SSAOSection( QWidget * const p_parent ) : Section( "Shadows", p_parent )
	{
		using namespace Renderer;
		_comboBoxMethod = Helper::makeQObject<QComboBox>( this );
		addWidget( _comboBoxMethod );
		constexpr std::array METHOD_STR { QUtf8StringView { "SSAO" },
										  QUtf8StringView { "SSAO line" },
										  QUtf8StringView { "SAO" },
										  QUtf8StringView { "BMGTAO" },
										  QUtf8StringView { "HBAO" } };

		static_assert( METHOD_STR.size() == static_cast<size_t>( E_SSAO_METHOD::COUNT ) );

		for ( const QUtf8StringView method : METHOD_STR )
		{
			_comboBoxMethod->addItem( method.toString() );
		}

		_comboBoxScale = Helper::makeQObject<QComboBox>( this );
		addWidget( _comboBoxScale );
		_comboBoxScale->addItem( "Normal" );
		_comboBoxScale->addItem( "Downscale" );
		_comboBoxMethod->setVisible( App::ARGS().debug );
		_comboBoxScale->setVisible( App::ARGS().debug );

		addWidget( Helper::makeQObject<QLabel>( "Intensity", this ) );
		_sliderIntensity = Helper::makeQObject<EditableSlider>( Qt::Orientation::Horizontal, this );
		addWidget( _sliderIntensity );
		_sliderIntensity->setMinimum( SSAO_INTENSITY_MIN );
		_sliderIntensity->setMaximum( SSAO_INTENSITY_MAX );
		addWidget( Helper::makeQObject<QLabel>( "Blur size", this ) );
		_sliderBlurSize = Helper::makeQObject<EditableSlider>( Qt::Orientation::Horizontal, this );
		addWidget( _sliderBlurSize );
		_sliderBlurSize->setMinimum( BLUR_SIZE_MIN );
		_sliderBlurSize->setMaximum( BLUR_SIZE_MAX );

		connect( this,
				 &HideableGroupBox::toggled,
				 [ this ]( const bool p_state )
				 { _changeValue<E_GRAPHICS_CONFIG_VALUES::ACTIVE_SSAO, bool>( p_state ); } );
		connect( _comboBoxMethod,
				 QOverload<int>::of( &QComboBox::currentIndexChanged ),
				 [ this ]( const int p_index )
				 { _changeValue<E_GRAPHICS_CONFIG_VALUES::SSAO_METHOD, E_SSAO_METHOD>( E_SSAO_METHOD( p_index ) ); } );
		connect( _comboBoxScale,
				 QOverload<int>::of( &QComboBox::currentIndexChanged ),
				 [ this ]( const int p_index )
				 { _changeValue<E_GRAPHICS_CONFIG_VALUES::SSAO_SCALE, float>( _scaleFromUi( p_index ) ); } );
		connect( _sliderIntensity,
				 &EditableSlider::valueChanged,
				 [ this ]( const float p_value )
				 { _changeValue<E_GRAPHICS_CONFIG_VALUES::SSAO_INTENSITY, float>( p_value ); } );
		connect( _sliderBlurSize,
				 &EditableSlider::valueChanged,
				 [ this ]( const float p_value )
				 { _changeValue<E_GRAPHICS_CONFIG_VALUES::BLUR_SIZE, float>( p_value ); } );
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
		_comboBoxMethod->setCurrentIndex( static_cast<int>( config.method ) );
		_comboBoxScale->setCurrentIndex( _scaleToUi( config.scale ) );
		_sliderIntensity->setValue( config.intensity );
		_sliderBlurSize->setValue( config.blurSize );
	}
} // namespace VTX::UI::QT::Widget::Library::GraphicsConfig
