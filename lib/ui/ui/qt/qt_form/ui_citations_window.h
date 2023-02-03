/********************************************************************************
** Form generated from reading UI file 'citations_window.ui'
**
** Created by: Qt User Interface Compiler version 5.15.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CITATIONS_WINDOW_H
#define UI_CITATIONS_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DialogCitations
{
  public:
	QVBoxLayout *  verticalLayout_2;
	QVBoxLayout *  verticalLayout;
	QLabel *	   labelCitations;
	QTextBrowser * textBrowserCitations;
	QPushButton *  pushButtonCopyCitations;

	void setupUi( QDialog * DialogCitations )
	{
		if ( DialogCitations->objectName().isEmpty() )
			DialogCitations->setObjectName( QString::fromUtf8( "DialogCitations" ) );
		DialogCitations->resize( 405, 300 );
		verticalLayout_2 = new QVBoxLayout( DialogCitations );
		verticalLayout_2->setObjectName( QString::fromUtf8( "verticalLayout_2" ) );
		verticalLayout = new QVBoxLayout();
		verticalLayout->setObjectName( QString::fromUtf8( "verticalLayout" ) );
		labelCitations = new QLabel( DialogCitations );
		labelCitations->setObjectName( QString::fromUtf8( "labelCitations" ) );

		verticalLayout->addWidget( labelCitations );

		textBrowserCitations = new QTextBrowser( DialogCitations );
		textBrowserCitations->setObjectName( QString::fromUtf8( "textBrowserCitations" ) );
		textBrowserCitations->setStyleSheet(
			QString::fromUtf8( "background-color: rgb(255, 255, 255);\n"
							   "color: rgb(0, 0, 0);\n"
							   "" ) );
		textBrowserCitations->setOpenExternalLinks( true );

		verticalLayout->addWidget( textBrowserCitations );

		pushButtonCopyCitations = new QPushButton( DialogCitations );
		pushButtonCopyCitations->setObjectName( QString::fromUtf8( "pushButtonCopyCitations" ) );

		verticalLayout->addWidget( pushButtonCopyCitations );

		verticalLayout_2->addLayout( verticalLayout );

		retranslateUi( DialogCitations );
		QObject::connect( pushButtonCopyCitations,
						  SIGNAL( clicked() ),
						  DialogCitations,
						  SLOT( onCopyCitationsToClipboardPressed() ) );

		QMetaObject::connectSlotsByName( DialogCitations );
	} // setupUi

	void retranslateUi( QDialog * DialogCitations )
	{
		DialogCitations->setWindowTitle( QCoreApplication::translate( "DialogCitations", "Citations", nullptr ) );
		labelCitations->setText( QCoreApplication::translate(
			"DialogCitations", "If you use this tool in your project please cite the following papers:", nullptr ) );
#if QT_CONFIG( tooltip )
		pushButtonCopyCitations->setToolTip(
			QCoreApplication::translate( "DialogCitations", "Copy citations to clipbord", nullptr ) );
#endif // QT_CONFIG(tooltip)
		pushButtonCopyCitations->setText( QCoreApplication::translate( "DialogCitations", "Copy", nullptr ) );
	} // retranslateUi
};

namespace Ui
{
	class DialogCitations : public Ui_DialogCitations
	{
	};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CITATIONS_WINDOW_H
