#include "ui/qt/util.hpp"
#include <QAction>
#include <app/old/component/chemistry/atom.hpp>
#include <app/old/component/chemistry/bond.hpp>
#include <app/old/component/chemistry/molecule.hpp>
#include <app/old/component/chemistry/residue.hpp>

namespace VTX::UI::QT::Util
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

	void appendBondInfo( const App::Old::Component::Chemistry::Bond & p_bond, QString & p_str )
	{
		const App::Old::Component::Chemistry::Molecule * const moleculePtr = p_bond.getMoleculePtr();

		const App::Old::Component::Chemistry::Atom * const firstAtom	 = moleculePtr->getAtom( p_bond.getIndexFirstAtom() );
		const App::Old::Component::Chemistry::Atom * const secondAtom = moleculePtr->getAtom( p_bond.getIndexSecondAtom() );

		if ( firstAtom == nullptr || secondAtom == nullptr )
			return;

		const QString firstAtomStr
			= QString::fromStdString( firstAtom->getName() + std::to_string( firstAtom->getIndex() ) );
		const QString secondAtomStr
			= QString::fromStdString( secondAtom->getName() + std::to_string( secondAtom->getIndex() ) );

		const App::Old::Component::Chemistry::Residue * const firstResidue  = firstAtom->getResiduePtr();
		const App::Old::Component::Chemistry::Residue * const secondResidue = secondAtom->getResiduePtr();

		const QString firstResidueStr = QString::fromStdString(
			firstResidue->getSymbolStr() + " " + std::to_string( firstResidue->getIndexInOriginalChain() ) );
		const QString secondResidueStr = QString::fromStdString(
			secondResidue->getSymbolStr() + " " + std::to_string( secondResidue->getIndexInOriginalChain() ) );

		QString									 linkCountStr;
		const VTX::Core::ChemDB::Bond::ORDER bondOrder = p_bond.getOrder();

		if ( bondOrder == VTX::Core::ChemDB::Bond::ORDER::SINGLE )
		{
			linkCountStr = '1';
		}
		else if ( bondOrder == VTX::Core::ChemDB::Bond::ORDER::DOUBLE )
		{
			linkCountStr = '2';
		}
		else if ( bondOrder == VTX::Core::ChemDB::Bond::ORDER::TRIPLE )
		{
			linkCountStr = '3';
		}
		else if ( bondOrder == VTX::Core::ChemDB::Bond::ORDER::QUADRUPLE )
		{
			linkCountStr = '4';
		}
		else if ( bondOrder == VTX::Core::ChemDB::Bond::ORDER::QUINTUPLET )
		{
			linkCountStr = '5';
		}
		else if ( bondOrder == VTX::Core::ChemDB::Bond::ORDER::AROMATIC )
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

} // namespace VTX::UI::QT::Util
