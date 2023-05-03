#include "ui/old_ui/view/ui/widget/representation/representation_library_view.hpp"
#include "ui/old_ui/ui/dialog.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <app/action/representation.hpp>
#include <app/application/representation/representation_preset.hpp>
#include <app/event/global.hpp>
#include <app/id.hpp>

namespace VTX::View::UI::Widget::Representation
{
	RepresentationLibraryView::RepresentationLibraryView(
		App::Application::Representation::RepresentationLibrary * const p_model,
		QWidget * const													p_parent ) :
		App::Core::View::BaseView<App::Application::Representation::RepresentationLibrary>( p_model ),
		VTX::UI::Widget::BaseManualWidget<QWidget>( p_parent )
	{
		_registerEvent( VTX::App::Event::Global::REPRESENTATION_ADDED );
	}

	void RepresentationLibraryView::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );
		setContentsMargins( 0, 0, 0, 0 );

		QVBoxLayout * const verticalLayout = new QVBoxLayout( this );
		QHBoxLayout * const headerLayout   = new QHBoxLayout();
		QHBoxLayout * const bottomLayout   = new QHBoxLayout();

		const int currentIndex = 0;
		_presetList
			= VTX::UI::WidgetFactory::get().instantiateWidget<RepresentationLibraryComboBox>( this, "PresetList" );
		_presetList->setHighlightDefaultRepresentation( true );
		_presetList->setCurrentIndex( currentIndex );
		_presetList->setSizePolicy( QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::Preferred );
		_presetList->setMinimumWidth( 50 );

		_representationPresetEditor = VTX::UI::WidgetFactory::get().instantiateWidget<RepresentationPresetEditor>(
			this, "renderEffectPresetEdition" );

		_addPresetButton = new QPushButton( this );
		_addPresetButton->setIcon( QIcon( ":/sprite/add_preset_icon.png" ) );
		_copyPresetButton = new QPushButton( this );
		_copyPresetButton->setIcon( QIcon( ":/sprite/copy_preset_icon.png" ) );
		_deletePresetButton = new QPushButton( this );
		_deletePresetButton->setIcon( QIcon( ":/sprite/delete_preset_icon.png" ) );

		_importPresetButton = new QPushButton( this );
		_importPresetButton->setText( "Import" );
		_reloadLibraryButton = new QPushButton( this );
		_reloadLibraryButton->setText( "Reload" );
		_resetLibraryButton = new QPushButton( this );
		_resetLibraryButton->setText( "Reset" );

		headerLayout->addWidget( _presetList, 10 );
		headerLayout->addWidget( _addPresetButton );
		headerLayout->addWidget( _copyPresetButton );
		headerLayout->addWidget( _deletePresetButton );

		bottomLayout->addStretch( 100 );
		bottomLayout->addWidget( _importPresetButton );
		bottomLayout->addWidget( _reloadLibraryButton );
		bottomLayout->addWidget( _resetLibraryButton );

		verticalLayout->addItem( headerLayout );
		verticalLayout->addWidget( _representationPresetEditor );
		verticalLayout->addItem( bottomLayout );

		_refreshPresetDisplayed( false );
	}

	void RepresentationLibraryView::_setupSlots()
	{
		connect( _presetList,
				 QOverload<int>::of( &QComboBox::currentIndexChanged ),
				 this,
				 &RepresentationLibraryView::_onPresetIndexChanged );

		connect( _addPresetButton, &QPushButton::clicked, this, &RepresentationLibraryView::_onAddPreset );
		connect( _copyPresetButton, &QPushButton::clicked, this, &RepresentationLibraryView::_onCopyPreset );
		connect( _deletePresetButton, &QPushButton::clicked, this, &RepresentationLibraryView::_onDeletePreset );

		connect( _importPresetButton, &QPushButton::clicked, this, &RepresentationLibraryView::_onImportPreset );
		connect( _reloadLibraryButton, &QPushButton::clicked, this, &RepresentationLibraryView::_onReloadLibrary );
		connect( _resetLibraryButton, &QPushButton::clicked, this, &RepresentationLibraryView::_onResetLibrary );
	}

	void RepresentationLibraryView::receiveEvent( const VTX::App::Core::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::App::Event::Global::REPRESENTATION_ADDED )
		{
			const int representationIndex
				= dynamic_cast<const VTX::App::Core::Event::VTXEventArg<int> &>( p_event ).get();

			_presetList->setCurrentIndex( representationIndex );
		}
	}

	void RepresentationLibraryView::localize() {}

	void RepresentationLibraryView::_refreshView() { _refreshPresetDisplayed( true ); }

	void RepresentationLibraryView::_onPresetIndexChanged( const int p_newIndex ) { _refreshPresetDisplayed( false ); }

	void RepresentationLibraryView::_onAddPreset() const
	{
		VTX_ACTION(
			new App::Action::Representation::AddNewPresetInLibrary( VTX::App::Application::Setting::NEW_REPRESENTATION_DEFAULT_NAME ) );
	}
	void RepresentationLibraryView::_onCopyPreset() const
	{
		VTX_ACTION( new App::Action::Representation::CopyPresetInLibrary( _presetList->currentIndex() ) );
	}
	void RepresentationLibraryView::_onDeletePreset()
	{
		VTX::UI::Dialog::confirmActionDialog(
			new App::Action::Representation::DeletePresetInLibrary( _presetList->currentIndex() ),
			"Confirm",
			"Are you sure you want to delete this preset ?" );
	}
	void RepresentationLibraryView::_onImportPreset() const { VTX::UI::Dialog::importRepresentationPresetDialog(); }
	void RepresentationLibraryView::_onReloadLibrary() const
	{
		VTX::UI::Dialog::confirmActionDialog(
			new App::Action::Representation::ReloadPresets(),
			"Confirm",
			"Are you sure you want to reload all presets ? Current changes will be lost." );
	}
	void RepresentationLibraryView::_onResetLibrary() const
	{
		VTX::UI::Dialog::confirmActionDialog(
			new App::Action::Representation::ResetPresetsToDefault(),
			"Confirm",
			"Are you sure you want to reset the preset library ? All changes will be lost." );
	}

	void RepresentationLibraryView::_refreshPresetDisplayed( const bool p_applyPreset )
	{
		const int currentIndex = _presetList->currentIndex();

		App::Application::Representation::RepresentationPreset * const representation
			= _model->getRepresentation( currentIndex );

		_representationPresetEditor->setPreset( representation, p_applyPreset );
		_deletePresetButton->setEnabled( _model->canDeleteRepresentation( representation ) );
	}

} // namespace VTX::View::UI::Widget::Representation
