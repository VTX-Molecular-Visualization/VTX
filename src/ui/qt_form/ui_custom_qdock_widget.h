/********************************************************************************
** Form generated from reading UI file 'custom_qdock_widget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CUSTOM_QDOCK_WIDGET_H
#define UI_CUSTOM_QDOCK_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CustomDockWidget
{
public:
    QWidget *dockWidgetContents;
    QGridLayout *gridLayout;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QGridLayout *gridLayout_3;
    QGridLayout *gridLayoutScrollArea;

    void setupUi(QDockWidget *CustomDockWidget)
    {
        if (CustomDockWidget->objectName().isEmpty())
            CustomDockWidget->setObjectName(QString::fromUtf8("CustomDockWidget"));
        CustomDockWidget->resize(346, 548);
        CustomDockWidget->setFloating(true);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        gridLayout = new QGridLayout(dockWidgetContents);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(2, 5, 2, 2);
        scrollArea = new QScrollArea(dockWidgetContents);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 342, 522));
        gridLayout_3 = new QGridLayout(scrollAreaWidgetContents);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setContentsMargins(2, 9, 2, 0);
        gridLayoutScrollArea = new QGridLayout();
        gridLayoutScrollArea->setObjectName(QString::fromUtf8("gridLayoutScrollArea"));

        gridLayout_3->addLayout(gridLayoutScrollArea, 0, 0, 1, 1);

        scrollArea->setWidget(scrollAreaWidgetContents);

        gridLayout->addWidget(scrollArea, 0, 0, 1, 1);

        CustomDockWidget->setWidget(dockWidgetContents);

        retranslateUi(CustomDockWidget);

        QMetaObject::connectSlotsByName(CustomDockWidget);
    } // setupUi

    void retranslateUi(QDockWidget *CustomDockWidget)
    {
        CustomDockWidget->setWindowTitle(QCoreApplication::translate("CustomDockWidget", "DockWidget", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CustomDockWidget: public Ui_CustomDockWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CUSTOM_QDOCK_WIDGET_H
