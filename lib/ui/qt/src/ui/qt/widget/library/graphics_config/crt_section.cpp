#include "ui/qt/widget/library/graphics_config/crt_section.hpp"
#include <QLabel>
#include <QSignalBlocker>

namespace VTX::UI::QT::Widget::Library::GraphicsConfig
{
	CRTSection::CRTSection( QWidget * const p_parent ) : Section( "CRT", p_parent )
	{
		using namespace Renderer;
		auto addSlider
			= [ this ](
				  const QString & p_label, QPointer<EditableSlider> & p_slider, const float p_min, const float p_max
			  )
		{
			addWidget( new QLabel( p_label, this ) );
			p_slider = new EditableSlider( Qt::Orientation::Horizontal, this );
			addWidget( p_slider );
			p_slider->setMinimum( p_min );
			p_slider->setMaximum( p_max );
		};
		addSlider( "Curvature X", _sliderCurvatureX, CRT_CURVATURE_MIN, CRT_CURVATURE_MAX );
		addSlider( "Curvature Y", _sliderCurvatureY, CRT_CURVATURE_MIN, CRT_CURVATURE_MAX );
		addSlider( "Ratio", _sliderRatio, CRT_RATIO_MIN, CRT_RATIO_MAX );
		addSlider( "Graniness X", _sliderGraninessX, CRT_GRANINESS_MIN, CRT_GRANINESS_MAX );
		addSlider( "Graniness Y", _sliderGraninessY, CRT_GRANINESS_MIN, CRT_GRANINESS_MAX );
		addSlider(
			"Vignette roundness", _sliderVignetteRoundness, CRT_VIGNETTE_ROUNDNESS_MIN, CRT_VIGNETTE_ROUNDNESS_MAX
		);
		addSlider(
			"Vignette intensity", _sliderVignetteIntensity, CRT_VIGNETTE_INTENSITY_MIN, CRT_VIGNETTE_INTENSITY_MAX
		);
		addSlider( "Brightness", _sliderBrightness, CRT_BRIGHTNESS_MIN, CRT_BRIGHTNESS_MAX );

		connect(
			this,
			&HideableGroupBox::toggled,
			[ this ]( const bool p_state ) { _changeValue<E_GRAPHICS_CONFIG_VALUES::ACTIVE_CRT, bool>( p_state ); }
		);
		auto connectSlider = [ this ]<E_GRAPHICS_CONFIG_VALUES S>( EditableSlider * const p_slider )
		{
			connect(
				p_slider,
				&EditableSlider::valueChanged,
				[ this ]( const float p_value ) { _changeValue<S, float>( p_value ); }
			);
		};
		connectSlider.operator()<E_GRAPHICS_CONFIG_VALUES::CRT_CURVATURE_X>( _sliderCurvatureX );
		connectSlider.operator()<E_GRAPHICS_CONFIG_VALUES::CRT_CURVATURE_Y>( _sliderCurvatureY );
		connectSlider.operator()<E_GRAPHICS_CONFIG_VALUES::CRT_RATIO>( _sliderRatio );
		connectSlider.operator()<E_GRAPHICS_CONFIG_VALUES::CRT_GRANINESS_X>( _sliderGraninessX );
		connectSlider.operator()<E_GRAPHICS_CONFIG_VALUES::CRT_GRANINESS_Y>( _sliderGraninessY );
		connectSlider.operator()<E_GRAPHICS_CONFIG_VALUES::CRT_VIGNETTE_ROUNDNESS>( _sliderVignetteRoundness );
		connectSlider.operator()<E_GRAPHICS_CONFIG_VALUES::CRT_VIGNETTE_INTENSITY>( _sliderVignetteIntensity );
		connectSlider.operator()<E_GRAPHICS_CONFIG_VALUES::CRT_BRIGHTNESS>( _sliderBrightness );
	}

	void CRTSection::setConfig( const Entity p_preset, const VTX::Renderer::GraphicsConfig & p_config )
	{
		_setCurrentPreset( p_preset );
		const QSignalBlocker blockerGroup( this );
		const QSignalBlocker blockerCurvatureX( _sliderCurvatureX );
		const QSignalBlocker blockerCurvatureY( _sliderCurvatureY );
		const QSignalBlocker blockerRatio( _sliderRatio );
		const QSignalBlocker blockerGraninessX( _sliderGraninessX );
		const QSignalBlocker blockerGraninessY( _sliderGraninessY );
		const QSignalBlocker blockerVignetteRoundness( _sliderVignetteRoundness );
		const QSignalBlocker blockerVignetteIntensity( _sliderVignetteIntensity );
		const QSignalBlocker blockerBrightness( _sliderBrightness );
		const auto			 config = p_config.crt.value_or( VTX::Renderer::GraphicsConfigs::CRT_DEFAULT );
		setChecked( p_config.crt.has_value() );
		_sliderCurvatureX->setValue( config.curvatureX );
		_sliderCurvatureY->setValue( config.curvatureY );
		_sliderRatio->setValue( config.ratio );
		_sliderGraninessX->setValue( config.graninessX );
		_sliderGraninessY->setValue( config.graninessY );
		_sliderVignetteRoundness->setValue( config.vignetteRoundness );
		_sliderVignetteIntensity->setValue( config.vignetteIntensity );
		_sliderBrightness->setValue( config.brightness );
	}
} // namespace VTX::UI::QT::Widget::Library::GraphicsConfig
