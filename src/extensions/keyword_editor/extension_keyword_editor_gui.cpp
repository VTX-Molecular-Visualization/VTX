#include "extension_keyword_editor_gui.hpp"
#include <QFileDialog>
#include <QPushButton>
#include <QStandardPaths>

ExtensionKeywordEditorGUI::ExtensionKeywordEditorGUI( QWidget * parent ) : QDockWidget( parent )
{
	ui.setupUi( this );
	// QWidget *	  title_bar = new QWidget();
	// QHBoxLayout * layout	= new QHBoxLayout();
	// title_bar->setLayout( layout );
	// QPushButton * button = new QPushButton();
	// layout->addWidget( button );
	// this->setTitleBarWidget( title_bar );
}

void ExtensionKeywordEditorGUI::onOpenKeyfilePushButtonClicked()
{
	QString fileName = QFileDialog::getOpenFileName( this,
													 tr( "Open Keyword file" ),
													 QString( QStandardPaths::DocumentsLocation ),
													 tr( "Keyword file (*.key);;All files (*.*)" ) );

	if ( fileName != "" )
	{
		keywordEditor = new ExtensionKeywordEditor( fileName );
		keywordEditor->readKeywordFile();
	}
}

void ExtensionKeywordEditorGUI::loadSettings() {}

void ExtensionKeywordEditorGUI::saveSettings() {}
