#include "ui/qt/dialog/download.hpp"
#include "ui/qt/application.hpp"
#include <QButtonGroup>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>
#include <app/action/scene.hpp>
#include <app/core/network/network_system.hpp>

namespace VTX::UI::QT::Dialog
{

	Download::Download() : BaseWidget<Download, QDialog>( APP_QT::getMainWindow() )
	{
		setWindowTitle( "Download" );
		// TODO: try size policy?
		setFixedSize( 300, 180 );

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

		// Cache found label.
		// QLabel * labelCache = new QLabel( "File found", this );

		// Radio buttons.
		auto * buttonGroup		= new QButtonGroup( this );
		auto * layoutCacheRadio = new QHBoxLayout( this );
		_radioButtonOpen		= new QRadioButton( "Open from cache" );
		_radioButtonDownload	= new QRadioButton( "Download" );
		buttonGroup->addButton( _radioButtonOpen );
		buttonGroup->addButton( _radioButtonDownload );
		layoutCacheRadio->addWidget( _radioButtonOpen );
		layoutCacheRadio->addWidget( _radioButtonDownload );

		// Layout.
		layout->addWidget( labelURL );
		layout->addWidget( _comboBoxURL );
		layout->addWidget( labelPDB );
		layout->addWidget( _comboBoxPDB );
		layout->addLayout( layoutCacheRadio );
		layout->addWidget( buttonBox );

		// Callbacks.
		// Update radio button if file is found in cache.
		connect(
			_comboBoxPDB,
			&QComboBox::editTextChanged,
			[ this ]( const QString & )
			{
				auto pdb = _comboBoxPDB->currentText().toStdString();
				Util::String::trim( pdb );
				if ( pdb.length() == 4 )
				{
					FilePath path = App::Filesystem::getCachePath( pdb + ".pdb" );
					if ( std::filesystem::exists( path ) )
					{
						// Show radio buttons.
						_radioButtonOpen->setEnabled( true );
						_radioButtonDownload->setEnabled( true );
						_radioButtonOpen->setChecked( true );
						return;
					}
				}
				// Hide radio buttons.
				_radioButtonOpen->setEnabled( false );
				_radioButtonDownload->setEnabled( true );
				_radioButtonDownload->setChecked( true );
			}
		);

		// Open button.
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

				FilePath path = App::Filesystem::getCachePath( _pdb + ".pdb" );
				if ( std::filesystem::exists( path ) and _radioButtonOpen->isChecked() )
				{
					App::ACTION_SYSTEM().execute<App::Action::Scene::LoadMolecule>( path );
				}
				else
				{
					if ( _url.empty() )
					{
						VTX_ERROR( "URL is empty" );
						return;
					}

					if ( _pdb.empty() )
					{
						VTX_ERROR( "PDB id is empty" );
						return;
					}

					if ( _pdb.length() != 4 )
					{
						VTX_ERROR( "PDB id must be 4 characters" );
						return;
					}

					std::string	 urlReplaced = _url;
					const size_t pos		 = _url.find( _PDB_ID_TEMPLATE );
					if ( pos == std::string::npos )
					{
						VTX_ERROR( "URL does not contain {}", _PDB_ID_TEMPLATE );
					}
					else
					{
						urlReplaced = urlReplaced.replace(
							urlReplaced.find( _PDB_ID_TEMPLATE ), _PDB_ID_TEMPLATE.length(), _pdb
						);
					}

					App::ACTION_SYSTEM().execute<App::Action::Scene::DownloadMolecule>( urlReplaced, _pdb + ".pdb" );
				}

				save();
				close();
			}
		);

		// Cancel button.
		connect(
			buttonBox->button( QDialogButtonBox::StandardButton::Cancel ),
			&QPushButton::clicked,
			[ this ]() { close(); }
		);

		// Load histories manually because dialog is destroyed when closed.
		restore();

		// FIXME: Avoid losing default url if not in history.
		if ( _comboBoxURL->findText( QString::fromStdString( _DEFAULT_URL ) ) == -1 )
		{
			_comboBoxURL->addItem( _DEFAULT_URL.c_str() );
		}
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
