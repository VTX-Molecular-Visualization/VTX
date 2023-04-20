#include "ui/old_ui/ui/widget/selection/selection_widget.hpp"
#include "ui/old_ui/style.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/old_ui/view/ui/widget/selection_view.hpp"
#include "ui/old_ui/vtx_app.hpp"
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QtGlobal>
#include <app/core/mvc/mvc_manager.hpp>
#include <app/old_app/selection/selection_enum.hpp>
#include <app/old_app/selection/selection_manager.hpp>
#include <app/old_app/setting.hpp>
#include <string>

namespace VTX::UI::Widget::Selection
{
	SelectionWidget::SelectionWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
	{
		_registerEvent( VTX::Event::Global::SELECTION_ADDED );
		_registerEvent( VTX::Event::Global::SELECTION_REMOVED );
	}

	void SelectionWidget::receiveEvent( const VTX::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::Event::Global::SELECTION_ADDED )
		{
			const VTX::Event::VTXEventPtr<Model::Selection> & castedEvent
				= dynamic_cast<const VTX::Event::VTXEventPtr<Model::Selection> &>( p_event );
			_addSelectionModel( castedEvent.ptr );
		}
		else if ( p_event.name == VTX::Event::Global::SELECTION_REMOVED )
		{
			// Only 1 selection at this time.
		}
	}

	void SelectionWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		_mainWidget = new CustomWidget::DockWindowMainWidget<QWidget>(
			Style::SELECTION_PREFERRED_SIZE, Style::SELECTION_MINIMUM_SIZE, this );

		QSizePolicy sizePolicy = QSizePolicy( QSizePolicy::Policy::MinimumExpanding,
											  QSizePolicy::Policy::MinimumExpanding,
											  QSizePolicy::ControlType::Frame );
		sizePolicy.setHorizontalStretch( 10 );
		sizePolicy.setVerticalStretch( 5 );

		_mainWidget->setSizePolicy( sizePolicy );

		_layout				   = new QVBoxLayout( _mainWidget );
		_selectionTypeComboBox = new QComboBox( this );

		for ( int i = 0; i < (int)VTX::Selection::Granularity::COUNT; i++ )
		{
			_selectionTypeComboBox->addItem( "" );
		}
		_selectionTypeComboBox->setCurrentIndex( int( Setting::SELECTION_GRANULARITY_DEFAULT ) );
		_layout->addStretch( 100 );
		_layout->addWidget( _selectionTypeComboBox );

		setWidget( _mainWidget );

		_addSelectionModel( &VTX::Selection::SelectionManager::get().getSelectionModel() );
	}

	void SelectionWidget::_setupSlots()
	{
		connect( _selectionTypeComboBox,
				 QOverload<int>::of( &QComboBox::currentIndexChanged ),
				 this,
				 &SelectionWidget::_selectionTypeCurrentIndexChanged );
	}

	void SelectionWidget::_selectionTypeCurrentIndexChanged( const int p_newIndex )
	{
		VTX_INFO( std::to_string( p_newIndex ) + " selected." );

		// TODO: action to change global selection mode.
	}

	void SelectionWidget::localize()
	{
		_populateItemList();

		this->setWindowTitle( "Selection" );
		// this->setWindowTitle( QCoreApplication::translate( "SceneWidget", "Scene", nullptr ) );
	}

	void SelectionWidget::_addSelectionModel( Model::Selection * const p_selection )
	{
		View::UI::Widget::SelectionView * const item
			= WidgetFactory::get().instantiateViewWidget<View::UI::Widget::SelectionView>(
				p_selection, ID::View::UI_SELECTION, nullptr, "SelectionWidget" );
		_layout->insertWidget( _layout->count() - 1, item );
	}

	void SelectionWidget::_populateItemList()
	{
		for ( int i = 0; i < (int)VTX::Selection::Granularity::COUNT; i++ )
		{
			QString							  txt;
			const VTX::Selection::Granularity selectionType = (VTX::Selection::Granularity)i;

			switch ( selectionType )
			{
			case VTX::Selection::Granularity::ATOM: txt = "Atom"; break;
			// case VTX::Selection::Granularity::BOND: txt = "Bond"; break;
			case VTX::Selection::Granularity::CHAIN: txt = "Chain"; break;
			case VTX::Selection::Granularity::MOLECULE: txt = "Molecule"; break;
			case VTX::Selection::Granularity::RESIDUE: txt = "Residue"; break;

			default:
				VTX_WARNING( "Selection " + std::to_string( i )
							 + " not managed in SelectionWidget::getSelectionTypeText." );
				txt = "Unknown";
				break;
			}

			_selectionTypeComboBox->setItemText( i, txt );
		}
	}
} // namespace VTX::UI::Widget::Selection
