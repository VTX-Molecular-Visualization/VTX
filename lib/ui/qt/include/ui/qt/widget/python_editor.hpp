#pragma once

#include "ui/qt/events.hpp"
#include "ui/qt/selection_manager.hpp"
#include "ui/qt/widget/base_widget.hpp"
#include <QAction>
#include <QEvent>
#include <QItemSelection>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QPointer>
#include <QSlider>
#include <util/event_hub.hpp>
#include <util/filesystem.hpp>

namespace VTX::UI::QT::Widget
{
	class PythonHighlighter;
	class PythonTextEdit;

	/**
	 * @brief Python script editor widget.
	 */
	class PythonEditor : public BaseWidget<PythonEditor, QWidget>
	{
		Q_OBJECT

	  public:
		/**
		 * @brief Constructor.
		 */
		explicit PythonEditor( QWidget * p_parent );
		~PythonEditor() override;

		/**
		 * @brief Override events.
		 */
		bool eventFilter( QObject *, QEvent * ) override;

	  protected:
		/**
		 * @brief Override events.
		 */
		bool event( QEvent * ) override;
		void paintEvent( QPaintEvent * ) override;
		void keyPressEvent( QKeyEvent * ) override;
		void keyReleaseEvent( QKeyEvent * ) override;

	  private:
		/**
		 * @brief Editor widget.
		 */
		QPointer<PythonTextEdit> _textEdit;

		/**
		 * @brief Syntax highlighter.
		 */
		QPointer<PythonHighlighter> _highlighter;

		/**
		 * @brief Run action.
		 */
		QPointer<QAction> _runAction;

		/**
		 * @brief Opacity slider.
		 */
		QPointer<QSlider> _opacitySlider;

		/**
		 * @brief Path of the currently edited script.
		 */
		FilePath _currentPath;

		/**
		 * @brief Update the geometry of the widget based on its parent.
		 */
		void _updateGeometry();

		/**
		 * @brief Handle selection change events to open or close the editor based on the selected script.
		 */
		void _onSelectionChanged( E_SELECTION_GROUP, const QItemSelection & );

		/**
		 * @brief Open the editor with the script located at the given path.
		 */
		void _open( const FilePath & );

		/**
		 * @brief Close the editor.
		 */
		void _close();

		/**
		 * @brief Handle script run events to execute the currently edited script.
		 */
		void _onScriptRun( const Events::ScriptRun & );

		/**
		 * @brief Connections.
		 */
		VTX::Util::EventHub::ScopedConnection _scriptRunConnection;
	};
} // namespace VTX::UI::QT::Widget
