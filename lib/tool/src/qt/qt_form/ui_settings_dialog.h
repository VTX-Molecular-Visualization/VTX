/********************************************************************************
** Form generated from reading UI file 'settings_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGS_DIALOG_H
#define UI_SETTINGS_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DialogSettings
{
  public:
	QGridLayout * gridLayout;
	QHBoxLayout * horizontalLayout;
	QLineEdit *	  lineEditSettingsName;
	QPushButton * pushButtonSaveSettings;
	QHBoxLayout * horizontalLayout_2;
	QListWidget * listWidgetSavedSettings;
	QVBoxLayout * verticalLayout_2;
	QPushButton * pushButtonLoadSetting;
	QPushButton * pushButtonRemoveSetting;
	QSpacerItem * verticalSpacer;

	void setupUi( QDialog * DialogSettings )
	{
		if ( DialogSettings->objectName().isEmpty() )
			DialogSettings->setObjectName( QString::fromStdString( "DialogSettings" ) );
		DialogSettings->resize( 338, 300 );
		gridLayout = new QGridLayout( DialogSettings );
		gridLayout->setObjectName( QString::fromStdString( "gridLayout" ) );
		horizontalLayout = new QHBoxLayout();
		horizontalLayout->setObjectName( QString::fromStdString( "horizontalLayout" ) );
		lineEditSettingsName = new QLineEdit( DialogSettings );
		lineEditSettingsName->setObjectName( QString::fromStdString( "lineEditSettingsName" ) );

		horizontalLayout->addWidget( lineEditSettingsName );

		pushButtonSaveSettings = new QPushButton( DialogSettings );
		pushButtonSaveSettings->setObjectName( QString::fromStdString( "pushButtonSaveSettings" ) );

		horizontalLayout->addWidget( pushButtonSaveSettings );

		gridLayout->addLayout( horizontalLayout, 0, 0, 1, 1 );

		horizontalLayout_2 = new QHBoxLayout();
		horizontalLayout_2->setObjectName( QString::fromStdString( "horizontalLayout_2" ) );
		listWidgetSavedSettings = new QListWidget( DialogSettings );
		listWidgetSavedSettings->setObjectName( QString::fromStdString( "listWidgetSavedSettings" ) );
		listWidgetSavedSettings->setSelectionBehavior( QAbstractItemView::SelectRows );

		horizontalLayout_2->addWidget( listWidgetSavedSettings );

		verticalLayout_2 = new QVBoxLayout();
		verticalLayout_2->setObjectName( QString::fromStdString( "verticalLayout_2" ) );
		pushButtonLoadSetting = new QPushButton( DialogSettings );
		pushButtonLoadSetting->setObjectName( QString::fromStdString( "pushButtonLoadSetting" ) );

		verticalLayout_2->addWidget( pushButtonLoadSetting );

		pushButtonRemoveSetting = new QPushButton( DialogSettings );
		pushButtonRemoveSetting->setObjectName( QString::fromStdString( "pushButtonRemoveSetting" ) );

		verticalLayout_2->addWidget( pushButtonRemoveSetting );

		verticalSpacer = new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );

		verticalLayout_2->addItem( verticalSpacer );

		horizontalLayout_2->addLayout( verticalLayout_2 );

		gridLayout->addLayout( horizontalLayout_2, 1, 0, 1, 1 );

		retranslateUi( DialogSettings );
		QObject::connect(
			pushButtonSaveSettings, SIGNAL( clicked() ), DialogSettings, SLOT( onSaveSettingsClicked() ) );

		QMetaObject::connectSlotsByName( DialogSettings );
	} // setupUi

	void retranslateUi( QDialog * DialogSettings )
	{
		DialogSettings->setWindowTitle( QCoreApplication::translate( "DialogSettings", "Window settings", nullptr ) );
		lineEditSettingsName->setPlaceholderText(
			QCoreApplication::translate( "DialogSettings", "Enter the name of the settings profile", nullptr ) );
		pushButtonSaveSettings->setText( QCoreApplication::translate( "DialogSettings", "Save", nullptr ) );
		pushButtonLoadSetting->setText( QCoreApplication::translate( "DialogSettings", "Load", nullptr ) );
		pushButtonRemoveSetting->setText( QCoreApplication::translate( "DialogSettings", "Remove", nullptr ) );
	} // retranslateUi
};

namespace Ui
{
	class DialogSettings : public Ui_DialogSettings
	{
	};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGS_DIALOG_H
