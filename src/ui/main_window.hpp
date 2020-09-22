#ifndef __VTX_UI_MAIN_WINDOW__
#define __VTX_UI_MAIN_WINDOW__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui_MainWindow.h"
#include "vtx_app.hpp"
#include <QtWidgets/QMainWindow>

namespace VTX
{
	namespace UI
	{
		class MainWindow : public QMainWindow
		{
			Q_OBJECT

		  public:
			MainWindow( QObject * parent = 0 ) {
				ui.setupUi( this );
			}

		  signals:

		  public slots:
			void actionTest();

		  private:
			Ui_MainWindow ui;
		};

	} // namespace UI
} // namespace VTX

#endif
