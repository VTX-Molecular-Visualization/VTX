#include "action/action_manager.hpp"
#include "id.hpp"
#include "model/renderer/render_effect_preset.hpp"
#include "render_effect_preset_library_view.hpp"
#include "render_effect_preset_view.hpp"
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
	}

	void RenderEffectPresetLibraryView::notify( const Event::VTXEvent * const p_event ) {}

	void RenderEffectPresetLibraryView::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );
		setContentsMargins( 0, 0, 0, 0 );

		QHBoxLayout * const horizontalLayout = new QHBoxLayout( this );
		QVBoxLayout * const verticalLayout	 = new QVBoxLayout();

		const int currentIndex = 0;
		_presetList			   = new QComboBox( this );
		for ( const Model::Renderer::RenderEffectPreset * const preset : _model->getPresets() )
			_presetList->addItem( QString::fromStdString( preset->getName() ) );
		_presetList->setCurrentIndex( currentIndex );
		_renderPresetEditor = VTX::UI::WidgetFactory::get().instantiateWidget<RenderEffectPresetEditor>(
			this, "renderEffectPresetEdition" );

		verticalLayout->addWidget( _presetList );
		verticalLayout->addWidget( _renderPresetEditor );
		// verticalLayout->addStretch( 1 );

		// horizontalLayout->addStretch( 2 );
		horizontalLayout->addItem( verticalLayout );
		// horizontalLayout->addStretch( 2 );

		_refreshView();
	}

	void RenderEffectPresetLibraryView::_setupSlots()
	{
		connect( _presetList,
				 QOverload<int>::of( &QComboBox::currentIndexChanged ),
				 this,
				 &RenderEffectPresetLibraryView::_onPresetIndexChanged );
	}

	void RenderEffectPresetLibraryView::localize() {}

	void RenderEffectPresetLibraryView::_refreshView() { _refreshPresetDisplayed(); }

	void RenderEffectPresetLibraryView::_onPresetIndexChanged( const int p_newIndex ) { _refreshPresetDisplayed(); }

	void RenderEffectPresetLibraryView::_buildPresetList()
	{
		int previousCurrentIndex = _presetList->currentIndex();

		for ( const Model::Renderer::RenderEffectPreset * const preset : _model->getPresets() )
		{
			_presetList->addItem( QString::fromStdString( preset->getName() ) );
		}

		if ( previousCurrentIndex >= _presetList->count() )
		{
			previousCurrentIndex = _presetList->count() - 1;
		}

		_presetList->setCurrentIndex( previousCurrentIndex );
	}
	void RenderEffectPresetLibraryView::_refreshPresetDisplayed()
	{
		int currentIndex = _presetList->currentIndex();
		_renderPresetEditor->setPreset( _model->getPreset( currentIndex ) );
	}

} // namespace VTX::View::UI::Widget::Renderer
