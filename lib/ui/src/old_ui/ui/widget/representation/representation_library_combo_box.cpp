#include "ui/old_ui/ui/widget/representation/representation_library_combo_box.hpp"
#include "ui/old_ui/style.hpp"
#include <app/application/representation/representation_library.hpp>
#include <app/application/representation/representation_manager.hpp>
#include <app/core/view/callback_view.hpp>
#include <app/mvc.hpp>

namespace VTX::UI::Widget::Representation
{
	RepresentationLibraryComboBox::RepresentationLibraryComboBox( QWidget * p_parent ) :
		BaseManualWidget( p_parent ),
		_viewID( ID::View::UI_REPRESENTATION_LIBRARY_COMBO_BOX + std::to_string( ID_VIEW_COUNTER ) )
	{
		ID_VIEW_COUNTER++;
	}
	RepresentationLibraryComboBox::~RepresentationLibraryComboBox()
	{
		// Check view if setting window destroy before combo box (view can be destroyed twice)
		if ( VTX::MVC_MANAGER().hasView( &App::Application::Representation::RepresentationLibrary::get(), _viewID ) )
			VTX::MVC_MANAGER().deleteView( &App::Application::Representation::RepresentationLibrary::get(), _viewID );
	}

	void RepresentationLibraryComboBox::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );
		_fillItemList();

		App::Core::View::CallbackView<App::Application::Representation::RepresentationLibrary,
									  RepresentationLibraryComboBox> * const view
			= VTX::MVC_MANAGER()
				  .instantiateView<
					  App::Core::View::CallbackView<App::Application::Representation::RepresentationLibrary,
													RepresentationLibraryComboBox>>(
					  &App::Application::Representation::RepresentationLibrary::get(), _viewID );

		view->setCallback( this, &RepresentationLibraryComboBox::_onRepresentationLibraryChange );
	}
	void RepresentationLibraryComboBox::_setupSlots() {}
	void RepresentationLibraryComboBox::localize() {}

	void RepresentationLibraryComboBox::setHighlightDefaultRepresentation( const bool p_highlight )
	{
		_highlightDefault = p_highlight;
		_updateHighlightDefaultRepresentationFeedback();
	}

	void RepresentationLibraryComboBox::_fillItemList()
	{
		const bool oldSignalState		= blockSignals( true );
		int		   previousCurrentIndex = currentIndex();
		clear();

		for ( const App::Application::Representation::RepresentationPreset * const preset :
			  App::Application::Representation::RepresentationLibrary::get().getRepresentations() )
		{
			addItem( QString::fromStdString( preset->getName() ) );
		}

		if ( _highlightDefault )
			_updateHighlightDefaultRepresentationFeedback();

		if ( previousCurrentIndex >= count() )
		{
			previousCurrentIndex = count() - 1;
		}

		setCurrentIndex( previousCurrentIndex );
		blockSignals( oldSignalState );
	}

	void RepresentationLibraryComboBox::_updateHighlightDefaultRepresentationFeedback()
	{
		for ( int i = 0; i < count(); i++ )
		{
			const bool displayDefaultFeedback
				= _highlightDefault
				  && App::Application::Representation::RepresentationLibrary::get().getDefaultRepresentationIndex()
						 == i;

			const QIcon & displayedIcon
				= displayDefaultFeedback ? Style::IconConst::get().DEFAULT_ITEM : NOT_DEFAULT_REPRESENTATION_FEEDABCK;

			setItemIcon( i, displayedIcon );
		}
	}

	void RepresentationLibraryComboBox::_onRepresentationLibraryChange(
		const VTX::App::Core::Event::VTXEvent * const p_event )
	{
		if ( p_event->name == VTX::App::Event::Model::DISPLAY_NAME_CHANGE
			 || p_event->name == VTX::App::Event::Model::DATA_CHANGE )
		{
			_fillItemList();

			if ( hasDifferentData() )
				_displayDifferentsDataFeedback();
		}
	}

	void RepresentationLibraryComboBox::wheelEvent( QWheelEvent * event )
	{
		// Don't scroll on wheel event
	}

} // namespace VTX::UI::Widget::Representation
