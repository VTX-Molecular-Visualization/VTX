#pragma once

#include <QCompleter>
#include <QHash>
#include <QPointer>
#include <QRect>
#include <QString>
#include <QStringList>
#include <QStringListModel>
#include <QWidget>
#include <app/events.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief A completer for Python code.
	 */
	class PythonCompleter : public QCompleter
	{
	  public:
		/**
		 * @brief Variables type.
		 */
		using Variables = QHash<QString, QString>;

		/**
		 * @brief Constructor.
		 */
		PythonCompleter( QWidget * p_widget );

		/**
		 * @brief Update the completion candidates based on the current line and the variables in scope.
		 */
		bool updateCompletions( const QString & p_lineBeforeCursor, const Variables & p_variables );

		/**
		 * @brief Show the completion popup at the given cursor rect.
		 */
		void showPopup( const QRect & p_cursorRect );

		/**
		 * @brief Infer the variables in scope based on the given lines of code.
		 */
		Variables inferVariables( const QStringList & p_lines ) const;

	  private:
		/**
		 * @brief Completion table.
		 */
		using CompletionTable = QHash<QString, QHash<QString, QString>>;

		/**
		 * @brief Model used to display the completion candidates.
		 */
		QPointer<QStringListModel> _model;

		/**
		 * @brief Completion table from Python module.
		 */
		CompletionTable _table;

		/**
		 * @brief Resolve the type of an expression based on the variables in scope.
		 */
		QString _resolveType( const QString &, const Variables & ) const;

		/**
		 * @brief Get the element type of a container type.
		 */
		void _onPythonInitialized( const App::Events::PythonInitialized & );

		/**
		 * @brief Connections.
		 */
		VTX::Util::EventHub::ScopedConnection _pythonInitializedConnection;
	};
} // namespace VTX::UI::QT::Widget
