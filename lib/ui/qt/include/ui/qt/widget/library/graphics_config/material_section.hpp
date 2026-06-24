#ifndef __VTX_UI_QT_WIDGET_LIBRARY_GRAPHICS_CONFIG_MATERIAL_SECTION__
#define __VTX_UI_QT_WIDGET_LIBRARY_GRAPHICS_CONFIG_MATERIAL_SECTION__

#include "ui/qt/services.hpp"
#include "ui/qt/widget/editable_slider.hpp"
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <app/action/action_manager.hpp>
#include <app/action/graphics_config.hpp>
#include <app/services.hpp>
#include <array>
#include <optional>

namespace VTX::UI::QT::Widget::Library::GraphicsConfig
{
	class MaterialSection final : public QWidget
	{
	  public:
		explicit MaterialSection( QWidget * );

		void setConfig( Entity, const VTX::Renderer::GraphicsConfig & );

	  private:
		struct TextureWidgets
		{
			QPointer<QLabel>	  label;
			QPointer<QWidget>	  pathWidget;
			QPointer<QLineEdit>	  linePath;
			QPointer<QPushButton> buttonClear;
		};

		template<VTX::Renderer::E_GRAPHICS_CONFIG_VALUES S, typename T>
		void _changeValue( const T & p_value )
		{
			using ChangeAction = ::VTX::App::Action::GraphicsConfig::Change<S, T>;
			ChangeAction action;
			::VTX::App::ACTION().execute( action, _currentPreset, p_value );
		}

		static const std::optional<VTX::Renderer::MaterialTexture> & _texture(
			const VTX::Renderer::Material &,
			VTX::Renderer::Material::E_TEXTURE
		);
		void _changeTexture( VTX::Renderer::Material::E_TEXTURE, const FilePath & );

		Entity _currentPreset;

		QPointer<QListWidget>															_listMaterials;
		QPointer<EditableSlider>														_sliderMetallic;
		QPointer<EditableSlider>														_sliderRoughness;
		QPointer<EditableSlider>														_sliderEmissiveIntensity;
		QPointer<EditableSlider>														_sliderTextureScale;
		std::array<TextureWidgets, size_t( VTX::Renderer::Material::E_TEXTURE::COUNT )> _textureWidgets;
	};
} // namespace VTX::UI::QT::Widget::Library::GraphicsConfig

#endif
