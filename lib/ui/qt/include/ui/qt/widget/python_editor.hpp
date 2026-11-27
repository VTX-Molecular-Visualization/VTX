#pragma once

#include "ui/qt/events.hpp"
#include "ui/qt/selection_manager.hpp"
#include "ui/qt/widget/base_widget.hpp"
#include <QAction>
#include <QEvent>
#include <QFileSystemWatcher>
#include <QItemSelection>
#include <QKeyEvent>
#include <QLabel>
#include <QPaintEvent>
#include <QPointer>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
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

		/**
		 * @brief Reset default width.
		 */
		void resetLayout();

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
		 * @brief Save action.
		 */
		QPointer<QAction> _saveAction;

		/**
		 * @brief Save button, hidden for read-only scripts.
		 */
		QPointer<QPushButton> _saveButton;

		/**
		 * @brief Opacity slider.
		 */
		QPointer<QSlider> _opacitySlider;

		/**
		 * @brief Button to duplicate a read-only script.
		 */
		QPointer<QPushButton> _duplicateButton;

		/**
		 * @brief Status displaying the script path and its modified state.
		 */
		QPointer<QLabel> _pathLabel;

		/**
		 * @brief Watch the current script to reload it when modified outside VTX.
		 */
		QPointer<QFileSystemWatcher> _fileWatcher;
		QPointer<QTimer>			 _reloadTimer;

		/**
		 * @brief Path of the currently edited script.
		 */
		FilePath _currentPath;

		/**
		 * @brief Read-only for sample scripts.
		 */
		bool _currentReadOnly = false;

		/**
		 * @brief Width ratio (%).
		 */
		float _widthRatio = 0.f;

		/**
		 * @brief Update the geometry of the widget based on its parent.
		 */
		void _updateGeometry();

		/**
		 * @brief Set the width of the editor based on a ratio of the parent widget's width.
		 */
		void _setWidth( int );

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
		 * @brief Handle script save events to write the currently edited script.
		 */
		void _onScriptSave( const Events::ScriptSave & );

		/**
		 * @brief Write the currently edited script to disk.
		 */
		bool _save();

		/**
		 * @brief Reload the current script from disk.
		 */
		void _reload();

		/**
		 * @brief Watch only the given path.
		 */
		void _watch( const FilePath & );

		/**
		 * @brief Update the status label.
		 */
		void _updateStatus();

		/**
		 * @brief Connections.
		 */
		VTX::Util::EventHub::ScopedConnection _scriptRunConnection;
		VTX::Util::EventHub::ScopedConnection _scriptSaveConnection;
	};
} // namespace VTX::UI::QT::Widget
