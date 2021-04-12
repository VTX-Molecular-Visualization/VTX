#include "representation_library_combo_box.hpp"
#include "model/representation/representation_library.hpp"
#include "mvc/mvc_manager.hpp"
#include "view/callback_view.hpp"

namespace VTX::UI::Widget::Representation
{
	RepresentationLibraryComboBox::RepresentationLibraryComboBox( QWidget * p_parent ) : BaseManualWidget( p_parent )
	{
		_viewID = ID::View::UI_REPRESENTATION_LIBRARY_COMBO_BOX + std::to_string( ID_VIEW_COUNTER );
		ID_VIEW_COUNTER++;
	}
	RepresentationLibraryComboBox::~RepresentationLibraryComboBox()
	{
		// Check view if setting window destroy before combo box (view can be destroyed twice)
		if ( MVC::MvcManager::get().hasView( &Model::Representation::RepresentationLibrary::get(), _viewID ) )
			MVC::MvcManager::get().deleteView( &Model::Representation::RepresentationLibrary::get(), _viewID );
	}

	void RepresentationLibraryComboBox::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );
		_fillItemList();

		View::CallbackView<Model::Representation::RepresentationLibrary, RepresentationLibraryComboBox> * const view
			= MVC::MvcManager::get()
				  .instantiateView<
					  View::CallbackView<Model::Representation::RepresentationLibrary, RepresentationLibraryComboBox>>(
					  &Model::Representation::RepresentationLibrary::get(), _viewID );

		view->setCallback( this, &RepresentationLibraryComboBox::_onRepresentationLibraryChange );
	}
	void RepresentationLibraryComboBox::_setupSlots() {}
	void RepresentationLibraryComboBox::localize() {}

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

		if ( previousCurrentIndex >= count() )
		{
			previousCurrentIndex = count() - 1;
		}

		setCurrentIndex( previousCurrentIndex );
		blockSignals( oldSignalState );
	}

	void RepresentationLibraryComboBox::_onRepresentationLibraryChange( const Event::VTXEvent * const p_event )
	{
		if ( p_event->name == Event::Model::DISPLAY_NAME_CHANGE || p_event->name == Event::Model::DATA_CHANGE )
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
