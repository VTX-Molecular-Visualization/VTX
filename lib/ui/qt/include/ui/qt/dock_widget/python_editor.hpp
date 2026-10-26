#ifndef __VTX_UI_QT_DOCK_WIDGET_PYTHON_EDITOR__
#define __VTX_UI_QT_DOCK_WIDGET_PYTHON_EDITOR__

#include "ui/qt/dock_widget/base_dock_widget.hpp"
#include "ui/qt/events.hpp"
#include <QAction>
#include <QPlainTextEdit>
#include <QPointer>
#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::DockWidget
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

	/**
	 * @brief Python script editor.
	 */
	class PythonEditor : public BaseDockWidget<PythonEditor, true, true>
	{
	  public:
		PythonEditor( QWidget * p_parent );

	  private:
		/**
		 * @brief Editor widget.
		 */
		QPointer<QPlainTextEdit> _textEdit;

		/**
		 * @brief Syntax highlighter.
		 */
		QPointer<PythonHighlighter> _highlighter;

		/**
		 * @brief Run action.
		 */
		QPointer<QAction> _runAction;

		/**
		 * @brief Path of the currently edited script.
		 */
		FilePath _currentPath;

		void _onScriptSelected( const Events::ScriptSelected & p_event );
		void _saveAndRun();

		VTX::Util::EventHub::ScopedConnection _scriptSelectedConnection;
	};
} // namespace VTX::UI::QT::DockWidget

#endif
