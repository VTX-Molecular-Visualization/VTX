#include "extension_keyword_editor_gui.hpp"
//#include "vtx_app.hpp"
#include <QFileDialog>
#include <QPushButton>
#include <QSpacerItem>
#include <QStandardPaths>

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
						// 1: lockable
						// 2: citable
						std::vector<bool> * windowProperties = new std::vector<bool>();
						windowProperties->push_back( true );
						windowProperties->push_back( true );
						windowProperties->push_back( true );
						return windowProperties;
					}

					QWidget * ExtensionKeywordEditorGUI::moduleWidget()
					{
						// set module window widgets with a widget containing all the module's widgets
						return ui.tabWidgetKeywordEditor;
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
