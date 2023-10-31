#ifndef __VTX_APP_COMPONENT_CHEMISTRY_RESIDUE__
#define __VTX_APP_COMPONENT_CHEMISTRY_RESIDUE__

#include "_fwd.hpp"
#include "app/component/chemistry/iterator/atom.hpp"
#include <core/chemdb/atom.hpp>
#include <core/chemdb/residue.hpp>
#include <core/chemdb/secondary_structure.hpp>

namespace VTX::App::Component::Chemistry
{
	namespace ChemDB = VTX::Core::ChemDB;

	class Residue
	{
	  public:
		Residue() = default;
		Residue( Molecule * const p_molecule, const size_t p_index ) : _index( p_index ), _moleculePtr( p_molecule ) {}

		const Chain * const getConstChainPtr() const;
		Chain * const		getChainPtr() const;
		void				setChainPtr( Chain * const p_chainPtr );

		const Molecule * const getConstMoleculePtr() const { return _moleculePtr; }
		Molecule * const	   getMoleculePtr() const { return _moleculePtr; }

		size_t getIndex() const { return _index; }
		void   setIndex( const size_t p_index ) { _index = p_index; }

		size_t					getIndexFirstAtom() const;
		void					setIndexFirstAtom( const size_t p_indexFirstAtom );
		size_t					getIndexLastAtom() const;
		size_t					getAtomCount() const;
		void					setAtomCount( const size_t p_atomCount );
		size_t					getIndexFirstBond() const;
		void					setIndexFirstBond( const size_t p_indexFirstBond );
		size_t					getBondCount() const;
		void					setBondCount( const size_t p_bondCount );
		ChemDB::Residue::SYMBOL getSymbol() const;
		void					setSymbol( const ChemDB::Residue::SYMBOL p_symbol );

		const std::string & getShortName() const;
		const std::string & getName() const;
		const std::string & getLongName() const;

		const Util::Color::Rgba & getColor() const { return _color; };
		void					  setColor( const Util::Color::Rgba p_color ) { _color = p_color; };

		ChemDB::Residue::TYPE getType() { return _type; };
		void				  setType( const ChemDB::Residue::TYPE p_type ) { _type = p_type; };

		ChemDB::Atom::TYPE getAtomType() const;
		void			   setAtomType( const ChemDB::Atom::TYPE p_type );

		size_t getIndexInOriginalChain() const { return _indexInOriginalChain; };
		void   setIndexInOriginalChain( const size_t p_indexInOriginalChain )
		{
			_indexInOriginalChain = p_indexInOriginalChain;
		};

		const char getInsertionCode() const { return _insertionCode; }
		void	   setInsertionCode( const char p_insertionCode ) { _insertionCode = p_insertionCode; }

		ChemDB::SecondaryStructure::TYPE getSecondaryStructure() const { return _ssType; }
		void setSecondaryStructure( const ChemDB::SecondaryStructure::TYPE p_ssType ) { _ssType = p_ssType; }

		Iterator::AtomContainer atoms() const;

	  private:
		Molecule * _moleculePtr = nullptr;
		size_t	   _index		= INVALID_INDEX;

		Util::Color::Rgba	  _color				= COLOR_WHITE;
		size_t				  _indexInOriginalChain = INVALID_INDEX;
		ChemDB::Residue::TYPE _type					= ChemDB::Residue::TYPE::STANDARD;
		char				  _insertionCode		= ' ';

		ChemDB::SecondaryStructure::TYPE _ssType;
	};
} // namespace VTX::App::Component::Chemistry
#endif
