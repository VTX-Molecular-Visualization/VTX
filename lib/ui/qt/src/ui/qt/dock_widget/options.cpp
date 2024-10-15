#include "ui/qt/dock_widget/options.hpp"
#include "ui/qt/application.hpp"
#include "ui/qt/widget/opengl_widget.hpp"
#include <QDesktopServices>
#include <QFileDialog>
#include <QPushButton>
#include <app/core/network/network_system.hpp>
#include <app/filesystem.hpp>
#include <util/string.hpp>

namespace VTX::UI::QT::DockWidget
{

	Options::Options( QWidget * p_parent ) : Core::BaseDockWidget<Options>( "Options", p_parent )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

		// Graphics.
		auto * groupBoxGraphics = new QGroupBox( "Graphics" );
		auto * layoutGraphics	= new QVBoxLayout( groupBoxGraphics );
		_checkBoxVSync			= new QCheckBox( "Vertical synchronization", this );

		APP_QT::onUICreated += [ this ]()
		{
			auto * glWidget = Core::WIDGETS::get().get<Widget::OpenGLWidget>();
			_checkBoxVSync->setChecked( glWidget->isVSync() );
		};

		connect(
			_checkBoxVSync,
			&QCheckBox::stateChanged,
			[ this ]( const int p_state )
			{
				// TODO: use action? available in script?
				Core::WIDGETS::get().get<Widget::OpenGLWidget>()->setVSync( p_state == Qt::Checked );
			}
		);

		layoutGraphics->addWidget( _checkBoxVSync );

		// Cache.
		auto * groupBoxCache = new QGroupBox( "Data cache" );
		auto * layoutCache	 = new QVBoxLayout( groupBoxCache );

		auto * layoutCacheButton  = new QHBoxLayout();
		auto * buttonOpenCache	  = new QPushButton( "Open", this );
		auto * buttonClearCache	  = new QPushButton( "Clear", this );
		auto * buttonRefreshCache = new QPushButton( "Refresh", this );

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

		_layout->addWidget( groupBoxGraphics );
		_layout->addWidget( groupBoxCache );
		_layout->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ) );

		_refreshCacheInfos();

		App::NETWORK_SYSTEM().onFileCached += [ this ]() { _refreshCacheInfos(); };
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
		_labelCacheSize->setText( _TEXT_CACHE_SIZE.arg( QString::fromStdString( Util::String::memSizeToStr( size ) ) )
		);
	}

	void Options::save() { SETTINGS.setValue( _SETTING_KEY_VSYNC, _checkBoxVSync->isChecked() ); }

	void Options::restore() { _checkBoxVSync->setChecked( SETTINGS.value( _SETTING_KEY_VSYNC, true ).toBool() ); }

} // namespace VTX::UI::QT::DockWidget
