#include "ui/old_ui/util/ui.hpp"
#include <app/model/atom.hpp>
#include <app/model/bond.hpp>
#include <app/model/molecule.hpp>
#include <app/model/residue.hpp>

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

	void fillMenu( QMenu &							p_menu,
				   const int						p_enumSize,
				   const std::vector<std::string> & p_names,
				   const bool						p_actionCheckable )
	{
		for ( int i = 0; i < p_enumSize; i++ )
		{
			QAction * const action = new QAction( QString::fromStdString( p_names[ i ] ), &p_menu );
			action->setData( i );
			action->setCheckable( p_actionCheckable );

			p_menu.addAction( action );
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
			= QString::fromStdString( firstAtom->getName() + std::to_string( firstAtom->getIndex() ) );
		const QString secondAtomStr
			= QString::fromStdString( secondAtom->getName() + std::to_string( secondAtom->getIndex() ) );

		const Model::Residue * const firstResidue  = firstAtom->getResiduePtr();
		const Model::Residue * const secondResidue = secondAtom->getResiduePtr();

		const QString firstResidueStr = QString::fromStdString(
			firstResidue->getSymbolStr() + " " + std::to_string( firstResidue->getIndexInOriginalChain() ) );
		const QString secondResidueStr = QString::fromStdString(
			secondResidue->getSymbolStr() + " " + std::to_string( secondResidue->getIndexInOriginalChain() ) );

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
			p_str.append( "(" + firstResidueStr + ") " + firstAtomStr + "--" + linkCountStr + "--" + secondAtomStr
						  + " (" + secondResidueStr + ")" );
		}
		else
		{
			p_str.append( "\n(" + firstResidueStr + ") " + firstAtomStr + "--" + linkCountStr + "--" + secondAtomStr
						  + " (" + secondResidueStr + ")" );
		}
	}

} // namespace VTX::Util::UI
