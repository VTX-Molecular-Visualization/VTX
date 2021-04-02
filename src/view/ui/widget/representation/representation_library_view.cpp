#include "representation_library_view.hpp"
#include "action/action_manager.hpp"
#include "id.hpp"
#include "model/representation/representation.hpp"
#include "ui/widget_factory.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace VTX::View::UI::Widget::Representation
{
	RepresentationLibraryView::RepresentationLibraryView( Model::Representation::RepresentationLibrary * const p_model,
														  QWidget * const p_parent ) :
		View::BaseView<Model::Representation::RepresentationLibrary>( p_model ),
		VTX::UI::Widget::BaseManualWidget<QWidget>( p_parent )
	{
	}

	void RepresentationLibraryView::notify( const Event::VTXEvent * const p_event ) {}

	void RepresentationLibraryView::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );
		setContentsMargins( 0, 0, 0, 0 );

		QHBoxLayout * const horizontalLayout = new QHBoxLayout( this );
		QVBoxLayout * const verticalLayout	 = new QVBoxLayout();

		const int currentIndex = 0;
		_presetList
			= VTX::UI::WidgetFactory::get().instantiateWidget<RepresentationLibraryComboBox>( this, "PresetList" );

		_presetList->setCurrentIndex( currentIndex );
		_representationPresetEditor = VTX::UI::WidgetFactory::get().instantiateWidget<RepresentationPresetEditor>(
			this, "renderEffectPresetEdition" );

		verticalLayout->addWidget( _presetList );
		verticalLayout->addWidget( _representationPresetEditor );
		// verticalLayout->addStretch( 1 );

		// horizontalLayout->addStretch( 2 );
		horizontalLayout->addItem( verticalLayout );
		// horizontalLayout->addStretch( 2 );

		_representationPresetEditor->setPreset( _model->getRepresentation( _presetList->currentIndex() ), false );
	}

	void RepresentationLibraryView::_setupSlots()
	{
		connect( _presetList,
				 QOverload<int>::of( &QComboBox::currentIndexChanged ),
				 this,
				 &RepresentationLibraryView::_onPresetIndexChanged );
	}

	void RepresentationLibraryView::localize() {}

	void RepresentationLibraryView::_refreshView() { _refreshPresetDisplayed(); }

	void RepresentationLibraryView::_onPresetIndexChanged( const int p_newIndex ) { _refreshPresetDisplayed(); }

	void RepresentationLibraryView::_buildPresetList()
	{
		int previousCurrentIndex = _presetList->currentIndex();

		for ( const Model::Representation::Representation * const preset : _model->getRepresentations() )
		{
			_presetList->addItem( QString::fromStdString( preset->getName() ) );
		}

		if ( previousCurrentIndex >= _presetList->count() )
		{
			previousCurrentIndex = _presetList->count() - 1;
		}

		_presetList->setCurrentIndex( previousCurrentIndex );
	}
	void RepresentationLibraryView::_refreshPresetDisplayed()
	{
		const int currentIndex = _presetList->currentIndex();
		_representationPresetEditor->setPreset( _model->getRepresentation( currentIndex ) );
	}

} // namespace VTX::View::UI::Widget::Representation
