#include "selection_widget.hpp"
#include "mvc/mvc_manager.hpp"
#include "style.hpp"
#include "ui/widget_factory.hpp"
#include "view/ui/widget/selection_view.hpp"
#include "vtx_app.hpp"
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QtGlobal>
#include <string>

namespace VTX::UI::Widget::Selection
{
	SelectionWidget::SelectionWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
	{
		_registerEvent( Event::Global::SELECTION_ADDED );
		_registerEvent( Event::Global::SELECTION_REMOVED );
	}

	void SelectionWidget::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::SELECTION_ADDED )
		{
			const Event::VTXEventPtr<Model::Selection> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Selection> &>( p_event );
			View::UI::Widget::SelectionView * const item
				= WidgetFactory::get().instantiateViewWidget<View::UI::Widget::SelectionView>(
					castedEvent.ptr, ID::View::UI_SELECTION, nullptr, "SelectionWidget" );
			setWidget( item );
		}
		else if ( p_event.name == Event::Global::SELECTION_REMOVED )
		{
			// Only 1 selection at this time.
		}
	}

	void SelectionWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		_mainWidget = new CustomWidget::DockWindowMainWidget<QWidget>( this );
		_mainWidget->setSizeHint( Style::SELECTION_PREFERED_SIZE );
		_mainWidget->setMinimumSizeHint( Style::SELECTION_MINIMUM_SIZE );

		_layout				   = new QVBoxLayout( _mainWidget );
		_selectionTypeComboBox = new QComboBox( this );

		for ( int i = 0; i < (int)VTX::Selection::SelectionType::COUNT; i++ )
		{
			_selectionTypeComboBox->addItem( "" );
		}
		_selectionTypeComboBox->setCurrentIndex( (int)Style::SELECTION_WINDOW_DEFAULT_SELECTION );
		_layout->addStretch( 100 );
		_layout->addWidget( _selectionTypeComboBox );

		setWidget( _mainWidget );
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

	void SelectionWidget::_populateItemList()
	{
		for ( int i = 0; i < (int)VTX::Selection::SelectionType::COUNT; i++ )
		{
			QString								txt;
			const VTX::Selection::SelectionType selectionType = (VTX::Selection::SelectionType)i;

			switch ( selectionType )
			{
			case VTX::Selection::SelectionType::ATOM: txt = "Atom"; break;
			case VTX::Selection::SelectionType::BOND: txt = "Bond"; break;
			case VTX::Selection::SelectionType::CHAINS: txt = "Chain"; break;
			case VTX::Selection::SelectionType::MOLECULE: txt = "Molecule"; break;
			case VTX::Selection::SelectionType::RESIDUE: txt = "Residue"; break;

			default:
				VTX_WARNING( "Selection " + std::to_string( i )
							 + " not managed in SelectionWidget::getSelectionTypeText." );
				txt = "<unknown>";
				break;
			}

			_selectionTypeComboBox->setItemText( i, txt );
		}
	}
} // namespace VTX::UI::Widget::Selection
