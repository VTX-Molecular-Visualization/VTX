/********************************************************************************
** Form generated from reading UI file 'job_manager_window.ui'
**
** Created by: Qt User Interface Compiler version 5.15.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_JOB_MANAGER_WINDOW_H
#define UI_JOB_MANAGER_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_JobManagerGUI
{
public:
    QVBoxLayout *verticalLayout_2;
    QWidget *widgetContainer;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QRadioButton *radioButtonLocalJobs;
    QRadioButton *radioButtonCloudJobs;
    QRadioButton *radioButtonLocalJobs_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButtonResume;
    QPushButton *pushButtonPause;
    QPushButton *pushButtonStop;
    QPushButton *pushButtonRemove;
    QTableWidget *tableWidgetJobsList;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *pushButtonRetrieveJobs;
    QProgressBar *progressBarJobs;

    void setupUi(QWidget *JobManagerGUI)
    {
        if (JobManagerGUI->objectName().isEmpty())
            JobManagerGUI->setObjectName(QString::fromUtf8("JobManagerGUI"));
        JobManagerGUI->resize(568, 422);
        verticalLayout_2 = new QVBoxLayout(JobManagerGUI);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        widgetContainer = new QWidget(JobManagerGUI);
        widgetContainer->setObjectName(QString::fromUtf8("widgetContainer"));
        verticalLayout = new QVBoxLayout(widgetContainer);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(widgetContainer);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        radioButtonLocalJobs = new QRadioButton(widgetContainer);
        radioButtonLocalJobs->setObjectName(QString::fromUtf8("radioButtonLocalJobs"));
        radioButtonLocalJobs->setChecked(true);

        horizontalLayout->addWidget(radioButtonLocalJobs);

        radioButtonCloudJobs = new QRadioButton(widgetContainer);
        radioButtonCloudJobs->setObjectName(QString::fromUtf8("radioButtonCloudJobs"));

        horizontalLayout->addWidget(radioButtonCloudJobs);

        radioButtonLocalJobs_2 = new QRadioButton(widgetContainer);
        radioButtonLocalJobs_2->setObjectName(QString::fromUtf8("radioButtonLocalJobs_2"));

        horizontalLayout->addWidget(radioButtonLocalJobs_2);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButtonResume = new QPushButton(widgetContainer);
        pushButtonResume->setObjectName(QString::fromUtf8("pushButtonResume"));

        horizontalLayout->addWidget(pushButtonResume);

        pushButtonPause = new QPushButton(widgetContainer);
        pushButtonPause->setObjectName(QString::fromUtf8("pushButtonPause"));

        horizontalLayout->addWidget(pushButtonPause);

        pushButtonStop = new QPushButton(widgetContainer);
        pushButtonStop->setObjectName(QString::fromUtf8("pushButtonStop"));

        horizontalLayout->addWidget(pushButtonStop);

        pushButtonRemove = new QPushButton(widgetContainer);
        pushButtonRemove->setObjectName(QString::fromUtf8("pushButtonRemove"));

        horizontalLayout->addWidget(pushButtonRemove);


        verticalLayout->addLayout(horizontalLayout);

        tableWidgetJobsList = new QTableWidget(widgetContainer);
        if (tableWidgetJobsList->columnCount() < 5)
            tableWidgetJobsList->setColumnCount(5);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidgetJobsList->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidgetJobsList->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidgetJobsList->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableWidgetJobsList->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableWidgetJobsList->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        tableWidgetJobsList->setObjectName(QString::fromUtf8("tableWidgetJobsList"));
        tableWidgetJobsList->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        tableWidgetJobsList->setAutoScroll(false);
        tableWidgetJobsList->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableWidgetJobsList->setSortingEnabled(true);
        tableWidgetJobsList->horizontalHeader()->setStretchLastSection(true);

        verticalLayout->addWidget(tableWidgetJobsList);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        pushButtonRetrieveJobs = new QPushButton(widgetContainer);
        pushButtonRetrieveJobs->setObjectName(QString::fromUtf8("pushButtonRetrieveJobs"));

        horizontalLayout_2->addWidget(pushButtonRetrieveJobs);

        progressBarJobs = new QProgressBar(widgetContainer);
        progressBarJobs->setObjectName(QString::fromUtf8("progressBarJobs"));
        progressBarJobs->setValue(24);

        horizontalLayout_2->addWidget(progressBarJobs);


        verticalLayout->addLayout(horizontalLayout_2);


        verticalLayout_2->addWidget(widgetContainer);


        retranslateUi(JobManagerGUI);

        QMetaObject::connectSlotsByName(JobManagerGUI);
    } // setupUi

    void retranslateUi(QWidget *JobManagerGUI)
    {
        JobManagerGUI->setWindowTitle(QCoreApplication::translate("JobManagerGUI", "Form", nullptr));
        label->setText(QCoreApplication::translate("JobManagerGUI", "Shown jobs:", nullptr));
        radioButtonLocalJobs->setText(QCoreApplication::translate("JobManagerGUI", "all", nullptr));
        radioButtonCloudJobs->setText(QCoreApplication::translate("JobManagerGUI", "cloud", nullptr));
        radioButtonLocalJobs_2->setText(QCoreApplication::translate("JobManagerGUI", "local", nullptr));
        pushButtonResume->setText(QString());
        pushButtonPause->setText(QString());
        pushButtonStop->setText(QString());
        pushButtonRemove->setText(QString());
        QTableWidgetItem *___qtablewidgetitem = tableWidgetJobsList->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("JobManagerGUI", "Title", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidgetJobsList->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("JobManagerGUI", "Type", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidgetJobsList->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("JobManagerGUI", "Task", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tableWidgetJobsList->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("JobManagerGUI", "Status", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tableWidgetJobsList->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("JobManagerGUI", "Elapsed time", nullptr));
        pushButtonRetrieveJobs->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class JobManagerGUI: public Ui_JobManagerGUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_JOB_MANAGER_WINDOW_H
