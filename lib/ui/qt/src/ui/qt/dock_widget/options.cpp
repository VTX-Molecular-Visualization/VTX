#include "ui/qt/dock_widget/options.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/application.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/settings.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include "ui/qt/widget/actionable_push_button.hpp"
#include "ui/qt/widget/main_window.hpp"
#include "ui/qt/widget/opengl_widget.hpp"
#include <QActionGroup>
#include <QDesktopServices>
#include <QFileDialog>
#include <QGroupBox>
#include <QPushButton>
#include <app/action/action_manager.hpp>
#include <app/action/application.hpp>
#include <app/filesystem.hpp>
#include <app/network/network_manager.hpp>
#include <app/services.hpp>
#include <util/string.hpp>

namespace
{
	const QString _TEXT_CACHE_COUNT = "Files : %1";
	const QString _TEXT_CACHE_SIZE	= "Size : %1";

} // namespace

namespace VTX::UI::QT::DockWidget
{

	Options::Options( QWidget * p_parent ) : BaseDockWidget( p_parent, "Options" )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		setWindowIcon( STYLE().iconFromCodepoint( Style::Icons::SETTINGS ) );

		using namespace Widget;
		using namespace Action;
		using namespace Option;

		// Display.
		// Theme.
		auto * groupBoxDisplay = new QGroupBox( "Display" );
		auto * layoutDisplay   = new QVBoxLayout( groupBoxDisplay );

		// TODO: refacto.
		auto * system = addAction<Action::Theme::System>();
		auto * light  = addAction<Action::Theme::Light>();
		auto * dark	  = addAction<Action::Theme::Dark>();

		_comboBoxTheme = new QComboBox( this );
		_comboBoxTheme->addItem( "System", QVariant::fromValue( system ) );
		_comboBoxTheme->addItem( "Light", QVariant::fromValue( light ) );
		_comboBoxTheme->addItem( "Dark", QVariant::fromValue( dark ) );

		// Set default value.
		QSignalBlocker blocker0( _comboBoxTheme );
		switch ( STYLE().getCurrentTheme() )
		{
		case Style::E_THEME::SYSTEM: _comboBoxTheme->setCurrentText( "System" ); break;
		case Style::E_THEME::LIGHT: _comboBoxTheme->setCurrentText( "Light" ); break;
		case Style::E_THEME::DARK: _comboBoxTheme->setCurrentText( "Dark" ); break;
		default: break;
		}

		connect(
			_comboBoxTheme,
			&QComboBox::currentTextChanged,
			[ this ]( const QString & p_text )
			{
				QAction * const action = _comboBoxTheme->currentData().value<QAction *>();
				if ( action )
				{
					action->trigger();
				}
			}
		);

		// TODO: move in Action factory?
		system->actionGroup();
		connect(
			system->actionGroup(),
			&QActionGroup::triggered,
			[ this ]( QAction * p_action )
			{
				const int index = _comboBoxTheme->findData( QVariant::fromValue( p_action ) );
				if ( index != -1 )
				{
					QSignalBlocker blocker1( _comboBoxTheme );
					_comboBoxTheme->setCurrentIndex( index );
				}
			}
		);

		layoutDisplay->addWidget( _comboBoxTheme );

		// Font.
		_comboBoxFont = new QComboBox( this );
		uint i		  = 0;
		for ( const QString & fontName : STYLE().getAvailableFonts() )
		{
			_comboBoxFont->addItem( fontName );
			_comboBoxFont->setItemData( i++, QFont( fontName, Style::DEFAULT_FONT_SIZE ), Qt::FontRole );
		}

		QSignalBlocker blocker( _comboBoxFont );
		_comboBoxFont->setCurrentText( STYLE().getCurrentFontFamily() );

		connect(
			_comboBoxFont,
			&QComboBox::currentTextChanged,
			[ this ]( const QString & p_fontName ) { STYLE().setFontFamily( p_fontName ); }
		);

		layoutDisplay->addWidget( _comboBoxFont );

		// Reset layout.
		auto * buttonResetLayout = new ActionablePushButton( Application::getAction<Theme::ResetLayout>(), this );
		layoutDisplay->addWidget( buttonResetLayout );

		// Graphics.
		auto * groupBoxGraphics = new QGroupBox( "Graphics" );
		auto * layoutGraphics	= new QVBoxLayout( groupBoxGraphics );
		_checkBoxVSync			= new QCheckBox( "Vertical synchronization", this );
		_checkBoxVSync->setToolTip( "Synchronize the frame rate with the screen refresh rate" );
		_checkBoxVSync->setWhatsThis( "Synchronize the frame rate with the screen refresh rate" );

		connect(
			_checkBoxVSync,
			&QCheckBox::checkStateChanged,
			[ this ]( const int p_state )
			{
				MAIN_WINDOW()
					.findChild<Widget::OpenGLWidget *>( Util::typeName<Widget::OpenGLWidget>() )
					->setVSync( p_state == Qt::Checked );
			}
		);

		layoutGraphics->addWidget( _checkBoxVSync );

		_checkBoxSavePower = new QCheckBox( "Save GPU power", this );
		_checkBoxSavePower->setToolTip( "Render frame only on update" );
		_checkBoxSavePower->setWhatsThis( "Render frame only on update" );

		connect(
			_checkBoxSavePower,
			&QCheckBox::checkStateChanged,
			[ this ]( const int p_state )
			{ App::ACTION().execute<App::Action::Application::SetSavePower>( p_state == Qt::Checked ); }
		);

		layoutGraphics->addWidget( _checkBoxSavePower );

		// Cache.
		auto * groupBoxCache = new QGroupBox( "Data cache" );
		auto * layoutCache	 = new QVBoxLayout( groupBoxCache );

		auto * layoutCacheButton = new QHBoxLayout();

		auto * buttonOpenCache	  = new ActionablePushButton( Application::getAction<Option::Cache::Open>(), this );
		auto * buttonClearCache	  = new ActionablePushButton( Application::getAction<Option::Cache::Clear>(), this );
		auto * buttonRefreshCache = new ActionablePushButton( Application::getAction<Option::Cache::Refresh>(), this );

		const FilePath cachePath = App::Filesystem::getCacheDir();
		connect(
			buttonOpenCache,
			&QPushButton::clicked,
			[ this, cachePath ]()
			{ QDesktopServices::openUrl( QUrl::fromLocalFile( QDir( cachePath ).absolutePath() ) ); }
		);

		connect(
			buttonClearCache,
			&QPushButton::clicked,
			[ this, cachePath ]()
			{
				QDir( cachePath ).removeRecursively();
				_refreshCacheInfos();
			}
		);

		connect( buttonRefreshCache, &QPushButton::clicked, [ this ]() { _refreshCacheInfos(); } );

		_labelCacheCount = new QLabel( this );
		_labelCacheSize	 = new QLabel( this );

		layoutCache->addWidget( _labelCacheCount );
		layoutCache->addWidget( _labelCacheSize );
		layoutCacheButton->addWidget( buttonOpenCache );
		layoutCacheButton->addWidget( buttonClearCache );
		layoutCacheButton->addWidget( buttonRefreshCache );
		layoutCache->addLayout( layoutCacheButton );

		_layout->addWidget( groupBoxDisplay );
		_layout->addWidget( groupBoxGraphics );
		_layout->addWidget( groupBoxCache );
		_layout->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ) );

		_refreshCacheInfos();

		// Settings.
		QSignalBlocker b( _checkBoxVSync ); // Do not emit, opengl widget already knows.
		_checkBoxVSync->setChecked( SETTINGS().value( SETTING_KEY_VSYNC, VSYNC_DEFAULT ).toBool() );
		_checkBoxSavePower->setChecked( SETTINGS().value( SETTING_KEY_SAVE_POWER, SAVE_POWER_DEFAULT ).toBool() );

		QTimer::singleShot(
			0,
			this,
			[ this ]()
			{
				MAIN_WINDOW()
					.findChild<Widget::OpenGLWidget *>( Util::typeName<Widget::OpenGLWidget>() )
					->setVSync( _checkBoxVSync->isChecked() );
			}
		);

		App::NETWORK().onFileCached += [ this ]() { _refreshCacheInfos(); };
	}

	Options::~Options()
	{
		SETTINGS().setValue( SETTING_KEY_VSYNC, _checkBoxVSync->isChecked() );
		SETTINGS().setValue( SETTING_KEY_SAVE_POWER, _checkBoxSavePower->isChecked() );
	}

	void Options::_refreshCacheInfos()
	{
		const FilePath		cachePath = App::Filesystem::getCacheDir();
		const QDir			cacheDir( cachePath );
		const uint			fileCount = cacheDir.entryList( QDir::Files ).size();
		const QFileInfoList list	  = cacheDir.entryInfoList();
		size_t				size	  = 0;
		for ( const auto & info : list )
		{
			size += info.size();
		}

		_labelCacheCount->setText( _TEXT_CACHE_COUNT.arg( fileCount ) );
		_labelCacheSize->setText(
			_TEXT_CACHE_SIZE.arg( QString::fromStdString( Util::String::memSizeToStr( size ) ) )
		);
	}

} // namespace VTX::UI::QT::DockWidget
