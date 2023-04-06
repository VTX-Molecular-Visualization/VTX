/********************************************************************************
** Form generated from reading UI file 'cloud_connector_window.ui'
**
** Created by: Qt User Interface Compiler version 5.15.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLOUD_CONNECTOR_WINDOW_H
#define UI_CLOUD_CONNECTOR_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ManageCloudsWindowDialog
{
  public:
	QVBoxLayout *	 verticalLayout_3;
	QWidget *		 widgetContainer;
	QVBoxLayout *	 verticalLayout;
	QHBoxLayout *	 horizontalLayout_2;
	QPushButton *	 pushButtonConnectCloud;
	QPushButton *	 pushButtonRemove;
	QSpacerItem *	 horizontalSpacer;
	QHBoxLayout *	 horizontalLayout_3;
	QListWidget *	 listWidgetClouds;
	QStackedWidget * stackedWidgetCloudInformation;
	QWidget *		 pageAWS;
	QWidget *		 pageGENCI;
	QVBoxLayout *	 verticalLayout_2;
	QHBoxLayout *	 horizontalLayout_4;
	QSpacerItem *	 horizontalSpacer_2;
	QLabel *		 label;
	QLabel *		 label_2;
	QSpacerItem *	 verticalSpacer;

	void setupUi( QWidget * ManageCloudsWindowDialog )
	{
		if ( ManageCloudsWindowDialog->objectName().isEmpty() )
			ManageCloudsWindowDialog->setObjectName( QString::fromUtf8( "ManageCloudsWindowDialog" ) );
		ManageCloudsWindowDialog->resize( 516, 311 );
		verticalLayout_3 = new QVBoxLayout( ManageCloudsWindowDialog );
		verticalLayout_3->setObjectName( QString::fromUtf8( "verticalLayout_3" ) );
		widgetContainer = new QWidget( ManageCloudsWindowDialog );
		widgetContainer->setObjectName( QString::fromUtf8( "widgetContainer" ) );
		verticalLayout = new QVBoxLayout( widgetContainer );
		verticalLayout->setObjectName( QString::fromUtf8( "verticalLayout" ) );
		verticalLayout->setContentsMargins( 0, 0, 0, 0 );
		horizontalLayout_2 = new QHBoxLayout();
		horizontalLayout_2->setObjectName( QString::fromUtf8( "horizontalLayout_2" ) );
		pushButtonConnectCloud = new QPushButton( widgetContainer );
		pushButtonConnectCloud->setObjectName( QString::fromUtf8( "pushButtonConnectCloud" ) );

		horizontalLayout_2->addWidget( pushButtonConnectCloud );

		pushButtonRemove = new QPushButton( widgetContainer );
		pushButtonRemove->setObjectName( QString::fromUtf8( "pushButtonRemove" ) );

		horizontalLayout_2->addWidget( pushButtonRemove );

		horizontalSpacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

		horizontalLayout_2->addItem( horizontalSpacer );

		verticalLayout->addLayout( horizontalLayout_2 );

		horizontalLayout_3 = new QHBoxLayout();
		horizontalLayout_3->setObjectName( QString::fromUtf8( "horizontalLayout_3" ) );
		listWidgetClouds = new QListWidget( widgetContainer );
		new QListWidgetItem( listWidgetClouds );
		new QListWidgetItem( listWidgetClouds );
		listWidgetClouds->setObjectName( QString::fromUtf8( "listWidgetClouds" ) );
		QSizePolicy sizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding );
		sizePolicy.setHorizontalStretch( 0 );
		sizePolicy.setVerticalStretch( 0 );
		sizePolicy.setHeightForWidth( listWidgetClouds->sizePolicy().hasHeightForWidth() );
		listWidgetClouds->setSizePolicy( sizePolicy );
		listWidgetClouds->setMinimumSize( QSize( 80, 0 ) );
		listWidgetClouds->setMaximumSize( QSize( 120, 16777215 ) );

		horizontalLayout_3->addWidget( listWidgetClouds );

		stackedWidgetCloudInformation = new QStackedWidget( widgetContainer );
		stackedWidgetCloudInformation->setObjectName( QString::fromUtf8( "stackedWidgetCloudInformation" ) );
		QSizePolicy sizePolicy1( QSizePolicy::Expanding, QSizePolicy::Expanding );
		sizePolicy1.setHorizontalStretch( 0 );
		sizePolicy1.setVerticalStretch( 0 );
		sizePolicy1.setHeightForWidth( stackedWidgetCloudInformation->sizePolicy().hasHeightForWidth() );
		stackedWidgetCloudInformation->setSizePolicy( sizePolicy1 );
		stackedWidgetCloudInformation->setFrameShape( QFrame::Panel );
		pageAWS = new QWidget();
		pageAWS->setObjectName( QString::fromUtf8( "pageAWS" ) );
		stackedWidgetCloudInformation->addWidget( pageAWS );
		pageGENCI = new QWidget();
		pageGENCI->setObjectName( QString::fromUtf8( "pageGENCI" ) );
		verticalLayout_2 = new QVBoxLayout( pageGENCI );
		verticalLayout_2->setObjectName( QString::fromUtf8( "verticalLayout_2" ) );
		horizontalLayout_4 = new QHBoxLayout();
		horizontalLayout_4->setObjectName( QString::fromUtf8( "horizontalLayout_4" ) );
		horizontalSpacer_2 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

		horizontalLayout_4->addItem( horizontalSpacer_2 );

		label = new QLabel( pageGENCI );
		label->setObjectName( QString::fromUtf8( "label" ) );

		horizontalLayout_4->addWidget( label );

		label_2 = new QLabel( pageGENCI );
		label_2->setObjectName( QString::fromUtf8( "label_2" ) );

		horizontalLayout_4->addWidget( label_2 );

		verticalLayout_2->addLayout( horizontalLayout_4 );

		verticalSpacer = new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );

		verticalLayout_2->addItem( verticalSpacer );

		stackedWidgetCloudInformation->addWidget( pageGENCI );

		horizontalLayout_3->addWidget( stackedWidgetCloudInformation );

		horizontalLayout_3->setStretch( 1, 1 );

		verticalLayout->addLayout( horizontalLayout_3 );

		verticalLayout_3->addWidget( widgetContainer );

		retranslateUi( ManageCloudsWindowDialog );
		QObject::connect( listWidgetClouds,
						  SIGNAL( currentRowChanged( int ) ),
						  stackedWidgetCloudInformation,
						  SLOT( setCurrentIndex( int ) ) );

		stackedWidgetCloudInformation->setCurrentIndex( 1 );

		QMetaObject::connectSlotsByName( ManageCloudsWindowDialog );
	} // setupUi

	void retranslateUi( QWidget * ManageCloudsWindowDialog )
	{
		ManageCloudsWindowDialog->setWindowTitle(
			QCoreApplication::translate( "ManageCloudsWindowDialog", "Form", nullptr ) );
#if QT_CONFIG( tooltip )
		pushButtonConnectCloud->setToolTip(
			QCoreApplication::translate( "ManageCloudsWindowDialog",
										 "<html><head/><body><p>Connect to a cloud service</p></body></html>",
										 nullptr ) );
#endif // QT_CONFIG(tooltip)
		pushButtonConnectCloud->setText( QString() );
#if QT_CONFIG( tooltip )
		pushButtonRemove->setToolTip( QCoreApplication::translate(
			"ManageCloudsWindowDialog",
			"<html><head/><body><p>Remove a connexion to a cloud service</p></body></html>",
			nullptr ) );
#endif // QT_CONFIG(tooltip)
		pushButtonRemove->setText( QString() );

		const bool __sortingEnabled = listWidgetClouds->isSortingEnabled();
		listWidgetClouds->setSortingEnabled( false );
		QListWidgetItem * ___qlistwidgetitem = listWidgetClouds->item( 0 );
		___qlistwidgetitem->setText( QCoreApplication::translate( "ManageCloudsWindowDialog", "AWS", nullptr ) );
		QListWidgetItem * ___qlistwidgetitem1 = listWidgetClouds->item( 1 );
		___qlistwidgetitem1->setText( QCoreApplication::translate( "ManageCloudsWindowDialog", "Genci", nullptr ) );
		listWidgetClouds->setSortingEnabled( __sortingEnabled );

		label->setText( QCoreApplication::translate( "ManageCloudsWindowDialog", "Status:", nullptr ) );
		label_2->setText( QCoreApplication::translate( "ManageCloudsWindowDialog", "Connected", nullptr ) );
	} // retranslateUi
};

namespace Ui
{
	class ManageCloudsWindowDialog : public Ui_ManageCloudsWindowDialog
	{
	};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLOUD_CONNECTOR_WINDOW_H
