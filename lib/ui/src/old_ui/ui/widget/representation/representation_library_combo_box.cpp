#include "ui/old_ui/ui/widget/representation/representation_library_combo_box.hpp"
#include "ui/old_ui/style.hpp"
#include <app/model/representation/representation_library.hpp>
#include <app/core/mvc/mvc_manager.hpp>
#include <app/old_app/representation/representation_manager.hpp>
#include <app/view/callback_view.hpp>

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
		if ( VTX::Core::MVC::MvcManager::get().hasView( &Model::Representation::RepresentationLibrary::get(), _viewID ) )
			VTX::Core::MVC::MvcManager::get().deleteView( &Model::Representation::RepresentationLibrary::get(), _viewID );
	}

	void RepresentationLibraryComboBox::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );
		_fillItemList();

		View::CallbackView<Model::Representation::RepresentationLibrary, RepresentationLibraryComboBox> * const view
			= VTX::Core::MVC::MvcManager::get()
				  .instantiateView<
					  View::CallbackView<Model::Representation::RepresentationLibrary, RepresentationLibraryComboBox>>(
					  &Model::Representation::RepresentationLibrary::get(), _viewID );

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

		for ( const Model::Representation::Representation * const preset :
			  Model::Representation::RepresentationLibrary::get().getRepresentations() )
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
				  && Model::Representation::RepresentationLibrary::get().getDefaultRepresentationIndex() == i;

			const QIcon & displayedIcon
				= displayDefaultFeedback ? Style::IconConst::get().DEFAULT_ITEM : NOT_DEFAULT_REPRESENTATION_FEEDABCK;

			setItemIcon( i, displayedIcon );
		}
	}

	void RepresentationLibraryComboBox::_onRepresentationLibraryChange( const VTX::Event::VTXEvent * const p_event )
	{
		if ( p_event->name == VTX::Event::Model::DISPLAY_NAME_CHANGE
			 || p_event->name == VTX::Event::Model::DATA_CHANGE )
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
