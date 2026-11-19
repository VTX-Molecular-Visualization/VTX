#include "ui/qt/widget/python_completer.hpp"
#include "ui/qt/helper.hpp"
#include <QAbstractItemView>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QScrollBar>
#include <app/ecs.hpp>
#include <app/python_binding/interpretor.hpp>
#include <app/services.hpp>
#include <util/thread/thread_manager.hpp>
#include <utility>
#include <vector>

namespace
{
	constexpr int MIN_COMPLETION_PREFIX = 2;

	/**
	 * @brief Get the expression before the cursor.
	 */
	QString _expressionBeforeCursor( const QString & p_line )
	{
		qsizetype start = p_line.size();
		int		  depth = 0;
		while ( start > 0 )
		{
			const QChar character = p_line.at( start - 1 );
			if ( character == ')' || character == ']' )
			{
				++depth;
			}
			else if ( character == '(' || character == '[' )
			{
				if ( depth == 0 )
				{
					break;
				}
				--depth;
			}
			else if ( depth == 0 && not character.isLetterOrNumber() && character != '_' && character != '.' )
			{
				break;
			}
			--start;
		}

		return depth == 0 ? p_line.mid( start ) : QString();
	}

	QString _elementType( const QString & p_type )
	{
		for ( const QString & container : { QStringLiteral( "list[" ), QStringLiteral( "List[" ) } )
		{
			if ( p_type.startsWith( container ) && p_type.endsWith( ']' ) )
			{
				return p_type.mid( container.size(), p_type.size() - container.size() - 1 );
			}
		}

		return {};
	}
} // namespace

namespace VTX::UI::QT::Widget
{
	PythonCompleter::PythonCompleter( QWidget * const p_widget ) : QCompleter( p_widget )
	{
		_model = Helper::makeQObject<QStringListModel>( this );
		setModel( _model );
		setWidget( p_widget );
		setCaseSensitivity( Qt::CaseInsensitive );
		setCompletionMode( QCompleter::PopupCompletion );

		_pythonInitializedConnection
			= App::HUB().connect<App::Events::PythonInitialized, &PythonCompleter::_onPythonInitialized>( this );
	}

	bool PythonCompleter::updateCompletions( const QString & p_lineBeforeCursor, const Variables & p_variables )
	{
		if ( _table.isEmpty() )
		{
			return false;
		}

		static const QRegularExpression identifierPattern( "^[A-Za-z_]\\w*$" );

		const QString	expression = _expressionBeforeCursor( p_lineBeforeCursor );
		const qsizetype dot		   = expression.lastIndexOf( '.' );
		const QString	prefix	   = expression.mid( dot + 1 );
		if ( not prefix.isEmpty() && not identifierPattern.match( prefix ).hasMatch() )
		{
			return false;
		}

		QStringList candidates;
		if ( dot < 0 )
		{
			if ( prefix.size() < MIN_COMPLETION_PREFIX )
			{
				return false;
			}

			for ( const QString & scope : _table.keys() )
			{
				if ( not scope.contains( '.' ) )
				{
					candidates.append( scope );
				}
			}
			candidates.append( p_variables.keys() );
		}
		else
		{
			const QString type = _resolveType( expression.left( dot ), p_variables );
			if ( not _table.contains( type ) )
			{
				return false;
			}
			candidates = _table.value( type ).keys();
		}

		candidates.removeDuplicates();
		candidates.sort( Qt::CaseInsensitive );
		_model->setStringList( candidates );
		setCompletionPrefix( prefix );
		return completionCount() > 0;
	}

	void PythonCompleter::showPopup( const QRect & p_cursorRect )
	{
		popup()->setCurrentIndex( completionModel()->index( 0, 0 ) );

		QRect rect = p_cursorRect;
		rect.setWidth( popup()->sizeHintForColumn( 0 ) + popup()->verticalScrollBar()->sizeHint().width() );
		complete( rect );
	}

	PythonCompleter::Variables PythonCompleter::inferVariables( const QStringList & p_lines ) const
	{
		static const QRegularExpression assignmentPattern( "^\\s*([A-Za-z_]\\w*)\\s*=(?!=)\\s*(.+?)\\s*;?\\s*$" );
		static const QRegularExpression loopPattern( "^\\s*for\\s+([A-Za-z_]\\w*)\\s+in\\s+(.+?)\\s*:\\s*$" );

		Variables variables;
		for ( const QString & line : p_lines )
		{
			if ( const QRegularExpressionMatch match = assignmentPattern.match( line ); match.hasMatch() )
			{
				const QString type = _resolveType( match.captured( 2 ), variables );
				if ( type.isEmpty() )
				{
					variables.remove( match.captured( 1 ) );
				}
				else
				{
					variables.insert( match.captured( 1 ), type );
				}
			}
			else if ( const QRegularExpressionMatch loop = loopPattern.match( line ); loop.hasMatch() )
			{
				const QString type = _elementType( _resolveType( loop.captured( 2 ), variables ) );
				if ( type.isEmpty() )
				{
					variables.remove( loop.captured( 1 ) );
				}
				else
				{
					variables.insert( loop.captured( 1 ), type );
				}
			}
		}

		return variables;
	}

	QString PythonCompleter::_resolveType( const QString & p_expression, const Variables & p_variables ) const
	{
		static const QRegularExpression segmentPattern( "^([A-Za-z_]\\w*)(.*)$" );

		QStringList segments;
		QString		current;
		int			depth = 0;
		for ( const QChar character : p_expression )
		{
			if ( character == '(' || character == '[' )
			{
				++depth;
			}
			else if ( character == ')' || character == ']' )
			{
				--depth;
			}

			if ( character == '.' && depth == 0 )
			{
				segments.append( current );
				current.clear();
			}
			else
			{
				current.append( character );
			}
		}
		segments.append( current );

		QString type;
		for ( qsizetype i = 0; i < segments.size(); ++i )
		{
			const QRegularExpressionMatch match = segmentPattern.match( segments.at( i ).trimmed() );
			if ( not match.hasMatch() )
			{
				return {};
			}

			const QString name = match.captured( 1 );
			if ( i == 0 )
			{
				type = _table.contains( name ) ? name : p_variables.value( name );
			}
			else
			{
				type = _table.value( type ).value( name );
			}

			if ( type.startsWith( "Optional[" ) && type.endsWith( ']' ) )
			{
				type = type.mid( 9, type.size() - 10 );
			}

			int depthSuffix = 0;
			for ( const QChar character : match.captured( 2 ) )
			{
				if ( depthSuffix == 0 && character == '[' )
				{
					type = _elementType( type );
				}

				if ( character == '(' || character == '[' )
				{
					++depthSuffix;
				}
				else if ( character == ')' || character == ']' )
				{
					--depthSuffix;
				}
				else if ( depthSuffix == 0 && not character.isSpace() )
				{
					return {};
				}
			}

			if ( type.isEmpty() )
			{
				return {};
			}
		}

		return type;
	}

	void PythonCompleter::_onPythonInitialized( const App::Events::PythonInitialized & )
	{
#if VTX_PYTHON_BINDING
		if ( not App::ECS::hasCtx<App::PythonBinding::Interpretor>() )
		{
			return;
		}

		App::INTERPRETOR().requestCompletionTable(
			[ completer		 = QPointer<PythonCompleter>( this ),
			  &threadManager = App::THREAD() ]( std::vector<App::PythonBinding::Interpretor::CompletionEntry> entries )
			{
				threadManager.dispatch(
					[ completer, entries = std::move( entries ) ]()
					{
						if ( not completer )
						{
							return;
						}

						CompletionTable table;
						for ( const App::PythonBinding::Interpretor::CompletionEntry & entry : entries )
						{
							table[ QString::fromStdString( entry.scope ) ].insert(
								QString::fromStdString( entry.name ), QString::fromStdString( entry.type )
							);
						}

						completer->_table = std::move( table );
					}
				);
			}
		);
#endif
	}
} // namespace VTX::UI::QT::Widget
