#include "multiple_atom_inspector_widget.hpp"
#include "action/action_manager.hpp"
#include "style.hpp"
#include "ui/widget_factory.hpp"
#include <QBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPixmap>

namespace VTX::UI::Widget::Inspector
{
	MultipleAtomWidget::MultipleAtomWidget( QWidget * p_parent ) :
		MultipleModelInspectorWidget( p_parent, ID::View::UI_INSPECTOR_ATOM ) {};

	MultipleAtomWidget::~MultipleAtomWidget() {}

	void MultipleAtomWidget::_setupUi( const QString & p_name )
	{
		MultipleModelInspectorWidget::_setupUi( p_name );

		_infoSection = VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSectionVLayout>(
			this, "inspector_item_section" );
		_genericNameLabel = new CustomWidget::QLabelMultiField( this );
		_genericNameLabel->setWordWrap( true );
		_nameLabel = new CustomWidget::QLabelMultiField( this );
		_nameLabel->setWordWrap( true );

		_infoSection->appendField( "Name", _nameLabel );
		_infoSection->appendField( "Full Name", _genericNameLabel );

		_appendSection( _infoSection );

		const bool oldBlockState = blockSignals( true );
		refresh();
		blockSignals( oldBlockState );
	}

	void MultipleAtomWidget::_setupSlots() {};

	void MultipleAtomWidget::_endOfFrameRefresh( const SectionFlag & p_flag )
	{
		MultipleModelInspectorWidget::_endOfFrameRefresh( p_flag );

		_resetFieldStates( p_flag );

		const std::unordered_set<Model::Atom *> & targets = _getTargets();

		if ( targets.size() > 0 )
		{
			const QString headerTitle = QString::fromStdString( "Atom (" + std::to_string( targets.size() ) + ")" );
			_getHeader()->setHeaderTitle( headerTitle );

			const QPixmap * symbolPixmap = Style::IconConst::get().getModelSymbol( VTX::ID::Model::MODEL_ATOM );
			_getHeader()->setHeaderIcon( *symbolPixmap );

			for ( const Model::Atom * atom : targets )
			{
				if ( bool( p_flag & SectionFlag::INFOS ) )
				{
					if ( !_nameLabel->hasDifferentData() )
						_nameLabel->updateWithNewValue( atom->getName() );
					if ( !_genericNameLabel->hasDifferentData() )
						_genericNameLabel->updateWithNewValue( atom->getSymbolName() );
				}
			}
		}
	}

	void MultipleAtomWidget::_resetFieldStates( const SectionFlag & p_flag )
	{
		if ( bool( p_flag & SectionFlag::INFOS ) )
		{
			_nameLabel->resetState();
			_genericNameLabel->resetState();
		}
	}

	void MultipleAtomWidget::localize()
	{
		_infoSection->setHeaderTitle( "Infos" );
		_infoSection->localize();
	}
} // namespace VTX::UI::Widget::Inspector
