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
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <ui/widget/custom_widget/collapsing_groupbox_widget.hpp>

QT_BEGIN_NAMESPACE

class Ui_DockWidget
{
public:
    QWidget *dockWidgetContents;
    QGridLayout *gridLayout;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QGridLayout *gridLayout_2;
    QTabWidget *tabWidgetKeywordEditor;
    QWidget *tabDynamic;
    QVBoxLayout *verticalLayout_9;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButtonOpenKeyfile;
    QWidget *widgetDynamic;
    QVBoxLayout *verticalLayout_6;
    QHBoxLayout *horizontalLayout_3;
    QLabel *labelIntegrator;
    QComboBox *comboBoxIntegrator;
    QLabel *labelIntegratorInfo;
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
    QHBoxLayout *horizontalLayout_23;
    QLabel *labelShortRangePolarizationSolver;
    QComboBox *comboBoxShortRangePolarizationSolver;
    QHBoxLayout *horizontalLayout_7;
    QLabel *labelMassPiston;
    QSpinBox *spinBoxMassPiston;
    QHBoxLayout *horizontalLayout_24;
    QLabel *labelFrictionPiston;
    QSpinBox *spinBoxFrictionPiston;
    CollapsingGroupboxWidget *groupBoxTcgParameters;
    QVBoxLayout *verticalLayout_7;
    QWidget *widgetContainingTcgParameters;
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
    QWidget *widgetTcgParametersShort;
    QVBoxLayout *verticalLayout_8;
    QHBoxLayout *horizontalLayout_18;
    QLabel *labelTcgOrderShort;
    QComboBox *comboBoxTcgOrderShort;
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
    CollapsingGroupboxWidget *groupBoxAdditionnalKeywords;
    QVBoxLayout *verticalLayout_3;
    QTextBrowser *textBrowserAdditionnalKeywords;
    QPushButton *pushButtonGenerate;
    QSpacerItem *verticalSpacer;
    QWidget *tab_2;

    void setupUi(QDockWidget *DockWidget)
    {
        if (DockWidget->objectName().isEmpty())
            DockWidget->setObjectName(QString::fromUtf8("DockWidget"));
        DockWidget->resize(400, 1008);
        DockWidget->setFloating(true);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        gridLayout = new QGridLayout(dockWidgetContents);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        scrollArea = new QScrollArea(dockWidgetContents);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 382, 971));
        gridLayout_2 = new QGridLayout(scrollAreaWidgetContents);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        tabWidgetKeywordEditor = new QTabWidget(scrollAreaWidgetContents);
        tabWidgetKeywordEditor->setObjectName(QString::fromUtf8("tabWidgetKeywordEditor"));
        tabDynamic = new QWidget();
        tabDynamic->setObjectName(QString::fromUtf8("tabDynamic"));
        verticalLayout_9 = new QVBoxLayout(tabDynamic);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        pushButtonOpenKeyfile = new QPushButton(tabDynamic);
        pushButtonOpenKeyfile->setObjectName(QString::fromUtf8("pushButtonOpenKeyfile"));

        horizontalLayout_2->addWidget(pushButtonOpenKeyfile);


        verticalLayout_9->addLayout(horizontalLayout_2);

        widgetDynamic = new QWidget(tabDynamic);
        widgetDynamic->setObjectName(QString::fromUtf8("widgetDynamic"));
        verticalLayout_6 = new QVBoxLayout(widgetDynamic);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(5);
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
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(comboBoxIntegrator->sizePolicy().hasHeightForWidth());
        comboBoxIntegrator->setSizePolicy(sizePolicy);
        comboBoxIntegrator->setMinimumSize(QSize(140, 0));
        comboBoxIntegrator->setMaximumSize(QSize(140, 16777215));

        horizontalLayout_3->addWidget(comboBoxIntegrator);

        labelIntegratorInfo = new QLabel(widgetDynamic);
        labelIntegratorInfo->setObjectName(QString::fromUtf8("labelIntegratorInfo"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(labelIntegratorInfo->sizePolicy().hasHeightForWidth());
        labelIntegratorInfo->setSizePolicy(sizePolicy1);
        labelIntegratorInfo->setMinimumSize(QSize(20, 19));

        horizontalLayout_3->addWidget(labelIntegratorInfo);

        horizontalLayout_3->setStretch(0, 1);

        verticalLayout_6->addLayout(horizontalLayout_3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        labelFriction = new QLabel(widgetDynamic);
        labelFriction->setObjectName(QString::fromUtf8("labelFriction"));

        horizontalLayout->addWidget(labelFriction);

        spinBoxFriction = new QSpinBox(widgetDynamic);
        spinBoxFriction->setObjectName(QString::fromUtf8("spinBoxFriction"));
        QSizePolicy sizePolicy2(QSizePolicy::Maximum, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(spinBoxFriction->sizePolicy().hasHeightForWidth());
        spinBoxFriction->setSizePolicy(sizePolicy2);
        spinBoxFriction->setMinimumSize(QSize(165, 0));
        spinBoxFriction->setMaximumSize(QSize(165, 16777215));
        spinBoxFriction->setValue(1);

        horizontalLayout->addWidget(spinBoxFriction);

        horizontalLayout->setStretch(0, 1);

        verticalLayout_6->addLayout(horizontalLayout);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        labelInnerTimestep = new QLabel(widgetDynamic);
        labelInnerTimestep->setObjectName(QString::fromUtf8("labelInnerTimestep"));

        horizontalLayout_4->addWidget(labelInnerTimestep);

        spinBoxInnerTimestep = new QSpinBox(widgetDynamic);
        spinBoxInnerTimestep->setObjectName(QString::fromUtf8("spinBoxInnerTimestep"));
        spinBoxInnerTimestep->setMinimumSize(QSize(165, 0));
        spinBoxInnerTimestep->setMaximumSize(QSize(165, 16777215));
        spinBoxInnerTimestep->setValue(1);

        horizontalLayout_4->addWidget(spinBoxInnerTimestep);

        horizontalLayout_4->setStretch(0, 1);

        verticalLayout_6->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        labelIntermediateTimestep = new QLabel(widgetDynamic);
        labelIntermediateTimestep->setObjectName(QString::fromUtf8("labelIntermediateTimestep"));

        horizontalLayout_5->addWidget(labelIntermediateTimestep);

        spinBoxIntermediateTimestep = new QSpinBox(widgetDynamic);
        spinBoxIntermediateTimestep->setObjectName(QString::fromUtf8("spinBoxIntermediateTimestep"));
        spinBoxIntermediateTimestep->setMinimumSize(QSize(165, 0));
        spinBoxIntermediateTimestep->setMaximumSize(QSize(165, 16777215));
        spinBoxIntermediateTimestep->setValue(1);

        horizontalLayout_5->addWidget(spinBoxIntermediateTimestep);

        horizontalLayout_5->setStretch(0, 1);

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
        comboBoxThermostat->setMinimumSize(QSize(165, 0));
        comboBoxThermostat->setMaximumSize(QSize(165, 16777215));

        horizontalLayout_8->addWidget(comboBoxThermostat);

        horizontalLayout_8->setStretch(0, 1);

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
        comboBoxBarostat->setMinimumSize(QSize(165, 0));
        comboBoxBarostat->setMaximumSize(QSize(165, 16777215));

        horizontalLayout_9->addWidget(comboBoxBarostat);

        horizontalLayout_9->setStretch(0, 1);

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
        comboBoxPolarizationEquations->setMinimumSize(QSize(165, 0));
        comboBoxPolarizationEquations->setMaximumSize(QSize(165, 16777215));

        horizontalLayout_10->addWidget(comboBoxPolarizationEquations);

        horizontalLayout_10->setStretch(0, 1);

        verticalLayout_6->addLayout(horizontalLayout_10);

        horizontalLayout_23 = new QHBoxLayout();
        horizontalLayout_23->setObjectName(QString::fromUtf8("horizontalLayout_23"));
        labelShortRangePolarizationSolver = new QLabel(widgetDynamic);
        labelShortRangePolarizationSolver->setObjectName(QString::fromUtf8("labelShortRangePolarizationSolver"));

        horizontalLayout_23->addWidget(labelShortRangePolarizationSolver);

        comboBoxShortRangePolarizationSolver = new QComboBox(widgetDynamic);
        comboBoxShortRangePolarizationSolver->addItem(QString());
        comboBoxShortRangePolarizationSolver->addItem(QString());
        comboBoxShortRangePolarizationSolver->addItem(QString());
        comboBoxShortRangePolarizationSolver->addItem(QString());
        comboBoxShortRangePolarizationSolver->setObjectName(QString::fromUtf8("comboBoxShortRangePolarizationSolver"));
        comboBoxShortRangePolarizationSolver->setMinimumSize(QSize(165, 0));
        comboBoxShortRangePolarizationSolver->setMaximumSize(QSize(165, 16777215));

        horizontalLayout_23->addWidget(comboBoxShortRangePolarizationSolver);

        horizontalLayout_23->setStretch(0, 1);

        verticalLayout_6->addLayout(horizontalLayout_23);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        labelMassPiston = new QLabel(widgetDynamic);
        labelMassPiston->setObjectName(QString::fromUtf8("labelMassPiston"));

        horizontalLayout_7->addWidget(labelMassPiston);

        spinBoxMassPiston = new QSpinBox(widgetDynamic);
        spinBoxMassPiston->setObjectName(QString::fromUtf8("spinBoxMassPiston"));
        spinBoxMassPiston->setMinimumSize(QSize(165, 0));
        spinBoxMassPiston->setMaximumSize(QSize(165, 16777215));

        horizontalLayout_7->addWidget(spinBoxMassPiston);

        horizontalLayout_7->setStretch(0, 1);

        verticalLayout_6->addLayout(horizontalLayout_7);

        horizontalLayout_24 = new QHBoxLayout();
        horizontalLayout_24->setObjectName(QString::fromUtf8("horizontalLayout_24"));
        labelFrictionPiston = new QLabel(widgetDynamic);
        labelFrictionPiston->setObjectName(QString::fromUtf8("labelFrictionPiston"));

        horizontalLayout_24->addWidget(labelFrictionPiston);

        spinBoxFrictionPiston = new QSpinBox(widgetDynamic);
        spinBoxFrictionPiston->setObjectName(QString::fromUtf8("spinBoxFrictionPiston"));
        spinBoxFrictionPiston->setMinimumSize(QSize(165, 0));
        spinBoxFrictionPiston->setMaximumSize(QSize(165, 16777215));

        horizontalLayout_24->addWidget(spinBoxFrictionPiston);

        horizontalLayout_24->setStretch(0, 1);

        verticalLayout_6->addLayout(horizontalLayout_24);

        groupBoxTcgParameters = new CollapsingGroupboxWidget(widgetDynamic);
        groupBoxTcgParameters->setObjectName(QString::fromUtf8("groupBoxTcgParameters"));
        verticalLayout_7 = new QVBoxLayout(groupBoxTcgParameters);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        widgetContainingTcgParameters = new QWidget(groupBoxTcgParameters);
        widgetContainingTcgParameters->setObjectName(QString::fromUtf8("widgetContainingTcgParameters"));
        widgetContainingTcgParameters->setAutoFillBackground(false);
        verticalLayout_4 = new QVBoxLayout(widgetContainingTcgParameters);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        labelTcgOrder = new QLabel(widgetContainingTcgParameters);
        labelTcgOrder->setObjectName(QString::fromUtf8("labelTcgOrder"));

        horizontalLayout_11->addWidget(labelTcgOrder);

        comboBoxTcgOrder = new QComboBox(widgetContainingTcgParameters);
        comboBoxTcgOrder->addItem(QString());
        comboBoxTcgOrder->addItem(QString());
        comboBoxTcgOrder->setObjectName(QString::fromUtf8("comboBoxTcgOrder"));
        comboBoxTcgOrder->setMinimumSize(QSize(165, 0));
        comboBoxTcgOrder->setMaximumSize(QSize(165, 16777215));

        horizontalLayout_11->addWidget(comboBoxTcgOrder);

        horizontalLayout_11->setStretch(0, 1);

        verticalLayout_4->addLayout(horizontalLayout_11);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        labelDiagPrec = new QLabel(widgetContainingTcgParameters);
        labelDiagPrec->setObjectName(QString::fromUtf8("labelDiagPrec"));

        horizontalLayout_12->addWidget(labelDiagPrec);

        comboBoxDiagPrec = new QComboBox(widgetContainingTcgParameters);
        comboBoxDiagPrec->addItem(QString());
        comboBoxDiagPrec->addItem(QString());
        comboBoxDiagPrec->setObjectName(QString::fromUtf8("comboBoxDiagPrec"));
        comboBoxDiagPrec->setMinimumSize(QSize(165, 0));
        comboBoxDiagPrec->setMaximumSize(QSize(165, 16777215));

        horizontalLayout_12->addWidget(comboBoxDiagPrec);

        horizontalLayout_12->setStretch(0, 1);

        verticalLayout_4->addLayout(horizontalLayout_12);

        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setObjectName(QString::fromUtf8("horizontalLayout_13"));
        labelTcgGuess = new QLabel(widgetContainingTcgParameters);
        labelTcgGuess->setObjectName(QString::fromUtf8("labelTcgGuess"));

        horizontalLayout_13->addWidget(labelTcgGuess);

        comboBoxTcgGuess = new QComboBox(widgetContainingTcgParameters);
        comboBoxTcgGuess->addItem(QString());
        comboBoxTcgGuess->addItem(QString());
        comboBoxTcgGuess->setObjectName(QString::fromUtf8("comboBoxTcgGuess"));
        comboBoxTcgGuess->setMinimumSize(QSize(165, 0));
        comboBoxTcgGuess->setMaximumSize(QSize(165, 16777215));

        horizontalLayout_13->addWidget(comboBoxTcgGuess);

        horizontalLayout_13->setStretch(0, 1);

        verticalLayout_4->addLayout(horizontalLayout_13);

        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setObjectName(QString::fromUtf8("horizontalLayout_14"));
        labelTcgPeek = new QLabel(widgetContainingTcgParameters);
        labelTcgPeek->setObjectName(QString::fromUtf8("labelTcgPeek"));

        horizontalLayout_14->addWidget(labelTcgPeek);

        comboBoxTcgPeek = new QComboBox(widgetContainingTcgParameters);
        comboBoxTcgPeek->addItem(QString());
        comboBoxTcgPeek->addItem(QString());
        comboBoxTcgPeek->setObjectName(QString::fromUtf8("comboBoxTcgPeek"));
        comboBoxTcgPeek->setMinimumSize(QSize(165, 0));
        comboBoxTcgPeek->setMaximumSize(QSize(165, 16777215));

        horizontalLayout_14->addWidget(comboBoxTcgPeek);

        horizontalLayout_14->setStretch(0, 1);

        verticalLayout_4->addLayout(horizontalLayout_14);

        widgetPeekStep = new QWidget(widgetContainingTcgParameters);
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
        spinBoxTcgOmega->setMinimumSize(QSize(165, 0));
        spinBoxTcgOmega->setMaximumSize(QSize(165, 16777215));
        spinBoxTcgOmega->setValue(1);

        horizontalLayout_15->addWidget(spinBoxTcgOmega);

        horizontalLayout_15->setStretch(0, 1);

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
        comboBoxTcgMegaFit->setMinimumSize(QSize(165, 0));
        comboBoxTcgMegaFit->setMaximumSize(QSize(165, 16777215));

        horizontalLayout_16->addWidget(comboBoxTcgMegaFit);

        horizontalLayout_16->setStretch(0, 1);

        verticalLayout_2->addLayout(horizontalLayout_16);

        horizontalLayout_17 = new QHBoxLayout();
        horizontalLayout_17->setObjectName(QString::fromUtf8("horizontalLayout_17"));
        labelTcgMegaFitFreq = new QLabel(widgetPeekStep);
        labelTcgMegaFitFreq->setObjectName(QString::fromUtf8("labelTcgMegaFitFreq"));

        horizontalLayout_17->addWidget(labelTcgMegaFitFreq);

        spinBoxTcgMegaFitFreq = new QSpinBox(widgetPeekStep);
        spinBoxTcgMegaFitFreq->setObjectName(QString::fromUtf8("spinBoxTcgMegaFitFreq"));
        spinBoxTcgMegaFitFreq->setMinimumSize(QSize(165, 0));
        spinBoxTcgMegaFitFreq->setMaximumSize(QSize(165, 16777215));
        spinBoxTcgMegaFitFreq->setMaximum(100000000);
        spinBoxTcgMegaFitFreq->setValue(1000);

        horizontalLayout_17->addWidget(spinBoxTcgMegaFitFreq);

        horizontalLayout_17->setStretch(0, 1);

        verticalLayout_2->addLayout(horizontalLayout_17);


        verticalLayout_4->addWidget(widgetPeekStep);


        verticalLayout_7->addWidget(widgetContainingTcgParameters);


        verticalLayout_6->addWidget(groupBoxTcgParameters);

        groupBoxTcgShortParameters = new CollapsingGroupboxWidget(widgetDynamic);
        groupBoxTcgShortParameters->setObjectName(QString::fromUtf8("groupBoxTcgShortParameters"));
        verticalLayout_5 = new QVBoxLayout(groupBoxTcgShortParameters);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        widgetTcgParametersShort = new QWidget(groupBoxTcgShortParameters);
        widgetTcgParametersShort->setObjectName(QString::fromUtf8("widgetTcgParametersShort"));
        verticalLayout_8 = new QVBoxLayout(widgetTcgParametersShort);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        verticalLayout_8->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_18 = new QHBoxLayout();
        horizontalLayout_18->setObjectName(QString::fromUtf8("horizontalLayout_18"));
        labelTcgOrderShort = new QLabel(widgetTcgParametersShort);
        labelTcgOrderShort->setObjectName(QString::fromUtf8("labelTcgOrderShort"));

        horizontalLayout_18->addWidget(labelTcgOrderShort);

        comboBoxTcgOrderShort = new QComboBox(widgetTcgParametersShort);
        comboBoxTcgOrderShort->addItem(QString());
        comboBoxTcgOrderShort->addItem(QString());
        comboBoxTcgOrderShort->setObjectName(QString::fromUtf8("comboBoxTcgOrderShort"));
        comboBoxTcgOrderShort->setMinimumSize(QSize(165, 0));
        comboBoxTcgOrderShort->setMaximumSize(QSize(165, 16777215));

        horizontalLayout_18->addWidget(comboBoxTcgOrderShort);

        horizontalLayout_18->setStretch(0, 1);

        verticalLayout_8->addLayout(horizontalLayout_18);

        horizontalLayout_22 = new QHBoxLayout();
        horizontalLayout_22->setObjectName(QString::fromUtf8("horizontalLayout_22"));
        labelDiagPrecShort = new QLabel(widgetTcgParametersShort);
        labelDiagPrecShort->setObjectName(QString::fromUtf8("labelDiagPrecShort"));

        horizontalLayout_22->addWidget(labelDiagPrecShort);

        comboBoxDiagPrecShort = new QComboBox(widgetTcgParametersShort);
        comboBoxDiagPrecShort->addItem(QString());
        comboBoxDiagPrecShort->addItem(QString());
        comboBoxDiagPrecShort->setObjectName(QString::fromUtf8("comboBoxDiagPrecShort"));
        comboBoxDiagPrecShort->setMinimumSize(QSize(165, 0));
        comboBoxDiagPrecShort->setMaximumSize(QSize(165, 16777215));

        horizontalLayout_22->addWidget(comboBoxDiagPrecShort);

        horizontalLayout_22->setStretch(0, 1);

        verticalLayout_8->addLayout(horizontalLayout_22);

        horizontalLayout_20 = new QHBoxLayout();
        horizontalLayout_20->setObjectName(QString::fromUtf8("horizontalLayout_20"));
        labelTcgGuessShort = new QLabel(widgetTcgParametersShort);
        labelTcgGuessShort->setObjectName(QString::fromUtf8("labelTcgGuessShort"));

        horizontalLayout_20->addWidget(labelTcgGuessShort);

        comboBoxTcgGuessShort = new QComboBox(widgetTcgParametersShort);
        comboBoxTcgGuessShort->addItem(QString());
        comboBoxTcgGuessShort->addItem(QString());
        comboBoxTcgGuessShort->setObjectName(QString::fromUtf8("comboBoxTcgGuessShort"));
        comboBoxTcgGuessShort->setMinimumSize(QSize(165, 0));
        comboBoxTcgGuessShort->setMaximumSize(QSize(165, 16777215));

        horizontalLayout_20->addWidget(comboBoxTcgGuessShort);

        horizontalLayout_20->setStretch(0, 1);

        verticalLayout_8->addLayout(horizontalLayout_20);

        horizontalLayout_21 = new QHBoxLayout();
        horizontalLayout_21->setObjectName(QString::fromUtf8("horizontalLayout_21"));
        labelTcgPeekShort = new QLabel(widgetTcgParametersShort);
        labelTcgPeekShort->setObjectName(QString::fromUtf8("labelTcgPeekShort"));

        horizontalLayout_21->addWidget(labelTcgPeekShort);

        comboBoxTcgPeekShort = new QComboBox(widgetTcgParametersShort);
        comboBoxTcgPeekShort->addItem(QString());
        comboBoxTcgPeekShort->addItem(QString());
        comboBoxTcgPeekShort->setObjectName(QString::fromUtf8("comboBoxTcgPeekShort"));
        comboBoxTcgPeekShort->setMinimumSize(QSize(165, 0));
        comboBoxTcgPeekShort->setMaximumSize(QSize(165, 16777215));

        horizontalLayout_21->addWidget(comboBoxTcgPeekShort);

        horizontalLayout_21->setStretch(0, 1);

        verticalLayout_8->addLayout(horizontalLayout_21);

        horizontalLayout_19 = new QHBoxLayout();
        horizontalLayout_19->setObjectName(QString::fromUtf8("horizontalLayout_19"));
        labelTcgOmegaShort = new QLabel(widgetTcgParametersShort);
        labelTcgOmegaShort->setObjectName(QString::fromUtf8("labelTcgOmegaShort"));

        horizontalLayout_19->addWidget(labelTcgOmegaShort);

        spinBoxTcgOmegaShort = new QSpinBox(widgetTcgParametersShort);
        spinBoxTcgOmegaShort->setObjectName(QString::fromUtf8("spinBoxTcgOmegaShort"));
        spinBoxTcgOmegaShort->setMinimumSize(QSize(165, 0));
        spinBoxTcgOmegaShort->setMaximumSize(QSize(165, 16777215));
        spinBoxTcgOmegaShort->setValue(1);

        horizontalLayout_19->addWidget(spinBoxTcgOmegaShort);

        horizontalLayout_19->setStretch(0, 1);

        verticalLayout_8->addLayout(horizontalLayout_19);


        verticalLayout_5->addWidget(widgetTcgParametersShort);


        verticalLayout_6->addWidget(groupBoxTcgShortParameters);


        verticalLayout_9->addWidget(widgetDynamic);

        groupBoxAdditionnalKeywords = new CollapsingGroupboxWidget(tabDynamic);
        groupBoxAdditionnalKeywords->setObjectName(QString::fromUtf8("groupBoxAdditionnalKeywords"));
        groupBoxAdditionnalKeywords->setCheckable(true);
        verticalLayout_3 = new QVBoxLayout(groupBoxAdditionnalKeywords);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(9, 9, 9, 9);
        textBrowserAdditionnalKeywords = new QTextBrowser(groupBoxAdditionnalKeywords);
        textBrowserAdditionnalKeywords->setObjectName(QString::fromUtf8("textBrowserAdditionnalKeywords"));
        textBrowserAdditionnalKeywords->setAutoFormatting(QTextEdit::AutoNone);
        textBrowserAdditionnalKeywords->setUndoRedoEnabled(true);
        textBrowserAdditionnalKeywords->setReadOnly(false);

        verticalLayout_3->addWidget(textBrowserAdditionnalKeywords);


        verticalLayout_9->addWidget(groupBoxAdditionnalKeywords);

        pushButtonGenerate = new QPushButton(tabDynamic);
        pushButtonGenerate->setObjectName(QString::fromUtf8("pushButtonGenerate"));

        verticalLayout_9->addWidget(pushButtonGenerate);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_9->addItem(verticalSpacer);

        tabWidgetKeywordEditor->addTab(tabDynamic, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        tabWidgetKeywordEditor->addTab(tab_2, QString());

        gridLayout_2->addWidget(tabWidgetKeywordEditor, 0, 0, 1, 1);

        scrollArea->setWidget(scrollAreaWidgetContents);

        gridLayout->addWidget(scrollArea, 0, 0, 1, 1);

        DockWidget->setWidget(dockWidgetContents);

        retranslateUi(DockWidget);
        QObject::connect(pushButtonOpenKeyfile, SIGNAL(clicked()), DockWidget, SLOT(onOpenKeyfilePushButtonClicked()));
        QObject::connect(comboBoxIntegrator, SIGNAL(currentIndexChanged(QString)), DockWidget, SLOT(comboBoxIntegratorIndexChanged(QString)));
        QObject::connect(comboBoxPolarizationEquations, SIGNAL(currentIndexChanged(QString)), DockWidget, SLOT(comboBoxPolEquaIndexChanged(QString)));
        QObject::connect(comboBoxShortRangePolarizationSolver, SIGNAL(currentIndexChanged(QString)), DockWidget, SLOT(comboBoxShortRangePolSolndexChanged(QString)));
        QObject::connect(comboBoxTcgPeek, SIGNAL(currentIndexChanged(QString)), DockWidget, SLOT(comboBoxPeekStepIndexChanged(QString)));
        QObject::connect(comboBoxTcgMegaFit, SIGNAL(currentIndexChanged(QString)), DockWidget, SLOT(comboBoxFittingIndexChanged(QString)));
        QObject::connect(pushButtonGenerate, SIGNAL(clicked()), DockWidget, SLOT(onGenerateKeyfilePushButtonClicked()));

        tabWidgetKeywordEditor->setCurrentIndex(0);
        comboBoxThermostat->setCurrentIndex(0);
        comboBoxBarostat->setCurrentIndex(0);
        comboBoxPolarizationEquations->setCurrentIndex(0);
        comboBoxShortRangePolarizationSolver->setCurrentIndex(0);
        comboBoxDiagPrec->setCurrentIndex(1);
        comboBoxTcgPeek->setCurrentIndex(1);
        comboBoxTcgMegaFit->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(DockWidget);
    } // setupUi

    void retranslateUi(QDockWidget *DockWidget)
    {
        DockWidget->setWindowTitle(QCoreApplication::translate("DockWidget", "Tinker-HP - Keyword Editor", nullptr));
        pushButtonOpenKeyfile->setText(QCoreApplication::translate("DockWidget", "Open", nullptr));
#if QT_CONFIG(tooltip)
        labelIntegrator->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p><span style=\" font-size:9pt;\">Integrators: </span></p><p><span style=\" font-size:9pt; font-weight:600;\">BEEMAN</span><span style=\" font-size:9pt;\">: The default one</span></p><p><span style=\" font-size:9pt; font-weight:600;\">VERLET</span><span style=\" font-size:9pt;\">: Verlet</span></p><p><span style=\" font-size:9pt; font-weight:600;\">BBK</span><span style=\" font-size:9pt;\">: Langevin Dynamics for constant temperature simulations</span></p><p><span style=\" font-size:9pt; font-weight:600;\">BAOAB</span><span style=\" font-size:9pt;\">: Langevin Dynamics for constant temperature simulations</span></p><p><span style=\" font-size:9pt; font-weight:600;\">RESPA</span><span style=\" font-size:9pt;\">: Bonded/non bonded respa-split with a velocity-verlet inner loop and with a 0.25 fs default timestep for the inner loop</span></p><p><span style=\" font-size:9pt; font-weight:600;\">BAOABRESPA</span><span style=\" font-size:9pt;\">: Bonded/non bonded respa-split for Langevin dynamics "
                        "with a BAOAB inner loop, the default timestep for the inner loop is also 0.25 fs</span></p><p><span style=\" font-size:9pt; font-weight:600;\">RESPA1</span><span style=\" font-size:9pt;\">: (Bonded)/(short range non bonded)/(long range non bonded) three level respa1-split with a velocity verlet inner loop. The default timesteps are 0.25 fs for the inner loop and 2 fs for the intermediate one</span></p><p><span style=\" font-size:9pt; font-weight:600;\">BAOABRESPA1</span><span style=\" font-size:9pt;\">: (Bonded)/(short range non bonded)/(long range non bonded) three level respa1-split for Langevin dynamics with a BAOAB inner loop. The default timesteps are 0.25 fs for the inner loop and 2 fs for the intermediate one</span></p><p><span style=\" font-size:9pt; font-weight:600;\">BAOABPISTON</span><span style=\" font-size:9pt;\">: Constant pressure BAOAB Langevin dynamics with a Langevin Piston pressure control and a BAOAB evolution of the volume extended variable. The default mass of the piston is 2e</span><span"
                        " style=\" font-size:9pt; vertical-align:super;\">-5</span><span style=\" font-size:9pt;\"> atomic units and the default friction for the piston is 20.0 ps</span><span style=\" font-size:9pt; vertical-align:super;\">-1</span><span style=\" font-size:9pt;\">.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelIntegrator->setText(QCoreApplication::translate("DockWidget", "Integrator", nullptr));
        comboBoxIntegrator->setItemText(0, QCoreApplication::translate("DockWidget", "BEEMAN", nullptr));
        comboBoxIntegrator->setItemText(1, QCoreApplication::translate("DockWidget", "VERLET", nullptr));
        comboBoxIntegrator->setItemText(2, QCoreApplication::translate("DockWidget", "BBK", nullptr));
        comboBoxIntegrator->setItemText(3, QCoreApplication::translate("DockWidget", "BAOAB", nullptr));
        comboBoxIntegrator->setItemText(4, QCoreApplication::translate("DockWidget", "RESPA", nullptr));
        comboBoxIntegrator->setItemText(5, QCoreApplication::translate("DockWidget", "BAOABRESPA", nullptr));
        comboBoxIntegrator->setItemText(6, QCoreApplication::translate("DockWidget", "RESPA1", nullptr));
        comboBoxIntegrator->setItemText(7, QCoreApplication::translate("DockWidget", "BAOABRESPA1", nullptr));
        comboBoxIntegrator->setItemText(8, QCoreApplication::translate("DockWidget", "BAOABPISTON", nullptr));

#if QT_CONFIG(tooltip)
        comboBoxIntegrator->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p><span style=\" font-size:9pt;\">Integrators: </span></p><p><span style=\" font-size:9pt; font-weight:600;\">BEEMAN</span><span style=\" font-size:9pt;\">: The default one</span></p><p><span style=\" font-size:9pt; font-weight:600;\">VERLET</span><span style=\" font-size:9pt;\">: Verlet</span></p><p><span style=\" font-size:9pt; font-weight:600;\">BBK</span><span style=\" font-size:9pt;\">: Langevin Dynamics for constant temperature simulations</span></p><p><span style=\" font-size:9pt; font-weight:600;\">BAOAB</span><span style=\" font-size:9pt;\">: Langevin Dynamics for constant temperature simulations</span></p><p><span style=\" font-size:9pt; font-weight:600;\">RESPA</span><span style=\" font-size:9pt;\">: Bonded/non bonded respa-split with a velocity-verlet inner loop and with a 0.25 fs default timestep for the inner loop</span></p><p><span style=\" font-size:9pt; font-weight:600;\">BAOABRESPA</span><span style=\" font-size:9pt;\">: Bonded/non bonded respa-split for Langevin dynamics "
                        "with a BAOAB inner loop, the default timestep for the inner loop is also 0.25 fs</span></p><p><span style=\" font-size:9pt; font-weight:600;\">RESPA1</span><span style=\" font-size:9pt;\">: (Bonded)/(short range non bonded)/(long range non bonded) three level respa1-split with a velocity verlet inner loop. The default timesteps are 0.25 fs for the inner loop and 2 fs for the intermediate one</span></p><p><span style=\" font-size:9pt; font-weight:600;\">BAOABRESPA1</span><span style=\" font-size:9pt;\">: (Bonded)/(short range non bonded)/(long range non bonded) three level respa1-split for Langevin dynamics with a BAOAB inner loop. The default timesteps are 0.25 fs for the inner loop and 2 fs for the intermediate one</span></p><p><span style=\" font-size:9pt; font-weight:600;\">BAOABPISTON</span><span style=\" font-size:9pt;\">: Constant pressure BAOAB Langevin dynamics with a Langevin Piston pressure control and a BAOAB evolution of the volume extended variable. The default mass of the piston is 2e</span><span"
                        " style=\" font-size:9pt; vertical-align:super;\">-5</span><span style=\" font-size:9pt;\"> atomic units and the default friction for the piston is 20.0 ps</span><span style=\" font-size:9pt; vertical-align:super;\">-1</span><span style=\" font-size:9pt;\">.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        labelIntegratorInfo->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p><span style=\" font-size:9pt;\">For all the Langevin integrators (</span><span style=\" font-size:9pt; font-weight:600;\">BBK, BAOAB, BAOABRESPA, BAOABRESPA1 </span><span style=\" font-size:9pt;\">and</span><span style=\" font-size:9pt; font-weight:600;\"> BAOABPISTON</span><span style=\" font-size:9pt;\">), the friction (in ps</span><span style=\" font-size:9pt; vertical-align:super;\">-1</span><span style=\" font-size:9pt;\">) can be controlled by the </span><span style=\" font-size:9pt; font-style:italic;\">Friction</span><span style=\" font-size:9pt;\"> parameter, the default being 1 ps</span><span style=\" font-size:9pt; vertical-align:super;\">-1</span><span style=\" font-size:9pt;\">.</span></p><p><span style=\" font-size:9pt;\">For </span><span style=\" font-size:9pt; font-weight:600;\">RESPA, BAOABRESPA, RESPA1 </span><span style=\" font-size:9pt;\">and</span><span style=\" font-size:9pt; font-weight:600;\"> BAOABRESPA1</span><span style=\" font-size:9pt;\">, the inner timestep c"
                        "an be imposed by the </span><span style=\" font-size:9pt; font-style:italic;\">Inner timestep</span><span style=\" font-size:9pt;\"> parameter, value in ps.</span></p><p><span style=\" font-size:9pt;\">For </span><span style=\" font-size:9pt; font-weight:600;\">RESPA1</span><span style=\" font-size:9pt;\"> and </span><span style=\" font-size:9pt; font-weight:600;\">BAOABRESPA1</span><span style=\" font-size:9pt;\">, the intermediate timestep can be imposed by the </span><span style=\" font-size:9pt; font-style:italic;\">Intermediate timestep</span><span style=\" font-size:9pt;\"> parameter, value in ps.</span></p><p><span style=\" font-size:9pt;\">For </span><span style=\" font-size:9pt; font-weight:600;\">BAOABPISTON</span><span style=\" font-size:9pt;\">, the mass of the piston (in atomic units) can be set by the parameters </span><span style=\" font-size:9pt; font-style:italic;\">Piston mass</span><span style=\" font-size:9pt;\"> and </span><span style=\" font-size:9pt; font-style:italic;\">Piston friction<"
                        "/span><span style=\" font-size:9pt;\"> (in ps</span><span style=\" font-size:9pt; vertical-align:super;\">-1</span><span style=\" font-size:9pt;\">).</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelIntegratorInfo->setText(QString());
#if QT_CONFIG(tooltip)
        labelFriction->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p><span style=\" font-size:9pt;\">Friction of all the Langevin integrators (</span><span style=\" font-size:9pt; font-weight:600;\">BBK</span><span style=\" font-size:9pt;\">, </span><span style=\" font-size:9pt; font-weight:600;\">BAOAB</span><span style=\" font-size:9pt;\">, </span><span style=\" font-size:9pt; font-weight:600;\">BAOABRESPA</span><span style=\" font-size:9pt;\">, </span><span style=\" font-size:9pt; font-weight:600;\">BAOABRESPA1</span><span style=\" font-size:9pt;\"> and </span><span style=\" font-size:9pt; font-weight:600;\">BAOABPISTON</span><span style=\" font-size:9pt;\">), the default being 1 ps</span><span style=\" font-size:9pt; vertical-align:super;\">-1</span><span style=\" font-size:9pt;\">.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelFriction->setText(QCoreApplication::translate("DockWidget", "Friction", nullptr));
#if QT_CONFIG(tooltip)
        spinBoxFriction->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p><span style=\" font-size:9pt;\">Friction of all the Langevin integrators (</span><span style=\" font-size:9pt; font-weight:600;\">BBK</span><span style=\" font-size:9pt;\">, </span><span style=\" font-size:9pt; font-weight:600;\">BAOAB</span><span style=\" font-size:9pt;\">, </span><span style=\" font-size:9pt; font-weight:600;\">BAOABRESPA</span><span style=\" font-size:9pt;\">, </span><span style=\" font-size:9pt; font-weight:600;\">BAOABRESPA1</span><span style=\" font-size:9pt;\"> and </span><span style=\" font-size:9pt; font-weight:600;\">BAOABPISTON</span><span style=\" font-size:9pt;\">), the default being 1 ps</span><span style=\" font-size:9pt; vertical-align:super;\">-1</span><span style=\" font-size:9pt;\">.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        spinBoxFriction->setSuffix(QString());
#if QT_CONFIG(tooltip)
        labelInnerTimestep->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p><span style=\" font-size:9pt;\">The inner timestep for </span><span style=\" font-size:9pt; font-weight:600;\">RESPA</span><span style=\" font-size:9pt;\">, </span><span style=\" font-size:9pt; font-weight:600;\">BAOABRESPA</span><span style=\" font-size:9pt;\">, </span><span style=\" font-size:9pt; font-weight:600;\">RESPA1</span><span style=\" font-size:9pt;\"> and </span><span style=\" font-size:9pt; font-weight:600;\">BAOABRESPA1.</span><span style=\" font-size:9pt;\"> Value in ps.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelInnerTimestep->setText(QCoreApplication::translate("DockWidget", "Inner timestep", nullptr));
#if QT_CONFIG(tooltip)
        spinBoxInnerTimestep->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p><span style=\" font-size:9pt;\">The inner timestep for </span><span style=\" font-size:9pt; font-weight:600;\">RESPA</span><span style=\" font-size:9pt;\">, </span><span style=\" font-size:9pt; font-weight:600;\">BAOABRESPA</span><span style=\" font-size:9pt;\">, </span><span style=\" font-size:9pt; font-weight:600;\">RESPA1</span><span style=\" font-size:9pt;\"> and </span><span style=\" font-size:9pt; font-weight:600;\">BAOABRESPA1.</span><span style=\" font-size:9pt;\"> Value in ps.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        labelIntermediateTimestep->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p>Set the intermediate timestep for <span style=\" font-weight:600;\">RESPA1</span> and <span style=\" font-weight:600;\">BAOABRESPA1</span>. value in ps.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelIntermediateTimestep->setText(QCoreApplication::translate("DockWidget", "Intermediate timestep", nullptr));
#if QT_CONFIG(tooltip)
        spinBoxIntermediateTimestep->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p>Set the intermediate timestep for <span style=\" font-weight:600;\">RESPA1</span> and <span style=\" font-weight:600;\">BAOABRESPA1</span>. value in ps.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        labelThermostat->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p>The thermostats available in Tinker-HP are <span style=\" font-weight:600;\">Berendsen</span>, <span style=\" font-weight:600;\">Bussi</span> (which is the default one) and <span style=\" font-weight:600;\">Andersen</span>.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelThermostat->setText(QCoreApplication::translate("DockWidget", "Thermostat", nullptr));
        comboBoxThermostat->setItemText(0, QCoreApplication::translate("DockWidget", "ANDERSEN", nullptr));
        comboBoxThermostat->setItemText(1, QCoreApplication::translate("DockWidget", "BERENDSEN", nullptr));
        comboBoxThermostat->setItemText(2, QCoreApplication::translate("DockWidget", "BUSSI", nullptr));

#if QT_CONFIG(tooltip)
        comboBoxThermostat->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p>The thermostats available in Tinker-HP are <span style=\" font-weight:600;\">Berendsen</span>, <span style=\" font-weight:600;\">Bussi</span> (which is the default one) and <span style=\" font-weight:600;\">Andersen</span>.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        labelBarostat->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p>The barostats available in Tinker-HP are the <span style=\" font-weight:600;\">Berendsen</span> (which is the default one) and the <span style=\" font-weight:600;\">Monte-Carlo</span> one</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelBarostat->setText(QCoreApplication::translate("DockWidget", "Barostat", nullptr));
        comboBoxBarostat->setItemText(0, QCoreApplication::translate("DockWidget", "BERENDSEN", nullptr));
        comboBoxBarostat->setItemText(1, QCoreApplication::translate("DockWidget", "MONTE-CARLO", nullptr));

#if QT_CONFIG(tooltip)
        comboBoxBarostat->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p>The barostats available in Tinker-HP are the <span style=\" font-weight:600;\">Berendsen</span> (which is the default one) and the <span style=\" font-weight:600;\">Monte-Carlo</span> one</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        labelPolarizationEquations->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p>The algorithm used to converge the polarization equations (compute the dipoles solution):</p><p><span style=\" font-weight:600;\">Divide and Conquer Jacobi/DIIS</span> (default)</p><p><span style=\" font-weight:600;\">PCG</span> (Conjugate Gradient with a diagonal preconditioner)</p><p><span style=\" font-weight:600;\">Jacobi/DIIS</span></p><p><span style=\" font-weight:600;\">TCG</span> (Truncated Conjugate Gradient)</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelPolarizationEquations->setText(QCoreApplication::translate("DockWidget", "Polarization solver", nullptr));
        comboBoxPolarizationEquations->setItemText(0, QCoreApplication::translate("DockWidget", "D&C Jacobi/DIIS", nullptr));
        comboBoxPolarizationEquations->setItemText(1, QCoreApplication::translate("DockWidget", "PCG", nullptr));
        comboBoxPolarizationEquations->setItemText(2, QCoreApplication::translate("DockWidget", "Jacobi/DIIS", nullptr));
        comboBoxPolarizationEquations->setItemText(3, QCoreApplication::translate("DockWidget", "TCG", nullptr));

#if QT_CONFIG(tooltip)
        comboBoxPolarizationEquations->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p>The algorithm used to converge the polarization equations (compute the dipoles solution):</p><p><span style=\" font-weight:600;\">Divide and Conquer Jacobi/DIIS</span> (default)</p><p><span style=\" font-weight:600;\">PCG</span> (Conjugate Gradient with a diagonal preconditioner)</p><p><span style=\" font-weight:600;\">Jacobi/DIIS</span></p><p><span style=\" font-weight:600;\">TCG</span> (Truncated Conjugate Gradient)</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        labelShortRangePolarizationSolver->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p>The algorithm used to converge the polarization equations (compute the dipoles solution) for short range real space at the intermediate time steps. By default, this is done using the same algorithm as the one used to solve the complete polarization equations at the outer time steps. But one can chose a different algorithm to solve the short range. TCG is chosen as a short range polarization solver, one can define all the related option for this solver in the &quot;TCG parameters for short range polarization&quot; groupbox displayed below. </p><p><span style=\" font-weight:600;\">Divide and Conquer Jacobi/DIIS</span> (default)</p><p><span style=\" font-weight:600;\">PCG</span> (Conjugate Gradient with a diagonal preconditioner)</p><p><span style=\" font-weight:600;\">Jacobi/DIIS</span></p><p><span style=\" font-weight:600;\">TCG</span> (Truncated Conjugate Gradient)</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelShortRangePolarizationSolver->setText(QCoreApplication::translate("DockWidget", "Short range polarization solver", nullptr));
        comboBoxShortRangePolarizationSolver->setItemText(0, QCoreApplication::translate("DockWidget", "D&C Jacobi/DIIS", nullptr));
        comboBoxShortRangePolarizationSolver->setItemText(1, QCoreApplication::translate("DockWidget", "PCG", nullptr));
        comboBoxShortRangePolarizationSolver->setItemText(2, QCoreApplication::translate("DockWidget", "Jacobi/DIIS", nullptr));
        comboBoxShortRangePolarizationSolver->setItemText(3, QCoreApplication::translate("DockWidget", "TCG", nullptr));

#if QT_CONFIG(tooltip)
        comboBoxShortRangePolarizationSolver->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p>The algorithm used to converge the polarization equations (compute the dipoles solution) for short range real space at the intermediate time steps. By default, this is done using the same algorithm as the one used to solve the complete polarization equations at the outer time steps. But one can chose a different algorithm to solve the short range. TCG is chosen as a short range polarization solver, one can define all the related option for this solver in the &quot;TCG parameters for short range polarization&quot; groupbox displayed below. </p><p><span style=\" font-weight:600;\">Divide and Conquer Jacobi/DIIS</span> (default)</p><p><span style=\" font-weight:600;\">PCG</span> (Conjugate Gradient with a diagonal preconditioner)</p><p><span style=\" font-weight:600;\">Jacobi/DIIS</span></p><p><span style=\" font-weight:600;\">TCG</span> (Truncated Conjugate Gradient)</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        labelMassPiston->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p>Mass of the piston (in atomic units).</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelMassPiston->setText(QCoreApplication::translate("DockWidget", "Piston mass", nullptr));
#if QT_CONFIG(tooltip)
        spinBoxMassPiston->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p>Mass of the piston (in atomic units).</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        labelFrictionPiston->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p>Friction of the piston (in ps<span style=\" vertical-align:super;\">-1</span>).</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelFrictionPiston->setText(QCoreApplication::translate("DockWidget", "Piston friction", nullptr));
#if QT_CONFIG(tooltip)
        spinBoxFrictionPiston->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p>Friction of the piston (in ps<span style=\" vertical-align:super;\">-1</span>).</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        groupBoxTcgParameters->setTitle(QCoreApplication::translate("DockWidget", "TCG parameters", nullptr));
#if QT_CONFIG(tooltip)
        labelTcgOrder->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p><span style=\" font-size:9pt;\">Order of the TCG truncation, TCG1 or TCG2, default is TCG2.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelTcgOrder->setText(QCoreApplication::translate("DockWidget", "TCG order", nullptr));
        comboBoxTcgOrder->setItemText(0, QCoreApplication::translate("DockWidget", "TCG2", nullptr));
        comboBoxTcgOrder->setItemText(1, QCoreApplication::translate("DockWidget", "TCG1", nullptr));

#if QT_CONFIG(tooltip)
        comboBoxTcgOrder->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p><span style=\" font-size:9pt;\">Order of the TCG truncation, TCG1 or TCG2, default is TCG2.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        labelDiagPrec->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p><span style=\" font-size:9pt;\">Use of a diagonal preconditioner, default is yes.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelDiagPrec->setText(QCoreApplication::translate("DockWidget", "Diagonal preconditioner", nullptr));
        comboBoxDiagPrec->setItemText(0, QCoreApplication::translate("DockWidget", "No", nullptr));
        comboBoxDiagPrec->setItemText(1, QCoreApplication::translate("DockWidget", "Yes", nullptr));

#if QT_CONFIG(tooltip)
        comboBoxDiagPrec->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p><span style=\" font-size:9pt;\">Use of a diagonal preconditioner, default is yes.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        labelTcgGuess->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p><span style=\" font-size:9pt;\">Use of a \342\200\235direct guess\342\200\235 (polarizability x permanent electric field) as a guess., default is no.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelTcgGuess->setText(QCoreApplication::translate("DockWidget", "Direct guess", nullptr));
        comboBoxTcgGuess->setItemText(0, QCoreApplication::translate("DockWidget", "No", nullptr));
        comboBoxTcgGuess->setItemText(1, QCoreApplication::translate("DockWidget", "Yes", nullptr));

#if QT_CONFIG(tooltip)
        comboBoxTcgGuess->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p><span style=\" font-size:9pt;\">Use of a \342\200\235direct guess\342\200\235 (polarizability x permanent electric field) as a guess., default is no.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        labelTcgPeek->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p><span style=\" font-size:9pt;\">Use of a peek step, default is yes. When a peek step is used, a Jacobi Over Relaxation (JOR) is applied to the TCG values of the dipoles with a parameter </span><span style=\" font-family:'Arial Unicode MS','DejaVu Sans','Athena','Gentium','Palatino Linotype','Lucida Sans Unicode','Lucida Grande','Code2000','sans-serif'; font-size:14px; color:#202122; background-color:#fcfcfc;\">\317\211.</span><span style=\" font-size:9pt;\"> By default, this value is </span><span style=\" font-family:'Arial Unicode MS','DejaVu Sans','Athena','Gentium','Palatino Linotype','Lucida Sans Unicode','Lucida Grande','Code2000','sans-serif'; font-size:14px; color:#202122; background-color:#fcfcfc;\">\317\211</span><span style=\" font-size:9pt;\"> = 1. (regular Jacobi step) but the following keywords can modify this.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelTcgPeek->setText(QCoreApplication::translate("DockWidget", "Peek step", nullptr));
        comboBoxTcgPeek->setItemText(0, QCoreApplication::translate("DockWidget", "No", nullptr));
        comboBoxTcgPeek->setItemText(1, QCoreApplication::translate("DockWidget", "Yes", nullptr));

#if QT_CONFIG(tooltip)
        comboBoxTcgPeek->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p><span style=\" font-size:9pt;\">Use of a peek step, default is yes. When a peek step is used, a Jacobi Over Relaxation (JOR) is applied to the TCG values of the dipoles with a parameter </span><span style=\" font-family:'Arial Unicode MS','DejaVu Sans','Athena','Gentium','Palatino Linotype','Lucida Sans Unicode','Lucida Grande','Code2000','sans-serif'; font-size:14px; color:#202122; background-color:#fcfcfc;\">\317\211.</span><span style=\" font-size:9pt;\"> By default, this value is </span><span style=\" font-family:'Arial Unicode MS','DejaVu Sans','Athena','Gentium','Palatino Linotype','Lucida Sans Unicode','Lucida Grande','Code2000','sans-serif'; font-size:14px; color:#202122; background-color:#fcfcfc;\">\317\211</span><span style=\" font-size:9pt;\"> = 1. (regular Jacobi step) but the following keywords can modify this.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        labelTcgOmega->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p><span style=\" font-size:9pt;\">Value of the \317\211 parameter</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelTcgOmega->setText(QCoreApplication::translate("DockWidget", "Omega ( \317\211 )", nullptr));
#if QT_CONFIG(tooltip)
        spinBoxTcgOmega->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p><span style=\" font-size:9pt;\">Value of the \317\211 parameter</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        labelTcgMegaFit->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p>Impose a regular fitting of the \317\211 parameter to match at regular intervals the fully converged polarization energy.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelTcgMegaFit->setText(QCoreApplication::translate("DockWidget", "Fitting", nullptr));
        comboBoxTcgMegaFit->setItemText(0, QCoreApplication::translate("DockWidget", "No", nullptr));
        comboBoxTcgMegaFit->setItemText(1, QCoreApplication::translate("DockWidget", "Yes", nullptr));

#if QT_CONFIG(tooltip)
        comboBoxTcgMegaFit->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p>Impose a regular fitting of the \317\211 parameter to match at regular intervals the fully converged polarization energy.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        labelTcgMegaFitFreq->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p>Number of timesteps between two updates of the fitted \317\211 parameter. The default of x is 1000.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelTcgMegaFitFreq->setText(QCoreApplication::translate("DockWidget", "Fitting frequency", nullptr));
#if QT_CONFIG(tooltip)
        spinBoxTcgMegaFitFreq->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p>Number of timesteps between two updates of the fitted \317\211 parameter. The default of x is 1000.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        groupBoxTcgShortParameters->setTitle(QCoreApplication::translate("DockWidget", "TCG parameters of short range polarization solver", nullptr));
        labelTcgOrderShort->setText(QCoreApplication::translate("DockWidget", "TCG order", nullptr));
        comboBoxTcgOrderShort->setItemText(0, QCoreApplication::translate("DockWidget", "TCG2", nullptr));
        comboBoxTcgOrderShort->setItemText(1, QCoreApplication::translate("DockWidget", "TCG1", nullptr));

#if QT_CONFIG(tooltip)
        labelDiagPrecShort->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p><span style=\" font-size:9pt;\">Use of a diagonal preconditioner, default is yes.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelDiagPrecShort->setText(QCoreApplication::translate("DockWidget", "Diagonal preconditioner", nullptr));
        comboBoxDiagPrecShort->setItemText(0, QCoreApplication::translate("DockWidget", "Yes", nullptr));
        comboBoxDiagPrecShort->setItemText(1, QCoreApplication::translate("DockWidget", "No", nullptr));

#if QT_CONFIG(tooltip)
        comboBoxDiagPrecShort->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p><span style=\" font-size:9pt;\">Use of a diagonal preconditioner, default is yes.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        labelTcgGuessShort->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p><span style=\" font-size:9pt;\">Use of a \342\200\235direct guess\342\200\235 (polarizability x permanent electric field) as a guess., default is no.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelTcgGuessShort->setText(QCoreApplication::translate("DockWidget", "Direct guess", nullptr));
        comboBoxTcgGuessShort->setItemText(0, QCoreApplication::translate("DockWidget", "No", nullptr));
        comboBoxTcgGuessShort->setItemText(1, QCoreApplication::translate("DockWidget", "Yes", nullptr));

#if QT_CONFIG(tooltip)
        comboBoxTcgGuessShort->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p><span style=\" font-size:9pt;\">Use of a \342\200\235direct guess\342\200\235 (polarizability x permanent electric field) as a guess., default is no.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        labelTcgPeekShort->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p><span style=\" font-size:9pt;\">Use of a peek step, default is yes. When a peek step is used, a Jacobi Over Relaxation (JOR) is applied to the TCG values of the dipoles with a parameter </span><span style=\" font-family:'Arial Unicode MS','DejaVu Sans','Athena','Gentium','Palatino Linotype','Lucida Sans Unicode','Lucida Grande','Code2000','sans-serif'; font-size:14px; color:#202122; background-color:#fcfcfc;\">\317\211.</span><span style=\" font-size:9pt;\"> By default, this value is </span><span style=\" font-family:'Arial Unicode MS','DejaVu Sans','Athena','Gentium','Palatino Linotype','Lucida Sans Unicode','Lucida Grande','Code2000','sans-serif'; font-size:14px; color:#202122; background-color:#fcfcfc;\">\317\211</span><span style=\" font-size:9pt;\"> = 1. (regular Jacobi step) but the following keywords can modify this.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelTcgPeekShort->setText(QCoreApplication::translate("DockWidget", "Peek step", nullptr));
        comboBoxTcgPeekShort->setItemText(0, QCoreApplication::translate("DockWidget", "Yes", nullptr));
        comboBoxTcgPeekShort->setItemText(1, QCoreApplication::translate("DockWidget", "No", nullptr));

#if QT_CONFIG(tooltip)
        comboBoxTcgPeekShort->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p><span style=\" font-size:9pt;\">Use of a peek step, default is yes. When a peek step is used, a Jacobi Over Relaxation (JOR) is applied to the TCG values of the dipoles with a parameter </span><span style=\" font-family:'Arial Unicode MS','DejaVu Sans','Athena','Gentium','Palatino Linotype','Lucida Sans Unicode','Lucida Grande','Code2000','sans-serif'; font-size:14px; color:#202122; background-color:#fcfcfc;\">\317\211.</span><span style=\" font-size:9pt;\"> By default, this value is </span><span style=\" font-family:'Arial Unicode MS','DejaVu Sans','Athena','Gentium','Palatino Linotype','Lucida Sans Unicode','Lucida Grande','Code2000','sans-serif'; font-size:14px; color:#202122; background-color:#fcfcfc;\">\317\211</span><span style=\" font-size:9pt;\"> = 1. (regular Jacobi step) but the following keywords can modify this.</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        labelTcgOmegaShort->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p><span style=\" font-size:9pt;\">Value of the \317\211 parameter</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        labelTcgOmegaShort->setText(QCoreApplication::translate("DockWidget", "Omega (\317\211)", nullptr));
#if QT_CONFIG(tooltip)
        spinBoxTcgOmegaShort->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p><span style=\" font-size:9pt;\">Value of the \317\211 parameter</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        groupBoxAdditionnalKeywords->setTitle(QCoreApplication::translate("DockWidget", "Additionnal Keywords", nullptr));
#if QT_CONFIG(tooltip)
        pushButtonGenerate->setToolTip(QCoreApplication::translate("DockWidget", "<html><head/><body><p>Generate keyword file</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButtonGenerate->setText(QCoreApplication::translate("DockWidget", "Generate", nullptr));
        tabWidgetKeywordEditor->setTabText(tabWidgetKeywordEditor->indexOf(tabDynamic), QCoreApplication::translate("DockWidget", "Dynamic", nullptr));
        tabWidgetKeywordEditor->setTabText(tabWidgetKeywordEditor->indexOf(tab_2), QCoreApplication::translate("DockWidget", "Free Energy", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DockWidget: public Ui_DockWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KEYWORD_EDITOR_MAIN_WINDOW_H
