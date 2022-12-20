#ifndef __VTX_UI_WIDGET_RENDER_EFFECT_PRESET_EDITOR__
#define __VTX_UI_WIDGET_RENDER_EFFECT_PRESET_EDITOR__

#include "color/rgba.hpp"
#include "model/renderer/render_effect_preset.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/custom_widget/color_field_button.hpp"
#include "ui/widget/custom_widget/filename_field_widget.hpp"
#include "ui/widget/custom_widget/float_field_draggable_widget.hpp"
#include "ui/widget/custom_widget/float_field_slider_widget.hpp"
#include "ui/widget/custom_widget/integer_field_draggable_widget.hpp"
#include "ui/widget/custom_widget/integer_field_slider_widget.hpp"
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
		using IntegerFieldSliderWidget	  = VTX::UI::Widget::CustomWidget::IntegerFieldSliderWidget;
		using IntegerFieldDraggableWidget = VTX::UI::Widget::CustomWidget::IntegerFieldDraggableWidget;
		using FloatFieldSliderWidget	  = VTX::UI::Widget::CustomWidget::FloatFieldSliderWidget;
		using FloatFieldDraggableWidget	  = VTX::UI::Widget::CustomWidget::FloatFieldDraggableWidget;
		using ColorFieldButton			  = VTX::UI::Widget::CustomWidget::ColorFieldButton;

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

		QWidget *	  _viewport = nullptr;
		QGridLayout * _layout	= nullptr;

		CustomWidget::FilenameFieldWidget * _name		 = nullptr;
		QCheckBox *							_quickAccess = nullptr;

		QComboBox * _shading = nullptr;

		QCheckBox *				   _enableSSAO	  = nullptr;
		IntegerFieldSliderWidget * _ssaoIntensity = nullptr;
		IntegerFieldSliderWidget * _ssaoBlurSize  = nullptr;

		QCheckBox *				   _enableOutline	 = nullptr;
		IntegerFieldSliderWidget * _outlineThickness = nullptr;
		FloatFieldSliderWidget *   _outlineSensivity = nullptr;
		ColorFieldButton *		   _outlineColor	 = nullptr;

		QCheckBox * _enableFog = nullptr;
		// TODO RangeScrollBar
		// CustomWidget::QRangeScrollBar * const _nearFarRangeFog = nullptr;
		IntegerFieldDraggableWidget * _nearFog	  = nullptr;
		IntegerFieldDraggableWidget * _farFog	  = nullptr;
		FloatFieldSliderWidget *	  _fogDensity = nullptr;
		ColorFieldButton *			  _fogColor	  = nullptr;

		ColorFieldButton *			_backgroundColor   = nullptr;
		FloatFieldSliderWidget *	_backgroundOpacity = nullptr;
		ColorFieldButton *			_cameraLightColor  = nullptr;
		FloatFieldSliderWidget *	_cameraFOV		   = nullptr;
		FloatFieldDraggableWidget * _cameraNear		   = nullptr;
		FloatFieldDraggableWidget * _cameraFar		   = nullptr;
		QCheckBox *					_antialiasing	   = nullptr;

		// !V0.1
		// QCheckBox *				 _perspective		= nullptr;

		void _addItem( QWidget * const p_widget );
		void _addItem( QWidget * const p_widget, const QString & p_label );
		void _addItem( QWidget * const p_widget, QLabel * const p_labelWidget );
		void _addSpace( const int p_space = 10 );

		void _fillShaderComboBox();

		void _onNameChanged() const;
		void _onQuickAccessChanged( const bool p_quickAccess ) const;

		void _onShadingChange( const int p_newIndex ) const;

		void _onSSAOStateChanged( const int p_state ) const;
		void _onSSAOIntensityChanged( const int p_value ) const;
		void _onSSAOBlurSizeChanged( const int p_value ) const;

		void _onOutlineStateChanged( const int p_state ) const;
		void _onOutlineThicknessChanged( const uint p_value ) const;
		void _onOutlineSensivityChanged( const float p_value ) const;
		void _onOutlineColorChanged( const Color::Rgba & p_color ) const;

		void _onFogStateChanged( const int p_state ) const;
		void _onFogNearChanged( const int p_value ) const;
		void _onFogFarChanged( const int p_value ) const;
		void _onFogDensityChanged( const float p_value ) const;
		void _onFogColorChanged( const Color::Rgba & p_color ) const;

		void _onBackgroundColorChanged( const Color::Rgba & p_color ) const;
		void _onCameraLightColorChanged( const Color::Rgba & p_color ) const;
		void _onCameraFOVChanged( const float p_value ) const;
		void _onCameraNearChanged( const float p_value ) const;
		void _onCameraFarChanged( const float p_value ) const;
		void _onAntialiasingChanged( const int p_state ) const;
		void _onPerspectiveChanged( const int p_state ) const;
	};

} // namespace VTX::UI::Widget::Settings

#endif
