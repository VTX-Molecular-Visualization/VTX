#include "ui/qt/widget/library/graphics_config/material_section.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include <QFileDialog>
#include <QHBoxLayout>
#include <QSignalBlocker>
#include <QVBoxLayout>
#include <app/generic/name.hpp>
#include <app/services.hpp>
#include <string_view>

namespace VTX::UI::QT::Widget::Library::GraphicsConfig
{
	MaterialSection::MaterialSection( QWidget * const p_parent ) : QWidget( p_parent )
	{
		using namespace Renderer;

		auto * const layout = new QVBoxLayout( this );
		layout->setContentsMargins( 0, 0, 0, 0 );

		layout->addWidget( new QLabel( "Material", this ) );
		_listMaterials = new QListWidget( this );
		_listMaterials->setSelectionMode( QAbstractItemView::SingleSelection );
		_listMaterials->setSortingEnabled( true );
		layout->addWidget( _listMaterials );

		auto materialPresets = App::REG().view<App::Generic::Name, Material>();
		for ( const Entity entity : materialPresets )
		{
			auto * const item = new QListWidgetItem(
				QString::fromStdString( materialPresets.get<App::Generic::Name>( entity ).name ), _listMaterials
			);
			item->setData( Qt::UserRole, QVariant::fromValue<Entity>( entity ) );
		}

		layout->addWidget( new QLabel( "Metallic", this ) );
		_sliderMetallic = new EditableSlider( Qt::Orientation::Horizontal, this );
		layout->addWidget( _sliderMetallic );
		_sliderMetallic->setMinimum( MATERIAL_METALLIC_MIN );
		_sliderMetallic->setMaximum( MATERIAL_METALLIC_MAX );

		layout->addWidget( new QLabel( "Roughness", this ) );
		_sliderRoughness = new EditableSlider( Qt::Orientation::Horizontal, this );
		layout->addWidget( _sliderRoughness );
		_sliderRoughness->setMinimum( MATERIAL_ROUGHNESS_MIN );
		_sliderRoughness->setMaximum( MATERIAL_ROUGHNESS_MAX );

		layout->addWidget( new QLabel( "Emissive intensity", this ) );
		_sliderEmissiveIntensity = new EditableSlider( Qt::Orientation::Horizontal, this );
		layout->addWidget( _sliderEmissiveIntensity );
		_sliderEmissiveIntensity->setMinimum( MATERIAL_EMISSIVE_INTENSITY_MIN );
		_sliderEmissiveIntensity->setMaximum( MATERIAL_EMISSIVE_INTENSITY_MAX );

		layout->addWidget( new QLabel( "Texture scale", this ) );
		_sliderTextureScale = new EditableSlider( Qt::Orientation::Horizontal, this );
		layout->addWidget( _sliderTextureScale );
		_sliderTextureScale->setMinimum( MATERIAL_TEXTURE_SCALE_MIN );
		_sliderTextureScale->setMaximum( MATERIAL_TEXTURE_SCALE_MAX );

		constexpr std::array<std::string_view, size_t( Material::E_TEXTURE::COUNT )> TEXTURE_LABELS {
			"Albedo map", "Normal map", "Metallic map", "Roughness map", "Ambient occlusion map", "Emissive map"
		};
		for ( size_t i = 0; i < _textureWidgets.size(); ++i )
		{
			const Material::E_TEXTURE textureType = Material::E_TEXTURE( i );
			auto &					  widgets	  = _textureWidgets[ i ];
			const QString			  label		  = QString::fromUtf8( TEXTURE_LABELS[ i ].data() );

			widgets.label = new QLabel( label, this );
			layout->addWidget( widgets.label );
			widgets.pathWidget		= new QWidget( this );
			auto * const pathLayout = new QHBoxLayout( widgets.pathWidget );
			pathLayout->setContentsMargins( 0, 0, 0, 0 );
			widgets.linePath = new QLineEdit( widgets.pathWidget );
			widgets.linePath->setReadOnly( true );
			auto * const buttonBrowse = new QPushButton( widgets.pathWidget );
			buttonBrowse->setIcon( STYLE().iconFromCodepoint( Style::Icons::OPEN ) );
			buttonBrowse->setToolTip( "Open " + label.toLower() );
			buttonBrowse->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
			buttonBrowse->setMinimumWidth( 0 );
			widgets.buttonClear = new QPushButton( widgets.pathWidget );
			widgets.buttonClear->setIcon( STYLE().iconFromCodepoint( Style::Icons::DELETE ) );
			widgets.buttonClear->setToolTip( "Remove " + label.toLower() );
			widgets.buttonClear->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
			widgets.buttonClear->setMinimumWidth( 0 );
			pathLayout->addWidget( widgets.linePath );
			pathLayout->addWidget( buttonBrowse );
			pathLayout->addWidget( widgets.buttonClear );
			layout->addWidget( widgets.pathWidget );

			connect(
				buttonBrowse,
				&QPushButton::clicked,
				[ this, textureType, label ]
				{
					const auto &  config  = App::REG().get<VTX::Renderer::GraphicsConfig>( _currentPreset );
					const auto &  texture = _texture( config.shading.material, textureType );
					const QString initialPath
						= texture ? QString::fromStdString( FilePath( texture->path ).parent_path().string() )
								  : QString {};
					const QString selectedPath = QFileDialog::getOpenFileName(
						this,
						"Select " + label.toLower(),
						initialPath,
						"Image maps (*.png *.PNG *.jpg *.JPG *.jpeg *.JPEG *.bmp *.BMP *.tga *.TGA)"
					);
					if ( not selectedPath.isEmpty() )
					{
						_changeTexture( textureType, FilePath( selectedPath.toStdString() ) );
					}
				}
			);
			connect(
				widgets.buttonClear,
				&QPushButton::clicked,
				[ this, textureType ] { _changeTexture( textureType, FilePath {} ); }
			);
		}

		connect(
			_listMaterials,
			&QListWidget::itemClicked,
			[ this ]( QListWidgetItem * const p_item )
			{
				const Entity materialPreset = p_item->data( Qt::UserRole ).value<Entity>();
				_changeValue<E_GRAPHICS_CONFIG_VALUES::MATERIAL, Material>(
					App::REG().get<Material>( materialPreset )
				);
			}
		);
		connect(
			_sliderMetallic,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::MATERIAL_METALLIC, float>( p_value ); }
		);
		connect(
			_sliderRoughness,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::MATERIAL_ROUGHNESS, float>( p_value ); }
		);
		connect(
			_sliderEmissiveIntensity,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::MATERIAL_EMISSIVE_INTENSITY, float>( p_value ); }
		);
		connect(
			_sliderTextureScale,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::MATERIAL_TEXTURE_SCALE, float>( p_value ); }
		);
	}

	const std::optional<VTX::Renderer::MaterialTexture> & MaterialSection::_texture(
		const VTX::Renderer::Material &			 p_material,
		const VTX::Renderer::Material::E_TEXTURE p_texture
	)
	{
		using TextureMember = std::optional<VTX::Renderer::MaterialTexture> VTX::Renderer::Material::*;
		static constexpr std::array<TextureMember, size_t( VTX::Renderer::Material::E_TEXTURE::COUNT )> MEMBERS {
			&VTX::Renderer::Material::albedoTexture,		   &VTX::Renderer::Material::normalTexture,
			&VTX::Renderer::Material::metallicTexture,		   &VTX::Renderer::Material::roughnessTexture,
			&VTX::Renderer::Material::ambientOcclusionTexture, &VTX::Renderer::Material::emissiveTexture
		};

		return p_material.*MEMBERS[ size_t( p_texture ) ];
	}

	void MaterialSection::_changeTexture( const VTX::Renderer::Material::E_TEXTURE p_texture, const FilePath & p_path )
	{
		using namespace VTX::Renderer;
		switch ( p_texture )
		{
		case Material::E_TEXTURE::ALBEDO:
			_changeValue<E_GRAPHICS_CONFIG_VALUES::MATERIAL_ALBEDO_TEXTURE, FilePath>( p_path );
			break;
		case Material::E_TEXTURE::NORMAL:
			_changeValue<E_GRAPHICS_CONFIG_VALUES::MATERIAL_NORMAL_TEXTURE, FilePath>( p_path );
			break;
		case Material::E_TEXTURE::METALLIC:
			_changeValue<E_GRAPHICS_CONFIG_VALUES::MATERIAL_METALLIC_TEXTURE, FilePath>( p_path );
			break;
		case Material::E_TEXTURE::ROUGHNESS:
			_changeValue<E_GRAPHICS_CONFIG_VALUES::MATERIAL_ROUGHNESS_TEXTURE, FilePath>( p_path );
			break;
		case Material::E_TEXTURE::AMBIENT_OCCLUSION:
			_changeValue<E_GRAPHICS_CONFIG_VALUES::MATERIAL_AMBIENT_OCCLUSION_TEXTURE, FilePath>( p_path );
			break;
		case Material::E_TEXTURE::EMISSIVE:
			_changeValue<E_GRAPHICS_CONFIG_VALUES::MATERIAL_EMISSIVE_TEXTURE, FilePath>( p_path );
			break;
		case Material::E_TEXTURE::COUNT: break;
		}
	}

	void MaterialSection::setConfig( const Entity p_preset, const VTX::Renderer::GraphicsConfig & p_config )
	{
		_currentPreset = p_preset;
		const QSignalBlocker blockerMaterial( _listMaterials );
		const QSignalBlocker blockerMetallic( _sliderMetallic );
		const QSignalBlocker blockerRoughness( _sliderRoughness );
		const QSignalBlocker blockerEmissiveIntensity( _sliderEmissiveIntensity );
		const QSignalBlocker blockerTextureScale( _sliderTextureScale );

		_listMaterials->setCurrentRow( -1 );
		for ( int i = 0; i < _listMaterials->count(); ++i )
		{
			const Entity materialPreset = _listMaterials->item( i )->data( Qt::UserRole ).value<Entity>();
			if ( App::REG().get<VTX::Renderer::Material>( materialPreset ).name == p_config.shading.material.name )
			{
				_listMaterials->setCurrentRow( i );
				break;
			}
		}
		_sliderMetallic->setValue( p_config.shading.material.metallic );
		_sliderRoughness->setValue( p_config.shading.material.roughness );
		_sliderEmissiveIntensity->setValue( p_config.shading.material.emissiveIntensity );
		_sliderTextureScale->setValue( p_config.shading.material.textureScale );
		for ( size_t i = 0; i < _textureWidgets.size(); ++i )
		{
			const auto &  texture = _texture( p_config.shading.material, VTX::Renderer::Material::E_TEXTURE( i ) );
			const QString path	  = texture ? QString::fromStdString( texture->path ) : QString {};
			auto &		  widgets = _textureWidgets[ i ];
			widgets.linePath->setText( path );
			widgets.linePath->setToolTip( path );
			widgets.buttonClear->setEnabled( texture.has_value() );
		}
		setVisible( p_config.shading.mode == VTX::Renderer::E_SHADING::PBR );
	}
} // namespace VTX::UI::QT::Widget::Library::GraphicsConfig
