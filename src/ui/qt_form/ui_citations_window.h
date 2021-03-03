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
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DialogCitations
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QLabel *labelCitations;
    QTextBrowser *textBrowserCitations;

    void setupUi(QDialog *DialogCitations)
    {
        if (DialogCitations->objectName().isEmpty())
            DialogCitations->setObjectName(QString::fromUtf8("DialogCitations"));
        DialogCitations->resize(400, 300);
        verticalLayout_2 = new QVBoxLayout(DialogCitations);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        labelCitations = new QLabel(DialogCitations);
        labelCitations->setObjectName(QString::fromUtf8("labelCitations"));

        verticalLayout->addWidget(labelCitations);

        textBrowserCitations = new QTextBrowser(DialogCitations);
        textBrowserCitations->setObjectName(QString::fromUtf8("textBrowserCitations"));

        verticalLayout->addWidget(textBrowserCitations);


        verticalLayout_2->addLayout(verticalLayout);


        retranslateUi(DialogCitations);

        QMetaObject::connectSlotsByName(DialogCitations);
    } // setupUi

    void retranslateUi(QDialog *DialogCitations)
    {
        DialogCitations->setWindowTitle(QCoreApplication::translate("DialogCitations", "Citations", nullptr));
        labelCitations->setText(QCoreApplication::translate("DialogCitations", "If you use this tool in your project please cite the following papers:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DialogCitations: public Ui_DialogCitations {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CITATIONS_WINDOW_H
