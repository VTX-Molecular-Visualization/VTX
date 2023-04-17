#ifndef __VTX_UI_QT_TOOL_UI_FEATURES_WIDGET_INFORMATION__
#define __VTX_UI_QT_TOOL_UI_FEATURES_WIDGET_INFORMATION__

#include "ui/qt/qt_panel.hpp"
#include "ui/qt/widget/base_manual_widget.hpp"
#include <QImageReader>
#include <QMovie>
#include <QPushButton>
#include <QShowEvent>
#include <QString>
#include <QWidget>

namespace VTX::UI::QT::Tool::UIFeatures::Widget
{
	class InformationWidget : public QT::QtFloatingWindowPanel
	{
		NEW_ARCHI_VTX_WIDGET

	  public:
		~InformationWidget();

	  protected:
		InformationWidget( QWidget * );

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void showEvent( QShowEvent * p_event ) override;

	  private:
		QPushButton * _websiteButton	 = nullptr;
		QPushButton * _gitButton		 = nullptr;
		QPushButton * _docButton		 = nullptr;
		QPushButton * _bugReportButton	 = nullptr;
		QPushButton * _checkUpdateButton = nullptr;

		QString _getVersionText() const;
		void	_getLicenseText( QString & p_txt ) const;

		QMovie * _movie = nullptr;

		void _displayMovieError( QImageReader::ImageReaderError p_error ) const;
		void _onFrameChange( const int p_frame );

		void _goToWebsite() const;
		void _goToGit() const;
		void _goToDocumentation() const;
		void _goToBugReport() const;
		void _checkForUpdate() const;
	};
} // namespace VTX::UI::QT::Tool::UIFeatures::Widget
#endif
