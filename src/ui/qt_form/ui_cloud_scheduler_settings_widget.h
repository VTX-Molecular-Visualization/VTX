/********************************************************************************
** Form generated from reading UI file 'cloud_scheduler_settings_widget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLOUD_SCHEDULER_SETTINGS_WIDGET_H
#define UI_CLOUD_SCHEDULER_SETTINGS_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <ui/widget/custom_widget/collapsing_groupbox_widget.hpp>

QT_BEGIN_NAMESPACE

class Ui_SchedulerSettingsWidget
{
public:
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *labelName;
    QLineEdit *lineEditName;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labelHostname;
    QLineEdit *lineEditHostname;
    QHBoxLayout *horizontalLayout_3;
    QLabel *labelUser;
    QLineEdit *lineEditUser;
    QHBoxLayout *horizontalLayout_4;
    QLabel *labelIdentityFile;
    QLineEdit *lineEditIdentityFile;
    QPushButton *pushButtonBrowseIdentityfile;
    CollapsingGroupboxWidget *groupBoxAdvanced;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_8;
    QLabel *labelSshExe;
    QLineEdit *lineEditSshExe;
    QHBoxLayout *horizontalLayout_9;
    QLabel *labelScpExe;
    QLineEdit *lineEditScpExe;
    QHBoxLayout *horizontalLayout_10;
    QLabel *labelPort;
    QSpinBox *spinBoxPort;
    QPushButton *pushButtonTestConnection;
    QHBoxLayout *horizontalLayout_11;
    QPushButton *pushButtonOk;
    QPushButton *pushButtonCancel;

    void setupUi(QWidget *SchedulerSettingsWidget)
    {
        if (SchedulerSettingsWidget->objectName().isEmpty())
            SchedulerSettingsWidget->setObjectName(QString::fromUtf8("SchedulerSettingsWidget"));
        SchedulerSettingsWidget->resize(372, 341);
        verticalLayout_2 = new QVBoxLayout(SchedulerSettingsWidget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        labelName = new QLabel(SchedulerSettingsWidget);
        labelName->setObjectName(QString::fromUtf8("labelName"));

        horizontalLayout->addWidget(labelName);

        lineEditName = new QLineEdit(SchedulerSettingsWidget);
        lineEditName->setObjectName(QString::fromUtf8("lineEditName"));

        horizontalLayout->addWidget(lineEditName);


        verticalLayout_2->addLayout(horizontalLayout);

        groupBox = new QGroupBox(SchedulerSettingsWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout_3 = new QVBoxLayout(groupBox);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        labelHostname = new QLabel(groupBox);
        labelHostname->setObjectName(QString::fromUtf8("labelHostname"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(labelHostname->sizePolicy().hasHeightForWidth());
        labelHostname->setSizePolicy(sizePolicy);
        labelHostname->setMinimumSize(QSize(100, 0));
        labelHostname->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_2->addWidget(labelHostname);

        lineEditHostname = new QLineEdit(groupBox);
        lineEditHostname->setObjectName(QString::fromUtf8("lineEditHostname"));

        horizontalLayout_2->addWidget(lineEditHostname);

        horizontalLayout_2->setStretch(1, 1);

        verticalLayout_3->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        labelUser = new QLabel(groupBox);
        labelUser->setObjectName(QString::fromUtf8("labelUser"));
        sizePolicy.setHeightForWidth(labelUser->sizePolicy().hasHeightForWidth());
        labelUser->setSizePolicy(sizePolicy);
        labelUser->setMinimumSize(QSize(100, 0));
        labelUser->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_3->addWidget(labelUser);

        lineEditUser = new QLineEdit(groupBox);
        lineEditUser->setObjectName(QString::fromUtf8("lineEditUser"));

        horizontalLayout_3->addWidget(lineEditUser);


        verticalLayout_3->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        labelIdentityFile = new QLabel(groupBox);
        labelIdentityFile->setObjectName(QString::fromUtf8("labelIdentityFile"));
        sizePolicy.setHeightForWidth(labelIdentityFile->sizePolicy().hasHeightForWidth());
        labelIdentityFile->setSizePolicy(sizePolicy);
        labelIdentityFile->setMinimumSize(QSize(100, 0));
        labelIdentityFile->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_4->addWidget(labelIdentityFile);

        lineEditIdentityFile = new QLineEdit(groupBox);
        lineEditIdentityFile->setObjectName(QString::fromUtf8("lineEditIdentityFile"));

        horizontalLayout_4->addWidget(lineEditIdentityFile);

        pushButtonBrowseIdentityfile = new QPushButton(groupBox);
        pushButtonBrowseIdentityfile->setObjectName(QString::fromUtf8("pushButtonBrowseIdentityfile"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(pushButtonBrowseIdentityfile->sizePolicy().hasHeightForWidth());
        pushButtonBrowseIdentityfile->setSizePolicy(sizePolicy1);
        pushButtonBrowseIdentityfile->setMaximumSize(QSize(30, 16777215));

        horizontalLayout_4->addWidget(pushButtonBrowseIdentityfile);

        horizontalLayout_4->setStretch(1, 1);

        verticalLayout_3->addLayout(horizontalLayout_4);

        groupBoxAdvanced = new CollapsingGroupboxWidget(groupBox);
        groupBoxAdvanced->setObjectName(QString::fromUtf8("groupBoxAdvanced"));
        verticalLayout = new QVBoxLayout(groupBoxAdvanced);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        labelSshExe = new QLabel(groupBoxAdvanced);
        labelSshExe->setObjectName(QString::fromUtf8("labelSshExe"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(labelSshExe->sizePolicy().hasHeightForWidth());
        labelSshExe->setSizePolicy(sizePolicy2);
        labelSshExe->setMinimumSize(QSize(90, 0));
        labelSshExe->setMaximumSize(QSize(90, 16777215));

        horizontalLayout_8->addWidget(labelSshExe);

        lineEditSshExe = new QLineEdit(groupBoxAdvanced);
        lineEditSshExe->setObjectName(QString::fromUtf8("lineEditSshExe"));

        horizontalLayout_8->addWidget(lineEditSshExe);


        verticalLayout->addLayout(horizontalLayout_8);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        labelScpExe = new QLabel(groupBoxAdvanced);
        labelScpExe->setObjectName(QString::fromUtf8("labelScpExe"));
        sizePolicy2.setHeightForWidth(labelScpExe->sizePolicy().hasHeightForWidth());
        labelScpExe->setSizePolicy(sizePolicy2);
        labelScpExe->setMinimumSize(QSize(90, 0));
        labelScpExe->setMaximumSize(QSize(90, 16777215));

        horizontalLayout_9->addWidget(labelScpExe);

        lineEditScpExe = new QLineEdit(groupBoxAdvanced);
        lineEditScpExe->setObjectName(QString::fromUtf8("lineEditScpExe"));

        horizontalLayout_9->addWidget(lineEditScpExe);


        verticalLayout->addLayout(horizontalLayout_9);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        labelPort = new QLabel(groupBoxAdvanced);
        labelPort->setObjectName(QString::fromUtf8("labelPort"));
        sizePolicy2.setHeightForWidth(labelPort->sizePolicy().hasHeightForWidth());
        labelPort->setSizePolicy(sizePolicy2);
        labelPort->setMinimumSize(QSize(90, 0));
        labelPort->setMaximumSize(QSize(90, 16777215));

        horizontalLayout_10->addWidget(labelPort);

        spinBoxPort = new QSpinBox(groupBoxAdvanced);
        spinBoxPort->setObjectName(QString::fromUtf8("spinBoxPort"));

        horizontalLayout_10->addWidget(spinBoxPort);


        verticalLayout->addLayout(horizontalLayout_10);


        verticalLayout_3->addWidget(groupBoxAdvanced);

        pushButtonTestConnection = new QPushButton(groupBox);
        pushButtonTestConnection->setObjectName(QString::fromUtf8("pushButtonTestConnection"));

        verticalLayout_3->addWidget(pushButtonTestConnection);


        verticalLayout_2->addWidget(groupBox);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        pushButtonOk = new QPushButton(SchedulerSettingsWidget);
        pushButtonOk->setObjectName(QString::fromUtf8("pushButtonOk"));

        horizontalLayout_11->addWidget(pushButtonOk);

        pushButtonCancel = new QPushButton(SchedulerSettingsWidget);
        pushButtonCancel->setObjectName(QString::fromUtf8("pushButtonCancel"));

        horizontalLayout_11->addWidget(pushButtonCancel);


        verticalLayout_2->addLayout(horizontalLayout_11);


        retranslateUi(SchedulerSettingsWidget);
        QObject::connect(pushButtonOk, SIGNAL(clicked()), SchedulerSettingsWidget, SLOT(save()));
        QObject::connect(pushButtonCancel, SIGNAL(clicked()), SchedulerSettingsWidget, SLOT(reset()));
        QObject::connect(pushButtonBrowseIdentityfile, SIGNAL(clicked()), SchedulerSettingsWidget, SLOT(browseIdentityFile()));
        QObject::connect(pushButtonTestConnection, SIGNAL(clicked()), SchedulerSettingsWidget, SLOT(testConnection()));

        QMetaObject::connectSlotsByName(SchedulerSettingsWidget);
    } // setupUi

    void retranslateUi(QWidget *SchedulerSettingsWidget)
    {
        SchedulerSettingsWidget->setWindowTitle(QCoreApplication::translate("SchedulerSettingsWidget", "Form", nullptr));
        labelName->setText(QCoreApplication::translate("SchedulerSettingsWidget", "Name:", nullptr));
        groupBox->setTitle(QCoreApplication::translate("SchedulerSettingsWidget", "SSH configuration", nullptr));
        labelHostname->setText(QCoreApplication::translate("SchedulerSettingsWidget", "Hostname:", nullptr));
        labelUser->setText(QCoreApplication::translate("SchedulerSettingsWidget", "User:", nullptr));
        labelIdentityFile->setText(QCoreApplication::translate("SchedulerSettingsWidget", "Identity file:", nullptr));
        pushButtonBrowseIdentityfile->setText(QCoreApplication::translate("SchedulerSettingsWidget", "...", nullptr));
        groupBoxAdvanced->setTitle(QCoreApplication::translate("SchedulerSettingsWidget", "Advanced", nullptr));
        labelSshExe->setText(QCoreApplication::translate("SchedulerSettingsWidget", "SSH executable:", nullptr));
        labelScpExe->setText(QCoreApplication::translate("SchedulerSettingsWidget", "SCP executable:", nullptr));
        labelPort->setText(QCoreApplication::translate("SchedulerSettingsWidget", "Port:", nullptr));
        pushButtonTestConnection->setText(QCoreApplication::translate("SchedulerSettingsWidget", "Test connection...", nullptr));
        pushButtonOk->setText(QCoreApplication::translate("SchedulerSettingsWidget", "Ok", nullptr));
        pushButtonCancel->setText(QCoreApplication::translate("SchedulerSettingsWidget", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SchedulerSettingsWidget: public Ui_SchedulerSettingsWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLOUD_SCHEDULER_SETTINGS_WIDGET_H
