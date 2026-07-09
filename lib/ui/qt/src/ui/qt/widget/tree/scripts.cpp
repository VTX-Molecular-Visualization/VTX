#include "ui/qt/widget/tree/scripts.hpp"
#include "ui/qt/events.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/settings.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include <QDir>
#include <app/action/io.hpp>
#include <app/services.hpp>
#include <app/session.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::Widget::Tree
{
	namespace
	{
		class ScriptsProxyModel : public QSortFilterProxyModel
		{
		  public:
			ScriptsProxyModel( const QString & p_scriptsDir, QObject * const p_parent ) :
				QSortFilterProxyModel( p_parent ), _scriptsDir( _normalize( p_scriptsDir ) )
			{
			}

			QVariant data( const QModelIndex & p_index, const int p_role ) const override
			{
				if ( p_index.column() == 0 )
				{
					const auto * const fileModel = qobject_cast<const QFileSystemModel *>( sourceModel() );
					if ( fileModel != nullptr
						 && _normalize( fileModel->filePath( mapToSource( p_index ) ) ) == _scriptsDir )
					{
						if ( p_role == Qt::DisplayRole )
						{
							return "Scripts";
						}
						if ( p_role == Qt::DecorationRole )
						{
							return STYLE().iconFromCodepoint( Style::Icons::SCRIPT );
						}
					}
				}

				return QSortFilterProxyModel::data( p_index, p_role );
			}

		  protected:
			bool filterAcceptsRow( const int p_sourceRow, const QModelIndex & p_sourceParent ) const override
			{
				const auto * const fileModel = qobject_cast<const QFileSystemModel *>( sourceModel() );
				if ( fileModel == nullptr )
				{
					return false;
				}

				const QModelIndex index = fileModel->index( p_sourceRow, 0, p_sourceParent );
				const QString	  path	= _normalize( fileModel->filePath( index ) );
				return _isSameOrChild( path, _scriptsDir ) || _isSameOrChild( _scriptsDir, path );
			}

		  private:
			const QString _scriptsDir;

			static QString _normalize( const QString & p_path )
			{
				return QDir::fromNativeSeparators( QDir::cleanPath( p_path ) );
			}

			static bool _isSameOrChild( const QString & p_path, const QString & p_parent )
			{
#ifdef _WIN32
				constexpr Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive;
#else
				constexpr Qt::CaseSensitivity caseSensitivity = Qt::CaseSensitive;
#endif
				if ( p_path.compare( p_parent, caseSensitivity ) == 0 )
				{
					return true;
				}

				QString parentPrefix = p_parent;
				if ( not parentPrefix.endsWith( '/' ) )
				{
					parentPrefix += '/';
				}
				return p_path.startsWith( parentPrefix, caseSensitivity );
			}
		};
	} // namespace

	Scripts::Scripts( QWidget * const p_parent ) : BaseTree( p_parent )
	{
		const FilePath scriptsDir  = App::SESSION().getDefaultScriptsDir();
		const FilePath parentDir   = scriptsDir.parent_path();
		const QString  scriptsPath = QString::fromStdString( scriptsDir.string() );
		const QString  parentPath  = QString::fromStdString( parentDir.string() );

		_fileModel = new QFileSystemModel( this );
		_fileModel->setFilter( QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot );
		_fileModel->setNameFilters( { "*.py" } );
		_fileModel->setNameFilterDisables( false );
		_fileModel->setReadOnly( true );
		_fileModel->setRootPath( parentPath );

		_proxyModel = new ScriptsProxyModel( scriptsPath, this );
		_proxyModel->setSourceModel( _fileModel );
		_proxyModel->setDynamicSortFilter( true );
		_proxyModel->sort( 0, Qt::AscendingOrder );

		setModel( _proxyModel );
		setRootIndex( _proxyModel->mapFromSource( _fileModel->index( parentPath ) ) );
		setColumnHidden( 1, true );
		setColumnHidden( 2, true );
		setColumnHidden( 3, true );

		const QModelIndex scriptsIndex = _proxyModel->mapFromSource( _fileModel->index( scriptsPath ) );
		setExpanded( scriptsIndex, SETTINGS().value( SETTING_KEY_SCRIPTS_EXPANDED, true ).toBool() );

		connect( this, &QTreeView::clicked, this, &Scripts::_selectScript );
		connect( this, &QTreeView::doubleClicked, this, &Scripts::_runScript );
		connect(
			this,
			&QTreeView::expanded,
			this,
			[ this, scriptsPath ]( const QModelIndex & p_index )
			{
				if ( _fileModel->filePath( _proxyModel->mapToSource( p_index ) ) == scriptsPath )
				{
					SETTINGS().setValue( SETTING_KEY_SCRIPTS_EXPANDED, true );
				}
			}
		);
		connect(
			this,
			&QTreeView::collapsed,
			this,
			[ this, scriptsPath ]( const QModelIndex & p_index )
			{
				if ( _fileModel->filePath( _proxyModel->mapToSource( p_index ) ) == scriptsPath )
				{
					SETTINGS().setValue( SETTING_KEY_SCRIPTS_EXPANDED, false );
				}
			}
		);
		connect(
			_fileModel, &QFileSystemModel::directoryLoaded, this, [ this ]( const QString & ) { updateGeometry(); }
		);
		connect(
			_proxyModel,
			&QAbstractItemModel::rowsInserted,
			this,
			[ this ]( const QModelIndex &, const int, const int ) { updateGeometry(); }
		);
		connect(
			_proxyModel,
			&QAbstractItemModel::rowsRemoved,
			this,
			[ this ]( const QModelIndex &, const int, const int ) { updateGeometry(); }
		);
	}

	void Scripts::_selectScript( const QModelIndex & p_index )
	{
		if ( not p_index.isValid() )
		{
			return;
		}

		const QModelIndex sourceIndex = _proxyModel->mapToSource( p_index );
		if ( _fileModel->isDir( sourceIndex ) )
		{
			return;
		}

		App::HUB().trigger<Events::ScriptSelected>( FilePath( _fileModel->filePath( sourceIndex ).toStdString() ) );
	}

	void Scripts::_runScript( const QModelIndex & p_index )
	{
		if ( not p_index.isValid() )
		{
			return;
		}

		const QModelIndex sourceIndex = _proxyModel->mapToSource( p_index );
		if ( _fileModel->isDir( sourceIndex ) )
		{
			return;
		}

		App::ACTION().execute<App::Action::IO::RunPythonScript>(
			FilePath( _fileModel->filePath( sourceIndex ).toStdString() )
		);
	}
} // namespace VTX::UI::QT::Widget::Tree
