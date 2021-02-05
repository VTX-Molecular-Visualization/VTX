/********************************************************************************
** Form generated from reading UI file 'keyword_editor_main_window.ui'
**
** Created by: Qt User Interface Compiler version 5.15.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_KEYWORD_EDITOR_MAIN_WINDOW_H
#define UI_KEYWORD_EDITOR_MAIN_WINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DockWidgetKeywordEditor
{
public:
    QWidget *dockWidgetContents;
    QHBoxLayout *horizontalLayout;
    QSpinBox *inputSpinBox1;

    void setupUi(QDockWidget *DockWidgetKeywordEditor)
    {
        if (DockWidgetKeywordEditor->objectName().isEmpty())
            DockWidgetKeywordEditor->setObjectName(QString::fromUtf8("DockWidgetKeywordEditor"));
        DockWidgetKeywordEditor->resize(372, 327);
        QIcon icon;
        icon.addFile(QString::fromUtf8("../../../../asset/qt_resources/sprite/keyword_editor.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon.addFile(QString::fromUtf8("../../../../asset/qt_resources/sprite/keyword_editor.png"), QSize(), QIcon::Normal, QIcon::On);
        DockWidgetKeywordEditor->setWindowIcon(icon);
        DockWidgetKeywordEditor->setFloating(true);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        horizontalLayout = new QHBoxLayout(dockWidgetContents);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        inputSpinBox1 = new QSpinBox(dockWidgetContents);
        inputSpinBox1->setObjectName(QString::fromUtf8("inputSpinBox1"));

        horizontalLayout->addWidget(inputSpinBox1);

        DockWidgetKeywordEditor->setWidget(dockWidgetContents);

        retranslateUi(DockWidgetKeywordEditor);
        QObject::connect(inputSpinBox1, SIGNAL(valueChanged(int)), DockWidgetKeywordEditor, SLOT(on_inputSpinBox1_valueChanged(int)));

        QMetaObject::connectSlotsByName(DockWidgetKeywordEditor);
    } // setupUi

    void retranslateUi(QDockWidget *DockWidgetKeywordEditor)
    {
        DockWidgetKeywordEditor->setWindowTitle(QCoreApplication::translate("DockWidgetKeywordEditor", "Keyword Editor", nullptr));
#if QT_CONFIG(tooltip)
        inputSpinBox1->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p>Test test test Test test testTest test testTest test testTest test testTest test test</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
    } // retranslateUi

};

namespace Ui {
    class DockWidgetKeywordEditor: public Ui_DockWidgetKeywordEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KEYWORD_EDITOR_MAIN_WINDOW_H
