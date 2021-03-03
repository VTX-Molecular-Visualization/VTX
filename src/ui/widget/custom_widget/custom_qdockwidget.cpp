#include "custom_qdockwidget.hpp"
#include <QFileDialog>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QStandardPaths>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			CustomQDockWidget::CustomQDockWidget( QWidget * p_parent ) : QDockWidget( p_parent )
			{
				ui.setupUi( this );

				// set dock window style
				ui.dockWidgetContents->setStyleSheet(
					"QWidget#dockWidgetContents {"
					"border-top: none;"
					"border-left: 1px solid #444444;"
					"border-bottom: 1px solid #111111;"
					"border-right: 1px solid #111111;"
					"}" );

				// set title bar and its style
				QWidget * title_bar = new QWidget( this );
				title_bar->setObjectName( "titleBarWidget" );
				title_bar->setFixedHeight( 25 );
				QHBoxLayout * layout = new QHBoxLayout();
				layout->setContentsMargins( QMargins( 5, 0, 5, 0 ) );
				title_bar->setLayout( layout );
				title_bar->setStyleSheet(
					"QWidget#titleBarWidget {"
					"background-color: rgba( 34, 34, 34, 34 ); "
					"border-top: 1px solid #444444; "
					"border-left: 1px solid #444444; "
					"border-bottom: 1px solid #111111; "
					"border-right: 1px solid #111111;"
					"}" );
				this->setTitleBarWidget( title_bar );
			}

			void CustomQDockWidget::setupModuleWindow()
			{
				// set module's window content
				setWindowContent( moduleWidget() );
				// set module's window icon
				setWindowIcon( moduleIcon() );
				// set module's window title
				setWindowTitle( moduleTitle() );
				// set module's window properties
				setWindowProperties( moduleWindowProperties() );
				// set module's window undock button
				setWindowDockableButton();
				// set module's window exit button
				setWindowExitButton();
			}

			void CustomQDockWidget::setWindowContent( QWidget * moduleWidget )
			{
				if ( moduleWidget != nullptr )
				{
					ui.scrollAreaWidgetContents->layout()->addWidget( moduleWidget );
					ui.scrollAreaWidgetContents->updateGeometry();
				}
			}

			void CustomQDockWidget::setWindowIcon( QString windowIcon )
			{
				QWidget *	  title_bar	  = this->titleBarWidget();
				QHBoxLayout * titleLayout = static_cast<QHBoxLayout *>( title_bar->layout() );
				QIcon		  winIcon	  = QIcon::fromTheme( windowIcon );
				QLabel *	  iconLabel	  = new QLabel( title_bar );
				iconLabel->setPixmap( winIcon.pixmap( 15, 15 ) );
				iconLabel->setStyleSheet( "QLabel { background-color: rgba(255, 255, 255, 0); }" );
				titleLayout->addWidget( iconLabel );
			}

			void CustomQDockWidget::setWindowTitle( QString windowTitle )
			{
				QWidget *	  title_bar	  = this->titleBarWidget();
				QHBoxLayout * titleLayout = static_cast<QHBoxLayout *>( title_bar->layout() );
				QLabel *	  winTitle	  = new QLabel();
				winTitle->setText( windowTitle );
				winTitle->setStyleSheet( "QLabel { color : white; }" );
				QSpacerItem * spacaer
					= new QSpacerItem( 20, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum );
				titleLayout->addWidget( winTitle );
				titleLayout->addSpacerItem( spacaer );
			}

			void CustomQDockWidget::setWindowProperties( std::vector<bool> * windowProperties )
			{
				if ( windowProperties != nullptr )
				{
					if ( windowProperties->size() == 3 )
					{
						QWidget *	  title_bar	  = this->titleBarWidget();
						QHBoxLayout * titleLayout = static_cast<QHBoxLayout *>( title_bar->layout() );
						// if window is savable
						if ( windowProperties->at( 0 ) )
						{
							QPushButton * settingButton = new QPushButton( title_bar );
							settingButton->setToolTip( "Save settings window" );

							settingButton->setStyleSheet(
								"QPushButton {"
								"border-image: url(:/sprite/save_settings.png);"
								"background-color: rgba(255, 255, 255, 0);"
								"background-repeat: no-repeat;"
								"width: 10px;"
								"height : 10px;"
								"}"
								"QPushButton:hover {"
								"border-image: url(:/sprite/save_settings_hovered.png);"
								"background-color: rgba(255, 255, 255, 0);"
								"background-repeat: no-repeat;"
								"}" );

							QObject::connect(
								settingButton, SIGNAL( clicked() ), this, SLOT( onSettingsButtonClicked() ) );
							titleLayout->addWidget( settingButton );
						}
						// if window is lockable
						if ( windowProperties->at( 1 ) )
						{
							QPushButton * lockButton = new QPushButton( title_bar );
							lockButton->setToolTip( "Lock window" );

							lockButton->setStyleSheet(
								"QPushButton {"
								"border-image: url(:/sprite/save_settings.png);"
								"background-color: rgba(255, 255, 255, 0);"
								"background-repeat: no-repeat;"
								"width: 10px;"
								"height : 10px;"
								"}"
								"QPushButton:hover {"
								"border-image: url(:/sprite/save_settings_hovered.png);"
								"background-color: rgba(255, 255, 255, 0);"
								"background-repeat: no-repeat;"
								"}" );

							QObject::connect( lockButton, SIGNAL( clicked() ), this, SLOT( onLockButtonClicked() ) );
							titleLayout->addWidget( lockButton );
						}
						// if window is citable
						if ( windowProperties->at( 2 ) )
						{
							QPushButton * citeButton = new QPushButton( title_bar );
							citeButton->setToolTip( "Open citations window" );

							citeButton->setStyleSheet(
								"QPushButton {"
								"border-image: url(:/sprite/save_settings.png);"
								"background-color: rgba(255, 255, 255, 0);"
								"background-repeat: no-repeat;"
								"width: 10px;"
								"height : 10px;"
								"}"
								"QPushButton:hover {"
								"border-image: url(:/sprite/save_settings_hovered.png);"
								"background-color: rgba(255, 255, 255, 0);"
								"background-repeat: no-repeat;"
								"}" );

							QObject::connect(
								citeButton, SIGNAL( clicked() ), this, SLOT( onCitationsButtonClicked() ) );
							titleLayout->addWidget( citeButton );
						}
					}
				}
			}

			void CustomQDockWidget::setWindowDockableButton()
			{
				QWidget *	  title_bar	   = this->titleBarWidget();
				QHBoxLayout * titleLayout  = static_cast<QHBoxLayout *>( title_bar->layout() );
				QPushButton * undockButton = new QPushButton( title_bar );
				undockButton->setToolTip( "Dock window" );
				undockButton->setStyleSheet(
					"QPushButton {"
					"border-image: url(:/sprite/undock_icon.png);"
					"background-color: rgba(255, 255, 255, 0);"
					"background-repeat: no-repeat;"
					"width: 10px;"
					"height : 10px;"
					"}"
					"QPushButton:hover {"
					"border-image: url(:/sprite/undock_icon_hovered.png);"
					"background-color: rgba(255, 255, 255, 0);"
					"background-repeat: no-repeat;"
					"width: 10px;"
					"height : 10px;"
					"}" );
				QObject::connect( undockButton, SIGNAL( clicked() ), this, SLOT( onUndockButtonClicked() ) );
				titleLayout->addWidget( undockButton );
			}

			void CustomQDockWidget::setWindowExitButton()
			{
				QWidget *	  title_bar	  = this->titleBarWidget();
				QHBoxLayout * titleLayout = static_cast<QHBoxLayout *>( title_bar->layout() );
				QPushButton * exitButton  = new QPushButton( title_bar );
				exitButton->setToolTip( "Close window" );
				exitButton->setStyleSheet(
					"QPushButton {"
					"border-image: url(:/sprite/close_icon.png);"
					"background-color: rgba(255, 255, 255, 0);"
					"background-repeat: no-repeat;"
					"width: 10px;"
					"height : 10px;"
					"}"
					"QPushButton:hover {"
					"border-image: url(:/sprite/close_icon_hovered.png);"
					"background-color: rgba(255, 255, 255, 0);"
					"background-repeat: no-repeat;"
					"width: 10px;"
					"height : 10px;"
					"}" );
				QObject::connect( exitButton, SIGNAL( clicked() ), this, SLOT( onExitButtonClicked() ) );
				titleLayout->addWidget( exitButton );
			}

			// QString CustomQDockWidget::moduleIcon() { return QString(); }

			// QString CustomQDockWidget::moduleTitle() { return QString(); }

			// std::vector<bool> * CustomQDockWidget::moduleWindowProperties() { return new std::vector<bool>(); }

			// QWidget * CustomQDockWidget::moduleWidget() { return nullptr; }

			void CustomQDockWidget::onUndockButtonClicked() { this->setFloating( true ); }

			void CustomQDockWidget::onExitButtonClicked() { this->hide(); }

			void CustomQDockWidget::onLockButtonClicked() {}

			void CustomQDockWidget::onCitationsButtonClicked() {}

			void CustomQDockWidget::onSettingsButtonClicked()
			{
				if ( settingsWindow == nullptr )
				{
					settingsWindow = new SettingsWindowWidget( this );
					// check if settings have been saved before
					QSettings settings = QSettings( "Qubit", "KeywordEditor" );
					if ( settings.childGroups().size() != 0 )
					{
						settingsWindow->populateSettingsList( &settings.childGroups() );
					}
					QObject::connect( settingsWindow->ui.pushButtonSaveSettings,
									  SIGNAL( clicked() ),
									  this,
									  SLOT( saveSettingsClicked() ) );
					QObject::connect( settingsWindow->ui.pushButtonLoadSetting,
									  SIGNAL( clicked() ),
									  this,
									  SLOT( loadSettingsClicked() ) );
					QObject::connect( settingsWindow->ui.pushButtonRemoveSetting,
									  SIGNAL( clicked() ),
									  this,
									  SLOT( removeSettingsClicked() ) );
					settingsWindow->show();
				}
				else
				{
					if ( settingsWindow->isVisible() )
					{
						settingsWindow->raise();
					}
					else
					{
						settingsWindow->show();
					}
				}
			}

			void CustomQDockWidget::saveSettingsClicked()
			{
				// QString settingName = settingsWindow->ui.lineEditSettingsName->text();
				// if ( settingName != "" )
				//{
				//	QSettings settings = QSettings( "Qubit", "KeywordEditor" );
				//	// save the values of all the widgets
				//	settings.beginGroup( settingName );
				//	settings.setValue( "integrator", ui.comboBoxIntegrator->currentIndex() );
				//	settings.setValue( "friction", ui.spinBoxFriction->value() );
				//	settings.setValue( "inner_timestep", ui.spinBoxInnerTimestep->value() );
				//	settings.setValue( "inter_timestep", ui.spinBoxIntermediateTimestep->value() );
				//	settings.setValue( "thermostat", ui.comboBoxThermostat->currentIndex() );
				//	settings.setValue( "barostat", ui.comboBoxBarostat->currentIndex() );
				//	settings.setValue( "polarisation_solver", ui.comboBoxPolarizationEquations->currentIndex() );
				//	settings.setValue( "sr_polarization_solver", ui.comboBoxShortRangePolarizationSolver->currentIndex()
				//); 	settings.setValue( "piston_mass", ui.spinBoxMassPiston->value() ); 	settings.setValue(
				//"piston_friction", ui.spinBoxFrictionPiston->value() ); 	settings.setValue( "tcg_roder",
				// ui.comboBoxTcgOrder->currentIndex() ); 	settings.setValue( "diagonal_preconditioner",
				// ui.comboBoxDiagPrec->currentIndex() ); 	settings.setValue( "tcg_guess",
				// ui.comboBoxTcgGuess->currentIndex() ); 	settings.setValue( "peek_step",
				// ui.comboBoxTcgPeek->currentIndex() ); 	settings.setValue( "omega", ui.spinBoxTcgOmega->value() );
				//	settings.setValue( "fitting", ui.comboBoxTcgMegaFit->currentIndex() );
				//	settings.setValue( "fitting_frequency", ui.spinBoxTcgMegaFitFreq->value() );
				//	settings.setValue( "tcg_order_short", ui.comboBoxTcgOrderShort->currentIndex() );
				//	settings.setValue( "diagonal_preconditioner_short", ui.comboBoxDiagPrecShort->currentIndex() );
				//	settings.setValue( "direct_guess_short", ui.comboBoxTcgGuessShort->currentIndex() );
				//	settings.setValue( "peek_step_short", ui.comboBoxTcgPeekShort->currentIndex() );
				//	settings.setValue( "omega_short", ui.spinBoxTcgOmegaShort->value() );
				//	settings.setValue( "additional_keywords", ui.textBrowserAdditionnalKeywords->toPlainText() );
				//	settings.endGroup();
				//}
				// settingsWindow->ui.lineEditSettingsName->clear();
			}

			void CustomQDockWidget::loadSettingsClicked()
			{
				// QList<QListWidgetItem *> selectedListItem =
				// settingsWindow->ui.listWidgetSavedSettings->selectedItems(); if (
				// settingsWindow->ui.listWidgetSavedSettings->selectedItems().size() == 1 )
				//{
				//	QSettings settings			  = QSettings( "Qubit", "KeywordEditor" );
				//	QString	  settingsProfileName = selectedListItem[ 0 ]->text();
				//	settings.beginGroup( settingsProfileName );
				//	ui.comboBoxIntegrator->setCurrentIndex( settings.value( "integrator" ).toInt() );
				//	ui.spinBoxFriction->setValue( settings.value( "friction" ).toInt() );
				//	ui.spinBoxInnerTimestep->setValue( settings.value( "inner_timestep" ).toInt() );
				//	ui.spinBoxIntermediateTimestep->setValue( settings.value( "inter_timestep" ).toInt() );
				//	ui.comboBoxThermostat->setCurrentIndex( settings.value( "thermostat" ).toInt() );
				//	ui.comboBoxBarostat->setCurrentIndex( settings.value( "barostat" ).toInt() );
				//	ui.comboBoxPolarizationEquations->setCurrentIndex( settings.value( "polarisation_solver" ).toInt()
				//); 	ui.comboBoxShortRangePolarizationSolver->setCurrentIndex( settings.value(
				//"sr_polarization_solver"
				//).toInt() ); 	ui.spinBoxMassPiston->setValue( settings.value( "piston_mass" ).toInt() );
				//	ui.spinBoxFrictionPiston->setValue( settings.value( "piston_friction" ).toInt() );
				//	ui.comboBoxTcgOrder->setCurrentIndex( settings.value( "tcg_roder" ).toInt() );
				//	ui.comboBoxDiagPrec->setCurrentIndex( settings.value( "diagonal_preconditioner" ).toInt() );
				//	ui.comboBoxTcgGuess->setCurrentIndex( settings.value( "tcg_guess" ).toInt() );
				//	ui.comboBoxTcgPeek->setCurrentIndex( settings.value( "peek_step" ).toInt() );
				//	ui.spinBoxTcgOmega->setValue( settings.value( "omega" ).toInt() );
				//	ui.comboBoxTcgMegaFit->setCurrentIndex( settings.value( "fitting" ).toInt() );
				//	ui.spinBoxTcgMegaFitFreq->setValue( settings.value( "fitting_frequency" ).toInt() );
				//	ui.comboBoxTcgOrderShort->setCurrentIndex( settings.value( "tcg_order_short" ).toInt() );
				//	ui.comboBoxDiagPrecShort->setCurrentIndex( settings.value( "diagonal_preconditioner_short" ).toInt()
				//); 	ui.comboBoxTcgGuessShort->setCurrentIndex( settings.value( "direct_guess_short" ).toInt() );
				//	ui.comboBoxTcgPeekShort->setCurrentIndex( settings.value( "peek_step_short" ).toInt() );
				//	ui.spinBoxTcgOmegaShort->setValue( settings.value( "omega_short" ).toInt() );
				//	ui.textBrowserAdditionnalKeywords->setText( settings.value( "additional_keywords" ).toString() );
				//	settings.endGroup();
				//}
			}

			void CustomQDockWidget::removeSettingsClicked()
			{
				QList<QListWidgetItem *> selectedListItem = settingsWindow->ui.listWidgetSavedSettings->selectedItems();
				if ( selectedListItem.size() == 1 )
				{
					QSettings settings		  = QSettings( "Qubit", "KeywordEditor" );
					QString	  selectedProfile = selectedListItem[ 0 ]->text();
					settings.remove( selectedProfile );
					delete settingsWindow->ui.listWidgetSavedSettings->takeItem(
						settingsWindow->ui.listWidgetSavedSettings->row( selectedListItem[ 0 ] ) );
				}
			}
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
