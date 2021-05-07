#ifndef __VTX_UI_WIDGET_RENDER_EFFECT_PRESET_EDITOR__
#define __VTX_UI_WIDGET_RENDER_EFFECT_PRESET_EDITOR__

#ifdef _MSC_VER
#pragma once
#endif

#include "color/rgb.hpp"
#include "model/renderer/render_effect_preset.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/custom_widget/color_field_button.hpp"
#include "ui/widget/custom_widget/float_field_slider_widget.hpp"
#include "ui/widget/custom_widget/integer_field_widget.hpp"
#include "view/base_view.hpp"
#include "view/ui/editor_view.hpp"
#include "view/ui/widget/renderer/render_effect_preset_view.hpp"
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QScrollArea>
#include <QSpinBox>

namespace VTX::UI::Widget::Settings
{
	class RenderEffectPresetEditor :
		public VTX::UI::Widget::BaseManualWidget<QScrollArea>,
		View::UI::EditorView<Model::Renderer::RenderEffectPreset>
	{
		VTX_WIDGET

	  private:
		using IntegerFieldWidget	 = VTX::UI::Widget::CustomWidget::IntegerFieldWidget;
		using FloatFieldSliderWidget = VTX::UI::Widget::CustomWidget::FloatFieldSliderWidget;
		using ColorFieldButton		 = VTX::UI::Widget::CustomWidget::ColorFieldButton;

		inline static const QStringList SHADING = { "Diffuse", "Glossy", "Toon", "Flat Color" };

	  public:
		void localize() override;
		void refresh();

		const Model::Renderer::RenderEffectPreset * const getPreset() const { return _preset; };
		void setPreset( Model::Renderer::RenderEffectPreset * const p_model, const bool p_updateRender = true );

	  protected:
		RenderEffectPresetEditor( QWidget * const p_parent );

		void _setupUi( const QString & ) override;
		void _setupSlots() override;

		void _catchModelEvent( const Event::VTXEvent * const p_event ) override;

	  private:
		int														  _itemCount		 = 0;
		Model::Renderer::RenderEffectPreset *					  _preset			 = nullptr;
		VTX::View::UI::Widget::Renderer::RenderEffectPresetView * _currentPresetView = nullptr;

		QGridLayout * _layout = nullptr;

		QLineEdit * _name		 = nullptr;
		QCheckBox * _quickAccess = nullptr;

		QComboBox * _shading = nullptr;

		QCheckBox *			 _enableSSAO	= nullptr;
		IntegerFieldWidget * _ssaoIntensity = nullptr;
		IntegerFieldWidget * _ssaoBlurSize	= nullptr;

		QCheckBox *				 _enableOutline	   = nullptr;
		FloatFieldSliderWidget * _outlineThickness = nullptr;
		ColorFieldButton *		 _outlineColor	   = nullptr;

		QCheckBox * _enableFog = nullptr;
		// TODO RangeScrollBar
		// CustomWidget::QRangeScrollBar * const _nearFarRangeFog = nullptr;
		QSpinBox *				 _nearFog	 = nullptr;
		QSpinBox *				 _farFog	 = nullptr;
		FloatFieldSliderWidget * _fogDensity = nullptr;
		ColorFieldButton *		 _fogColor	 = nullptr;

		ColorFieldButton *		 _backgroundColor	= nullptr;
		FloatFieldSliderWidget * _backgroundOpacity = nullptr;
		ColorFieldButton *		 _cameraLightColor	= nullptr;
		FloatFieldSliderWidget * _cameraFOV			= nullptr;
		QSpinBox *				 _cameraNear		= nullptr;
		QSpinBox *				 _cameraFar			= nullptr;
		QCheckBox *				 _antialiasing		= nullptr;
		QCheckBox *				 _perspective		= nullptr;

		void _addItem( QWidget * const p_widget );
		void _addItem( QWidget * const p_widget, const QString & p_label );
		void _addSpace( const int p_space = 10 );

		void _fillShaderComboBox();

		void _onNameChanged();
		void _onQuickAccessChanged( const bool p_quickAccess );

		void _onShadingChange( const int p_newIndex );

		void _onSSAOStateChanged( const int p_state );
		void _onSSAOIntensityChanged( const int p_value );
		void _onSSAOBlurSizeChanged( const int p_value );

		void _onOutlineStateChanged( const int p_state );
		void _onOutlineThicknessChanged( const float p_value );
		void _onOutlineColorChanged( const Color::Rgb & p_color );

		void _onFogStateChanged( const int p_state );
		void _onFogNearChanged();
		void _onFogFarChanged();
		void _onFogDensityChanged( const float p_value );
		void _onFogColorChanged( const Color::Rgb & p_color );

		void _onBackgroundColorChanged( const Color::Rgb & p_color );
		void _onBackgroundOpacityChanged( const float p_color );
		void _onCameraLightColorChanged( const Color::Rgb & p_color );
		void _onCameraFOVChanged( const float p_value );
		void _onCameraNearChanged( const float p_value );
		void _onCameraFarChanged( const float p_value );
		void _onAntialiasingChanged( const int p_state );
		void _onPerspectiveChanged( const int p_state );
	};

} // namespace VTX::UI::Widget::Settings

#endif
