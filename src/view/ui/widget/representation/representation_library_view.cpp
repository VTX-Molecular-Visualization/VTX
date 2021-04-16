#include "representation_library_view.hpp"
#include "action/action_manager.hpp"
#include "action/representation.hpp"
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
		_registerEvent( Event::Global::REPRESENTATION_ADDED );
	}

	void RepresentationLibraryView::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );
		setContentsMargins( 0, 0, 0, 0 );

		QHBoxLayout * const horizontalLayout = new QHBoxLayout( this );
		QHBoxLayout * const headerLayout	 = new QHBoxLayout();
		QVBoxLayout * const verticalLayout	 = new QVBoxLayout();

		const int currentIndex = 0;
		_presetList
			= VTX::UI::WidgetFactory::get().instantiateWidget<RepresentationLibraryComboBox>( this, "PresetList" );
		_presetList->setHighlightDefaultRepresentation( true );
		_presetList->setCurrentIndex( currentIndex );

		_representationPresetEditor = VTX::UI::WidgetFactory::get().instantiateWidget<RepresentationPresetEditor>(
			this, "renderEffectPresetEdition" );

		_addPresetButton = new QPushButton( this );
		_addPresetButton->setIcon( QIcon( ":/sprite/add_preset_icon.png" ) );
		_copyPresetButton = new QPushButton( this );
		_copyPresetButton->setIcon( QIcon( ":/sprite/copy_preset_icon.png" ) );
		_deletePresetButton = new QPushButton( this );
		_deletePresetButton->setIcon( QIcon( ":/sprite/delete_preset_icon.png" ) );

		headerLayout->addWidget( _presetList, 10 );
		headerLayout->addWidget( _addPresetButton );
		headerLayout->addWidget( _copyPresetButton );
		headerLayout->addWidget( _deletePresetButton );

		verticalLayout->addItem( headerLayout );
		verticalLayout->addWidget( _representationPresetEditor );

		horizontalLayout->addItem( verticalLayout );

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
	}

	void RepresentationLibraryView::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::REPRESENTATION_ADDED )
		{
			const Event::VTXEventValue<int> & castedEvent = dynamic_cast<const Event::VTXEventValue<int> &>( p_event );
			const int						  representationIndex = castedEvent.value;

			_presetList->setCurrentIndex( representationIndex );
		}
	}

	void RepresentationLibraryView::localize() {}

	void RepresentationLibraryView::_refreshView() { _refreshPresetDisplayed( true ); }

	void RepresentationLibraryView::_onPresetIndexChanged( const int p_newIndex ) { _refreshPresetDisplayed( false ); }

	void RepresentationLibraryView::_onAddPreset() const
	{
		VTX_ACTION( new Action::Representation::AddNewPresetInLibrary( Setting::NEW_REPRESENTATION_DEFAULT_NAME ) );
	}
	void RepresentationLibraryView::_onCopyPreset() const
	{
		VTX_ACTION( new Action::Representation::CopyPresetInLibrary( _presetList->currentIndex() ) );
	}
	void RepresentationLibraryView::_onDeletePreset() const
	{
		VTX_ACTION( new Action::Representation::DeletePresetInLibrary( _presetList->currentIndex() ) );
	}

	void RepresentationLibraryView::_refreshPresetDisplayed( const bool p_applyPreset )
	{
		const int									  currentIndex	 = _presetList->currentIndex();
		Model::Representation::Representation * const representation = _model->getRepresentation( currentIndex );

		_representationPresetEditor->setPreset( representation, p_applyPreset );

		_deletePresetButton->setEnabled( _model->canDeleteRepresentation( representation ) );
	}

} // namespace VTX::View::UI::Widget::Representation
