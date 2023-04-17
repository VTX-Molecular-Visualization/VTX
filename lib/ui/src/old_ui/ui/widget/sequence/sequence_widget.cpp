#include "ui/old_ui/ui/widget/sequence/sequence_widget.hpp"
#include "ui/old_ui/view/ui/widget/molecule_sequence_view.hpp"
#include "ui/old_ui/vtx_app.hpp"
#include <QLayout>
#include <QScrollBar>
#include <QWidget>
#include <app/old_app/model/molecule.hpp>
#include <app/old_app/mvc/mvc_manager.hpp>
#include <app/old_app/view/base_view.hpp>

namespace VTX::UI::Widget::Sequence
{
	SequenceWidget::SequenceWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
	{
		_registerEvent( VTX::Event::Global::MOLECULE_ADDED );
		_registerEvent( VTX::Event::Global::MOLECULE_REMOVED );
	}

	void SequenceWidget::receiveEvent( const VTX::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::Event::Global::MOLECULE_ADDED )
		{
			const VTX::Event::VTXEventPtr<Model::Molecule> & castedEvent
				= dynamic_cast<const VTX::Event::VTXEventPtr<Model::Molecule> &>( p_event );
			View::UI::Widget::MoleculeSequenceView * const moleculeSequenceView
				= MVC::MvcManager::get().instantiateViewWidget<View::UI::Widget::MoleculeSequenceView>(
					castedEvent.ptr, ID::View::UI_MOLECULE_SEQUENCE, this );
			MoleculeSequenceWidget * const widget = moleculeSequenceView->getWidget();
			_moleculeWidgets.emplace( widget );
			_layout->insertWidget( _layout->count() - 1, widget );
		}
		else if ( p_event.name == VTX::Event::Global::MOLECULE_REMOVED )
		{
			const VTX::Event::VTXEventPtr<Model::Molecule> & castedEvent
				= dynamic_cast<const VTX::Event::VTXEventPtr<Model::Molecule> &>( p_event );
			View::UI::Widget::MoleculeSequenceView * const moleculeSequenceView
				= MVC::MvcManager::get().getView<View::UI::Widget::MoleculeSequenceView>(
					castedEvent.ptr, ID::View::UI_MOLECULE_SEQUENCE );
			_moleculeWidgets.erase( moleculeSequenceView->getWidget() );
			MVC::MvcManager::get().deleteView<View::UI::Widget::MoleculeSequenceView>( castedEvent.ptr,
																					   ID::View::UI_MOLECULE_SEQUENCE );
		}
	}

	void SequenceWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		_scrollArea = new CustomWidget::DockWindowMainWidget<QScrollArea>(
			Style::SEQUENCE_PREFERRED_SIZE, Style::SEQUENCE_MINIMUM_SIZE, this );
		//_scrollArea->setSizePolicy( QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred );

		_scrollArea->setContentsMargins( 0, 0, 0, 0 );

		QWidget * const scrollWidget = new QWidget( _scrollArea );
		scrollWidget->setContentsMargins( 0, 0, 0, 0 );

		QSizePolicy sizePolicy = QSizePolicy( QSizePolicy::Policy::MinimumExpanding,
											  QSizePolicy::Policy::MinimumExpanding,
											  QSizePolicy::ControlType::Frame );
		sizePolicy.setHorizontalStretch( 10 );
		sizePolicy.setVerticalStretch( 10 );
		scrollWidget->setSizePolicy( sizePolicy );

		_layout = new QVBoxLayout( scrollWidget );
		_layout->setContentsMargins( 0, 0, 0, 0 );
		_layout->setSizeConstraint( QLayout::SizeConstraint::SetMinAndMaxSize );
		_layout->addStretch( 1000 );

		_scrollArea->setWidget( scrollWidget );
		_scrollArea->setWidgetResizable( true );
		this->setWidget( _scrollArea );
	}

	void SequenceWidget::_setupSlots() {}

	void SequenceWidget::refreshSelection() const
	{
		for ( const auto it : _moleculeWidgets )
		{
			it->repaintSelection();
		}
	}

	void SequenceWidget::localize()
	{
		this->setWindowTitle( "Sequence" );
		// this->setWindowTitle( QCoreApplication::translate( "SceneWidget", "Scene", nullptr ) );
	}

} // namespace VTX::UI::Widget::Sequence