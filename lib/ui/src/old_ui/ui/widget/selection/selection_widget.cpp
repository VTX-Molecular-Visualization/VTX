#include "ui/old_ui/ui/widget/selection/selection_widget.hpp"
#include "ui/old_ui/style.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/old_ui/view/ui/widget/selection_view.hpp"
#include "ui/old_ui/vtx_app.hpp"
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QtGlobal>
#include <app/mvc.hpp>
#include <app/event/global.hpp>
#include <app/application/selection/enum_selection.hpp>
#include <app/application/selection/selection_manager.hpp>
#include <app/application/setting.hpp>
#include <string>

namespace VTX::UI::Widget::Selection
{
	SelectionWidget::SelectionWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
	{
		_registerEvent( VTX::App::Event::Global::SELECTION_ADDED );
		_registerEvent( VTX::App::Event::Global::SELECTION_REMOVED );
	}

	void SelectionWidget::receiveEvent( const VTX::App::Core::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::App::Event::Global::SELECTION_ADDED )
		{
			const VTX::App::Core::Event::VTXEventArg<App::Application::Selection::SelectionModel *> & castedEvent
				= dynamic_cast<const VTX::App::Core::Event::VTXEventArg<App::Application::Selection::SelectionModel *> &>( p_event );
			_addSelectionModel( castedEvent.get() );
		}
		else if ( p_event.name == VTX::App::Event::Global::SELECTION_REMOVED )
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

		for ( int i = 0; i < (int)VTX::App::Application::Selection::GRANULARITY::COUNT; i++ )
		{
			_selectionTypeComboBox->addItem( "" );
		}
		_selectionTypeComboBox->setCurrentIndex( int( VTX::App::Application::Setting::SELECTION_GRANULARITY_DEFAULT ) );
		_layout->addStretch( 100 );
		_layout->addWidget( _selectionTypeComboBox );

		setWidget( _mainWidget );

		_addSelectionModel( &VTX::App::Application::Selection::SelectionManager::get().getSelectionModel() );
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

	void SelectionWidget::_addSelectionModel( App::Application::Selection::SelectionModel * const p_selection )
	{
		View::UI::Widget::SelectionView * const item
			= WidgetFactory::get().instantiateViewWidget<View::UI::Widget::SelectionView>(
				p_selection, ID::View::UI_SELECTION, nullptr, "SelectionWidget" );
		_layout->insertWidget( _layout->count() - 1, item );
	}

	void SelectionWidget::_populateItemList()
	{
		for ( int i = 0; i < (int)VTX::App::Application::Selection::GRANULARITY::COUNT; i++ )
		{
			QString							  txt;
			const VTX::App::Application::Selection::GRANULARITY selectionType = (VTX::App::Application::Selection::GRANULARITY)i;

			switch ( selectionType )
			{
			case VTX::App::Application::Selection::GRANULARITY::ATOM: txt = "Atom"; break;
			// case VTX::App::Application::Selection::GRANULARITY::BOND: txt = "Bond"; break;
			case VTX::App::Application::Selection::GRANULARITY::CHAIN: txt = "Chain"; break;
			case VTX::App::Application::Selection::GRANULARITY::MOLECULE: txt = "Molecule"; break;
			case VTX::App::Application::Selection::GRANULARITY::RESIDUE: txt = "Residue"; break;

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
