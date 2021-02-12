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
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "ui/widget/custom_widget/collapsing_groupbox_widget.hpp"

QT_BEGIN_NAMESPACE

class Ui_DockWidgetKeywordEditor
{
public:
    QWidget *dockWidgetContents;
    QGridLayout *gridLayout;
    CollapsingGroupboxWidget *groupBoxAdditionnalKeywords;
    QVBoxLayout *verticalLayout_3;
    QTextBrowser *textBrowserAdditionnalKeywords;
    CollapsingGroupboxWidget *groupBoxDynamic;
    QVBoxLayout *verticalLayout;
    QWidget *widgetDynamic;
    QVBoxLayout *verticalLayout_6;
    QHBoxLayout *horizontalLayout_3;
    QLabel *labelIntegrator;
    QComboBox *comboBoxIntegrator;
    QHBoxLayout *horizontalLayout;
    QLabel *labelFriction;
    QSpinBox *spinBoxFriction;
    QHBoxLayout *horizontalLayout_4;
    QLabel *labelInnerTimestep;
    QSpinBox *spinBoxInnerTimestep;
    QHBoxLayout *horizontalLayout_5;
    QLabel *labelIntermediateTimestep;
    QSpinBox *spinBoxIntermediateTimestep;
    QHBoxLayout *horizontalLayout_8;
    QLabel *labelThermostat;
    QComboBox *comboBoxThermostat;
    QHBoxLayout *horizontalLayout_9;
    QLabel *labelBarostat;
    QComboBox *comboBoxBarostat;
    QHBoxLayout *horizontalLayout_10;
    QLabel *labelPolarizationEquations;
    QComboBox *comboBoxPolarizationEquations;
    CollapsingGroupboxWidget *groupBoxTcgParameters;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_11;
    QLabel *labelTcgOrder;
    QComboBox *comboBoxTcgOrder;
    QHBoxLayout *horizontalLayout_12;
    QLabel *labelDiagPrec;
    QComboBox *comboBoxDiagPrec;
    QHBoxLayout *horizontalLayout_13;
    QLabel *labelTcgGuess;
    QComboBox *comboBoxTcgGuess;
    QHBoxLayout *horizontalLayout_14;
    QLabel *labelTcgPeek;
    QComboBox *comboBoxTcgPeek;
    QWidget *widgetPeekStep;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_15;
    QLabel *labelTcgOmega;
    QSpinBox *spinBoxTcgOmega;
    QHBoxLayout *horizontalLayout_16;
    QLabel *labelTcgMegaFit;
    QComboBox *comboBoxTcgMegaFit;
    QHBoxLayout *horizontalLayout_17;
    QLabel *labelTcgMegaFitFreq;
    QSpinBox *spinBoxTcgMegaFitFreq;
    CollapsingGroupboxWidget *groupBoxTcgShortParameters;
    QVBoxLayout *verticalLayout_5;
    QHBoxLayout *horizontalLayout_18;
    QLabel *labelTcgOrderShort;
    QComboBox *comboBox;
    QHBoxLayout *horizontalLayout_22;
    QLabel *labelDiagPrecShort;
    QComboBox *comboBoxDiagPrecShort;
    QHBoxLayout *horizontalLayout_20;
    QLabel *labelTcgGuessShort;
    QComboBox *comboBoxTcgGuessShort;
    QHBoxLayout *horizontalLayout_21;
    QLabel *labelTcgPeekShort;
    QComboBox *comboBoxTcgPeekShort;
    QHBoxLayout *horizontalLayout_19;
    QLabel *labelTcgOmegaShort;
    QSpinBox *spinBoxTcgOmegaShort;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButtonOpenKeyfile;
    QSpacerItem *verticalSpacer;

    void setupUi(QDockWidget *DockWidgetKeywordEditor)
    {
        if (DockWidgetKeywordEditor->objectName().isEmpty())
            DockWidgetKeywordEditor->setObjectName(QString::fromUtf8("DockWidgetKeywordEditor"));
        DockWidgetKeywordEditor->resize(374, 1212);
        QIcon icon;
        icon.addFile(QString::fromUtf8("../../../../asset/qt_resources/sprite/keyword_editor.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon.addFile(QString::fromUtf8("../../../../asset/qt_resources/sprite/keyword_editor.png"), QSize(), QIcon::Normal, QIcon::On);
        DockWidgetKeywordEditor->setWindowIcon(icon);
        DockWidgetKeywordEditor->setFloating(true);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        gridLayout = new QGridLayout(dockWidgetContents);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        groupBoxAdditionnalKeywords = new CollapsingGroupboxWidget(dockWidgetContents);
        groupBoxAdditionnalKeywords->setObjectName(QString::fromUtf8("groupBoxAdditionnalKeywords"));
        groupBoxAdditionnalKeywords->setCheckable(true);
        verticalLayout_3 = new QVBoxLayout(groupBoxAdditionnalKeywords);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(9, 9, 9, 9);
        textBrowserAdditionnalKeywords = new QTextBrowser(groupBoxAdditionnalKeywords);
        textBrowserAdditionnalKeywords->setObjectName(QString::fromUtf8("textBrowserAdditionnalKeywords"));

        verticalLayout_3->addWidget(textBrowserAdditionnalKeywords);


        gridLayout->addWidget(groupBoxAdditionnalKeywords, 2, 0, 1, 1);

        groupBoxDynamic = new CollapsingGroupboxWidget(dockWidgetContents);
        groupBoxDynamic->setObjectName(QString::fromUtf8("groupBoxDynamic"));
        groupBoxDynamic->setCheckable(true);
        verticalLayout = new QVBoxLayout(groupBoxDynamic);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(9, 9, 9, 9);
        widgetDynamic = new QWidget(groupBoxDynamic);
        widgetDynamic->setObjectName(QString::fromUtf8("widgetDynamic"));
        verticalLayout_6 = new QVBoxLayout(widgetDynamic);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        labelIntegrator = new QLabel(widgetDynamic);
        labelIntegrator->setObjectName(QString::fromUtf8("labelIntegrator"));

        horizontalLayout_3->addWidget(labelIntegrator);

        comboBoxIntegrator = new QComboBox(widgetDynamic);
        comboBoxIntegrator->addItem(QString());
        comboBoxIntegrator->addItem(QString());
        comboBoxIntegrator->addItem(QString());
        comboBoxIntegrator->addItem(QString());
        comboBoxIntegrator->addItem(QString());
        comboBoxIntegrator->addItem(QString());
        comboBoxIntegrator->addItem(QString());
        comboBoxIntegrator->addItem(QString());
        comboBoxIntegrator->addItem(QString());
        comboBoxIntegrator->setObjectName(QString::fromUtf8("comboBoxIntegrator"));

        horizontalLayout_3->addWidget(comboBoxIntegrator);


        verticalLayout_6->addLayout(horizontalLayout_3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        labelFriction = new QLabel(widgetDynamic);
        labelFriction->setObjectName(QString::fromUtf8("labelFriction"));

        horizontalLayout->addWidget(labelFriction);

        spinBoxFriction = new QSpinBox(widgetDynamic);
        spinBoxFriction->setObjectName(QString::fromUtf8("spinBoxFriction"));
        spinBoxFriction->setValue(1);

        horizontalLayout->addWidget(spinBoxFriction);


        verticalLayout_6->addLayout(horizontalLayout);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        labelInnerTimestep = new QLabel(widgetDynamic);
        labelInnerTimestep->setObjectName(QString::fromUtf8("labelInnerTimestep"));

        horizontalLayout_4->addWidget(labelInnerTimestep);

        spinBoxInnerTimestep = new QSpinBox(widgetDynamic);
        spinBoxInnerTimestep->setObjectName(QString::fromUtf8("spinBoxInnerTimestep"));
        spinBoxInnerTimestep->setValue(1);

        horizontalLayout_4->addWidget(spinBoxInnerTimestep);


        verticalLayout_6->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        labelIntermediateTimestep = new QLabel(widgetDynamic);
        labelIntermediateTimestep->setObjectName(QString::fromUtf8("labelIntermediateTimestep"));

        horizontalLayout_5->addWidget(labelIntermediateTimestep);

        spinBoxIntermediateTimestep = new QSpinBox(widgetDynamic);
        spinBoxIntermediateTimestep->setObjectName(QString::fromUtf8("spinBoxIntermediateTimestep"));
        spinBoxIntermediateTimestep->setValue(1);

        horizontalLayout_5->addWidget(spinBoxIntermediateTimestep);


        verticalLayout_6->addLayout(horizontalLayout_5);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        labelThermostat = new QLabel(widgetDynamic);
        labelThermostat->setObjectName(QString::fromUtf8("labelThermostat"));

        horizontalLayout_8->addWidget(labelThermostat);

        comboBoxThermostat = new QComboBox(widgetDynamic);
        comboBoxThermostat->addItem(QString());
        comboBoxThermostat->addItem(QString());
        comboBoxThermostat->addItem(QString());
        comboBoxThermostat->setObjectName(QString::fromUtf8("comboBoxThermostat"));

        horizontalLayout_8->addWidget(comboBoxThermostat);


        verticalLayout_6->addLayout(horizontalLayout_8);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        labelBarostat = new QLabel(widgetDynamic);
        labelBarostat->setObjectName(QString::fromUtf8("labelBarostat"));

        horizontalLayout_9->addWidget(labelBarostat);

        comboBoxBarostat = new QComboBox(widgetDynamic);
        comboBoxBarostat->addItem(QString());
        comboBoxBarostat->addItem(QString());
        comboBoxBarostat->setObjectName(QString::fromUtf8("comboBoxBarostat"));

        horizontalLayout_9->addWidget(comboBoxBarostat);


        verticalLayout_6->addLayout(horizontalLayout_9);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        labelPolarizationEquations = new QLabel(widgetDynamic);
        labelPolarizationEquations->setObjectName(QString::fromUtf8("labelPolarizationEquations"));

        horizontalLayout_10->addWidget(labelPolarizationEquations);

        comboBoxPolarizationEquations = new QComboBox(widgetDynamic);
        comboBoxPolarizationEquations->addItem(QString());
        comboBoxPolarizationEquations->addItem(QString());
        comboBoxPolarizationEquations->addItem(QString());
        comboBoxPolarizationEquations->addItem(QString());
        comboBoxPolarizationEquations->setObjectName(QString::fromUtf8("comboBoxPolarizationEquations"));

        horizontalLayout_10->addWidget(comboBoxPolarizationEquations);


        verticalLayout_6->addLayout(horizontalLayout_10);

        groupBoxTcgParameters = new CollapsingGroupboxWidget(widgetDynamic);
        groupBoxTcgParameters->setObjectName(QString::fromUtf8("groupBoxTcgParameters"));
        verticalLayout_4 = new QVBoxLayout(groupBoxTcgParameters);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        labelTcgOrder = new QLabel(groupBoxTcgParameters);
        labelTcgOrder->setObjectName(QString::fromUtf8("labelTcgOrder"));

        horizontalLayout_11->addWidget(labelTcgOrder);

        comboBoxTcgOrder = new QComboBox(groupBoxTcgParameters);
        comboBoxTcgOrder->addItem(QString());
        comboBoxTcgOrder->addItem(QString());
        comboBoxTcgOrder->setObjectName(QString::fromUtf8("comboBoxTcgOrder"));

        horizontalLayout_11->addWidget(comboBoxTcgOrder);


        verticalLayout_4->addLayout(horizontalLayout_11);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        labelDiagPrec = new QLabel(groupBoxTcgParameters);
        labelDiagPrec->setObjectName(QString::fromUtf8("labelDiagPrec"));

        horizontalLayout_12->addWidget(labelDiagPrec);

        comboBoxDiagPrec = new QComboBox(groupBoxTcgParameters);
        comboBoxDiagPrec->addItem(QString());
        comboBoxDiagPrec->addItem(QString());
        comboBoxDiagPrec->setObjectName(QString::fromUtf8("comboBoxDiagPrec"));

        horizontalLayout_12->addWidget(comboBoxDiagPrec);


        verticalLayout_4->addLayout(horizontalLayout_12);

        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setObjectName(QString::fromUtf8("horizontalLayout_13"));
        labelTcgGuess = new QLabel(groupBoxTcgParameters);
        labelTcgGuess->setObjectName(QString::fromUtf8("labelTcgGuess"));

        horizontalLayout_13->addWidget(labelTcgGuess);

        comboBoxTcgGuess = new QComboBox(groupBoxTcgParameters);
        comboBoxTcgGuess->addItem(QString());
        comboBoxTcgGuess->addItem(QString());
        comboBoxTcgGuess->setObjectName(QString::fromUtf8("comboBoxTcgGuess"));

        horizontalLayout_13->addWidget(comboBoxTcgGuess);


        verticalLayout_4->addLayout(horizontalLayout_13);

        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setObjectName(QString::fromUtf8("horizontalLayout_14"));
        labelTcgPeek = new QLabel(groupBoxTcgParameters);
        labelTcgPeek->setObjectName(QString::fromUtf8("labelTcgPeek"));

        horizontalLayout_14->addWidget(labelTcgPeek);

        comboBoxTcgPeek = new QComboBox(groupBoxTcgParameters);
        comboBoxTcgPeek->addItem(QString());
        comboBoxTcgPeek->addItem(QString());
        comboBoxTcgPeek->setObjectName(QString::fromUtf8("comboBoxTcgPeek"));

        horizontalLayout_14->addWidget(comboBoxTcgPeek);


        verticalLayout_4->addLayout(horizontalLayout_14);

        widgetPeekStep = new QWidget(groupBoxTcgParameters);
        widgetPeekStep->setObjectName(QString::fromUtf8("widgetPeekStep"));
        verticalLayout_2 = new QVBoxLayout(widgetPeekStep);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setObjectName(QString::fromUtf8("horizontalLayout_15"));
        labelTcgOmega = new QLabel(widgetPeekStep);
        labelTcgOmega->setObjectName(QString::fromUtf8("labelTcgOmega"));

        horizontalLayout_15->addWidget(labelTcgOmega);

        spinBoxTcgOmega = new QSpinBox(widgetPeekStep);
        spinBoxTcgOmega->setObjectName(QString::fromUtf8("spinBoxTcgOmega"));
        spinBoxTcgOmega->setValue(1);

        horizontalLayout_15->addWidget(spinBoxTcgOmega);


        verticalLayout_2->addLayout(horizontalLayout_15);

        horizontalLayout_16 = new QHBoxLayout();
        horizontalLayout_16->setObjectName(QString::fromUtf8("horizontalLayout_16"));
        labelTcgMegaFit = new QLabel(widgetPeekStep);
        labelTcgMegaFit->setObjectName(QString::fromUtf8("labelTcgMegaFit"));

        horizontalLayout_16->addWidget(labelTcgMegaFit);

        comboBoxTcgMegaFit = new QComboBox(widgetPeekStep);
        comboBoxTcgMegaFit->addItem(QString());
        comboBoxTcgMegaFit->addItem(QString());
        comboBoxTcgMegaFit->setObjectName(QString::fromUtf8("comboBoxTcgMegaFit"));

        horizontalLayout_16->addWidget(comboBoxTcgMegaFit);


        verticalLayout_2->addLayout(horizontalLayout_16);

        horizontalLayout_17 = new QHBoxLayout();
        horizontalLayout_17->setObjectName(QString::fromUtf8("horizontalLayout_17"));
        labelTcgMegaFitFreq = new QLabel(widgetPeekStep);
        labelTcgMegaFitFreq->setObjectName(QString::fromUtf8("labelTcgMegaFitFreq"));

        horizontalLayout_17->addWidget(labelTcgMegaFitFreq);

        spinBoxTcgMegaFitFreq = new QSpinBox(widgetPeekStep);
        spinBoxTcgMegaFitFreq->setObjectName(QString::fromUtf8("spinBoxTcgMegaFitFreq"));
        spinBoxTcgMegaFitFreq->setMaximum(100000000);
        spinBoxTcgMegaFitFreq->setValue(1000);

        horizontalLayout_17->addWidget(spinBoxTcgMegaFitFreq);


        verticalLayout_2->addLayout(horizontalLayout_17);


        verticalLayout_4->addWidget(widgetPeekStep);


        verticalLayout_6->addWidget(groupBoxTcgParameters);

        groupBoxTcgShortParameters = new CollapsingGroupboxWidget(widgetDynamic);
        groupBoxTcgShortParameters->setObjectName(QString::fromUtf8("groupBoxTcgShortParameters"));
        verticalLayout_5 = new QVBoxLayout(groupBoxTcgShortParameters);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        horizontalLayout_18 = new QHBoxLayout();
        horizontalLayout_18->setObjectName(QString::fromUtf8("horizontalLayout_18"));
        labelTcgOrderShort = new QLabel(groupBoxTcgShortParameters);
        labelTcgOrderShort->setObjectName(QString::fromUtf8("labelTcgOrderShort"));

        horizontalLayout_18->addWidget(labelTcgOrderShort);

        comboBox = new QComboBox(groupBoxTcgShortParameters);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        horizontalLayout_18->addWidget(comboBox);


        verticalLayout_5->addLayout(horizontalLayout_18);

        horizontalLayout_22 = new QHBoxLayout();
        horizontalLayout_22->setObjectName(QString::fromUtf8("horizontalLayout_22"));
        labelDiagPrecShort = new QLabel(groupBoxTcgShortParameters);
        labelDiagPrecShort->setObjectName(QString::fromUtf8("labelDiagPrecShort"));

        horizontalLayout_22->addWidget(labelDiagPrecShort);

        comboBoxDiagPrecShort = new QComboBox(groupBoxTcgShortParameters);
        comboBoxDiagPrecShort->addItem(QString());
        comboBoxDiagPrecShort->addItem(QString());
        comboBoxDiagPrecShort->setObjectName(QString::fromUtf8("comboBoxDiagPrecShort"));

        horizontalLayout_22->addWidget(comboBoxDiagPrecShort);


        verticalLayout_5->addLayout(horizontalLayout_22);

        horizontalLayout_20 = new QHBoxLayout();
        horizontalLayout_20->setObjectName(QString::fromUtf8("horizontalLayout_20"));
        labelTcgGuessShort = new QLabel(groupBoxTcgShortParameters);
        labelTcgGuessShort->setObjectName(QString::fromUtf8("labelTcgGuessShort"));

        horizontalLayout_20->addWidget(labelTcgGuessShort);

        comboBoxTcgGuessShort = new QComboBox(groupBoxTcgShortParameters);
        comboBoxTcgGuessShort->addItem(QString());
        comboBoxTcgGuessShort->addItem(QString());
        comboBoxTcgGuessShort->setObjectName(QString::fromUtf8("comboBoxTcgGuessShort"));

        horizontalLayout_20->addWidget(comboBoxTcgGuessShort);


        verticalLayout_5->addLayout(horizontalLayout_20);

        horizontalLayout_21 = new QHBoxLayout();
        horizontalLayout_21->setObjectName(QString::fromUtf8("horizontalLayout_21"));
        labelTcgPeekShort = new QLabel(groupBoxTcgShortParameters);
        labelTcgPeekShort->setObjectName(QString::fromUtf8("labelTcgPeekShort"));

        horizontalLayout_21->addWidget(labelTcgPeekShort);

        comboBoxTcgPeekShort = new QComboBox(groupBoxTcgShortParameters);
        comboBoxTcgPeekShort->addItem(QString());
        comboBoxTcgPeekShort->addItem(QString());
        comboBoxTcgPeekShort->setObjectName(QString::fromUtf8("comboBoxTcgPeekShort"));

        horizontalLayout_21->addWidget(comboBoxTcgPeekShort);


        verticalLayout_5->addLayout(horizontalLayout_21);

        horizontalLayout_19 = new QHBoxLayout();
        horizontalLayout_19->setObjectName(QString::fromUtf8("horizontalLayout_19"));
        labelTcgOmegaShort = new QLabel(groupBoxTcgShortParameters);
        labelTcgOmegaShort->setObjectName(QString::fromUtf8("labelTcgOmegaShort"));

        horizontalLayout_19->addWidget(labelTcgOmegaShort);

        spinBoxTcgOmegaShort = new QSpinBox(groupBoxTcgShortParameters);
        spinBoxTcgOmegaShort->setObjectName(QString::fromUtf8("spinBoxTcgOmegaShort"));
        spinBoxTcgOmegaShort->setValue(1);

        horizontalLayout_19->addWidget(spinBoxTcgOmegaShort);


        verticalLayout_5->addLayout(horizontalLayout_19);


        verticalLayout_6->addWidget(groupBoxTcgShortParameters);


        verticalLayout->addWidget(widgetDynamic);


        gridLayout->addWidget(groupBoxDynamic, 1, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        pushButtonOpenKeyfile = new QPushButton(dockWidgetContents);
        pushButtonOpenKeyfile->setObjectName(QString::fromUtf8("pushButtonOpenKeyfile"));

        horizontalLayout_2->addWidget(pushButtonOpenKeyfile);


        gridLayout->addLayout(horizontalLayout_2, 0, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 3, 0, 1, 1);

        gridLayout->setRowStretch(3, 1);
        DockWidgetKeywordEditor->setWidget(dockWidgetContents);

        retranslateUi(DockWidgetKeywordEditor);
        QObject::connect(pushButtonOpenKeyfile, SIGNAL(clicked()), DockWidgetKeywordEditor, SLOT(onOpenKeyfilePushButtonClicked()));
        QObject::connect(comboBoxPolarizationEquations, SIGNAL(currentTextChanged(QString)), DockWidgetKeywordEditor, SLOT(comboBoxPolEquaIndexChanged(QString)));
        QObject::connect(comboBoxTcgPeek, SIGNAL(currentTextChanged(QString)), DockWidgetKeywordEditor, SLOT(comboBoxPeekStepIndexChanged(QString)));
        QObject::connect(comboBoxTcgMegaFit, SIGNAL(currentTextChanged(QString)), DockWidgetKeywordEditor, SLOT(comboBoxFittingIndexChanged(QString)));

        comboBoxThermostat->setCurrentIndex(2);
        comboBoxBarostat->setCurrentIndex(0);
        comboBoxPolarizationEquations->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(DockWidgetKeywordEditor);
    } // setupUi

    void retranslateUi(QDockWidget *DockWidgetKeywordEditor)
    {
        DockWidgetKeywordEditor->setWindowTitle(QCoreApplication::translate("DockWidgetKeywordEditor", "Tinker-hp - Keyword Editor", nullptr));
        groupBoxAdditionnalKeywords->setTitle(QCoreApplication::translate("DockWidgetKeywordEditor", "Additionnal Keywords", nullptr));
        groupBoxDynamic->setTitle(QCoreApplication::translate("DockWidgetKeywordEditor", "Dynamic", nullptr));
#if QT_CONFIG(tooltip)
        labelIntegrator->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p><span style=\" font-size:9pt;\">Integrators: </span></p><p><span style=\" font-size:9pt; font-weight:600;\">BEEMAN</span><span style=\" font-size:9pt;\">: The default one</span></p><p><span style=\" font-size:9pt; font-weight:600;\">VERLET</span><span style=\" font-size:9pt;\">: Verlet</span></p><p><span style=\" font-size:9pt; font-weight:600;\">BBK</span><span style=\" font-size:9pt;\">: Langevin Dynamics for constant temperature simulations</span></p><p><span style=\" font-size:9pt; font-weight:600;\">BAOAB</span><span style=\" font-size:9pt;\">: Langevin Dynamics for constant temperature simulations</span></p><p><span style=\" font-size:9pt; font-weight:600;\">RESPA</span><span style=\" font-size:9pt;\">: Bonded/non bonded respa-split with a velocity-verlet inner loop and with a 0.25 fs default timestep for the inner loop</span></p><p><span style=\" font-size:9pt; font-weight:600;\">BAOABRESPA</span><span style=\" font-size:9pt;\">: Bonded/non bonded respa-split for Langevin dynamics "
                        "with a BAOAB inner loop, the default timestep for the inner loop is also 0.25 fs</span></p><p><span style=\" font-size:9pt; font-weight:600;\">RESPA1</span><span style=\" font-size:9pt;\">: (Bonded)/(short range non bonded)/(long range non bonded) three level respa1-split with a velocity verlet inner loop. The default timesteps are 0.25 fs for the inner loop and 2 fs for the intermediate one</span></p><p><span style=\" font-size:9pt; font-weight:600;\">BAOABRESPA1</span><span style=\" font-size:9pt;\">: (Bonded)/(short range non bonded)/(long range non bonded) three level respa1-split for Langevin dynamics with a BAOAB inner loop. The default timesteps are 0.25 fs for the inner loop and 2 fs for the intermediate one</span></p><p><span style=\" font-size:9pt; font-weight:600;\">BAOABPISTON</span><span style=\" font-size:9pt;\">: Constant pressure BAOAB Langevin dynamics with a Langevin Piston pressure control and a BAOAB evolution of the volume extended variable. The default mass of the piston is 2e</span><span"
                        " style=\" font-size:9pt; vertical-align:super;\">-5</span><span style=\" font-size:9pt;\"> atomic units and the default friction for the piston is 20.0 ps</span><span style=\" font-size:9pt; vertical-align:super;\">-1</span><span style=\" font-size:9pt;\">.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelIntegrator->setText(QCoreApplication::translate("DockWidgetKeywordEditor", "Integrator", nullptr));
        comboBoxIntegrator->setItemText(0, QCoreApplication::translate("DockWidgetKeywordEditor", "BEEMAN", nullptr));
        comboBoxIntegrator->setItemText(1, QCoreApplication::translate("DockWidgetKeywordEditor", "VERLET", nullptr));
        comboBoxIntegrator->setItemText(2, QCoreApplication::translate("DockWidgetKeywordEditor", "BBK", nullptr));
        comboBoxIntegrator->setItemText(3, QCoreApplication::translate("DockWidgetKeywordEditor", "BAOAB", nullptr));
        comboBoxIntegrator->setItemText(4, QCoreApplication::translate("DockWidgetKeywordEditor", "RESPA", nullptr));
        comboBoxIntegrator->setItemText(5, QCoreApplication::translate("DockWidgetKeywordEditor", "BAOABRESPA", nullptr));
        comboBoxIntegrator->setItemText(6, QCoreApplication::translate("DockWidgetKeywordEditor", "RESPA1", nullptr));
        comboBoxIntegrator->setItemText(7, QCoreApplication::translate("DockWidgetKeywordEditor", "BAOABRESPA1", nullptr));
        comboBoxIntegrator->setItemText(8, QCoreApplication::translate("DockWidgetKeywordEditor", "BAOABPISTON", nullptr));

#if QT_CONFIG(tooltip)
        comboBoxIntegrator->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p><span style=\" font-size:9pt;\">Integrators: </span></p><p><span style=\" font-size:9pt; font-weight:600;\">BEEMAN</span><span style=\" font-size:9pt;\">: The default one</span></p><p><span style=\" font-size:9pt; font-weight:600;\">VERLET</span><span style=\" font-size:9pt;\">: Verlet</span></p><p><span style=\" font-size:9pt; font-weight:600;\">BBK</span><span style=\" font-size:9pt;\">: Langevin Dynamics for constant temperature simulations</span></p><p><span style=\" font-size:9pt; font-weight:600;\">BAOAB</span><span style=\" font-size:9pt;\">: Langevin Dynamics for constant temperature simulations</span></p><p><span style=\" font-size:9pt; font-weight:600;\">RESPA</span><span style=\" font-size:9pt;\">: Bonded/non bonded respa-split with a velocity-verlet inner loop and with a 0.25 fs default timestep for the inner loop</span></p><p><span style=\" font-size:9pt; font-weight:600;\">BAOABRESPA</span><span style=\" font-size:9pt;\">: Bonded/non bonded respa-split for Langevin dynamics "
                        "with a BAOAB inner loop, the default timestep for the inner loop is also 0.25 fs</span></p><p><span style=\" font-size:9pt; font-weight:600;\">RESPA1</span><span style=\" font-size:9pt;\">: (Bonded)/(short range non bonded)/(long range non bonded) three level respa1-split with a velocity verlet inner loop. The default timesteps are 0.25 fs for the inner loop and 2 fs for the intermediate one</span></p><p><span style=\" font-size:9pt; font-weight:600;\">BAOABRESPA1</span><span style=\" font-size:9pt;\">: (Bonded)/(short range non bonded)/(long range non bonded) three level respa1-split for Langevin dynamics with a BAOAB inner loop. The default timesteps are 0.25 fs for the inner loop and 2 fs for the intermediate one</span></p><p><span style=\" font-size:9pt; font-weight:600;\">BAOABPISTON</span><span style=\" font-size:9pt;\">: Constant pressure BAOAB Langevin dynamics with a Langevin Piston pressure control and a BAOAB evolution of the volume extended variable. The default mass of the piston is 2e</span><span"
                        " style=\" font-size:9pt; vertical-align:super;\">-5</span><span style=\" font-size:9pt;\"> atomic units and the default friction for the piston is 20.0 ps</span><span style=\" font-size:9pt; vertical-align:super;\">-1</span><span style=\" font-size:9pt;\">.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        labelFriction->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p><span style=\" font-size:9pt;\">Friction of all the Langevin integrators (</span><span style=\" font-size:9pt; font-weight:600;\">BBK</span><span style=\" font-size:9pt;\">, </span><span style=\" font-size:9pt; font-weight:600;\">BAOAB</span><span style=\" font-size:9pt;\">, </span><span style=\" font-size:9pt; font-weight:600;\">BAOABRESPA</span><span style=\" font-size:9pt;\">, </span><span style=\" font-size:9pt; font-weight:600;\">BAOABRESPA1</span><span style=\" font-size:9pt;\"> and </span><span style=\" font-size:9pt; font-weight:600;\">BAOABPISTON</span><span style=\" font-size:9pt;\">), the default being 1 ps</span><span style=\" font-size:9pt; vertical-align:super;\">-1</span><span style=\" font-size:9pt;\">.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelFriction->setText(QCoreApplication::translate("DockWidgetKeywordEditor", "Friction", nullptr));
#if QT_CONFIG(tooltip)
        spinBoxFriction->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p><span style=\" font-size:9pt;\">Friction of all the Langevin integrators (</span><span style=\" font-size:9pt; font-weight:600;\">BBK</span><span style=\" font-size:9pt;\">, </span><span style=\" font-size:9pt; font-weight:600;\">BAOAB</span><span style=\" font-size:9pt;\">, </span><span style=\" font-size:9pt; font-weight:600;\">BAOABRESPA</span><span style=\" font-size:9pt;\">, </span><span style=\" font-size:9pt; font-weight:600;\">BAOABRESPA1</span><span style=\" font-size:9pt;\"> and </span><span style=\" font-size:9pt; font-weight:600;\">BAOABPISTON</span><span style=\" font-size:9pt;\">), the default being 1 ps</span><span style=\" font-size:9pt; vertical-align:super;\">-1</span><span style=\" font-size:9pt;\">.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        spinBoxFriction->setSuffix(QString());
#if QT_CONFIG(tooltip)
        labelInnerTimestep->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p><span style=\" font-size:9pt;\">The inner timestep for </span><span style=\" font-size:9pt; font-weight:600;\">RESPA</span><span style=\" font-size:9pt;\">, </span><span style=\" font-size:9pt; font-weight:600;\">BAOABRESPA</span><span style=\" font-size:9pt;\">, </span><span style=\" font-size:9pt; font-weight:600;\">RESPA1</span><span style=\" font-size:9pt;\"> and </span><span style=\" font-size:9pt; font-weight:600;\">BAOABRESPA1.</span><span style=\" font-size:9pt;\"> Value in ps.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelInnerTimestep->setText(QCoreApplication::translate("DockWidgetKeywordEditor", "Inner timestep", nullptr));
#if QT_CONFIG(tooltip)
        spinBoxInnerTimestep->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p><span style=\" font-size:9pt;\">The inner timestep for </span><span style=\" font-size:9pt; font-weight:600;\">RESPA</span><span style=\" font-size:9pt;\">, </span><span style=\" font-size:9pt; font-weight:600;\">BAOABRESPA</span><span style=\" font-size:9pt;\">, </span><span style=\" font-size:9pt; font-weight:600;\">RESPA1</span><span style=\" font-size:9pt;\"> and </span><span style=\" font-size:9pt; font-weight:600;\">BAOABRESPA1.</span><span style=\" font-size:9pt;\"> Value in ps.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        labelIntermediateTimestep->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p>Set the intermediate timestep for <span style=\" font-weight:600;\">RESPA1</span> and <span style=\" font-weight:600;\">BAOABRESPA1</span>. value in ps.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelIntermediateTimestep->setText(QCoreApplication::translate("DockWidgetKeywordEditor", "Intermediate timestep", nullptr));
#if QT_CONFIG(tooltip)
        spinBoxIntermediateTimestep->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p>Set the intermediate timestep for <span style=\" font-weight:600;\">RESPA1</span> and <span style=\" font-weight:600;\">BAOABRESPA1</span>. value in ps.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        labelThermostat->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p>The thermostats available in Tinker-HP are <span style=\" font-weight:600;\">Berendsen</span>, <span style=\" font-weight:600;\">Bussi</span> (which is the default one) and <span style=\" font-weight:600;\">Andersen</span>.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelThermostat->setText(QCoreApplication::translate("DockWidgetKeywordEditor", "Thermostat", nullptr));
        comboBoxThermostat->setItemText(0, QCoreApplication::translate("DockWidgetKeywordEditor", "Andersen", nullptr));
        comboBoxThermostat->setItemText(1, QCoreApplication::translate("DockWidgetKeywordEditor", "Berendsen", nullptr));
        comboBoxThermostat->setItemText(2, QCoreApplication::translate("DockWidgetKeywordEditor", "Bussi", nullptr));

#if QT_CONFIG(tooltip)
        comboBoxThermostat->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p>The thermostats available in Tinker-HP are <span style=\" font-weight:600;\">Berendsen</span>, <span style=\" font-weight:600;\">Bussi</span> (which is the default one) and <span style=\" font-weight:600;\">Andersen</span>.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        labelBarostat->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p>The barostats available in Tinker-HP are the <span style=\" font-weight:600;\">Berendsen</span> (which is the default one) and the <span style=\" font-weight:600;\">Monte-Carlo</span> one</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelBarostat->setText(QCoreApplication::translate("DockWidgetKeywordEditor", "Barostat", nullptr));
        comboBoxBarostat->setItemText(0, QCoreApplication::translate("DockWidgetKeywordEditor", "Berendsen", nullptr));
        comboBoxBarostat->setItemText(1, QCoreApplication::translate("DockWidgetKeywordEditor", "Monte-Carlo", nullptr));

#if QT_CONFIG(tooltip)
        comboBoxBarostat->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p>The barostats available in Tinker-HP are the <span style=\" font-weight:600;\">Berendsen</span> (which is the default one) and the <span style=\" font-weight:600;\">Monte-Carlo</span> one</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        labelPolarizationEquations->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p>The algorithm used to converge the polarization equations (compute the dipoles solution):</p><p><span style=\" font-weight:600;\">Divide and Conquer Jacobi/DIIS</span> (default)</p><p><span style=\" font-weight:600;\">PCG</span> (Conjugate Gradient with a diagonal preconditioner)</p><p><span style=\" font-weight:600;\">Jacobi/DIIS</span></p><p><span style=\" font-weight:600;\">TCG</span> (Truncated Conjugate Gradient)</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelPolarizationEquations->setText(QCoreApplication::translate("DockWidgetKeywordEditor", "Polarization Equations", nullptr));
        comboBoxPolarizationEquations->setItemText(0, QCoreApplication::translate("DockWidgetKeywordEditor", "D&C Jacobi/DIIS", nullptr));
        comboBoxPolarizationEquations->setItemText(1, QCoreApplication::translate("DockWidgetKeywordEditor", "PCG", nullptr));
        comboBoxPolarizationEquations->setItemText(2, QCoreApplication::translate("DockWidgetKeywordEditor", "Jacobi/DIIS", nullptr));
        comboBoxPolarizationEquations->setItemText(3, QCoreApplication::translate("DockWidgetKeywordEditor", "TCG", nullptr));

#if QT_CONFIG(tooltip)
        comboBoxPolarizationEquations->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p>The algorithm used to converge the polarization equations (compute the dipoles solution):</p><p><span style=\" font-weight:600;\">Divide and Conquer Jacobi/DIIS</span> (default)</p><p><span style=\" font-weight:600;\">PCG</span> (Conjugate Gradient with a diagonal preconditioner)</p><p><span style=\" font-weight:600;\">Jacobi/DIIS</span></p><p><span style=\" font-weight:600;\">TCG</span> (Truncated Conjugate Gradient)</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        groupBoxTcgParameters->setTitle(QCoreApplication::translate("DockWidgetKeywordEditor", "TCG parameters", nullptr));
#if QT_CONFIG(tooltip)
        labelTcgOrder->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p><span style=\" font-size:9pt;\">Order of the TCG truncation, TCG1 or TCG2, default is TCG2.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelTcgOrder->setText(QCoreApplication::translate("DockWidgetKeywordEditor", "TCG order", nullptr));
        comboBoxTcgOrder->setItemText(0, QCoreApplication::translate("DockWidgetKeywordEditor", "TCG2", nullptr));
        comboBoxTcgOrder->setItemText(1, QCoreApplication::translate("DockWidgetKeywordEditor", "TCG1", nullptr));

#if QT_CONFIG(tooltip)
        comboBoxTcgOrder->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p><span style=\" font-size:9pt;\">Order of the TCG truncation, TCG1 or TCG2, default is TCG2.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        labelDiagPrec->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p><span style=\" font-size:9pt;\">Use of a diagonal preconditioner, default is yes.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelDiagPrec->setText(QCoreApplication::translate("DockWidgetKeywordEditor", "Diagonal preconditioner", nullptr));
        comboBoxDiagPrec->setItemText(0, QCoreApplication::translate("DockWidgetKeywordEditor", "Yes", nullptr));
        comboBoxDiagPrec->setItemText(1, QCoreApplication::translate("DockWidgetKeywordEditor", "No", nullptr));

#if QT_CONFIG(tooltip)
        comboBoxDiagPrec->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p><span style=\" font-size:9pt;\">Use of a diagonal preconditioner, default is yes.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        labelTcgGuess->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p><span style=\" font-size:9pt;\">Use of a \342\200\235direct guess\342\200\235 (polarizability x permanent electric field) as a guess., default is no.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelTcgGuess->setText(QCoreApplication::translate("DockWidgetKeywordEditor", "Direct guess", nullptr));
        comboBoxTcgGuess->setItemText(0, QCoreApplication::translate("DockWidgetKeywordEditor", "No", nullptr));
        comboBoxTcgGuess->setItemText(1, QCoreApplication::translate("DockWidgetKeywordEditor", "Yes", nullptr));

#if QT_CONFIG(tooltip)
        comboBoxTcgGuess->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p><span style=\" font-size:9pt;\">Use of a \342\200\235direct guess\342\200\235 (polarizability x permanent electric field) as a guess., default is no.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        labelTcgPeek->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p><span style=\" font-size:9pt;\">Use of a peek step, default is yes. When a peek step is used, a Jacobi Over Relaxation (JOR) is applied to the TCG values of the dipoles with a parameter </span><span style=\" font-family:'Arial Unicode MS','DejaVu Sans','Athena','Gentium','Palatino Linotype','Lucida Sans Unicode','Lucida Grande','Code2000','sans-serif'; font-size:14px; color:#202122; background-color:#fcfcfc;\">\317\211.</span><span style=\" font-size:9pt;\"> By default, this value is </span><span style=\" font-family:'Arial Unicode MS','DejaVu Sans','Athena','Gentium','Palatino Linotype','Lucida Sans Unicode','Lucida Grande','Code2000','sans-serif'; font-size:14px; color:#202122; background-color:#fcfcfc;\">\317\211</span><span style=\" font-size:9pt;\"> = 1. (regular Jacobi step) but the following keywords can modify this.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelTcgPeek->setText(QCoreApplication::translate("DockWidgetKeywordEditor", "Peek step", nullptr));
        comboBoxTcgPeek->setItemText(0, QCoreApplication::translate("DockWidgetKeywordEditor", "Yes", nullptr));
        comboBoxTcgPeek->setItemText(1, QCoreApplication::translate("DockWidgetKeywordEditor", "No", nullptr));

#if QT_CONFIG(tooltip)
        comboBoxTcgPeek->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p><span style=\" font-size:9pt;\">Use of a peek step, default is yes. When a peek step is used, a Jacobi Over Relaxation (JOR) is applied to the TCG values of the dipoles with a parameter </span><span style=\" font-family:'Arial Unicode MS','DejaVu Sans','Athena','Gentium','Palatino Linotype','Lucida Sans Unicode','Lucida Grande','Code2000','sans-serif'; font-size:14px; color:#202122; background-color:#fcfcfc;\">\317\211.</span><span style=\" font-size:9pt;\"> By default, this value is </span><span style=\" font-family:'Arial Unicode MS','DejaVu Sans','Athena','Gentium','Palatino Linotype','Lucida Sans Unicode','Lucida Grande','Code2000','sans-serif'; font-size:14px; color:#202122; background-color:#fcfcfc;\">\317\211</span><span style=\" font-size:9pt;\"> = 1. (regular Jacobi step) but the following keywords can modify this.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelTcgOmega->setText(QCoreApplication::translate("DockWidgetKeywordEditor", "Omega ( \317\211 )", nullptr));
        labelTcgMegaFit->setText(QCoreApplication::translate("DockWidgetKeywordEditor", "Fitting", nullptr));
        comboBoxTcgMegaFit->setItemText(0, QCoreApplication::translate("DockWidgetKeywordEditor", "Yes", nullptr));
        comboBoxTcgMegaFit->setItemText(1, QCoreApplication::translate("DockWidgetKeywordEditor", "No", nullptr));

        labelTcgMegaFitFreq->setText(QCoreApplication::translate("DockWidgetKeywordEditor", "Fitting frequency", nullptr));
        groupBoxTcgShortParameters->setTitle(QCoreApplication::translate("DockWidgetKeywordEditor", "TCG parameters for short range polarization", nullptr));
        labelTcgOrderShort->setText(QCoreApplication::translate("DockWidgetKeywordEditor", "TCG order", nullptr));
        comboBox->setItemText(0, QCoreApplication::translate("DockWidgetKeywordEditor", "TCG2", nullptr));
        comboBox->setItemText(1, QCoreApplication::translate("DockWidgetKeywordEditor", "TCG1", nullptr));

#if QT_CONFIG(tooltip)
        labelDiagPrecShort->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p><span style=\" font-size:9pt;\">Use of a diagonal preconditioner, default is yes.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelDiagPrecShort->setText(QCoreApplication::translate("DockWidgetKeywordEditor", "Diagonal preconditioner", nullptr));
        comboBoxDiagPrecShort->setItemText(0, QCoreApplication::translate("DockWidgetKeywordEditor", "Yes", nullptr));
        comboBoxDiagPrecShort->setItemText(1, QCoreApplication::translate("DockWidgetKeywordEditor", "No", nullptr));

#if QT_CONFIG(tooltip)
        comboBoxDiagPrecShort->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p><span style=\" font-size:9pt;\">Use of a diagonal preconditioner, default is yes.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        labelTcgGuessShort->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p><span style=\" font-size:9pt;\">Use of a \342\200\235direct guess\342\200\235 (polarizability x permanent electric field) as a guess., default is no.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelTcgGuessShort->setText(QCoreApplication::translate("DockWidgetKeywordEditor", "Direct guess", nullptr));
        comboBoxTcgGuessShort->setItemText(0, QCoreApplication::translate("DockWidgetKeywordEditor", "No", nullptr));
        comboBoxTcgGuessShort->setItemText(1, QCoreApplication::translate("DockWidgetKeywordEditor", "Yes", nullptr));

#if QT_CONFIG(tooltip)
        comboBoxTcgGuessShort->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p><span style=\" font-size:9pt;\">Use of a \342\200\235direct guess\342\200\235 (polarizability x permanent electric field) as a guess., default is no.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        labelTcgPeekShort->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p><span style=\" font-size:9pt;\">Use of a peek step, default is yes. When a peek step is used, a Jacobi Over Relaxation (JOR) is applied to the TCG values of the dipoles with a parameter </span><span style=\" font-family:'Arial Unicode MS','DejaVu Sans','Athena','Gentium','Palatino Linotype','Lucida Sans Unicode','Lucida Grande','Code2000','sans-serif'; font-size:14px; color:#202122; background-color:#fcfcfc;\">\317\211.</span><span style=\" font-size:9pt;\"> By default, this value is </span><span style=\" font-family:'Arial Unicode MS','DejaVu Sans','Athena','Gentium','Palatino Linotype','Lucida Sans Unicode','Lucida Grande','Code2000','sans-serif'; font-size:14px; color:#202122; background-color:#fcfcfc;\">\317\211</span><span style=\" font-size:9pt;\"> = 1. (regular Jacobi step) but the following keywords can modify this.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelTcgPeekShort->setText(QCoreApplication::translate("DockWidgetKeywordEditor", "Peek step", nullptr));
        comboBoxTcgPeekShort->setItemText(0, QCoreApplication::translate("DockWidgetKeywordEditor", "Yes", nullptr));
        comboBoxTcgPeekShort->setItemText(1, QCoreApplication::translate("DockWidgetKeywordEditor", "No", nullptr));

#if QT_CONFIG(tooltip)
        comboBoxTcgPeekShort->setToolTip(QCoreApplication::translate("DockWidgetKeywordEditor", "<html><head/><body><p><span style=\" font-size:9pt;\">Use of a peek step, default is yes. When a peek step is used, a Jacobi Over Relaxation (JOR) is applied to the TCG values of the dipoles with a parameter </span><span style=\" font-family:'Arial Unicode MS','DejaVu Sans','Athena','Gentium','Palatino Linotype','Lucida Sans Unicode','Lucida Grande','Code2000','sans-serif'; font-size:14px; color:#202122; background-color:#fcfcfc;\">\317\211.</span><span style=\" font-size:9pt;\"> By default, this value is </span><span style=\" font-family:'Arial Unicode MS','DejaVu Sans','Athena','Gentium','Palatino Linotype','Lucida Sans Unicode','Lucida Grande','Code2000','sans-serif'; font-size:14px; color:#202122; background-color:#fcfcfc;\">\317\211</span><span style=\" font-size:9pt;\"> = 1. (regular Jacobi step) but the following keywords can modify this.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelTcgOmegaShort->setText(QCoreApplication::translate("DockWidgetKeywordEditor", "Omega (\317\211)", nullptr));
        pushButtonOpenKeyfile->setText(QCoreApplication::translate("DockWidgetKeywordEditor", "Open", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DockWidgetKeywordEditor: public Ui_DockWidgetKeywordEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KEYWORD_EDITOR_MAIN_WINDOW_H
