#include "ui/qt/widget/library/graphics_config/material_section.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include <QFileDialog>
#include <QHBoxLayout>
#include <QSignalBlocker>
#include <QVBoxLayout>
#include <app/action/scene.hpp>
#include <app/generic/name.hpp>
#include <app/scene/material.hpp>
#include <string_view>

namespace VTX::UI::QT::Widget::Library::GraphicsConfig
{
	MaterialSection::MaterialSection( QWidget * const p_parent ) : QWidget( p_parent )
	{
		using namespace Renderer;

		auto * const layout = Helper::makeQObject<QVBoxLayout>( this );
		layout->setContentsMargins( 0, 0, 0, 0 );

		layout->addWidget( Helper::makeQObject<QLabel>( "Material", this ) );
		_listMaterials = Helper::makeQObject<QListWidget>( this );
		_listMaterials->setSelectionMode( QAbstractItemView::SingleSelection );
		_listMaterials->setSortingEnabled( true );
		layout->addWidget( _listMaterials );

		auto materialPresets = App::REG().view<App::Generic::Name, Material>();
		for ( const Entity entity : materialPresets )
		{
			auto * const item = new QListWidgetItem(
				QString::fromStdString( materialPresets.get<App::Generic::Name>( entity ).name ), _listMaterials );
			item->setData( Qt::UserRole, QVariant::fromValue<Entity>( entity ) );
		}

		layout->addWidget( Helper::makeQObject<QLabel>( "Metallic", this ) );
		_sliderMetallic = Helper::makeQObject<EditableSlider>( Qt::Orientation::Horizontal, this );
		layout->addWidget( _sliderMetallic );
		_sliderMetallic->setMinimum( MATERIAL_METALLIC_MIN );
		_sliderMetallic->setMaximum( MATERIAL_METALLIC_MAX );

		layout->addWidget( Helper::makeQObject<QLabel>( "Roughness", this ) );
		_sliderRoughness = Helper::makeQObject<EditableSlider>( Qt::Orientation::Horizontal, this );
		layout->addWidget( _sliderRoughness );
		_sliderRoughness->setMinimum( MATERIAL_ROUGHNESS_MIN );
		_sliderRoughness->setMaximum( MATERIAL_ROUGHNESS_MAX );

		layout->addWidget( Helper::makeQObject<QLabel>( "Emissive intensity", this ) );
		_sliderEmissiveIntensity = Helper::makeQObject<EditableSlider>( Qt::Orientation::Horizontal, this );
		layout->addWidget( _sliderEmissiveIntensity );
		_sliderEmissiveIntensity->setMinimum( MATERIAL_EMISSIVE_INTENSITY_MIN );
		_sliderEmissiveIntensity->setMaximum( MATERIAL_EMISSIVE_INTENSITY_MAX );

		layout->addWidget( Helper::makeQObject<QLabel>( "Texture scale", this ) );
		_sliderTextureScale = Helper::makeQObject<EditableSlider>( Qt::Orientation::Horizontal, this );
		layout->addWidget( _sliderTextureScale );
		_sliderTextureScale->setMinimum( MATERIAL_TEXTURE_SCALE_MIN );
		_sliderTextureScale->setMaximum( MATERIAL_TEXTURE_SCALE_MAX );

		constexpr auto TEXTURE_LABELS = std::to_array<std::string_view>(
			{ "Albedo map", "Normal map", "Metallic map", "Roughness map", "Ambient occlusion map", "Emissive map" } );
		static_assert( TEXTURE_LABELS.size() == static_cast<size_t>( Material::E_TEXTURE::COUNT ) );
		for ( size_t i = 0; i < _textureWidgets.size(); ++i )
		{
			const Material::E_TEXTURE textureType = Material::E_TEXTURE( i );
			auto &					  widgets	  = _textureWidgets[ i ];
			const QString			  label		  = QString::fromUtf8( TEXTURE_LABELS[ i ].data() );

			widgets.label = Helper::makeQObject<QLabel>( label, this );
			layout->addWidget( widgets.label );
			widgets.pathWidget		= Helper::makeQObject<QWidget>( this );
			auto * const pathLayout = Helper::makeQObject<QHBoxLayout>( widgets.pathWidget );
			pathLayout->setContentsMargins( 0, 0, 0, 0 );
			widgets.linePath = Helper::makeQObject<QLineEdit>( widgets.pathWidget );
			widgets.linePath->setReadOnly( true );
			auto * const buttonBrowse = Helper::makeQObject<QPushButton>( widgets.pathWidget );
			buttonBrowse->setIcon( STYLE().iconFromCodepoint( Style::Icons::OPEN ) );
			buttonBrowse->setToolTip( "Open " + label.toLower() );
			buttonBrowse->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
			buttonBrowse->setMinimumWidth( 0 );
			widgets.buttonClear = Helper::makeQObject<QPushButton>( widgets.pathWidget );
			widgets.buttonClear->setIcon( STYLE().iconFromCodepoint( Style::Icons::DELETE ) );
			widgets.buttonClear->setToolTip( "Remove " + label.toLower() );
			widgets.buttonClear->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
			widgets.buttonClear->setMinimumWidth( 0 );
			pathLayout->addWidget( widgets.linePath );
			pathLayout->addWidget( buttonBrowse );
			pathLayout->addWidget( widgets.buttonClear );
			layout->addWidget( widgets.pathWidget );

			connect( buttonBrowse,
					 &QPushButton::clicked,
					 [ this, textureType, label ]
					 {
						 if ( not App::REG().valid( _currentMaterial ) )
						 {
							 return;
						 }

						 const VTX::Renderer::Material & material
							 = App::REG().get<VTX::Renderer::Material>( _currentMaterial );
						 const std::optional<VTX::Renderer::MaterialTexture> & texture = _texture( material, textureType );
						 const QString initialPath
							 = texture ? QString::fromStdString( FilePath( texture->path ).parent_path().string() )
									   : QString {};
						 const QString selectedPath = QFileDialog::getOpenFileName(
							 this,
							 "Select " + label.toLower(),
							 initialPath,
							 "Image maps (*.png *.PNG *.jpg *.JPG *.jpeg *.JPEG *.bmp *.BMP *.tga *.TGA)" );
						 if ( not selectedPath.isEmpty() )
						 {
							 _changeTexture( textureType, FilePath( selectedPath.toStdString() ) );
						 }
					 } );
			connect( widgets.buttonClear,
					 &QPushButton::clicked,
					 [ this, textureType ] { _changeTexture( textureType, FilePath {} ); } );
		}

		connect( _listMaterials,
				 &QListWidget::itemClicked,
				 [ this ]( QListWidgetItem * const p_item )
				 {
					 const Entity materialPreset = p_item->data( Qt::UserRole ).value<Entity>();
					 App::ACTION().execute<App::Action::Scene::SetMaterial>( materialPreset );
				 } );
		connect( _sliderMetallic,
				 &EditableSlider::valueChanged,
				 [ this ]( const float p_value )
				 { _changeValue<E_MATERIAL_VALUES::METALLIC, float>( p_value ); } );
		connect( _sliderRoughness,
				 &EditableSlider::valueChanged,
				 [ this ]( const float p_value )
				 { _changeValue<E_MATERIAL_VALUES::ROUGHNESS, float>( p_value ); } );
		connect( _sliderEmissiveIntensity,
				 &EditableSlider::valueChanged,
				 [ this ]( const float p_value )
				 { _changeValue<E_MATERIAL_VALUES::EMISSIVE_INTENSITY, float>( p_value ); } );
		connect( _sliderTextureScale,
				 &EditableSlider::valueChanged,
				 [ this ]( const float p_value )
				 { _changeValue<E_MATERIAL_VALUES::TEXTURE_SCALE, float>( p_value ); } );

		Registry & reg = App::REG();
		_sceneMaterialConstructedConnection
			= reg.on_construct<App::Scene::Material>().connect<&MaterialSection::_onSceneMaterial>( this );
		_sceneMaterialUpdatedConnection
			= reg.on_update<App::Scene::Material>().connect<&MaterialSection::_onSceneMaterial>( this );
		_materialUpdatedConnection = reg.on_update<Material>().connect<&MaterialSection::_onMaterialUpdated>( this );

		_refresh();
	}

	const std::optional<VTX::Renderer::MaterialTexture> & MaterialSection::_texture(
		const VTX::Renderer::Material &			 p_material,
		const VTX::Renderer::Material::E_TEXTURE p_texture )
	{
		using TextureMember = std::optional<VTX::Renderer::MaterialTexture> VTX::Renderer::Material::*;
		static constexpr auto												MEMBERS
			= std::to_array<TextureMember>( { &VTX::Renderer::Material::albedoTexture,
											  &VTX::Renderer::Material::normalTexture,
											  &VTX::Renderer::Material::metallicTexture,
											  &VTX::Renderer::Material::roughnessTexture,
											  &VTX::Renderer::Material::ambientOcclusionTexture,
											  &VTX::Renderer::Material::emissiveTexture } );
		static_assert( MEMBERS.size() == static_cast<size_t>( VTX::Renderer::Material::E_TEXTURE::COUNT ) );

		return p_material.*MEMBERS[ static_cast<size_t>( p_texture ) ];
	}

	void MaterialSection::_changeTexture( const VTX::Renderer::Material::E_TEXTURE p_texture, const FilePath & p_path )
	{
		using namespace VTX::Renderer;
		switch ( p_texture )
		{
		case Material::E_TEXTURE::ALBEDO:
			_changeValue<E_MATERIAL_VALUES::ALBEDO_TEXTURE, FilePath>( p_path );
			break;
		case Material::E_TEXTURE::NORMAL:
			_changeValue<E_MATERIAL_VALUES::NORMAL_TEXTURE, FilePath>( p_path );
			break;
		case Material::E_TEXTURE::METALLIC:
			_changeValue<E_MATERIAL_VALUES::METALLIC_TEXTURE, FilePath>( p_path );
			break;
		case Material::E_TEXTURE::ROUGHNESS:
			_changeValue<E_MATERIAL_VALUES::ROUGHNESS_TEXTURE, FilePath>( p_path );
			break;
		case Material::E_TEXTURE::AMBIENT_OCCLUSION:
			_changeValue<E_MATERIAL_VALUES::AMBIENT_OCCLUSION_TEXTURE, FilePath>( p_path );
			break;
		case Material::E_TEXTURE::EMISSIVE:
			_changeValue<E_MATERIAL_VALUES::EMISSIVE_TEXTURE, FilePath>( p_path );
			break;
		case Material::E_TEXTURE::COUNT: break;
		}
	}

	void MaterialSection::setConfig( const VTX::Renderer::GraphicsConfig & p_config )
	{ setVisible( p_config.shading.mode == VTX::Renderer::E_SHADING::PBR ); }

	void MaterialSection::_onSceneMaterial( Registry &, Entity ) { _refresh(); }

	void MaterialSection::_onMaterialUpdated( Registry &, const Entity p_e )
	{
		if ( p_e == _currentMaterial )
		{
			_refresh();
		}
	}

	void MaterialSection::_refresh()
	{
		const QSignalBlocker blockerMaterial( _listMaterials );
		const QSignalBlocker blockerMetallic( _sliderMetallic );
		const QSignalBlocker blockerRoughness( _sliderRoughness );
		const QSignalBlocker blockerEmissiveIntensity( _sliderEmissiveIntensity );
		const QSignalBlocker blockerTextureScale( _sliderTextureScale );

		auto sceneMaterials = App::REG().view<App::Scene::Material>();
		_currentMaterial	= sceneMaterials.begin() == sceneMaterials.end()
								  ? InvalidEntity
								  : sceneMaterials.get<App::Scene::Material>( *sceneMaterials.begin() ).preset;

		_listMaterials->setCurrentRow( -1 );
		if ( not App::REG().valid( _currentMaterial ) || not App::REG().all_of<VTX::Renderer::Material>( _currentMaterial ) )
		{
			_currentMaterial = InvalidEntity;
			return;
		}

		for ( int i = 0; i < _listMaterials->count(); ++i )
		{
			if ( _listMaterials->item( i )->data( Qt::UserRole ).value<Entity>() == _currentMaterial )
			{
				_listMaterials->setCurrentRow( i );
				break;
			}
		}

		const VTX::Renderer::Material & material = App::REG().get<VTX::Renderer::Material>( _currentMaterial );
		_sliderMetallic->setValue( material.metallic );
		_sliderRoughness->setValue( material.roughness );
		_sliderEmissiveIntensity->setValue( material.emissiveIntensity );
		_sliderTextureScale->setValue( material.textureScale );
		for ( size_t i = 0; i < _textureWidgets.size(); ++i )
		{
			const std::optional<VTX::Renderer::MaterialTexture> & texture
				= _texture( material, VTX::Renderer::Material::E_TEXTURE( i ) );
			const QString	 path	 = texture ? QString::fromStdString( texture->path ) : QString {};
			TextureWidgets & widgets = _textureWidgets[ i ];
			widgets.linePath->setText( path );
			widgets.linePath->setToolTip( path );
			widgets.buttonClear->setEnabled( texture.has_value() );
		}
	}
} // namespace VTX::UI::QT::Widget::Library::GraphicsConfig
