#ifndef __VTX_UI_QT_DIALOG_UPDATER__
#define __VTX_UI_QT_DIALOG_UPDATER__

#include "ui/qt/dialog/base_dialog.hpp"
#include <QDialogButtonBox>
#include <QPointer>
#include <QProgressBar>
#include <QPushButton>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::Dialog
{
	/**
	 * @brief Dialog to present an update + download progress.
	 */
	class Updater : public BaseDialog<Updater>
	{
	  public:
		Updater( const App::Events::UpdateAvailable & );

	  private:
		void _onUpdateDownloadProgress( const App::Events::UpdateDownloadProgress & );
		void _onUpdateReadyToRestart( const App::Events::UpdateReadyToRestart & );
		void _onUpdateDownloadFailed( const App::Events::UpdateDownloadFailed & );

		void _setDownloadingState();
		void _setReadyToRestartState();
		void _setIdleState();

		QPointer<QDialogButtonBox> _buttonBox;
		QPointer<QPushButton>	   _restartButton;
		QPointer<QProgressBar>	   _progressBar;

		VTX::Util::EventHub::ScopedConnection _downloadProgressConnection;
		VTX::Util::EventHub::ScopedConnection _readyToRestartConnection;
		VTX::Util::EventHub::ScopedConnection _downloadFailedConnection;
	};

} // namespace VTX::UI::QT::Dialog

#endif
