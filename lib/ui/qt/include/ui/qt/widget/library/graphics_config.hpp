#ifndef __VTX_UI_QT_WIDGET_LIBRARY_RENDER_SETTINGS__
#define __VTX_UI_QT_WIDGET_LIBRARY_RENDER_SETTINGS__

#include "base_preset_widget.hpp"
#include "ui/qt/widget/color_picker.hpp"
#include "ui/qt/widget/editable_slider.hpp"
#include "ui/qt/widget/hideable_goupe_box.hpp"
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <app/action/graphics_config.hpp>
#include <array>

namespace VTX::UI::QT::Widget::Library
{
	class GraphicsConfig final : public BasePresetWidget<VTX::Renderer::GraphicsConfig>
	{
	  public:
		GraphicsConfig( QWidget * p_parent );

	  protected:
		void _update( Entity ) override;

	  private:
		struct MaterialTextureWidgets
		{
			QPointer<QLabel>	  label;
			QPointer<QWidget>	  pathWidget;
			QPointer<QLineEdit>	  linePath;
			QPointer<QPushButton> buttonClear;
		};

		// Background.
		QPointer<HideableGroupBox> _groupboxBackground;

		// Shading.
		QPointer<HideableGroupBox> _groupboxShading;
		QPointer<QComboBox>		   _comboBoxShadingMode;
		QPointer<ColorPicker>	   _colorPickerBackground;
		QPointer<ColorPicker>	   _colorPickerLight;
		QPointer<QLabel>		   _labelLightIntensity;
		QPointer<EditableSlider>   _sliderLightIntensity;
		QPointer<QLabel>		   _labelAmbientIntensity;
		QPointer<EditableSlider>   _sliderAmbientIntensity;
		QPointer<QLabel>		   _labelToneMappingMode;
		QPointer<QComboBox>		   _comboBoxToneMappingMode;
		QPointer<QLabel>		   _labelToneMappingExposure;
		QPointer<EditableSlider>   _sliderToneMappingExposure;
		QPointer<QLabel>		   _labelSpecularFactor;
		QPointer<EditableSlider>   _sliderSpecularFactor;
		QPointer<QLabel>		   _labelShininess;
		QPointer<EditableSlider>   _sliderShininess;
		QPointer<QLabel>		   _labelToonSteps;
		QPointer<EditableSlider>   _sliderToonSteps;
		QPointer<QLabel>		   _labelMaterial;
		QPointer<QListWidget>	   _listMaterials;
		QPointer<QLabel>		   _labelMaterialMetallic;
		QPointer<EditableSlider>   _sliderMaterialMetallic;
		QPointer<QLabel>		   _labelMaterialRoughness;
		QPointer<EditableSlider>   _sliderMaterialRoughness;
		QPointer<QLabel>		   _labelMaterialEmissiveIntensity;
		QPointer<EditableSlider>   _sliderMaterialEmissiveIntensity;
		QPointer<QLabel>		   _labelMaterialTextureScale;
		QPointer<EditableSlider>   _sliderMaterialTextureScale;

		std::array<MaterialTextureWidgets, size_t( VTX::Renderer::Material::E_TEXTURE::COUNT )> _materialTextureWidgets;

		QPointer<QLineEdit>		 _lineEnvironmentPath;
		QPointer<QPushButton>	 _buttonEnvironmentBrowse;
		QPointer<QPushButton>	 _buttonEnvironmentClear;
		QPointer<QLabel>		 _labelSkyboxIntensity;
		QPointer<EditableSlider> _sliderSkyboxIntensity;
		QPointer<QLabel>		 _labelIblIntensity;
		QPointer<EditableSlider> _sliderIblIntensity;
		QPointer<QLabel>		 _labelEnvironmentRotation;
		QPointer<EditableSlider> _sliderEnvironmentRotation;

		// SSAO.
		QPointer<HideableGroupBox> _groupboxSSAO;
		QPointer<QComboBox>		   _comboBoxSSAOMethod;
		QPointer<QComboBox>		   _comboBoxSSAOScale;
		QPointer<EditableSlider>   _sliderSSAOIntensity;
		QPointer<EditableSlider>   _sliderBlurSize;

		// Outline.
		QPointer<HideableGroupBox> _groupboxOutline;
		QPointer<ColorPicker>	   _colorPickerOutline;
		QPointer<EditableSlider>   _sliderOutlineSensitivity;
		QPointer<EditableSlider>   _sliderOutlineThickness;

		// Fog.
		QPointer<HideableGroupBox> _groupboxFog;
		QPointer<ColorPicker>	   _colorPickerFog;
		QPointer<EditableSlider>   _sliderFogNear;
		QPointer<EditableSlider>   _sliderFogFar;
		QPointer<EditableSlider>   _sliderFogDensity;

		// Chromatic aberration.
		QPointer<HideableGroupBox> _groupboxChromaticAberration;
		QPointer<EditableSlider>   _sliderChromaticAberrationRed;
		QPointer<EditableSlider>   _sliderChromaticAberrationGreen;
		QPointer<EditableSlider>   _sliderChromaticAberrationBlue;

		// Pixelize.
		QPointer<HideableGroupBox> _groupboxPixelize;
		QPointer<EditableSlider>   _sliderPixelizeSize;
		QPointer<QCheckBox>		   _checkBoxPixelizeBackground;

		// CRT.
		QPointer<HideableGroupBox> _groupboxCRT;
		QPointer<EditableSlider>   _sliderCRTCurvatureX;
		QPointer<EditableSlider>   _sliderCRTCurvatureY;
		QPointer<EditableSlider>   _sliderCRTRatio;
		QPointer<EditableSlider>   _sliderCRTGraninessX;
		QPointer<EditableSlider>   _sliderCRTGraninessY;
		QPointer<EditableSlider>   _sliderCRTVignetteRoundness;
		QPointer<EditableSlider>   _sliderCRTVignetteIntensity;
		QPointer<EditableSlider>   _sliderCRTBrightness;

		// Selection.
		QPointer<HideableGroupBox> _groupboxSelection;
		QPointer<ColorPicker>	   _colorPickerSelection;

		template<VTX::Renderer::E_GRAPHICS_CONFIG_VALUES S, typename T>
		void _changeValue( const T p_value )
		{ App::ACTION().execute<App::Action::GraphicsConfig::Change<S, T>>( currentPreset(), p_value ); }

		static const std::optional<VTX::Renderer::MaterialTexture> & _materialTexture(
			const VTX::Renderer::Material &,
			const VTX::Renderer::Material::E_TEXTURE
		);
		void _changeMaterialTexture( VTX::Renderer::Material::E_TEXTURE, const FilePath & );

		void _applyLogic( const VTX::Renderer::GraphicsConfig & );
	};
} // namespace VTX::UI::QT::Widget::Library

#endif
