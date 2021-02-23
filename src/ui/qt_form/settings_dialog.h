/********************************************************************************
** Form generated from reading UI file 'settings_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

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
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QLineEdit *lineEditSettingsName;
    QPushButton *pushButtonSaveSettings;
    QHBoxLayout *horizontalLayout_2;
    QListWidget *listWidgetSavedSettings;
    QVBoxLayout *verticalLayout_2;
    QPushButton *pushButtonLoadSetting;
    QPushButton *pushButtonRemoveSetting;
    QSpacerItem *verticalSpacer;

    void setupUi(QDialog *DialogSettings)
    {
        if (DialogSettings->objectName().isEmpty())
            DialogSettings->setObjectName(QString::fromUtf8("DialogSettings"));
        DialogSettings->resize(338, 300);
        gridLayout = new QGridLayout(DialogSettings);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        lineEditSettingsName = new QLineEdit(DialogSettings);
        lineEditSettingsName->setObjectName(QString::fromUtf8("lineEditSettingsName"));

        horizontalLayout->addWidget(lineEditSettingsName);

        pushButtonSaveSettings = new QPushButton(DialogSettings);
        pushButtonSaveSettings->setObjectName(QString::fromUtf8("pushButtonSaveSettings"));

        horizontalLayout->addWidget(pushButtonSaveSettings);


        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        listWidgetSavedSettings = new QListWidget(DialogSettings);
        listWidgetSavedSettings->setObjectName(QString::fromUtf8("listWidgetSavedSettings"));

        horizontalLayout_2->addWidget(listWidgetSavedSettings);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        pushButtonLoadSetting = new QPushButton(DialogSettings);
        pushButtonLoadSetting->setObjectName(QString::fromUtf8("pushButtonLoadSetting"));

        verticalLayout_2->addWidget(pushButtonLoadSetting);

        pushButtonRemoveSetting = new QPushButton(DialogSettings);
        pushButtonRemoveSetting->setObjectName(QString::fromUtf8("pushButtonRemoveSetting"));

        verticalLayout_2->addWidget(pushButtonRemoveSetting);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);


        horizontalLayout_2->addLayout(verticalLayout_2);


        gridLayout->addLayout(horizontalLayout_2, 1, 0, 1, 1);


        retranslateUi(DialogSettings);
        QObject::connect(pushButtonSaveSettings, SIGNAL(clicked()), DialogSettings, SLOT(onSaveSettingsClicked()));
        QObject::connect(pushButtonLoadSetting, SIGNAL(clicked()), DialogSettings, SLOT(onLoadSettingsClicked()));
        QObject::connect(pushButtonRemoveSetting, SIGNAL(clicked()), DialogSettings, SLOT(onRemoveSettingsClicked()));

        QMetaObject::connectSlotsByName(DialogSettings);
    } // setupUi

    void retranslateUi(QDialog *DialogSettings)
    {
        DialogSettings->setWindowTitle(QCoreApplication::translate("DialogSettings", "Window settings", nullptr));
        lineEditSettingsName->setPlaceholderText(QCoreApplication::translate("DialogSettings", "Enter the name of the settings profile", nullptr));
        pushButtonSaveSettings->setText(QCoreApplication::translate("DialogSettings", "Save", nullptr));
        pushButtonLoadSetting->setText(QCoreApplication::translate("DialogSettings", "Load", nullptr));
        pushButtonRemoveSetting->setText(QCoreApplication::translate("DialogSettings", "Remove", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DialogSettings: public Ui_DialogSettings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // SETTINGS_DIALOG_H
