#include "ui/qt/widget/python_editor.hpp"
#include "ui/qt/action_registry.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/settings.hpp"
#include "ui/qt/widget/python_completer.hpp"
#include <QAbstractItemView>
#include <QBoxLayout>
#include <QFile>
#include <QFileSystemModel>
#include <QPainter>
#include <QPalette>
#include <QPlainTextEdit>
#include <QRegularExpression>
#include <QSaveFile>
#include <QSizePolicy>
#include <QStringList>
#include <QStyle>
#include <QSyntaxHighlighter>
#include <QTextBlock>
#include <QTextCursor>
#include <QToolBar>
#include <app/action/io.hpp>
#include <app/services.hpp>
#include <util/action_manager.hpp>
#include <util/event_hub.hpp>
#include <util/logger.hpp>
#include <utility>
#include <vector>

namespace
{
	constexpr int DEFAULT_OPACITY	   = 50;
	constexpr int OPACITY_SLIDER_WIDTH = 100;
} // namespace

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief Basic syntax highlighter for Python code.
	 */
	class PythonHighlighter : public QSyntaxHighlighter
	{
	  public:
		PythonHighlighter( QTextDocument * doc ) : QSyntaxHighlighter( doc )
		{
			QTextCharFormat keyword;
			keyword.setForeground( QColor( "#569CD6" ) );
			keyword.setFontWeight( QFont::Bold );

			const QStringList keywords
				= { "def",	  "class", "import",  "from",	"if",	"elif", "else", "for",	 "and",	 "or", "while",
					"return", "try",   "require", "except", "with", "as",	"True", "False", "None", "vtx" };

			for ( const auto & k : keywords )
			{
				_rules.emplace_back( "\\b" + k + "\\b", keyword );
			}

			QTextCharFormat comment;
			comment.setForeground( QColor( "#6A9955" ) );
			_rules.emplace_back( "#.*", comment );

			QTextCharFormat string;
			string.setForeground( QColor( "#CE9178" ) );
			_rules.emplace_back( "\".*\"", string );
			_rules.emplace_back( "'.*'", string );
		}

	  protected:
		void highlightBlock( const QString & text ) override
		{
			for ( const auto & [ regex, format ] : _rules )
			{
				auto matches = regex.globalMatch( text );

				while ( matches.hasNext() )
				{
					auto match = matches.next();
					setFormat( match.capturedStart(), match.capturedLength(), format );
				}
			}
		}

	  private:
		std::vector<std::pair<QRegularExpression, QTextCharFormat>> _rules;
	};

	class PythonTextEdit : public QPlainTextEdit
	{
	  public:
		explicit PythonTextEdit( QWidget * p_parent );

	  protected:
		void keyPressEvent( QKeyEvent * ) override;

	  private:
		QPointer<PythonCompleter> _completer;

		void _insertCompletion( const QString & );
	};

	PythonTextEdit::PythonTextEdit( QWidget * const p_parent ) :
		QPlainTextEdit( p_parent ), _completer( Helper::makeQObject<PythonCompleter>( this ) )
	{
		connect(
			_completer, qOverload<const QString &>( &QCompleter::activated ), this, &PythonTextEdit::_insertCompletion
		);
	}

	void PythonTextEdit::keyPressEvent( QKeyEvent * const p_event )
	{
		if ( _completer->popup()->isVisible() )
		{
			switch ( p_event->key() )
			{
			case Qt::Key_Enter:
			case Qt::Key_Return:
			case Qt::Key_Escape:
			case Qt::Key_Tab:
			case Qt::Key_Backtab: p_event->ignore(); return;
			default: break;
			}
		}

		QPlainTextEdit::keyPressEvent( p_event );

		if ( p_event->text().isEmpty() )
		{
			_completer->popup()->hide();
			return;
		}

		const QTextCursor cursor = textCursor();
		QStringList		  previousLines;
		for ( QTextBlock block = document()->begin(); block.isValid() && block != cursor.block(); block = block.next() )
		{
			previousLines.append( block.text() );
		}

		const QString lineBeforeCursor = cursor.block().text().left( cursor.positionInBlock() );
		if ( not _completer->updateCompletions( lineBeforeCursor, _completer->inferVariables( previousLines ) ) )
		{
			_completer->popup()->hide();
			return;
		}

		_completer->showPopup( cursorRect() );
	}

	void PythonTextEdit::_insertCompletion( const QString & p_completion )
	{
		QTextCursor cursor = textCursor();
		cursor.movePosition(
			QTextCursor::Left, QTextCursor::KeepAnchor, static_cast<int>( _completer->completionPrefix().size() )
		);
		cursor.insertText( p_completion );
		setTextCursor( cursor );
	}

	PythonEditor::PythonEditor( QWidget * const p_parent ) :
		BaseWidget( p_parent ), _textEdit( Helper::makeQObject<PythonTextEdit>( this ) )
	{
		setAttribute( Qt::WA_NoMousePropagation );

		_runAction = UI_ACTIONS().getAction( Action::Script::RUN );
		_runAction->setEnabled( false );

		auto * const toolbar = Helper::makeQObject<QToolBar>( this );
		toolbar->setMovable( false );
		toolbar->setFloatable( false );
		toolbar->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
		toolbar->layout()->setContentsMargins( 0, 0, 0, 0 );

		auto * const spacer = Helper::makeQObject<QWidget>( toolbar );
		spacer->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
		toolbar->addWidget( spacer );

		_opacitySlider = Helper::makeQObject<QSlider>( Qt::Horizontal, toolbar );
		_opacitySlider->setRange( 0, 100 );
		_opacitySlider->setFixedWidth( OPACITY_SLIDER_WIDTH );
		_opacitySlider->setToolTip( "Opacity" );
		connect(
			_opacitySlider,
			&QSlider::valueChanged,
			this,
			[ this ]( const int p_value )
			{
				SETTINGS().setValue( SETTING_KEY_PYTHON_EDITOR_OPACITY, p_value );
				update();
			}
		);
		_opacitySlider->setValue( SETTINGS().value( SETTING_KEY_PYTHON_EDITOR_OPACITY, DEFAULT_OPACITY ).toInt() );
		toolbar->addWidget( _opacitySlider );

		QAction * const closeAction
			= toolbar->addAction( style()->standardIcon( QStyle::SP_TitleBarCloseButton ), "Close" );
		connect( closeAction, &QAction::triggered, this, []() { SELECTION().clear( E_SELECTION_GROUP::SCRIPT ); } );

		_textEdit->setFrameShape( QFrame::NoFrame );
		QPalette textPalette = _textEdit->palette();
		textPalette.setColor( QPalette::Base, Qt::transparent );
		_textEdit->setPalette( textPalette );
		_highlighter = Helper::makeQObject<PythonHighlighter>( _textEdit->document() );

		auto * const layout = Helper::makeQObject<QVBoxLayout>( this );
		layout->setContentsMargins( 0, 0, 0, 0 );
		layout->setSpacing( 0 );
		layout->addWidget( toolbar );
		layout->addWidget( _textEdit );

		if ( p_parent != nullptr )
		{
			p_parent->installEventFilter( this );
		}
		_updateGeometry();

		connect( &SELECTION(), &SelectionManager::selectionChanged, this, &PythonEditor::_onSelectionChanged );
		_scriptRunConnection = App::HUB().connect<Events::ScriptRun, &PythonEditor::_onScriptRun>( this );
	}

	PythonEditor::~PythonEditor() = default;

	bool PythonEditor::event( QEvent * const p_event )
	{
		if ( p_event->type() == QEvent::Show )
		{
			_updateGeometry();
			raise();
		}

		return QWidget::event( p_event );
	}

	bool PythonEditor::eventFilter( QObject * const p_watched, QEvent * const p_event )
	{
		if ( p_watched == parentWidget() && p_event->type() == QEvent::Resize )
		{
			_updateGeometry();
		}

		return QWidget::eventFilter( p_watched, p_event );
	}

	void PythonEditor::paintEvent( QPaintEvent * const )
	{
		QColor background = palette().color( QPalette::Window );
		background.setAlphaF( static_cast<float>( _opacitySlider->value() ) / 100.f );

		QPainter painter( this );
		painter.fillRect( rect(), background );
	}

	void PythonEditor::keyPressEvent( QKeyEvent * const p_event ) { p_event->accept(); }

	void PythonEditor::keyReleaseEvent( QKeyEvent * const p_event ) { p_event->accept(); }

	void PythonEditor::_updateGeometry()
	{
		const QWidget * const parent = parentWidget();
		if ( parent == nullptr )
		{
			return;
		}

		setGeometry( parent->rect() );
	}

	void PythonEditor::_onSelectionChanged( const E_SELECTION_GROUP p_group, const QItemSelection & p_selection )
	{
		if ( p_group != E_SELECTION_GROUP::SCRIPT )
		{
			return;
		}

		const QModelIndexList indexes = p_selection.indexes();
		if ( indexes.isEmpty() )
		{
			_close();
			return;
		}

		_open( FilePath( indexes.first().data( QFileSystemModel::FilePathRole ).toString().toStdString() ) );
	}

	void PythonEditor::_open( const FilePath & p_path )
	{
		show();

		QFile file( QString::fromStdString( p_path.string() ) );
		if ( not file.open( QIODevice::ReadOnly | QIODevice::Text ) )
		{
			_currentPath.clear();
			_runAction->setEnabled( false );
			_textEdit->clear();
			return;
		}

		_currentPath = p_path;
		_runAction->setEnabled( true );
		_textEdit->setPlainText( QString::fromUtf8( file.readAll() ) );
	}

	void PythonEditor::_close()
	{
		hide();
		_currentPath.clear();
		_runAction->setEnabled( false );
		_textEdit->clear();
	}

	void PythonEditor::_onScriptRun( const Events::ScriptRun & )
	{
		if ( _currentPath.empty() )
		{
			return;
		}

		QSaveFile file( QString::fromStdString( _currentPath.string() ) );
		if ( not file.open( QIODevice::WriteOnly ) )
		{
			VTX_WARNING( "Unable to open script <{}> for writing.", _currentPath.string() );
			return;
		}

		const QByteArray content = _textEdit->toPlainText().toUtf8();
		if ( file.write( content ) != content.size() || not file.commit() )
		{
			VTX_WARNING( "Unable to save script <{}>.", _currentPath.string() );
			return;
		}

		App::ACTION().execute<App::Action::IO::RunPythonScript>( _currentPath );
	}
} // namespace VTX::UI::QT::Widget
