#include "multiple_atom_inspector_widget.hpp"
#include "action/action_manager.hpp"
#include "model/bond.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include "style.hpp"
#include "ui/widget_factory.hpp"
#include "util/ui.hpp"
#include <QBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPixmap>
#include <set>

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

		_bondInfoLabel = new QLabel( this );

		_infoSection->appendField( "Name", _nameLabel );
		_infoSection->appendField( "Full Name", _genericNameLabel );
		_infoSection->appendField( "Bonds", _bondInfoLabel );

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

		const std::unordered_set<Model::Atom *> & targets = getTargets();

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

			_fillBondData();
		}
	}

	void MultipleAtomWidget::_resetFieldStates( const SectionFlag & p_flag )
	{
		if ( bool( p_flag & SectionFlag::INFOS ) )
		{
			_nameLabel->resetState();
			_genericNameLabel->resetState();
			_bondInfoLabel->setText( "" );
		}
	}

	void MultipleAtomWidget::_fillBondData()
	{
		std::set<uint> pendingBondIDs = std::set<uint>();

		auto comparer = []( const Model::Bond * p_lhs, const Model::Bond * p_rhs )
		{ return Model::Bond::comparer( *p_lhs, *p_rhs ); };

		std::set<const Model::Bond *, decltype( comparer )> fullBonds( comparer );

		for ( const Model::Atom * const atom : getTargets() )
		{
			const uint					  atomIndex		 = atom->getIndex();
			const Model::Residue * const  residue		 = atom->getResiduePtr();
			const Model::Molecule * const moleculeParent = residue->getMoleculePtr();

			for ( uint bondIndex = residue->getIndexFirstBond();
				  bondIndex < ( residue->getIndexFirstBond() + residue->getBondCount() );
				  bondIndex++ )
			{
				const Model::Bond * const bond = moleculeParent->getBond( bondIndex );

				if ( bond == nullptr )
					continue;

				if ( bond->getIndexFirstAtom() == atomIndex || bond->getIndexSecondAtom() == atomIndex )
				{
					if ( pendingBondIDs.find( bondIndex ) == pendingBondIDs.cend() )
					{
						pendingBondIDs.emplace( bondIndex );
					}
					else
					{
						pendingBondIDs.erase( bondIndex );
						fullBonds.emplace( bond );
					}
				}
			}
		}

		QString bondInfoStr;
		for ( const Model::Bond * const bond : fullBonds )
		{
			Util::UI::appendBondInfo( *bond, bondInfoStr );
		}
		_bondInfoLabel->setText( bondInfoStr );
	}

	void MultipleAtomWidget::localize()
	{
		_infoSection->setHeaderTitle( "Infos" );
		_infoSection->localize();
	}
} // namespace VTX::UI::Widget::Inspector
