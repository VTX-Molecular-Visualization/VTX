#ifndef VTX_UI_QT_WIDGET_LIBRARY_GRAPHICS_CONFIG_MATERIAL_SECTION_HPP
#define VTX_UI_QT_WIDGET_LIBRARY_GRAPHICS_CONFIG_MATERIAL_SECTION_HPP

#include "ui/qt/services.hpp"
#include "ui/qt/widget/editable_slider.hpp"
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <app/action/material.hpp>
#include <app/ecs.hpp>
#include <app/services.hpp>
#include <array>
#include <optional>
#include <renderer/graphics_config.hpp>
#include <renderer/material.hpp>
#include <util/action_manager.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::Widget::Library::GraphicsConfig
{
	class MaterialSection final : public QWidget
	{
	  public:
		explicit MaterialSection( QWidget * );

		void setConfig( const VTX::Renderer::GraphicsConfig & );

	  private:
		struct TextureWidgets
		{
			QPointer<QLabel>	  label;
			QPointer<QWidget>	  pathWidget;
			QPointer<QLineEdit>	  linePath;
			QPointer<QPushButton> buttonClear;
		};

		template<VTX::Renderer::E_MATERIAL_VALUES S, typename T>
		void _changeValue( const T & p_value )
		{
			if ( not ::VTX::App::REG().valid( _currentMaterial ) )
			{
				return;
			}

			using ChangeAction = ::VTX::App::Action::Material::Change<S, T>;
			ChangeAction action;
			::VTX::App::ACTION().execute( action, _currentMaterial, p_value );
		}

		static const std::optional<VTX::Renderer::MaterialTexture> & _texture( const VTX::Renderer::Material &,
																			   VTX::Renderer::Material::E_TEXTURE );
		void _changeTexture( VTX::Renderer::Material::E_TEXTURE, const FilePath & );

		void _refresh();
		void _onSceneMaterial( Registry &, Entity );
		void _onMaterialUpdated( Registry &, Entity );

		Entity _currentMaterial = InvalidEntity;

		VTX::Util::EventHub::ScopedConnection _sceneMaterialConstructedConnection;
		VTX::Util::EventHub::ScopedConnection _sceneMaterialUpdatedConnection;
		VTX::Util::EventHub::ScopedConnection _materialUpdatedConnection;

		QPointer<QListWidget>	 _listMaterials;
		QPointer<EditableSlider> _sliderMetallic;
		QPointer<EditableSlider> _sliderRoughness;
		QPointer<EditableSlider> _sliderEmissiveIntensity;
		QPointer<EditableSlider> _sliderTextureScale;
		std::array<TextureWidgets, static_cast<size_t>( VTX::Renderer::Material::E_TEXTURE::COUNT )> _textureWidgets;
	};
} // namespace VTX::UI::QT::Widget::Library::GraphicsConfig

#endif
