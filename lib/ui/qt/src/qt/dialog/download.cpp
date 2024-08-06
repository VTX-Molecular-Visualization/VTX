#include "qt/dialog/download.hpp"
#include "qt/application.hpp"
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>

namespace VTX::UI::QT::Dialog
{

	Download::Download() : BaseWidget<Download, QDialog>( WIDGETS.get<Widget::MainWindow *>() )
	{
		setWindowTitle( "Download" );
		// TODO: try size policy?
		setMinimumSize( 300, 100 );

		QVBoxLayout * layout = new QVBoxLayout( this );

		// URL.
		QLabel *	labelURL	= new QLabel( "Server URL", this );
		QComboBox * comboBoxURL = new QComboBox( this );
		comboBoxURL->setEditable( true );
		comboBoxURL->setInsertPolicy( QComboBox::InsertPolicy::NoInsert );

		// PDB id.
		QLabel *	labelPDB	= new QLabel( "PDB id", this );
		QComboBox * comboBoxPDB = new QComboBox( this );
		comboBoxPDB->setEditable( true );
		comboBoxPDB->setInsertPolicy( QComboBox::InsertPolicy::NoInsert );
		comboBoxPDB->setFocus();

		// Buttons.
		QDialogButtonBox * buttonBox = new QDialogButtonBox(
			QDialogButtonBox::StandardButton::Cancel | QDialogButtonBox::StandardButton::Open, this
		);

		// Load histories.
		_loadHistory( _SETTING_KEY_URL, comboBoxURL );
		_loadHistory( _SETTING_KEY_PDB, comboBoxPDB );

		// FIXME: Avoid losing default url if not in history.
		if ( comboBoxURL->findText( QString::fromStdString( _DEFAULT_URL ) ) == -1 )
		{
			comboBoxURL->addItem( _DEFAULT_URL.c_str() );
		}

		// Layout.
		layout->addWidget( labelURL );
		layout->addWidget( comboBoxURL );
		layout->addWidget( labelPDB );
		layout->addWidget( comboBoxPDB );
		layout->addWidget( buttonBox );
		setLayout( layout );

		// Callbacks.
		connect(
			buttonBox->button( QDialogButtonBox::StandardButton::Open ),
			&QPushButton::clicked,
			[ this, comboBoxURL, comboBoxPDB ]()
			{
				// Check inputs.
				std::string url = comboBoxURL->currentText().toStdString();
				Util::String::trim( url );
				std::string pdb = comboBoxPDB->currentText().toStdString();
				Util::String::trim( pdb );

				if ( url.empty() )
				{
					VTX_WARNING( "URL is empty" );
					return;
				}

				if ( url.empty() || pdb.empty() )
				{
					VTX_WARNING( "PDB id is empty" );
					return;
				}

				std::string	 urlReplaced = url;
				const size_t pos		 = url.find( _PDB_ID_TEMPLATE );
				if ( pos == std::string::npos )
				{
					VTX_WARNING( "URL does not contain {}", _PDB_ID_TEMPLATE );
				}
				else
				{
					urlReplaced
						= urlReplaced.replace( urlReplaced.find( _PDB_ID_TEMPLATE ), _PDB_ID_TEMPLATE.length(), pdb );
				}

				// Save histories.
				_saveHistory( _SETTING_KEY_URL, url );
				_saveHistory( _SETTING_KEY_PDB, pdb );

				// TODO: App::Action::Download.
				VTX_DEBUG( "Download: {}", urlReplaced );

				close();
			}
		);

		connect(
			buttonBox->button( QDialogButtonBox::StandardButton::Cancel ),
			&QPushButton::clicked,
			[ this ]() { close(); }
		);
	}

	void Download::_loadHistory( const std::string & p_key, QComboBox * const p_comboBox )
	{
		QStringList history = SETTINGS.value( p_key.c_str() ).toStringList();
		p_comboBox->addItems( history );
	}

	void Download::_saveHistory( const std::string & p_key, const std::string & p_value )
	{
		QStringList history = SETTINGS.value( p_key.c_str() ).toStringList();
		// Remove duplicates.
		if ( history.contains( p_value.c_str() ) )
		{
			history.removeAll( p_value.c_str() );
		}
		// Prepend the last one.
		history.prepend( p_value.c_str() );
		// Limit to 10.
		while ( history.size() > _MAX_HISTORY_SIZE )
		{
			history.removeLast();
		}
		SETTINGS.setValue( p_key.c_str(), history );
	}

} // namespace VTX::UI::QT::Dialog
