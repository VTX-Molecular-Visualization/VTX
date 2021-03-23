#include "multiple_atom_inspector_widget.hpp"
#include "action/action_manager.hpp"
#include "style.hpp"
#include "ui/widget_factory.hpp"
#include "view/ui/widget/inspector/all_inspector_view.hpp"
#include <QBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPixmap>

namespace VTX::UI::Widget::Inspector
{
	MultipleAtomWidget::MultipleAtomWidget( QWidget * p_parent ) : InspectorItemWidget( p_parent ) {};

	MultipleAtomWidget::~MultipleAtomWidget() {}

	void MultipleAtomWidget::_setupUi( const QString & p_name )
	{
		InspectorItemWidget::_setupUi( p_name );

		_infoSection = VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSectionVLayout>(
			this, "inspector_item_section" );
		_fullnameLabel = new CustomWidget::QLabelMultiField( this );
		_fullnameLabel->setWordWrap( true );
		_infoSection->appendField( "Full Name", _fullnameLabel );

		_appendSection( _infoSection );

		const bool oldBlockState = blockSignals( true );
		refresh();
		blockSignals( oldBlockState );
	}

	void MultipleAtomWidget::_setupSlots() {};

	void MultipleAtomWidget::refresh( const SectionFlag & p_flag )
	{
		if ( _isRefreshFreezed() )
			return;

		_resetFieldStates( p_flag );

		if ( _targets.size() > 0 )
		{
			const QString headerTitle = QString::fromStdString( "Atom (" + std::to_string( _targets.size() ) + ")" );
			_getHeader()->setHeaderTitle( headerTitle );

			const QPixmap * symbolPixmap = Style::IconConst::get().getModelSymbol( ID::Model::MODEL_ATOM );
			_getHeader()->setHeaderIcon( *symbolPixmap );

			for ( const Model::Atom * atom : _targets )
			{
				if ( bool( p_flag & SectionFlag::INFOS ) )
				{
					if ( !_fullnameLabel->hasDifferentData() )
						_fullnameLabel->updateWithNewValue( atom->getSymbolName() );
				}
			}
		}
	}

	void MultipleAtomWidget::_resetFieldStates( const SectionFlag & p_flag )
	{
		if ( bool( p_flag & SectionFlag::INFOS ) )
		{
			_fullnameLabel->resetState();
		}
	}

	void MultipleAtomWidget::localize()
	{
		_infoSection->setHeaderTitle( "Infos" );
		_infoSection->localize();
	}

	void MultipleAtomWidget::clearTargets()
	{
		for ( Model::Atom * const atom : _targets )
		{
			// unlink inspector to prevent target update in view destructor
			MVC::MvcManager::get()
				.getView<View::Inspector::AtomInspectorView>( atom, ID::View::UI_INSPECTOR_ATOM )
				->setLinkedInspector( nullptr );
			MVC::MvcManager::get().deleteView( atom, ID::View::UI_INSPECTOR_ATOM );
		}

		_targets.clear();
	}
	void MultipleAtomWidget::addTarget( Model::Atom * const p_target, const bool p_refresh )
	{
		_targets.emplace( p_target );

		View::Inspector::AtomInspectorView * const view
			= MVC::MvcManager::get().instantiateView<View::Inspector::AtomInspectorView>( p_target,
																						  ID::View::UI_INSPECTOR_ATOM );

		view->setLinkedInspector( this );

		if ( p_refresh )
			refresh();
	}
	void MultipleAtomWidget::removeTarget( Model::Atom * const p_target, const bool p_refresh )
	{
		_targets.erase( p_target );

		if ( MVC::MvcManager::get().hasView( p_target, ID::View::UI_INSPECTOR_ATOM ) )
			MVC::MvcManager::get().deleteView( p_target, ID::View::UI_INSPECTOR_ATOM );

		if ( p_refresh )
			refresh();
	}
} // namespace VTX::UI::Widget::Inspector
