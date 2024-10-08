#include "ui/qt/dock_widget/options.hpp"
#include <QDesktopServices>
#include <QFileDialog>
#include <QPushButton>
#include <app/core/network/network_system.hpp>
#include <app/filesystem.hpp>
#include <util/string.hpp>

namespace VTX::UI::QT::DockWidget
{

	Options::Options( QWidget * p_parent ) : BaseWidget<Options, QDockWidget>( "Options", p_parent )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		setContentsMargins( 0, 0, 0, 0 );

		auto * widget = new QWidget( this );
		auto * layout = new QVBoxLayout( widget );
		layout->setContentsMargins( 0, 0, 0, 0 );
		setWidget( widget );

		// Cache.
		auto * groupCacheBox	= new QGroupBox( "Data cache" );
		auto * groupCacheLayout = new QVBoxLayout( this );

		groupCacheBox->setLayout( groupCacheLayout );

		auto * buttonCacheLayout = new QHBoxLayout( this );
		auto * buttonOpenCache	 = new QPushButton( "Open", this );
		auto * buttonClearCache	 = new QPushButton( "Clear", this );

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

		_labelCacheCount = new QLabel( this );
		_labelCacheSize	 = new QLabel( this );

		groupCacheLayout->addWidget( _labelCacheCount );
		groupCacheLayout->addWidget( _labelCacheSize );
		buttonCacheLayout->addWidget( buttonOpenCache );
		buttonCacheLayout->addWidget( buttonClearCache );
		groupCacheLayout->addLayout( buttonCacheLayout );

		layout->addWidget( groupCacheBox );
		layout->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ) );

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

} // namespace VTX::UI::QT::DockWidget
