#include "ui/qt/dock_widget/options.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/application.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/settings.hpp"
#include "ui/qt/widget/actionable_push_button.hpp"
#include "ui/qt/widget/main_window.hpp"
#include "ui/qt/widget/opengl_widget.hpp"
#include <QActionGroup>
#include <QDesktopServices>
#include <QFileDialog>
#include <QGroupBox>
#include <QPushButton>
#include <app/filesystem.hpp>
#include <app/network/network_manager.hpp>
#include <util/string.hpp>

namespace
{
	const QString			   _TEXT_CACHE_COUNT  = "Files : %1";
	const QString			   _TEXT_CACHE_SIZE	  = "Size : %1";
	constexpr std::string_view _SETTING_KEY_VSYNC = "options/vsync";
} // namespace

namespace VTX::UI::QT::DockWidget
{

	Options::Options( QWidget * p_parent ) : BaseDockWidget( p_parent )
	{
		setWindowTitle( "Options" );
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

		// Display.
		auto * groupBoxDisplay = new QGroupBox( "Display" );
		auto * layoutDisplay   = new QVBoxLayout( groupBoxDisplay );

		auto * system = addAction<Action::Theme::System>();
		auto * light  = addAction<Action::Theme::Light>();
		auto * dark	  = addAction<Action::Theme::Dark>();

		_comboBoxTheme = new QComboBox( this );
		_comboBoxTheme->addItem( "System", QVariant::fromValue( system ) );
		_comboBoxTheme->addItem( "Light", QVariant::fromValue( light ) );
		_comboBoxTheme->addItem( "Dark", QVariant::fromValue( dark ) );

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
					QSignalBlocker blocker( _comboBoxTheme );
					_comboBoxTheme->setCurrentIndex( index );
				}
			}
		);

		layoutDisplay->addWidget( _comboBoxTheme );

		// Graphics.
		auto * groupBoxGraphics = new QGroupBox( "Graphics" );
		auto * layoutGraphics	= new QVBoxLayout( groupBoxGraphics );
		_checkBoxVSync			= new QCheckBox( "Vertical synchronization", this );

		/*
		APP_QT::onUICreated += [ this ]()
		{
			auto * glWidget = Core::WIDGETS::get().get<Widget::OpenGLWidget>();
			_checkBoxVSync->setChecked( glWidget->isVSync() );
		};
		*/

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

		// Cache.
		auto * groupBoxCache = new QGroupBox( "Data cache" );
		auto * layoutCache	 = new QVBoxLayout( groupBoxCache );

		auto * layoutCacheButton = new QHBoxLayout();

		using namespace Widget;
		using namespace Action;
		using namespace Option;

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

		QSignalBlocker blocker( _checkBoxVSync );
		_checkBoxVSync->setChecked( SETTINGS().value( _SETTING_KEY_VSYNC, true ).toBool() );

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

	Options::~Options() { SETTINGS().setValue( _SETTING_KEY_VSYNC, _checkBoxVSync->isChecked() ); }

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
