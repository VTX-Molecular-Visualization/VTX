#include "ui/qt/widget/library/graphics_config/background_section.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include <QFileDialog>
#include <QHBoxLayout>
#include <QIcon>
#include <QSignalBlocker>
#include <QSize>
#include <app/services.hpp>
#include <app/session.hpp>
#include <filesystem>
#include <util/math.hpp>
#include <util/string.hpp>

namespace VTX::UI::QT::Widget::Library::GraphicsConfig
{
	namespace
	{
		std::string _normalizedPathString( const FilePath & p_path )
		{
			return std::filesystem::absolute( p_path ).lexically_normal().string();
		}

		float _exposureMultiplierFromEv( const float p_value ) { return Util::Math::exp2( p_value ); }

		float _exposureEvFromMultiplier( const float p_value )
		{
			const float minimumExposure = Util::Math::exp2( VTX::Renderer::TONE_MAPPING_EXPOSURE_EV_MIN );
			return Util::Math::log2( Util::Math::max( p_value, minimumExposure ) );
		}
	} // namespace

	BackgroundSection::BackgroundSection( QWidget * const p_parent ) : Section( "Background", p_parent )
	{
		using namespace Renderer;

		setChecked( true );
		freeze( true );

		auto * const backgroundModeWidget = new QWidget( this );
		auto * const backgroundModeLayout = new QHBoxLayout( backgroundModeWidget );
		backgroundModeLayout->setContentsMargins( 0, 0, 0, 0 );
		_radioBackgroundColor		= new QRadioButton( "Color", backgroundModeWidget );
		_radioBackgroundEnvironment = new QRadioButton( "Environment map", backgroundModeWidget );
		backgroundModeLayout->addWidget( _radioBackgroundColor );
		backgroundModeLayout->addWidget( _radioBackgroundEnvironment );
		addWidget( backgroundModeWidget );

		_colorPicker = new ColorPicker( this );
		addWidget( _colorPicker );
		_colorPicker->setText( "Color" );

		_listEnvironmentMaps = new QListWidget( this );
		_listEnvironmentMaps->setSelectionMode( QAbstractItemView::SingleSelection );
		_listEnvironmentMaps->setSortingEnabled( true );
		_listEnvironmentMaps->setIconSize( QSize( 192, 96 ) );
		_listEnvironmentMaps->setFixedHeight( 220 );
		_listEnvironmentMaps->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
		addWidget( _listEnvironmentMaps );

		auto * const environmentPathWidget = new QWidget( this );
		auto * const environmentPathLayout = new QHBoxLayout( environmentPathWidget );
		environmentPathLayout->setContentsMargins( 0, 0, 0, 0 );
		_lineEnvironmentPath = new QLineEdit( environmentPathWidget );
		_lineEnvironmentPath->setReadOnly( true );
		_buttonEnvironmentBrowse = new QPushButton( environmentPathWidget );
		_buttonEnvironmentBrowse->setIcon( STYLE().iconFromCodepoint( Style::Icons::OPEN ) );
		_buttonEnvironmentBrowse->setToolTip( "Open environment map" );
		_buttonEnvironmentBrowse->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
		_buttonEnvironmentBrowse->setMinimumWidth( 0 );
		_buttonEnvironmentClear = new QPushButton( environmentPathWidget );
		_buttonEnvironmentClear->setIcon( STYLE().iconFromCodepoint( Style::Icons::DELETE ) );
		_buttonEnvironmentClear->setToolTip( "Remove environment map" );
		_buttonEnvironmentClear->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
		_buttonEnvironmentClear->setMinimumWidth( 0 );
		environmentPathLayout->addWidget( _lineEnvironmentPath );
		environmentPathLayout->addWidget( _buttonEnvironmentBrowse );
		environmentPathLayout->addWidget( _buttonEnvironmentClear );
		addWidget( environmentPathWidget );

		_labelEnvironmentRotation = new QLabel( "Environment rotation", this );
		addWidget( _labelEnvironmentRotation );
		_sliderEnvironmentRotation = new EditableSlider( Qt::Orientation::Horizontal, this );
		addWidget( _sliderEnvironmentRotation );
		_sliderEnvironmentRotation->setMinimum( Util::Math::degrees( ENVIRONMENT_ROTATION_MIN ) );
		_sliderEnvironmentRotation->setMaximum( Util::Math::degrees( ENVIRONMENT_ROTATION_MAX ) );
		_sliderEnvironmentRotation->setStep( 1.f );
		_sliderEnvironmentRotation->setDecimals( 1 );
		_sliderEnvironmentRotation->setSuffix( "°" );

		_labelSkyboxIntensity = new QLabel( "Intensity", this );
		addWidget( _labelSkyboxIntensity );
		_sliderSkyboxIntensity = new EditableSlider( Qt::Orientation::Horizontal, this );
		addWidget( _sliderSkyboxIntensity );
		_sliderSkyboxIntensity->setMinimum( TONE_MAPPING_EXPOSURE_EV_MIN );
		_sliderSkyboxIntensity->setMaximum( TONE_MAPPING_EXPOSURE_EV_MAX );
		_sliderSkyboxIntensity->setStep( 0.1f );
		_sliderSkyboxIntensity->setDecimals( 2 );
		_sliderSkyboxIntensity->setSuffix( "EV" );

		_labelIblIntensity = new QLabel( "Reflections", this );
		addWidget( _labelIblIntensity );
		_sliderIblIntensity = new EditableSlider( Qt::Orientation::Horizontal, this );
		addWidget( _sliderIblIntensity );
		_sliderIblIntensity->setMinimum( TONE_MAPPING_EXPOSURE_EV_MIN );
		_sliderIblIntensity->setMaximum( TONE_MAPPING_EXPOSURE_EV_MAX );
		_sliderIblIntensity->setStep( 0.1f );
		_sliderIblIntensity->setDecimals( 2 );
		_sliderIblIntensity->setSuffix( "EV" );

		_colorPicker->onColorChanged += [ this ]( const QColor & p_color )
		{
			_changeValue<E_GRAPHICS_CONFIG_VALUES::COLOR_BACKGROUND, Util::Color::Rgba>(
				Helper::fromQColor( p_color )
			);
		};

		connect(
			_radioBackgroundColor,
			&QRadioButton::toggled,
			[ this ]( const bool p_checked )
			{
				if ( p_checked )
				{
					_changeValue<E_GRAPHICS_CONFIG_VALUES::BACKGROUND_MODE, E_BACKGROUND_MODE>(
						E_BACKGROUND_MODE::COLOR
					);
				}
			}
		);
		connect(
			_radioBackgroundEnvironment,
			&QRadioButton::toggled,
			[ this ]( const bool p_checked )
			{
				if ( p_checked )
				{
					_changeValue<E_GRAPHICS_CONFIG_VALUES::BACKGROUND_MODE, E_BACKGROUND_MODE>(
						E_BACKGROUND_MODE::ENVIRONMENT
					);
				}
			}
		);
		connect(
			_listEnvironmentMaps,
			&QListWidget::itemClicked,
			[ this ]( QListWidgetItem * const p_item )
			{
				_changeValue<E_GRAPHICS_CONFIG_VALUES::ENVIRONMENT_PATH, FilePath>(
					FilePath( p_item->data( Qt::UserRole ).toString().toStdString() )
				);
			}
		);
		connect(
			_buttonEnvironmentBrowse,
			&QPushButton::clicked,
			[ this ]
			{
				const auto &  config = App::REG().get<VTX::Renderer::GraphicsConfig>( _getCurrentPreset() );
				const QString initialPath
					= config.shading.environmentPath
						  ? QString::fromStdString( config.shading.environmentPath->parent_path().string() )
						  : QString::fromStdString( App::SESSION().getHdriDir().string() );
				const QString selectedPath = QFileDialog::getOpenFileName(
					this,
					"Select environment map",
					initialPath,
					"Environment maps (*.ktx2 *.KTX2 *.exr *.EXR *.hdr *.HDR)"
				);
				if ( not selectedPath.isEmpty() )
				{
					_changeValue<E_GRAPHICS_CONFIG_VALUES::ENVIRONMENT_PATH, FilePath>(
						FilePath( selectedPath.toStdString() )
					);
				}
			}
		);

		connect(
			_buttonEnvironmentClear,
			&QPushButton::clicked,
			[ this ] { _changeValue<E_GRAPHICS_CONFIG_VALUES::ENVIRONMENT_PATH, FilePath>( FilePath {} ); }
		);
		connect(
			_sliderEnvironmentRotation,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::ENVIRONMENT_ROTATION, float>( Util::Math::radians( p_value ) ); }
		);
		connect(
			_sliderSkyboxIntensity,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::SKYBOX_INTENSITY, float>( _exposureMultiplierFromEv( p_value ) ); }
		);
		connect(
			_sliderIblIntensity,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::IBL_INTENSITY, float>( _exposureMultiplierFromEv( p_value ) ); }
		);
	}

	void BackgroundSection::_refreshEnvironmentMaps()
	{
		_listEnvironmentMaps->clear();

		const FilePath hdriDir = App::SESSION().getHdriDir();
		if ( not std::filesystem::is_directory( hdriDir ) )
		{
			return;
		}

		for ( const std::filesystem::directory_entry & entry :
			  std::filesystem::recursive_directory_iterator( hdriDir ) )
		{
			if ( not entry.is_regular_file() || Util::String::toLower( entry.path().extension().string() ) != ".ktx2" )
			{
				continue;
			}

			FilePath displayPath = std::filesystem::relative( entry.path(), hdriDir );
			displayPath.replace_extension();

			const QString  path			 = QString::fromStdString( _normalizedPathString( entry.path() ) );
			const FilePath thumbnailPath = FilePath( entry.path() ).replace_extension( ".png" );
			auto * const   item			 = new QListWidgetItem(
				   QIcon( QString::fromStdString( thumbnailPath.string() ) ), QString {}, _listEnvironmentMaps
			   );
			item->setData( Qt::UserRole, path );
			item->setSizeHint( QSize( 208, 104 ) );
			item->setToolTip( QString::fromStdString( displayPath.string() ) + "\n" + path );
		}
	}

	void BackgroundSection::setConfig( const Entity p_preset, const VTX::Renderer::GraphicsConfig & p_config )
	{
		_setCurrentPreset( p_preset );
		_refreshEnvironmentMaps();
		const QSignalBlocker blockerBackgroundColor( _radioBackgroundColor );
		const QSignalBlocker blockerBackgroundEnvironment( _radioBackgroundEnvironment );
		const QSignalBlocker blockerColor( _colorPicker );
		const QSignalBlocker blockerEnvironmentMaps( _listEnvironmentMaps );
		const QSignalBlocker blockerRotation( _sliderEnvironmentRotation );
		const QSignalBlocker blockerSkyboxIntensity( _sliderSkyboxIntensity );
		const QSignalBlocker blockerIblIntensity( _sliderIblIntensity );

		_radioBackgroundColor->setChecked( p_config.shading.backgroundMode == VTX::Renderer::E_BACKGROUND_MODE::COLOR );
		_radioBackgroundEnvironment->setChecked(
			p_config.shading.backgroundMode == VTX::Renderer::E_BACKGROUND_MODE::ENVIRONMENT
		);
		_colorPicker->setColor( Helper::toQColor( p_config.shading.colorBackground ) );
		const QString environmentPath = p_config.shading.environmentPath
											? QString::fromStdString( p_config.shading.environmentPath->string() )
											: QString {};
		_lineEnvironmentPath->setText( environmentPath );
		_lineEnvironmentPath->setToolTip( environmentPath );
		_buttonEnvironmentClear->setEnabled( p_config.shading.environmentPath.has_value() );
		_listEnvironmentMaps->setCurrentRow( -1 );
		if ( p_config.shading.environmentPath )
		{
			const QString currentPath
				= QString::fromStdString( _normalizedPathString( *p_config.shading.environmentPath ) );
			for ( int i = 0; i < _listEnvironmentMaps->count(); ++i )
			{
				if ( _listEnvironmentMaps->item( i )->data( Qt::UserRole ).toString() == currentPath )
				{
					_listEnvironmentMaps->setCurrentRow( i );
					break;
				}
			}
		}
		_sliderSkyboxIntensity->setValue( _exposureEvFromMultiplier( p_config.shading.skyboxIntensity ) );
		_sliderIblIntensity->setValue( _exposureEvFromMultiplier( p_config.shading.iblIntensity ) );
		_sliderEnvironmentRotation->setValue( Util::Math::degrees( p_config.shading.environmentRotation ) );
		const bool environmentMode = p_config.shading.backgroundMode == VTX::Renderer::E_BACKGROUND_MODE::ENVIRONMENT;
		const bool hasEnvironment  = environmentMode && p_config.shading.environmentPath.has_value();
		const bool pbr			   = p_config.shading.mode == VTX::Renderer::E_SHADING::PBR;
		_colorPicker->setVisible( p_config.shading.backgroundMode == VTX::Renderer::E_BACKGROUND_MODE::COLOR );
		_listEnvironmentMaps->setVisible( environmentMode );
		_lineEnvironmentPath->parentWidget()->setVisible( environmentMode );
		_labelSkyboxIntensity->setVisible( hasEnvironment );
		_sliderSkyboxIntensity->setVisible( hasEnvironment );
		_labelIblIntensity->setVisible( hasEnvironment && pbr );
		_sliderIblIntensity->setVisible( hasEnvironment && pbr );
		_labelEnvironmentRotation->setVisible( hasEnvironment );
		_sliderEnvironmentRotation->setVisible( hasEnvironment );
	}
} // namespace VTX::UI::QT::Widget::Library::GraphicsConfig
