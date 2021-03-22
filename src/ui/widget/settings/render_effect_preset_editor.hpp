#ifndef __VTX_UI_WIDGET_RENDER_EFFECT_PRESET_EDITOR__
#define __VTX_UI_WIDGET_RENDER_EFFECT_PRESET_EDITOR__

#ifdef _MSC_VER
#pragma once
#endif

#include "color/rgb.hpp"
#include "model/renderer/render_effect_preset.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/custom_widget/color_field_button.hpp"
#include "ui/widget/custom_widget/float_field_widget.hpp"
#include "ui/widget/custom_widget/integer_field_widget.hpp"
#include "view/base_view.hpp"
#include "view/ui/widget/renderer/render_effect_preset_view.hpp"
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>

namespace VTX::UI::Widget::Settings
{
	class RenderEffectPresetEditor : public VTX::UI::Widget::BaseManualWidget<QScrollArea>
	{
		VTX_WIDGET

	  private:
		using IntegerFieldWidget = VTX::UI::Widget::CustomWidget::IntegerFieldWidget;
		using FloatFieldWidget	 = VTX::UI::Widget::CustomWidget::FloatFieldWidget;
		using ColorFieldButton	 = VTX::UI::Widget::CustomWidget::ColorFieldButton;

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
		void _applyPreset() const;

	  private:
		int														  _itemCount		 = 0;
		Model::Renderer::RenderEffectPreset *					  _preset			 = nullptr;
		VTX::View::UI::Widget::Renderer::RenderEffectPresetView * _currentPresetView = nullptr;

		QGridLayout * _layout = nullptr;

		QComboBox * _shading = nullptr;

		QCheckBox *			 _enableSSAO	= nullptr;
		IntegerFieldWidget * _ssaoIntensity = nullptr;
		IntegerFieldWidget * _ssaoBlurSize	= nullptr;

		QCheckBox *		   _enableOutline	 = nullptr;
		FloatFieldWidget * _outlineThickness = nullptr;
		ColorFieldButton * _outlineColor	 = nullptr;

		QCheckBox * _enableFog = nullptr;
		// TODO RangeScrollBar
		// CustomWidget::QRangeScrollBar * const _nearFarRangeFog = nullptr;
		QSpinBox *		   _nearFog	   = nullptr;
		QSpinBox *		   _farFog	   = nullptr;
		FloatFieldWidget * _fogDensity = nullptr;
		ColorFieldButton * _fogColor   = nullptr;

		ColorFieldButton * _backgroundColor	 = nullptr;
		ColorFieldButton * _cameraLightColor = nullptr;
		FloatFieldWidget * _cameraFOV		 = nullptr;
		QSpinBox *		   _cameraNear		 = nullptr;
		QSpinBox *		   _cameraFar		 = nullptr;
		QCheckBox *		   _antialiasing	 = nullptr;
		QCheckBox *		   _perspective		 = nullptr;

		void _addItem( QWidget * const p_widget, const QString & p_label );
		void _addSpace();

		void _fillShaderComboBox();

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
		void _onCameraLightColorChanged( const Color::Rgb & p_color );
		void _onCameraFOVChanged( const float p_value );
		void _onCameraNearChanged( const float p_value );
		void _onCameraFarChanged( const float p_value );
		void _onAntialiasingChanged( const int p_state );
		void _onPerspectiveChanged( const int p_state );
	};

} // namespace VTX::UI::Widget::Settings

#endif
