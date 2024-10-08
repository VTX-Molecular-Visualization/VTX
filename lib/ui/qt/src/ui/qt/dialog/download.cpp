#include "ui/qt/dialog/download.hpp"
#include "ui/qt/application.hpp"
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>
#include <app/action/scene.hpp>

namespace VTX::UI::QT::Dialog
{

	Download::Download() : BaseWidget<Download, QDialog>( APP_QT::getMainWindow() )
	{
		setWindowTitle( "Download" );
		// TODO: try size policy?
		setFixedSize( 300, 150 );

		auto * layout = new QVBoxLayout( this );

		// URL.
		auto * labelURL = new QLabel( "Server URL", this );
		_comboBoxURL	= new QComboBox( this );
		_comboBoxURL->setEditable( true );
		_comboBoxURL->setInsertPolicy( QComboBox::InsertPolicy::NoInsert );

		// PDB id.
		auto * labelPDB = new QLabel( "PDB id", this );
		_comboBoxPDB	= new QComboBox( this );
		_comboBoxPDB->setEditable( true );
		_comboBoxPDB->setInsertPolicy( QComboBox::InsertPolicy::NoInsert );
		_comboBoxPDB->setFocus();

		// Buttons.
		QDialogButtonBox * buttonBox = new QDialogButtonBox(
			QDialogButtonBox::StandardButton::Cancel | QDialogButtonBox::StandardButton::Open, this
		);

		// Load histories manually because dialog is destroyed when closed.
		restore();

		// FIXME: Avoid losing default url if not in history.
		if ( _comboBoxURL->findText( QString::fromStdString( _DEFAULT_URL ) ) == -1 )
		{
			_comboBoxURL->addItem( _DEFAULT_URL.c_str() );
		}

		// Layout.
		layout->addWidget( labelURL );
		layout->addWidget( _comboBoxURL );
		layout->addWidget( labelPDB );
		layout->addWidget( _comboBoxPDB );
		layout->addWidget( buttonBox );

		// Callbacks.
		connect(
			buttonBox->button( QDialogButtonBox::StandardButton::Open ),
			&QPushButton::clicked,
			[ this ]()
			{
				// Check inputs.
				_url = _comboBoxURL->currentText().toStdString();
				Util::String::trim( _url );
				_pdb = _comboBoxPDB->currentText().toStdString();
				Util::String::trim( _pdb );

				if ( _url.empty() )
				{
					VTX_WARNING( "URL is empty" );
					return;
				}

				if ( _url.empty() || _pdb.empty() )
				{
					VTX_WARNING( "PDB id is empty" );
					return;
				}

				std::string	 urlReplaced = _url;
				const size_t pos		 = _url.find( _PDB_ID_TEMPLATE );
				if ( pos == std::string::npos )
				{
					VTX_WARNING( "URL does not contain {}", _PDB_ID_TEMPLATE );
				}
				else
				{
					urlReplaced
						= urlReplaced.replace( urlReplaced.find( _PDB_ID_TEMPLATE ), _PDB_ID_TEMPLATE.length(), _pdb );
				}

				// Save histories.
				save();

				App::ACTION_SYSTEM().execute<App::Action::Scene::DownloadMolecule>( urlReplaced, _pdb + ".pdb" );

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

	void Download::save()
	{
		_saveHistory( _SETTING_KEY_URL, _url );
		_saveHistory( _SETTING_KEY_PDB, _pdb );
	}

	void Download::restore()
	{
		_loadHistory( _SETTING_KEY_URL, _comboBoxURL );
		_loadHistory( _SETTING_KEY_PDB, _comboBoxPDB );
	}

} // namespace VTX::UI::QT::Dialog
