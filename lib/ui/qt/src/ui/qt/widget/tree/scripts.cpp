#include "ui/qt/widget/tree/scripts.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/settings.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include <QSignalBlocker>
#include <algorithm>
#include <app/action/io.hpp>
#include <app/services.hpp>
#include <app/session.hpp>
#include <filesystem>
#include <system_error>
#include <util/action_manager.hpp>
#include <util/string.hpp>
#include <vector>

namespace VTX::UI::QT::Widget::Tree
{
	namespace
	{
		constexpr int REFRESH_DELAY = 200;

		bool _isHidden( const FilePath & p_path )
		{
			const std::string name = p_path.filename().string();
			return name.starts_with( '.' ) || name == "__pycache__";
		}

		bool _isScript( const std::filesystem::directory_entry & p_entry )
		{ return p_entry.is_regular_file() && p_entry.path().extension() == ".py"; }
	} // namespace

	Scripts::Scripts( QWidget * const p_parent ) : BaseTree( p_parent )
	{
		QTreeWidgetItem * const root = new QTreeWidgetItem( QStringList { "Scripts" } );
		root->setIcon( 0, STYLE().iconFromCodepoint( Style::Icons::SCRIPT ) );
		root->setFlags( Qt::ItemIsEnabled );
		addTopLevelItem( root );

		_builtInItem = new QTreeWidgetItem( root, QStringList { "Built-in" } );
		_builtInItem->setFlags( Qt::ItemIsEnabled );
		_userItem = new QTreeWidgetItem( root, QStringList { "User" } );
		_userItem->setFlags( Qt::ItemIsEnabled );

		_watcher	  = Helper::makeQObject<QFileSystemWatcher>( this );
		_refreshTimer = Helper::makeQObject<QTimer>( this );
		_refreshTimer->setSingleShot( true );
		_refreshTimer->setInterval( REFRESH_DELAY );
		connect( _watcher, &QFileSystemWatcher::directoryChanged, _refreshTimer, qOverload<>( &QTimer::start ) );
		connect( _refreshTimer, &QTimer::timeout, this, &Scripts::_refresh );

		_refresh();

		root->setExpanded( SETTINGS().value( SETTING_KEY_SCRIPTS_EXPANDED, true ).toBool() );
		_builtInItem->setExpanded( true );
		_userItem->setExpanded( true );

		connect(
			this,
			&QTreeWidget::itemExpanded,
			this,
			[ root ]( QTreeWidgetItem * const p_item )
			{
				if ( p_item == root )
				{
					SETTINGS().setValue( SETTING_KEY_SCRIPTS_EXPANDED, true );
				}
			}
		);
		connect(
			this,
			&QTreeWidget::itemCollapsed,
			this,
			[ root ]( QTreeWidgetItem * const p_item )
			{
				if ( p_item == root )
				{
					SETTINGS().setValue( SETTING_KEY_SCRIPTS_EXPANDED, false );
				}
			}
		);
		connect(
			this,
			&QTreeWidget::itemDoubleClicked,
			this,
			[]( QTreeWidgetItem * const p_item, const int )
			{
				if ( p_item == nullptr || not( p_item->flags() & Qt::ItemIsSelectable ) )
				{
					return;
				}

				App::ACTION().execute<App::Action::IO::RunPythonScript>(
					FilePath( p_item->data( 0, PATH_ROLE ).toString().toStdString() )
				);
			}
		);

		_scriptCopiedConnection = App::HUB().connect<Events::ScriptCopied, &Scripts::_onScriptCopied>( this );
	}

	void Scripts::_refresh()
	{
		std::error_code error;
		std::filesystem::create_directories( App::SESSION().getScriptsDir(), error );

		const QList<QTreeWidgetItem *> selected = selectedItems();
		const QString selectedPath = selected.isEmpty() ? QString() : selected.first()->data( 0, PATH_ROLE ).toString();
		if ( not selectedPath.isEmpty() && not std::filesystem::exists( FilePath( selectedPath.toStdString() ) ) )
		{
			clearSelection();
		}

		{
			const QSignalBlocker blocker( selectionModel() );

			QSet<QString> expanded;
			_collectExpanded( *_builtInItem, expanded );
			_collectExpanded( *_userItem, expanded );

			qDeleteAll( _builtInItem->takeChildren() );
			qDeleteAll( _userItem->takeChildren() );

			if ( not _watcher->directories().isEmpty() )
			{
				_watcher->removePaths( _watcher->directories() );
			}

			_fill( *_builtInItem, App::SESSION().getDefaultScriptsDir() / "examples", expanded );
			_fill( *_userItem, App::SESSION().getScriptsDir(), expanded );

			if ( QTreeWidgetItem * const item = _find( selectedPath ) )
			{
				item->setSelected( true );
			}
		}

		updateGeometry();
	}

	void Scripts::_fill( QTreeWidgetItem & p_parent, const FilePath & p_directory, const QSet<QString> & p_expanded )
	{
		std::error_code error;
		if ( not std::filesystem::is_directory( p_directory, error ) )
		{
			return;
		}

		_watcher->addPath( QString::fromStdString( p_directory.string() ) );

		std::vector<std::filesystem::directory_entry> entries;
		for ( const std::filesystem::directory_entry & entry : std::filesystem::directory_iterator( p_directory, error ) )
		{
			const bool isDirectory = entry.is_directory() && not entry.is_symlink();
			if ( not _isHidden( entry.path() ) && ( isDirectory || _isScript( entry ) ) )
			{
				entries.push_back( entry );
			}
		}

		std::ranges::sort(
			entries,
			[]( const std::filesystem::directory_entry & p_left, const std::filesystem::directory_entry & p_right )
			{
				if ( p_left.is_directory() != p_right.is_directory() )
				{
					return p_left.is_directory();
				}
				return Util::String::toLower( p_left.path().filename().string() )
					   < Util::String::toLower( p_right.path().filename().string() );
			}
		);

		for ( const std::filesystem::directory_entry & entry : entries )
		{
			const QString path = QString::fromStdString( entry.path().string() );
			QTreeWidgetItem * const item
				= new QTreeWidgetItem( &p_parent, QStringList { QString::fromStdString( entry.path().filename().string() ) } );
			item->setData( 0, PATH_ROLE, path );

			if ( entry.is_directory() )
			{
				item->setFlags( Qt::ItemIsEnabled );
				_fill( *item, entry.path(), p_expanded );
				item->setExpanded( p_expanded.contains( path ) );
			}
			else
			{
				item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );
			}
		}
	}

	void Scripts::_collectExpanded( const QTreeWidgetItem & p_item, QSet<QString> & p_expanded ) const
	{
		for ( int i = 0; i < p_item.childCount(); ++i )
		{
			const QTreeWidgetItem * const child = p_item.child( i );
			if ( child->isExpanded() )
			{
				p_expanded.insert( child->data( 0, PATH_ROLE ).toString() );
			}
			_collectExpanded( *child, p_expanded );
		}
	}

	QTreeWidgetItem * Scripts::_find( const QTreeWidgetItem & p_item, const QString & p_path ) const
	{
		for ( int i = 0; i < p_item.childCount(); ++i )
		{
			QTreeWidgetItem * const child = p_item.child( i );
			if ( child->data( 0, PATH_ROLE ).toString() == p_path )
			{
				return child;
			}
			if ( QTreeWidgetItem * const found = _find( *child, p_path ) )
			{
				return found;
			}
		}
		return nullptr;
	}

	QTreeWidgetItem * Scripts::_find( const QString & p_path ) const
	{
		if ( p_path.isEmpty() )
		{
			return nullptr;
		}

		if ( QTreeWidgetItem * const item = _find( *_builtInItem, p_path ) )
		{
			return item;
		}
		return _find( *_userItem, p_path );
	}

	void Scripts::_select( const FilePath & p_path )
	{
		QTreeWidgetItem * const item = _find( QString::fromStdString( p_path.string() ) );
		if ( item == nullptr )
		{
			return;
		}

		for ( QTreeWidgetItem * parent = item->parent(); parent != nullptr; parent = parent->parent() )
		{
			parent->setExpanded( true );
		}
		setCurrentItem( item );
		scrollToItem( item );
	}

	void Scripts::_onScriptCopied( const Events::ScriptCopied & p_event )
	{
		_refresh();
		_select( p_event.path );
	}
} // namespace VTX::UI::QT::Widget::Tree
