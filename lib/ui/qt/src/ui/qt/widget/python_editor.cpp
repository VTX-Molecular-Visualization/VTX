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
#include <QMouseEvent>
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
#include <QToolButton>
#include <algorithm>
#include <app/action/io.hpp>
#include <app/services.hpp>
#include <app/session.hpp>
#include <cmath>
#include <filesystem>
#include <functional>
#include <util/action_manager.hpp>
#include <util/event_hub.hpp>
#include <util/logger.hpp>
#include <utility>
#include <vector>

namespace
{
	constexpr int	DEFAULT_OPACITY		 = 50;
	constexpr int	OPACITY_SLIDER_WIDTH = 100;
	constexpr float DEFAULT_WIDTH_RATIO	 = 0.5f;
	constexpr int	MIN_WIDTH			 = 240;
	constexpr int	RESIZE_HANDLE_WIDTH	 = 6;
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

		if ( isReadOnly() || p_event->text().isEmpty() )
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
		if ( isReadOnly() )
		{
			return;
		}

		QTextCursor cursor = textCursor();
		cursor.movePosition(
			QTextCursor::Left, QTextCursor::KeepAnchor, static_cast<int>( _completer->completionPrefix().size() )
		);
		cursor.insertText( p_completion );
		setTextCursor( cursor );
	}

	class PythonEditorResizeHandle : public QWidget
	{
	  public:
		PythonEditorResizeHandle( QWidget * const p_parent, std::function<void( const QPoint & )> p_onDrag ) :
			QWidget( p_parent ), _onDrag( std::move( p_onDrag ) )
		{
			setCursor( Qt::SizeHorCursor );
			setFixedWidth( RESIZE_HANDLE_WIDTH );
		}

	  protected:
		void mousePressEvent( QMouseEvent * const p_event ) override { p_event->accept(); }

		void mouseMoveEvent( QMouseEvent * const p_event ) override
		{
			if ( p_event->buttons() & Qt::LeftButton )
			{
				_onDrag( p_event->globalPosition().toPoint() );
			}
			p_event->accept();
		}

	  private:
		std::function<void( const QPoint & )> _onDrag;
	};

	PythonEditor::PythonEditor( QWidget * const p_parent ) :
		BaseWidget( p_parent ), _textEdit( Helper::makeQObject<PythonTextEdit>( this ) ),
		_runAction( UI_ACTIONS().getAction( Action::Script::RUN ) )
	{
		setAttribute( Qt::WA_NoMousePropagation );

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
		if ( QToolButton * const closeButton = qobject_cast<QToolButton *>( toolbar->widgetForAction( closeAction ) ) )
		{
			closeButton->setToolButtonStyle( Qt::ToolButtonIconOnly );
		}

		_textEdit->setFrameShape( QFrame::NoFrame );
		QPalette textPalette = _textEdit->palette();
		textPalette.setColor( QPalette::Base, Qt::transparent );
		_textEdit->setPalette( textPalette );
		_highlighter = Helper::makeQObject<PythonHighlighter>( _textEdit->document() );

		auto * const content	   = Helper::makeQObject<QWidget>( this );
		auto * const contentLayout = Helper::makeQObject<QVBoxLayout>( content );
		contentLayout->setContentsMargins( 0, 0, 0, 0 );
		contentLayout->setSpacing( 0 );
		contentLayout->addWidget( toolbar );
		contentLayout->addWidget( _textEdit );

		auto * const resizeHandle = Helper::makeQObject<PythonEditorResizeHandle>(
			this, [ this ]( const QPoint & p_globalPosition ) { _setWidth( mapFromGlobal( p_globalPosition ).x() ); }
		);

		auto * const layout = Helper::makeQObject<QHBoxLayout>( this );
		layout->setContentsMargins( 0, 0, 0, 0 );
		layout->setSpacing( 0 );
		layout->addWidget( content );
		layout->addWidget( resizeHandle );

		_widthRatio = SETTINGS().value( SETTING_KEY_PYTHON_EDITOR_WIDTH, DEFAULT_WIDTH_RATIO ).toFloat();
		if ( _widthRatio <= 0.f || _widthRatio > 1.f )
		{
			_widthRatio = DEFAULT_WIDTH_RATIO;
		}

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

		const int parentWidth = parent->width();
		const int width		  = std::clamp(
			static_cast<int>( std::lround( _widthRatio * static_cast<float>( parentWidth ) ) ),
			std::min( MIN_WIDTH, parentWidth ),
			parentWidth
		);
		setGeometry( 0, 0, width, parent->height() );
	}

	void PythonEditor::resetLayout()
	{
		_widthRatio = DEFAULT_WIDTH_RATIO;
		SETTINGS().setValue( SETTING_KEY_PYTHON_EDITOR_WIDTH, _widthRatio );
		_updateGeometry();
	}

	void PythonEditor::_setWidth( const int p_width )
	{
		const QWidget * const parent = parentWidget();
		if ( parent == nullptr || parent->width() <= 0 )
		{
			return;
		}

		_widthRatio = std::clamp( static_cast<float>( p_width ) / static_cast<float>( parent->width() ), 0.f, 1.f );
		SETTINGS().setValue( SETTING_KEY_PYTHON_EDITOR_WIDTH, _widthRatio );
		_updateGeometry();
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

		_currentPath	 = p_path;
		_currentReadOnly = _isReadOnly( p_path );
		_runAction->setEnabled( true );
		_textEdit->setPlainText( QString::fromUtf8( file.readAll() ) );
		_textEdit->setReadOnly( _currentReadOnly );
	}

	bool PythonEditor::_isReadOnly( const FilePath & p_path )
	{
		const FilePath examples = std::filesystem::weakly_canonical( App::SESSION().getDefaultScriptsDir() );
		const FilePath path		= std::filesystem::weakly_canonical( p_path );
		const std::pair<FilePath::const_iterator, FilePath::const_iterator> mismatch
			= std::mismatch( examples.begin(), examples.end(), path.begin(), path.end() );
		return mismatch.first == examples.end();
	}

	void PythonEditor::_close()
	{
		hide();
		_currentPath.clear();
		_currentReadOnly = false;
		_runAction->setEnabled( false );
		_textEdit->clear();
	}

	void PythonEditor::_onScriptRun( const Events::ScriptRun & )
	{
		if ( _currentPath.empty() )
		{
			return;
		}

		if ( _currentReadOnly )
		{
			App::ACTION().execute<App::Action::IO::RunPythonScript>( _currentPath );
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
