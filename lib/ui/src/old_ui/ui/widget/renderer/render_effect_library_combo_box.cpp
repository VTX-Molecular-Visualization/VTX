#include "ui/old_ui/ui/widget/renderer/render_effect_library_combo_box.hpp"
#include "ui/old_ui/style.hpp"
#include <app/application/render_effect/render_effect_library.hpp>
#include <app/mvc.hpp>
#include <app/core/view/callback_view.hpp>

namespace VTX::UI::Widget::Renderer
{
	RenderEffectLibraryComboBox::RenderEffectLibraryComboBox( QWidget * p_parent ) :
		BaseManualWidget( p_parent ),
		_viewID( ID::View::UI_RENDER_EFFECT_LIBRARY_COMBO_BOX + std::to_string( ID_VIEW_COUNTER ) )
	{
		ID_VIEW_COUNTER++;
	}
	RenderEffectLibraryComboBox::~RenderEffectLibraryComboBox()
	{
		// Check view if setting window destroy before combo box (view can be destroyed twice)
		if ( VTX::MVC_MANAGER().hasView( &App::Application::RenderEffect::RenderEffectLibrary::get(), _viewID ) )
			VTX::MVC_MANAGER().deleteView( &App::Application::RenderEffect::RenderEffectLibrary::get(), _viewID );
	}

	void RenderEffectLibraryComboBox::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );
		_fillItemList();

		App::Core::View::CallbackView<App::Application::RenderEffect::RenderEffectLibrary, RenderEffectLibraryComboBox> * const view
			= VTX::MVC_MANAGER()
				  .instantiateView<
					  App::Core::View::CallbackView<App::Application::RenderEffect::RenderEffectLibrary, RenderEffectLibraryComboBox>>(
					  &App::Application::RenderEffect::RenderEffectLibrary::get(), _viewID );

		view->setCallback( this, &RenderEffectLibraryComboBox::_onLibraryChange );
	}
	void RenderEffectLibraryComboBox::_setupSlots() {}
	void RenderEffectLibraryComboBox::localize() {}

	void RenderEffectLibraryComboBox::setHighlightAppliedItem( const bool p_highlight )
	{
		_highlightApplied = p_highlight;
		_updateHighlightAppliedItemFeedback();
	}

	void RenderEffectLibraryComboBox::_fillItemList()
	{
		const bool oldSignalState		= blockSignals( true );
		int		   previousCurrentIndex = currentIndex();
		clear();

		for ( const App::Application::RenderEffect::RenderEffectPreset * const preset :
			  App::Application::RenderEffect::RenderEffectLibrary::get().getPresets() )
		{
			addItem( QString::fromStdString( preset->getName() ) );
		}

		if ( _highlightApplied )
			_updateHighlightAppliedItemFeedback();

		if ( previousCurrentIndex >= count() )
		{
			previousCurrentIndex = count() - 1;
		}

		setCurrentIndex( previousCurrentIndex );
		blockSignals( oldSignalState );
	}

	void RenderEffectLibraryComboBox::_updateHighlightAppliedItemFeedback()
	{
		for ( int i = 0; i < count(); i++ )
		{
			const App::Application::RenderEffect::RenderEffectPreset * const preset
				= App::Application::RenderEffect::RenderEffectLibrary::get().getPreset( i );

			const bool displayDefaultFeedback
				= _highlightApplied && App::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( preset );

			const QIcon & displayedIcon
				= displayDefaultFeedback ? Style::IconConst::get().DEFAULT_ITEM : NOT_APPLIED_ITEM_FEEDABCK;

			setItemIcon( i, displayedIcon );
		}
	}

	void RenderEffectLibraryComboBox::_onLibraryChange( const VTX::App::Core::Event::VTXEvent * const p_event )
	{
		if ( p_event->name == VTX::App::Event::Model::DISPLAY_NAME_CHANGE || p_event->name == VTX::App::Event::Model::DATA_CHANGE
			 || p_event->name == VTX::App::Event::Model::APPLIED_PRESET_CHANGE )
		{
			_fillItemList();

			if ( hasDifferentData() )
				_displayDifferentsDataFeedback();
		}
	}

	void RenderEffectLibraryComboBox::wheelEvent( QWheelEvent * event )
	{
		// Don't scroll on wheel event
	}

} // namespace VTX::UI::Widget::Renderer
