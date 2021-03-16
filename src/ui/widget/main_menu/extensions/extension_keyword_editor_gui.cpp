#include "extension_keyword_editor_gui.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace MainMenu
			{
				namespace Extensions
				{
					ExtensionKeywordEditorGUI::ExtensionKeywordEditorGUI( QWidget * parent ) :
						CustomQDockWidget(
							parent->parentWidget()->parentWidget()->parentWidget()->parentWidget()->parentWidget() )
					{
						ui.setupUi( this );
						setupModuleWindow();

						QIcon infoIcon = QIcon( ":/sprite/info_button.png" );
						ui.labelIntegratorInfo->setPixmap( infoIcon.pixmap( 16, 16 ) );
						ui.labelIntegratorInfo->setStyleSheet( "QLabel { background-color: rgba(255, 255, 255, 0); }" );
					}

					QString ExtensionKeywordEditorGUI::moduleIcon()
					{
						// set module window icon
						return QString( ":/sprite/keyword_editor.png" );
					}

					QString ExtensionKeywordEditorGUI::moduleTitle()
					{
						// set module window title
						return QString( "Tinker-HP Keyword Editor" );
					}

					std::vector<bool> * ExtensionKeywordEditorGUI::moduleWindowProperties()
					{
						// set window properties:
						// 0: savable
						// 1: citable
						std::vector<bool> * windowProperties = new std::vector<bool>();
						windowProperties->push_back( true );
						windowProperties->push_back( true );
						return windowProperties;
					}

					QWidget * ExtensionKeywordEditorGUI::moduleWidget()
					{
						// set module window widgets with a widget containing all the module's widgets
						return ui.tabWidgetKeywordEditor;
					}

					QString ExtensionKeywordEditorGUI::moduleCitations()
					{
						return QString(
							"Tinker-HP: a Massively Parallel Molecular Dynamics Package for Multiscale Simulations of "
							"Large Complex Systems with Advanced Polarizable Force Fields. L. Lagardère, L.-H. Jolly, "
							"F. Lipparini, F. Aviat, B. Stamm, Z. F. Jing, M. Harger, H. Torabifard, G. A. Cisneros, "
							"M. J. Schnieders, N. Gresh, Y. Maday, P. Ren, J. W. Ponder, J.-P. Piquemal, Chem. Sci., "
							"2018, 9, 956-972 (Open Access) "
							"[https://doi.org/10.1039/C7SC04531J](https://doi.org/10.1039/C7SC04531J) " );
					}

					void ExtensionKeywordEditorGUI::saveSettings( QSettings * settings )
					{
						// save the values of all the widgets
						settings->setValue( "integrator", ui.comboBoxIntegrator->currentIndex() );
						settings->setValue( "friction", ui.spinBoxFriction->value() );
						settings->setValue( "inner_timestep", ui.spinBoxInnerTimestep->value() );
						settings->setValue( "inter_timestep", ui.spinBoxIntermediateTimestep->value() );
						settings->setValue( "thermostat", ui.comboBoxThermostat->currentIndex() );
						settings->setValue( "barostat", ui.comboBoxBarostat->currentIndex() );
						settings->setValue( "polarisation_solver", ui.comboBoxPolarizationEquations->currentIndex() );
						settings->setValue( "sr_polarization_solver",
											ui.comboBoxShortRangePolarizationSolver->currentIndex() );
						settings->setValue( "piston_mass", ui.spinBoxMassPiston->value() );
						settings->setValue( "piston_friction", ui.spinBoxFrictionPiston->value() );
						settings->setValue( "tcg_roder", ui.comboBoxTcgOrder->currentIndex() );
						settings->setValue( "diagonal_preconditioner", ui.comboBoxDiagPrec->currentIndex() );
						settings->setValue( "tcg_guess", ui.comboBoxTcgGuess->currentIndex() );
						settings->setValue( "peek_step", ui.comboBoxTcgPeek->currentIndex() );
						settings->setValue( "omega", ui.spinBoxTcgOmega->value() );
						settings->setValue( "fitting", ui.comboBoxTcgMegaFit->currentIndex() );
						settings->setValue( "fitting_frequency", ui.spinBoxTcgMegaFitFreq->value() );
						settings->setValue( "tcg_order_short", ui.comboBoxTcgOrderShort->currentIndex() );
						settings->setValue( "diagonal_preconditioner_short", ui.comboBoxDiagPrecShort->currentIndex() );
						settings->setValue( "direct_guess_short", ui.comboBoxTcgGuessShort->currentIndex() );
						settings->setValue( "peek_step_short", ui.comboBoxTcgPeekShort->currentIndex() );
						settings->setValue( "omega_short", ui.spinBoxTcgOmegaShort->value() );
						settings->setValue( "additional_keywords", ui.textBrowserAdditionnalKeywords->toPlainText() );
					}

					void ExtensionKeywordEditorGUI::loadSettings( QSettings * settings )
					{
						ui.comboBoxIntegrator->setCurrentIndex( settings->value( "integrator" ).toInt() );
						ui.spinBoxFriction->setValue( settings->value( "friction" ).toInt() );
						ui.spinBoxInnerTimestep->setValue( settings->value( "inner_timestep" ).toInt() );
						ui.spinBoxIntermediateTimestep->setValue( settings->value( "inter_timestep" ).toInt() );
						ui.comboBoxThermostat->setCurrentIndex( settings->value( "thermostat" ).toInt() );
						ui.comboBoxBarostat->setCurrentIndex( settings->value( "barostat" ).toInt() );
						ui.comboBoxPolarizationEquations->setCurrentIndex(
							settings->value( "polarisation_solver" ).toInt() );
						ui.comboBoxShortRangePolarizationSolver->setCurrentIndex(
							settings->value( "sr_polarization_solver" ).toInt() );
						ui.spinBoxMassPiston->setValue( settings->value( "piston_mass" ).toInt() );
						ui.spinBoxFrictionPiston->setValue( settings->value( "piston_friction" ).toInt() );
						ui.comboBoxTcgOrder->setCurrentIndex( settings->value( "tcg_roder" ).toInt() );
						ui.comboBoxDiagPrec->setCurrentIndex( settings->value( "diagonal_preconditioner" ).toInt() );
						ui.comboBoxTcgGuess->setCurrentIndex( settings->value( "tcg_guess" ).toInt() );
						ui.comboBoxTcgPeek->setCurrentIndex( settings->value( "peek_step" ).toInt() );
						ui.spinBoxTcgOmega->setValue( settings->value( "omega" ).toInt() );
						ui.comboBoxTcgMegaFit->setCurrentIndex( settings->value( "fitting" ).toInt() );
						ui.spinBoxTcgMegaFitFreq->setValue( settings->value( "fitting_frequency" ).toInt() );
						ui.comboBoxTcgOrderShort->setCurrentIndex( settings->value( "tcg_order_short" ).toInt() );
						ui.comboBoxDiagPrecShort->setCurrentIndex(
							settings->value( "diagonal_preconditioner_short" ).toInt() );
						ui.comboBoxTcgGuessShort->setCurrentIndex( settings->value( "direct_guess_short" ).toInt() );
						ui.comboBoxTcgPeekShort->setCurrentIndex( settings->value( "peek_step_short" ).toInt() );
						ui.spinBoxTcgOmegaShort->setValue( settings->value( "omega_short" ).toInt() );
						ui.textBrowserAdditionnalKeywords->setText(
							settings->value( "additional_keywords" ).toString() );
					}

					void ExtensionKeywordEditorGUI::onOpenKeyfilePushButtonClicked()
					{
						QString fileName
							= QFileDialog::getOpenFileName( this,
															tr( "Open Keyword file" ),
															QString( QStandardPaths::DocumentsLocation ),
															tr( "Keyword file (*.key);;All files (*.*)" ) );
						if ( fileName != "" )
						{
							keywordEditor = new ExtensionKeywordEditor();
							if ( keywordEditor->readKeywordFile( fileName ) )
							{
								populateInterfaceWithValues();
							}
						}
					}

					void ExtensionKeywordEditorGUI::onGenerateKeyfilePushButtonClicked()
					{
						QString fileName = QFileDialog::getSaveFileName(
							this, tr( "Save File" ), QDir::homePath(), tr( "Keyword file (*.key);;All files (*.*)" ) );
						if ( fileName != "" )
						{
							if ( keywordEditor == nullptr )
							{
								keywordEditor = new ExtensionKeywordEditor();
							}
							QStringList keywordsWithValue;
							keywordsWithValue.append( "integrator " + ui.comboBoxIntegrator->currentText() );
							if ( ui.spinBoxFriction->isEnabled() )
							{
								keywordsWithValue.append( "friction "
														  + QString::number( ui.spinBoxFriction->value() ) );
							}
							if ( ui.spinBoxInnerTimestep->isEnabled() )
							{
								keywordsWithValue.append( "dshort "
														  + QString::number( ui.spinBoxInnerTimestep->value() ) );
							}
							if ( ui.spinBoxIntermediateTimestep->isEnabled() )
							{
								keywordsWithValue.append(
									"dinter " + QString::number( ui.spinBoxIntermediateTimestep->value() ) );
							}
							if ( ui.comboBoxThermostat->isEnabled() )
							{
								keywordsWithValue.append( "thermostat " + ui.comboBoxThermostat->currentText() );
							}
							if ( ui.comboBoxBarostat->isEnabled() )
							{
								keywordsWithValue.append( "barostat " + ui.comboBoxBarostat->currentText() );
							}
							if ( ui.comboBoxPolarizationEquations->isEnabled() )
							{
								if ( ui.comboBoxPolarizationEquations->currentIndex() != 0 )
								{
									keywordsWithValue.append( "polar-alg "
															  + ui.comboBoxPolarizationEquations->currentIndex() );
								}
								else
								{
									keywordsWithValue.append( "polar-alg 5" );
								}
							}
							if ( ui.comboBoxShortRangePolarizationSolver->isEnabled() )
							{
								if ( ui.comboBoxShortRangePolarizationSolver->currentIndex() != 0 )
								{
									keywordsWithValue.append(
										"polar-algshort " + ui.comboBoxShortRangePolarizationSolver->currentIndex() );
								}
								else
								{
									keywordsWithValue.append( "polar-algshort 5" );
								}
							}
							if ( ui.widgetContainingTcgParameters->isEnabled() )
							{
								if ( ui.comboBoxTcgOrder->isEnabled() )
								{
									keywordsWithValue.append( "tcgorder " + ui.comboBoxTcgOrder->currentText() );
								}
								if ( ui.comboBoxDiagPrec->isEnabled() )
								{
									keywordsWithValue.append(
										"tcgprec " + QString::number( ui.comboBoxDiagPrec->currentIndex() ) );
								}
								if ( ui.comboBoxTcgGuess->isEnabled() )
								{
									keywordsWithValue.append(
										"tcgguess " + QString::number( ui.comboBoxTcgGuess->currentIndex() ) );
								}
								if ( ui.comboBoxTcgPeek->isEnabled() )
								{
									keywordsWithValue.append( "tcgpeek  "
															  + QString::number( ui.comboBoxTcgPeek->currentIndex() ) );
								}
								if ( ui.spinBoxTcgOmega->isEnabled() )
								{
									keywordsWithValue.append( "tcgomega  "
															  + QString::number( ui.spinBoxTcgOmega->value() ) );
								}
								if ( ui.comboBoxTcgMegaFit->isEnabled() )
								{
									keywordsWithValue.append(
										"tcgomegafit " + QString::number( ui.comboBoxTcgMegaFit->currentIndex() ) );
								}
								if ( ui.spinBoxTcgMegaFitFreq->isEnabled() )
								{
									keywordsWithValue.append( "tcgomegafitfreq "
															  + QString::number( ui.spinBoxTcgMegaFitFreq->value() ) );
								}
							}
							if ( ui.widgetTcgParametersShort->isEnabled() )
							{
								if ( ui.comboBoxTcgOrderShort->isEnabled() )
								{
									keywordsWithValue.append( "tcgordershort "
															  + ui.comboBoxTcgOrderShort->currentText() );
								}
								if ( ui.comboBoxDiagPrecShort->isEnabled() )
								{
									keywordsWithValue.append(
										"tcgprecshort " + QString::number( ui.comboBoxDiagPrecShort->currentIndex() ) );
								}
								if ( ui.comboBoxTcgGuessShort->isEnabled() )
								{
									keywordsWithValue.append(
										"tcgguessshort "
										+ QString::number( ui.comboBoxTcgGuessShort->currentIndex() ) );
								}
								if ( ui.comboBoxTcgPeekShort->isEnabled() )
								{
									keywordsWithValue.append(
										"tcgpeekshort  " + QString::number( ui.comboBoxTcgPeekShort->currentIndex() ) );
								}
								if ( ui.spinBoxTcgOmegaShort->isEnabled() )
								{
									keywordsWithValue.append( "tcgomegashort  "
															  + QString::number( ui.spinBoxTcgOmegaShort->value() ) );
								}
							}
							if ( ui.spinBoxMassPiston->isEnabled() )
							{
								keywordsWithValue.append( "masspiston "
														  + QString::number( ui.spinBoxMassPiston->value() ) );
							}
							if ( ui.spinBoxFrictionPiston->isEnabled() )
							{
								keywordsWithValue.append( "frictionpiston "
														  + QString::number( ui.spinBoxFrictionPiston->value() ) );
							}
							if ( !ui.textBrowserAdditionnalKeywords->toPlainText().isEmpty() )
							{
								keywordsWithValue.append( ui.textBrowserAdditionnalKeywords->toPlainText() );
							}
							keywordEditor->writeKeywordFile( fileName, keywordsWithValue );
						}
					}

					void ExtensionKeywordEditorGUI::populateInterfaceWithValues()
					{
						std::vector<std::pair<QString, QString>> * keywords = keywordEditor->getKeywords();
						if ( keywords != nullptr )
						{
							for ( size_t keyword = 0; keyword < keywords->size(); keyword++ )
							{
								if ( keywords->at( keyword ).first == "integrator" )
								{
									ui.comboBoxIntegrator->setCurrentText( keywords->at( keyword ).second.toUpper() );
								}
								else if ( keywords->at( keyword ).first == "friction" )
								{
									ui.spinBoxFriction->setValue( keywords->at( keyword ).second.toInt() );
								}
								else if ( keywords->at( keyword ).first == "dshort" )
								{
									ui.spinBoxInnerTimestep->setValue( keywords->at( keyword ).second.toInt() );
								}
								else if ( keywords->at( keyword ).first == "dinter" )
								{
									ui.spinBoxIntermediateTimestep->setValue( keywords->at( keyword ).second.toInt() );
								}
								else if ( keywords->at( keyword ).first == "thermostat" )
								{
									ui.comboBoxThermostat->setCurrentText( keywords->at( keyword ).second );
								}
								else if ( keywords->at( keyword ).first == "barostat" )
								{
									ui.comboBoxBarostat->setCurrentText( keywords->at( keyword ).second );
								}
								else if ( keywords->at( keyword ).first == "polar-alg" )
								{
									if ( keywords->at( keyword ).second.toInt() < 5 )
									{
										ui.comboBoxPolarizationEquations->setCurrentIndex(
											keywords->at( keyword ).second.toInt() );
									}
									else if ( keywords->at( keyword ).second.toInt() == 5 )
									{
										ui.comboBoxPolarizationEquations->setCurrentIndex( 0 );
									}
								}
								// populate tcg parameters
								// populate tcg short parameters
								// if the keyword is not present in the interface -> add to the additionnal keywords
								else
								{
									ui.textBrowserAdditionnalKeywords->append( keywords->at( keyword ).first + " "
																			   + keywords->at( keyword ).second );
								}
							}
						}
					}

					void ExtensionKeywordEditorGUI::comboBoxIntegratorIndexChanged( QString newSelectedItem )
					{
						// if RESPA1 and BAOABRESPA1 integrators are used, enable short range polarization solver
						// widgets
						if ( newSelectedItem == "RESPA1" || newSelectedItem == "BAOABRESPA1" )
						{
							ui.labelShortRangePolarizationSolver->setEnabled( true );
							ui.comboBoxShortRangePolarizationSolver->setEnabled( true );
							QObject::connect( ui.comboBoxPolarizationEquations,
											  SIGNAL( currentTextChanged( QString ) ),
											  ui.comboBoxShortRangePolarizationSolver,
											  SLOT( setCurrentText( QString ) ) );
							ui.comboBoxShortRangePolarizationSolver->setCurrentText(
								ui.comboBoxPolarizationEquations->currentText() );
						}
						else
						{
							ui.labelShortRangePolarizationSolver->setEnabled( false );
							ui.comboBoxShortRangePolarizationSolver->setEnabled( false );
							QObject::disconnect( ui.comboBoxPolarizationEquations,
												 SIGNAL( currentTextChanged( QString ) ),
												 ui.comboBoxShortRangePolarizationSolver,
												 SLOT( setCurrentText( QString ) ) );
						}
						// For all the Langevin integrators (BBK, BAOAB, BAOABRESPA, BAOABRESPA1 and BAOABPISTON) enable
						// the friction
						if ( newSelectedItem.contains( "BAOAB" ) || newSelectedItem == "BBK" )
						{
							ui.labelFriction->setEnabled( true );
							ui.spinBoxFriction->setEnabled( true );
						}
						else
						{
							ui.labelFriction->setEnabled( false );
							ui.spinBoxFriction->setEnabled( false );
						}
						if ( newSelectedItem.contains( "RESPA" ) )
						{
							ui.labelInnerTimestep->setEnabled( true );
							ui.spinBoxInnerTimestep->setEnabled( true );
						}
						else
						{
							ui.labelInnerTimestep->setEnabled( false );
							ui.spinBoxInnerTimestep->setEnabled( false );
						}
						if ( newSelectedItem.contains( "RESPA1" ) )
						{
							ui.labelIntermediateTimestep->setEnabled( true );
							ui.spinBoxIntermediateTimestep->setEnabled( true );
						}
						else
						{
							ui.labelIntermediateTimestep->setEnabled( false );
							ui.spinBoxIntermediateTimestep->setEnabled( false );
						}
						if ( newSelectedItem == "BAOABPISTON" )
						{
							ui.labelMassPiston->setEnabled( true );
							ui.spinBoxMassPiston->setEnabled( true );
							ui.labelFrictionPiston->setEnabled( true );
							ui.spinBoxFrictionPiston->setEnabled( true );
						}
						else
						{
							ui.labelMassPiston->setEnabled( false );
							ui.spinBoxMassPiston->setEnabled( false );
							ui.labelFrictionPiston->setEnabled( false );
							ui.spinBoxFrictionPiston->setEnabled( false );
						}
					}

					void ExtensionKeywordEditorGUI::comboBoxShortRangePolSolndexChanged( QString newSelectedItem )
					{
						// if TCG is selected enable groupbox for TCG short range polarization solver
						if ( newSelectedItem == "TCG" )
						{
							ui.widgetTcgParametersShort->setEnabled( true );
						}
						// else disable TCG parameters groupbox
						else
						{
							ui.widgetTcgParametersShort->setEnabled( false );
						}
					}

					void ExtensionKeywordEditorGUI::comboBoxPolEquaIndexChanged( QString newSelectedItem )
					{
						// if TCG is selected enable TCG parameters groupbox
						if ( newSelectedItem == "TCG" )
						{
							ui.widgetContainingTcgParameters->setEnabled( true );
						}
						// else disable TCG parameters groupbox
						else
						{
							ui.widgetContainingTcgParameters->setEnabled( false );
						}
					}

					void ExtensionKeywordEditorGUI::comboBoxPeekStepIndexChanged( QString newSelectedItem )
					{
						// if direct guess is activated enable omega keywords widget
						if ( newSelectedItem == "Yes" )
						{
							ui.widgetPeekStep->setEnabled( true );
						}
						// else disable omega keywords widget
						else
						{
							ui.widgetPeekStep->setEnabled( false );
						}
					}

					void ExtensionKeywordEditorGUI::comboBoxFittingIndexChanged( QString newSelectedItem )
					{
						// if fitting is activated enable omega fitting frequency widgets
						if ( newSelectedItem == "Yes" )
						{
							ui.labelTcgMegaFitFreq->setEnabled( true );
							ui.spinBoxTcgMegaFitFreq->setEnabled( true );
						}
						// else disable omega fitting frequency widgets
						else
						{
							ui.labelTcgMegaFitFreq->setEnabled( false );
							ui.spinBoxTcgMegaFitFreq->setEnabled( false );
						}
					}
				} // namespace Extensions
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
