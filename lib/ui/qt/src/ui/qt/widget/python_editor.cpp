#include "ui/qt/widget/python_editor.hpp"
#include "ui/qt/action_registry.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/settings.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include "ui/qt/widget/python_completer.hpp"
#include "ui/qt/widget/tree/scripts.hpp"
#include <QAbstractItemView>
#include <QBoxLayout>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPalette>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QSaveFile>
#include <QScrollBar>
#include <QSizePolicy>
#include <QStringList>
#include <QStyle>
#include <QSyntaxHighlighter>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QToolBar>
#include <QToolButton>
#include <algorithm>
#include <app/action/io.hpp>
#include <app/helper/script.hpp>
#include <app/services.hpp>
#include <cmath>
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
	constexpr int	DUPLICATE_BUTTON_HEIGHT = 40;
	constexpr int	RELOAD_DELAY			= 200;
	constexpr int	STATUS_LABEL_MARGIN		= 6;
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
		bool event( QEvent * ) override;
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

	bool PythonTextEdit::event( QEvent * const p_event )
	{
		if ( p_event->type() == QEvent::ShortcutOverride
			 && static_cast<QKeyEvent *>( p_event )->matches( QKeySequence::Save ) )
		{
			p_event->accept();
			return true;
		}

		return QPlainTextEdit::event( p_event );
	}

	void PythonTextEdit::keyPressEvent( QKeyEvent * const p_event )
	{
		if ( p_event->matches( QKeySequence::Save ) )
		{
			App::HUB().trigger<Events::ScriptSave>();
			p_event->accept();
			return;
		}

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
		_runAction( UI_ACTIONS().getAction( Action::Script::RUN ) ),
		_saveAction( UI_ACTIONS().getAction( Action::Script::SAVE ) )
	{
		setAttribute( Qt::WA_NoMousePropagation );

		_runAction->setEnabled( false );
		_saveAction->setEnabled( false );

		auto * const toolbar = Helper::makeQObject<QToolBar>( this );
		toolbar->setMovable( false );
		toolbar->setFloatable( false );
		toolbar->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
		toolbar->layout()->setContentsMargins( 0, 0, 0, 0 );

		auto * const status		  = Helper::makeQObject<QWidget>( this );
		auto * const statusLayout = Helper::makeQObject<QVBoxLayout>( status );
		statusLayout->setContentsMargins( STATUS_LABEL_MARGIN, 0, 0, 0 );
		statusLayout->setSpacing( 0 );
		_pathLabel = Helper::makeQObject<QLabel>( status );
		_pathLabel->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Preferred );
		_modifiedLabel = Helper::makeQObject<QLabel>( "(modified)", status );
		QSizePolicy modifiedPolicy = _modifiedLabel->sizePolicy();
		modifiedPolicy.setRetainSizeWhenHidden( true );
		_modifiedLabel->setSizePolicy( modifiedPolicy );
		_modifiedLabel->setVisible( false );
		statusLayout->addWidget( _pathLabel );
		statusLayout->addWidget( _modifiedLabel );

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
		QFont font( Style::DEFAULT_FONT_FAMILY_SEQUENCE.toString(), Style::DEFAULT_FONT_SIZE );
		font.setStyleHint( QFont::Monospace );
		_textEdit->setFont( font );
		QPalette textPalette = _textEdit->palette();
		textPalette.setColor( QPalette::Base, Qt::transparent );
		_textEdit->setPalette( textPalette );
		_highlighter = Helper::makeQObject<PythonHighlighter>( _textEdit->document() );
		connect(
			_textEdit->document(),
			&QTextDocument::modificationChanged,
			this,
			[ this ]( const bool ) { _updateStatus(); }
		);

		_duplicateButton = Helper::makeQObject<QPushButton>( "DEMO SCRIPT - COPY TO EDIT", this );
		_duplicateButton->setIcon( STYLE().iconFromCodepoint( Style::Icons::COPY ) );
		_duplicateButton->setToolTip( "Copy this script into your scripts" );
		_duplicateButton->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Fixed );
		_duplicateButton->setMinimumHeight( DUPLICATE_BUTTON_HEIGHT );
		QFont duplicateFont = _duplicateButton->font();
		duplicateFont.setBold( true );
		_duplicateButton->setFont( duplicateFont );
		_duplicateButton->setVisible( false );
		connect( _duplicateButton, &QPushButton::clicked, this, [ this ]() { Helper::duplicateScript( _currentPath ); } );

		auto * const actions	   = Helper::makeQObject<QWidget>( this );
		auto * const actionsLayout = Helper::makeQObject<QHBoxLayout>( actions );
		actionsLayout->setContentsMargins( 0, 0, 0, 0 );
		actionsLayout->addStretch();
		_saveButton = Helper::makeActionButton( *_saveAction, actions );
		actionsLayout->addWidget( _saveButton );
		QPushButton * const runButton = Helper::makeActionButton( *_runAction, actions );
		actionsLayout->addWidget( runButton );

		auto * const header		  = Helper::makeQObject<QWidget>( this );
		auto * const headerLayout = Helper::makeQObject<QHBoxLayout>( header );
		headerLayout->setContentsMargins( 0, 0, 0, 0 );
		headerLayout->setSpacing( 0 );
		headerLayout->addWidget( status, 1 );
		headerLayout->addWidget( toolbar );

		auto * const content	   = Helper::makeQObject<QWidget>( this );
		auto * const contentLayout = Helper::makeQObject<QVBoxLayout>( content );
		contentLayout->setContentsMargins( 0, 0, 0, 0 );
		contentLayout->setSpacing( 0 );
		contentLayout->addWidget( _duplicateButton );
		contentLayout->addWidget( header );
		contentLayout->addWidget( _textEdit );
		contentLayout->addWidget( actions );

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

		_fileWatcher = Helper::makeQObject<QFileSystemWatcher>( this );
		_reloadTimer = Helper::makeQObject<QTimer>( this );
		_reloadTimer->setSingleShot( true );
		_reloadTimer->setInterval( RELOAD_DELAY );
		connect( _fileWatcher, &QFileSystemWatcher::fileChanged, _reloadTimer, qOverload<>( &QTimer::start ) );
		connect( _reloadTimer, &QTimer::timeout, this, &PythonEditor::_reload );

		connect( &SELECTION(), &SelectionManager::selectionChanged, this, &PythonEditor::_onSelectionChanged );
		_scriptRunConnection  = App::HUB().connect<Events::ScriptRun, &PythonEditor::_onScriptRun>( this );
		_scriptSaveConnection = App::HUB().connect<Events::ScriptSave, &PythonEditor::_onScriptSave>( this );
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

		_open( FilePath( indexes.first().data( Tree::Scripts::PATH_ROLE ).toString().toStdString() ) );
	}

	void PythonEditor::_open( const FilePath & p_path )
	{
		show();

		QFile file( QString::fromStdString( p_path.string() ) );
		if ( not file.open( QIODevice::ReadOnly | QIODevice::Text ) )
		{
			_currentPath.clear();
			_watch( {} );
			_currentReadOnly = false;
			_duplicateButton->setVisible( false );
			_runAction->setEnabled( false );
			_saveAction->setEnabled( false );
			_textEdit->clear();
			_textEdit->document()->setModified( false );
			_updateStatus();
			return;
		}

		_currentPath	 = p_path;
		_watch( p_path );
		_currentReadOnly = App::Helper::Script::isReadOnly( p_path );
		_duplicateButton->setVisible( _currentReadOnly );
		_runAction->setEnabled( true );
		_saveAction->setEnabled( not _currentReadOnly );
		_saveButton->setVisible( not _currentReadOnly );
		_textEdit->setPlainText( QString::fromUtf8( file.readAll() ) );
		_textEdit->document()->setModified( false );
		_textEdit->setReadOnly( _currentReadOnly );
		_updateStatus();
	}

	void PythonEditor::_close()
	{
		hide();
		_currentPath.clear();
		_watch( {} );
		_currentReadOnly = false;
		_duplicateButton->setVisible( false );
		_runAction->setEnabled( false );
		_saveAction->setEnabled( false );
		_textEdit->clear();
		_textEdit->document()->setModified( false );
		_updateStatus();
	}

	void PythonEditor::_onScriptRun( const Events::ScriptRun & )
	{
		if ( _currentPath.empty() )
		{
			return;
		}

		if ( not _currentReadOnly && not _save() )
		{
			return;
		}

		App::ACTION().execute<App::Action::IO::RunPythonScript>( _currentPath );
	}

	void PythonEditor::_onScriptSave( const Events::ScriptSave & )
	{
		if ( _currentPath.empty() || _currentReadOnly )
		{
			return;
		}

		_save();
	}

	void PythonEditor::_watch( const FilePath & p_path )
	{
		_reloadTimer->stop();
		if ( not _fileWatcher->files().isEmpty() )
		{
			_fileWatcher->removePaths( _fileWatcher->files() );
		}
		if ( not p_path.empty() )
		{
			_fileWatcher->addPath( QString::fromStdString( p_path.string() ) );
		}
	}

	void PythonEditor::_updateStatus()
	{
		if ( _currentPath.empty() )
		{
			_pathLabel->clear();
			_pathLabel->setToolTip( {} );
			_modifiedLabel->setVisible( false );
			return;
		}

		const QString path = QDir::toNativeSeparators(
			QFileInfo( QString::fromStdString( _currentPath.string() ) ).absoluteFilePath()
		);
		_pathLabel->setText( path );
		_pathLabel->setToolTip( path );
		_modifiedLabel->setVisible( _textEdit->document()->isModified() );
	}

	void PythonEditor::_reload()
	{
		if ( _currentPath.empty() )
		{
			return;
		}

		const QString path = QString::fromStdString( _currentPath.string() );
		QFile		  file( path );
		if ( not file.open( QIODevice::ReadOnly | QIODevice::Text ) )
		{
			return;
		}

		if ( not _fileWatcher->files().contains( path ) )
		{
			_fileWatcher->addPath( path );
		}

		const QString content = QString::fromUtf8( file.readAll() );
		if ( content == _textEdit->toPlainText() )
		{
			_textEdit->document()->setModified( false );
			return;
		}

		const int position = _textEdit->textCursor().position();
		const int scroll   = _textEdit->verticalScrollBar()->value();

		_textEdit->setPlainText( content );
		_textEdit->document()->setModified( false );

		QTextCursor cursor = _textEdit->textCursor();
		cursor.setPosition( std::min( position, _textEdit->document()->characterCount() - 1 ) );
		_textEdit->setTextCursor( cursor );
		_textEdit->verticalScrollBar()->setValue( scroll );
	}

	bool PythonEditor::_save()
	{
		QSaveFile file( QString::fromStdString( _currentPath.string() ) );
		if ( not file.open( QIODevice::WriteOnly ) )
		{
			VTX_WARNING( "Unable to open script <{}> for writing.", _currentPath.string() );
			return false;
		}

		const QByteArray content = _textEdit->toPlainText().toUtf8();
		if ( file.write( content ) != content.size() || not file.commit() )
		{
			VTX_WARNING( "Unable to save script <{}>.", _currentPath.string() );
			return false;
		}

		_textEdit->document()->setModified( false );
		return true;
	}
} // namespace VTX::UI::QT::Widget
