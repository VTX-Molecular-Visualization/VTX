#ifndef __VTX_UI_WIDGET_INFORMATION__
#define __VTX_UI_WIDGET_INFORMATION__

#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/custom_widget/dock_window_main_widget.hpp"
#include <QDialog>
#include <QImageReader>
#include <QMovie>
#include <QPushButton>
#include <QShowEvent>
#include <QString>
#include <QWidget>

namespace VTX::UI::Widget::Information
{
	class InformationWidget : public BaseManualWidget<QDialog>
	{
		VTX_WIDGET

	  public:
		~InformationWidget();
		void localize() override;

	  protected:
		InformationWidget( QWidget * );

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void showEvent( QShowEvent * p_event ) override;

	  private:
		QPushButton * _websiteButton   = nullptr;
		QPushButton * _gitButton	   = nullptr;
		QPushButton * _docButton	   = nullptr;
		QPushButton * _bugReportButton = nullptr;

		QString _getVersionText() const;
		QString _getLicenseText() const;

		QMovie * _movie = nullptr;

		void _displayMovieError( QImageReader::ImageReaderError p_error );
		void _onFrameChange( const int p_frame );

		void _goToWebsite();
		void _goToGit();
		void _goToDocumentation();
		void _goToBugReport();
	};
} // namespace VTX::UI::Widget::Information
#endif
