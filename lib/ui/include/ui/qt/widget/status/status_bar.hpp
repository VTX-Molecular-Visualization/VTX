#ifndef __VTX_UI_QT_STATUS_BAR__
#define __VTX_UI_QT_STATUS_BAR__

#include "ui/qt/widget/base_manual_widget.hpp"
#include <QLabel>
#include <QProgressBar>
#include <QStatusBar>
#include <QTimer>

namespace VTX::UI::QT::Widget::Status
{
	class StatusBar : public BaseManualWidget<QStatusBar>
	{
		NEW_ARCHI_VTX_WIDGET

	  public:
		void setProgress( const bool );
		void setText( const QString & );

		void localize() override;

	  protected:
		StatusBar( QWidget * p_parent );

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		QProgressBar * _progressBar = nullptr;
		QLabel *	   _infoLabel	= nullptr;
	};

} // namespace VTX::UI::QT::Widget::Status
#endif
