#include "util/ui.hpp"
#include "model/atom.hpp"
#include "model/bond.hpp"
#include "model/molecule.hpp"

namespace VTX::Util::UI
{
	void fillComboBox( QComboBox * const p_comboBox, const std::vector<std::string> & p_values )
	{
		for ( const std::string & str : p_values )
		{
			p_comboBox->addItem( QString::fromStdString( str ) );
		}
	}
	void fillComboBox( QComboBox * const p_comboBox, const std::vector<QString> & p_values )
	{
		for ( const QString & str : p_values )
		{
			p_comboBox->addItem( str );
		}
	}

	void appendBondInfo( const Model::Bond & p_bond, QString & p_str )
	{
		const Model::Molecule * const moleculePtr = p_bond.getMoleculePtr();

		const Model::Atom * const firstAtom	 = moleculePtr->getAtom( p_bond.getIndexFirstAtom() );
		const Model::Atom * const secondAtom = moleculePtr->getAtom( p_bond.getIndexSecondAtom() );

		if ( firstAtom == nullptr || secondAtom == nullptr )
			return;

		const QString firstAtomStr
			= QString::fromStdString( firstAtom->getSymbolStr() + std::to_string( firstAtom->getIndex() ) );
		const QString secondAtomStr
			= QString::fromStdString( secondAtom->getSymbolStr() + std::to_string( secondAtom->getIndex() ) );

		QString					 linkCountStr;
		const Model::Bond::ORDER bondOrder = p_bond.getOrder();

		if ( bondOrder == Model::Bond::ORDER::SINGLE )
		{
			linkCountStr = '1';
		}
		else if ( bondOrder == Model::Bond::ORDER::DOUBLE )
		{
			linkCountStr = '2';
		}
		else if ( bondOrder == Model::Bond::ORDER::TRIPLE )
		{
			linkCountStr = '3';
		}
		else if ( bondOrder == Model::Bond::ORDER::QUADRUPLE )
		{
			linkCountStr = '4';
		}
		else if ( bondOrder == Model::Bond::ORDER::QUINTUPLET )
		{
			linkCountStr = '5';
		}
		else if ( bondOrder == Model::Bond::ORDER::AROMATIC )
		{
			linkCountStr = 'A';
		}
		else // UNKNOWN
		{
			linkCountStr = '?';
		}

		if ( p_str.isEmpty() )
		{
			p_str.append( firstAtomStr + "--" + linkCountStr + "--" + secondAtomStr );
		}
		else
		{
			p_str.append( '\n' + firstAtomStr + "--" + linkCountStr + "--" + secondAtomStr );
		}
	}

} // namespace VTX::Util::UI
