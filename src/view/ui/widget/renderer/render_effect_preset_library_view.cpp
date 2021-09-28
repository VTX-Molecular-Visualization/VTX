#include "render_effect_preset_library_view.hpp"
#include "action/action_manager.hpp"
#include "action/renderer.hpp"
#include "event/event_manager.hpp"
#include "id.hpp"
#include "model/renderer/render_effect_preset.hpp"
#include "render_effect_preset_view.hpp"
#include "setting.hpp"
#include "ui/dialog.hpp"
#include "ui/widget_factory.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace VTX::View::UI::Widget::Renderer
{
	RenderEffectPresetLibraryView::RenderEffectPresetLibraryView(
		Model::Renderer::RenderEffectPresetLibrary * const p_model,
		QWidget * const									   p_parent ) :
		View::BaseView<Model::Renderer::RenderEffectPresetLibrary>( p_model ),
		VTX::UI::Widget::BaseManualWidget<QWidget>( p_parent )
	{
		_registerEvent( Event::Global::RENDER_EFFECT_ADDED );
		_registerEvent( Event::Global::RENDER_EFFECT_REMOVED );
	}

	void RenderEffectPresetLibraryView::notify( const Event::VTXEvent * const p_event )
	{
		if ( p_event->name == Event::Model::APPLIED_PRESET_CHANGE )
		{
			_presetList->setCurrentIndex( _model->getAppliedPresetIndex() );
		}
	}

	void RenderEffectPresetLibraryView::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::RENDER_EFFECT_ADDED )
		{
			const Event::VTXEventValue<int> & castedEvent = dynamic_cast<const Event::VTXEventValue<int> &>( p_event );
			const int						  representationIndex = castedEvent.value;

			_refreshDeletePresetButton();
		}
		else if ( p_event.name == Event::Global::RENDER_EFFECT_REMOVED )
		{
			_refreshPresetDisplayed( true );
			_refreshDeletePresetButton();
		}
	}

	void RenderEffectPresetLibraryView::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );
		setContentsMargins( 0, 0, 0, 0 );

		QHBoxLayout * const horizontalLayout = new QHBoxLayout( this );
		QHBoxLayout * const headerLayout	 = new QHBoxLayout();
		QHBoxLayout * const bottomLayout	 = new QHBoxLayout();
		QVBoxLayout * const verticalLayout	 = new QVBoxLayout();

		const int currentIndex = 0;

		_presetList = VTX::UI::WidgetFactory::get().instantiateWidget<RenderEffectLibraryComboBox>( this, "" );
		_presetList->setHighlightAppliedItem( true );
		_presetList->setCurrentIndex( currentIndex );

		_addPresetButton = new QPushButton( this );
		_addPresetButton->setIcon( QIcon( ":/sprite/add_preset_icon.png" ) );
		_copyPresetButton = new QPushButton( this );
		_copyPresetButton->setIcon( QIcon( ":/sprite/copy_preset_icon.png" ) );
		_deletePresetButton = new QPushButton( this );
		_deletePresetButton->setIcon( QIcon( ":/sprite/delete_preset_icon.png" ) );

		_renderPresetEditor = VTX::UI::WidgetFactory::get().instantiateWidget<RenderEffectPresetEditor>(
			this, "renderEffectPresetEdition" );

		_importPresetButton = new QPushButton( this );
		_importPresetButton->setText( "Import" );
		_reloadButton = new QPushButton( this );
		_reloadButton->setText( "Reload" );
		_resetButton = new QPushButton( this );
		_resetButton->setText( "Reset" );

		headerLayout->addWidget( _presetList, 10 );
		headerLayout->addWidget( _addPresetButton );
		headerLayout->addWidget( _copyPresetButton );
		headerLayout->addWidget( _deletePresetButton );

		bottomLayout->addStretch( 1000 );
		bottomLayout->addWidget( _importPresetButton );
		bottomLayout->addWidget( _reloadButton );
		bottomLayout->addWidget( _resetButton );

		verticalLayout->addItem( headerLayout );
		verticalLayout->addWidget( _renderPresetEditor );
		verticalLayout->addItem( bottomLayout );

		horizontalLayout->addItem( verticalLayout );

		_refreshPresetDisplayed( false );
	}

	void RenderEffectPresetLibraryView::_setupSlots()
	{
		connect( _presetList,
				 QOverload<int>::of( &QComboBox::currentIndexChanged ),
				 this,
				 &RenderEffectPresetLibraryView::_onPresetIndexChanged );

		connect( _addPresetButton, &QPushButton::clicked, this, &RenderEffectPresetLibraryView::_onAddPreset );
		connect( _copyPresetButton, &QPushButton::clicked, this, &RenderEffectPresetLibraryView::_onCopyPreset );
		connect( _deletePresetButton, &QPushButton::clicked, this, &RenderEffectPresetLibraryView::_onDeletePreset );

		connect( _importPresetButton, &QPushButton::clicked, this, &RenderEffectPresetLibraryView::_onImportPreset );
		connect( _reloadButton, &QPushButton::clicked, this, &RenderEffectPresetLibraryView::_onReloadLibrary );
		connect( _resetButton, &QPushButton::clicked, this, &RenderEffectPresetLibraryView::_onResetToDefaultLibrary );
	}

	void RenderEffectPresetLibraryView::localize() {}

	void RenderEffectPresetLibraryView::selectPreset( const int p_presetIndex )
	{
		const int currentIndex = p_presetIndex < 0 ? 0 : p_presetIndex;
		_presetList->setCurrentIndex( currentIndex );
	}

	void RenderEffectPresetLibraryView::_refreshView() { _refreshPresetDisplayed( true ); }

	void RenderEffectPresetLibraryView::_onPresetIndexChanged( const int p_newIndex )
	{
		Model::Renderer::RenderEffectPreset * const currentPreset = _model->getPreset( p_newIndex );

		if ( currentPreset != nullptr )
		{
			VTX_ACTION( new Action::Renderer::ApplyRenderEffectPreset( *currentPreset, true ) );
			_refreshPresetDisplayed( false );
		}
	}

	void RenderEffectPresetLibraryView::_onAddPreset() const
	{
		VTX_ACTION( new Action::Renderer::AddNewPresetInLibrary( Setting::NEW_RENDER_EFFECT_PRESET_DEFAULT_NAME ) );
	}
	void RenderEffectPresetLibraryView::_onCopyPreset() const
	{
		VTX_ACTION( new Action::Renderer::CopyPresetInLibrary( _presetList->currentIndex() ) );
	}
	void RenderEffectPresetLibraryView::_onDeletePreset()
	{
		VTX::UI::Dialog::confirmActionDialog(
			new Action::Renderer::DeletePresetInLibrary( _presetList->currentIndex() ),
			"Confirm",
			"Are you sure to delete this preset ?" );
	}

	void RenderEffectPresetLibraryView::_onImportPreset() const { VTX::UI::Dialog::importRenderEffectPresetDialog(); }
	void RenderEffectPresetLibraryView::_onReloadLibrary() const
	{
		VTX::UI::Dialog::confirmActionDialog( new Action::Renderer::ReloadPresets(),
											  "Confirm",
											  "Are you sure to reload all presets ? Current changes will be lost." );
	}
	void RenderEffectPresetLibraryView::_onResetToDefaultLibrary() const
	{
		VTX::UI::Dialog::confirmActionDialog( new Action::Renderer::ResetPresets(),
											  "Confirm",
											  "Are you sure to reset the preset library ? All changes will be lost." );
	}

	void RenderEffectPresetLibraryView::_refreshPresetDisplayed( const bool p_updateRenderer )
	{
		const int currentIndex = _presetList->currentIndex();

		Model::Renderer::RenderEffectPreset * const currentPreset = _model->getPreset( currentIndex );
		_renderPresetEditor->setPreset( currentPreset, p_updateRenderer );
	}
	void RenderEffectPresetLibraryView::_refreshDeletePresetButton()
	{
		_deletePresetButton->setEnabled( _model->getPresetCount() > 1 );
	}

} // namespace VTX::View::UI::Widget::Renderer
