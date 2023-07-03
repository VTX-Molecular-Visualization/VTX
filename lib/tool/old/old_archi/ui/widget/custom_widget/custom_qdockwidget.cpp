#include "tool/old_tool/ui/widget/custom_widget/custom_qdockwidget.hpp"
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
					if ( windowProperties->size() == 2 )
					{
						QWidget *	  title_bar	  = this->titleBarWidget();
						QHBoxLayout * titleLayout = static_cast<QHBoxLayout *>( title_bar->layout() );
						// if window is savable
						if ( windowProperties->at( 0 ) )
						{
							QPushButton * settingButton = new QPushButton( title_bar );
							settingButton->setToolTip( "Settings" );

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
						// if window is citable
						if ( windowProperties->at( 1 ) )
						{
							QPushButton * citeButton = new QPushButton( title_bar );
							citeButton->setToolTip( "Citations" );

							citeButton->setStyleSheet(
								"QPushButton {"
								"border-image: url(:/sprite/citations_icon.png);"
								"background-color: rgba(255, 255, 255, 0);"
								"background-repeat: no-repeat;"
								"width: 10px;"
								"height : 10px;"
								"}"
								"QPushButton:hover {"
								"border-image: url(:/sprite/citations_icon_hovered.png);"
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
				undockButton->setObjectName( "dockButton" );
				undockButton->setToolTip( "Dock" );
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
				exitButton->setToolTip( "Close" );
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

			void CustomQDockWidget::onUndockButtonClicked()
			{
				QWidget *	  title_bar	 = this->titleBarWidget();
				QPushButton * dockButton = title_bar->findChild<QPushButton *>( "dockButton" );
				if ( !this->isFloating() )
				{
					this->setFloating( true );
					if ( dockButton != nullptr )
					{
						dockButton->setToolTip( "Dock" );
					}
				}
				else
				{
					this->setFloating( false );
					if ( dockButton != nullptr )
					{
						dockButton->setToolTip( "Undock" );
					}
				}
			}

			void CustomQDockWidget::onExitButtonClicked() { this->hide(); }

			void CustomQDockWidget::onCitationsButtonClicked()
			{
				if ( citationsWindow == nullptr )
				{
					citationsWindow = new CitationsWindowWidget( this );
					citationsWindow->setWindowTitle( windowTitle() + " - Citations" );
					citationsWindow->setWindowModality( Qt::WindowModality::WindowModal );
					citationsWindow->ui.textBrowserCitations->setMarkdown( moduleCitations() );
					citationsWindow->show();
				}
				else
				{
					if ( citationsWindow->isVisible() )
					{
						citationsWindow->raise();
					}
					else
					{
						citationsWindow->show();
					}
				}
			}

			void CustomQDockWidget::onSettingsButtonClicked()
			{
				if ( settingsWindow == nullptr )
				{
					settingsWindow = new SettingsWindowWidget( this );
					settingsWindow->setWindowTitle( windowTitle() + " - Settings" );
					settingsWindow->setWindowModality( Qt::WindowModality::WindowModal );
					// check if settings have been saved before
					QString	  moduleNameWithoutSpaces = moduleTitle().remove( QRegularExpression( "\\s" ) );
					QSettings settings				  = QSettings( "Qubit", moduleNameWithoutSpaces );
					if ( settings.childGroups().size() != 0 )
					{
						QStringList list = settings.childGroups();
						settingsWindow->populateSettingsList( &list );
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
				QString settingName = settingsWindow->ui.lineEditSettingsName->text();
				if ( settingName != "" )
				{
					QString		moduleNameWithoutSpaces = moduleTitle().remove( QRegularExpression( "\\s" ) );
					QSettings * settings				= new QSettings( "Qubit", moduleNameWithoutSpaces );
					settings->beginGroup( settingName );
					saveSettings( settings );
					settings->endGroup();
					settingsWindow->ui.lineEditSettingsName->clear();
					delete settings;
				}
			}

			void CustomQDockWidget::loadSettingsClicked()
			{
				QList<QListWidgetItem *> selectedListItem = settingsWindow->ui.listWidgetSavedSettings->selectedItems();
				if ( settingsWindow->ui.listWidgetSavedSettings->selectedItems().size() == 1 )
				{
					QString		moduleNameWithoutSpaces = moduleTitle().remove( QRegularExpression( "\\s" ) );
					QSettings * settings				= new QSettings( "Qubit", moduleNameWithoutSpaces );
					QString		settingsProfileName		= selectedListItem[ 0 ]->text();

					if ( settings->childGroups().contains( settingsProfileName ) )
					{
						settings->beginGroup( settingsProfileName );
						loadSettings( settings );
						settings->endGroup();
					}
					delete settings;
				}
			}

			void CustomQDockWidget::removeSettingsClicked()
			{
				QList<QListWidgetItem *> selectedListItem = settingsWindow->ui.listWidgetSavedSettings->selectedItems();
				if ( selectedListItem.size() == 1 )
				{
					QString	  moduleNameWithoutSpaces = moduleTitle().remove( QRegularExpression( "\\s" ) );
					QSettings settings				  = QSettings( "Qubit", moduleNameWithoutSpaces );
					QString	  selectedProfile		  = selectedListItem[ 0 ]->text();
					if ( settings.childGroups().contains( selectedProfile ) )
					{
						settings.remove( selectedProfile );
						delete settingsWindow->ui.listWidgetSavedSettings->takeItem(
							settingsWindow->ui.listWidgetSavedSettings->row( selectedListItem[ 0 ] ) );
					}
				}
			}
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
