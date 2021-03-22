#include "inspector_widget.hpp"
#include "model/molecule.hpp"
#include "model/representation/representation.hpp"
#include "model/representation/representation_library.hpp"
#include "model/selection.hpp"
#include "representation/representation_manager.hpp"
#include "selection/selection_manager.hpp"
#include "style.hpp"
#include "view/ui/widget/molecule_inspector_view.hpp"
#include "view/ui/widget/representation_inspector_view.hpp"
#include <type_traits>

namespace VTX::UI::Widget::Inspector
{
	InspectorWidget::InspectorWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
	{
		_registerEvent( Event::Global::SELECTION_CHANGE );
	}

	InspectorWidget::~InspectorWidget() {}

	void InspectorWidget::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::SELECTION_CHANGE )
			refresh();
	}

	void InspectorWidget::refresh()
	{
		clear();

		for ( const auto it : VTX::Selection::SelectionManager::get().getSelectionModel().getItems() )
		{
			Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( it.first );
			_addMolecule( &molecule );
		}

		for ( auto it : VTX::Selection::SelectionManager::get().getSelectionModel().getRepresentations() )
		{
			_addRepresentation( it );
		}
	}

	void InspectorWidget::clear()
	{
		for ( const ViewData viewData : _inspectorViewsData )
		{
			Model::BaseModel & model = MVC::MvcManager::get().getModel<Model::BaseModel>( viewData._modelID );
			_verticalLayout->removeWidget( viewData._widget );
			MVC::MvcManager::get().deleteView( &model, viewData._viewID );
		}

		_inspectorViewsData.clear();
	}

	void InspectorWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		_scrollWidget = new QWidget( this );
		_scrollWidget->setSizePolicy( QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum );

		_verticalLayout = new QVBoxLayout( _scrollWidget );
		_verticalLayout->setSizeConstraint( QLayout::SizeConstraint::SetMinAndMaxSize );
		_verticalLayout->setSpacing( 4 );
		_verticalLayout->addStretch( 1000 );
		_verticalLayout->setContentsMargins( 0, 0, 0, 0 );

		_scrollArea = new CustomWidget::DockWindowMainWidget<QScrollArea>( this );
		_scrollArea->setFrameShape( QFrame::Shape::NoFrame );
		_scrollArea->setWidget( _scrollWidget );
		_scrollArea->setWidgetResizable( true );
		_scrollArea->setSizeAdjustPolicy( QAbstractScrollArea::SizeAdjustPolicy::AdjustIgnored );
		_scrollArea->setSizeHint( Style::INSPECTOR_PREFERED_SIZE );
		_scrollArea->setMinimumSizeHint( Style::INSPECTOR_MINIMUM_SIZE );

		setWidget( _scrollArea );
	}

	void InspectorWidget::_addMolecule( Model::Molecule * const p_molecule )
	{
		_addInspectorView<Model::Molecule, View::UI::Widget::MoleculeInspectorView>(
			p_molecule, ID::View::UI_INSPECTOR_MOLECULE_STRUCTURE );
	}
	void InspectorWidget::_addRepresentation(
		Model::Representation::InstantiatedRepresentation * const p_representation )
	{
		_addInspectorView<Model::Representation::InstantiatedRepresentation,
						  View::UI::Widget::RepresentationInspectorView>( p_representation,
																		  ID::View::UI_INSPECTOR_REPRESENTATION );
	}

	void InspectorWidget::_setupSlots() {}
	void InspectorWidget::localize()
	{
		this->setWindowTitle( "Inspector" );
		// this->setWindowTitle( QCoreApplication::translate( "InspectorWidget", "Inspector", nullptr ) );
	}
} // namespace VTX::UI::Widget::Inspector
