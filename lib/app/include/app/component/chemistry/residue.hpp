#ifndef __VTX_APP_COMPONENT_CHEMISTRY_RESIDUE__
#define __VTX_APP_COMPONENT_CHEMISTRY_RESIDUE__

#include "_fwd.hpp"
#include "app/component/chemistry/index_types.hpp"
#include "app/component/chemistry/iterator/atom.hpp"
#include "app/core/ecs/base_component.hpp"
#include <core/chemdb/atom.hpp>
#include <core/chemdb/residue.hpp>
#include <core/chemdb/secondary_structure.hpp>
#include <util/types.hpp>

namespace VTX::App::Component::Chemistry
{
	namespace ChemDB = VTX::Core::ChemDB;

	class Residue : public Core::ECS::BaseComponent
	{
	  public:
		Residue() = default;
		Residue( Molecule * const p_molecule, const size_t p_index ) : _moleculePtr( p_molecule ), _index( p_index ) {}

		const Chain * const getConstChainPtr() const;
		Chain * const		getChainPtr() const;
		void				setChainPtr( Chain * const p_chainPtr );

		const Molecule * const getConstMoleculePtr() const { return _moleculePtr; }
		Molecule * const	   getMoleculePtr() const { return _moleculePtr; }

		size_t getIndex() const { return _index; }
		void   setIndex( const size_t p_index ) { _index = p_index; }

		atom_index_t			getIndexFirstAtom() const;
		void					setIndexFirstAtom( const atom_index_t p_indexFirstAtom );
		atom_index_t			getIndexLastAtom() const;
		atom_index_t			getAtomCount() const;
		void					setAtomCount( const atom_index_t p_atomCount );
		size_t					getIndexFirstBond() const;
		void					setIndexFirstBond( const size_t p_indexFirstBond );
		size_t					getBondCount() const;
		void					setBondCount( const size_t p_bondCount );
		ChemDB::Residue::SYMBOL getSymbol() const;
		void					setSymbol( const ChemDB::Residue::SYMBOL p_symbol );
		size_t					getIndexInOriginalChain() const;
		void					setIndexInOriginalChain( const size_t p_index );

		const std::string & getShortName() const;
		const std::string & getName() const;
		const std::string & getLongName() const;

		const Util::Color::Rgba & getColor() const { return _color; };
		void					  setColor( const Util::Color::Rgba p_color ) { _color = p_color; };

		ChemDB::Residue::TYPE getType() { return _type; };
		void				  setType( const ChemDB::Residue::TYPE p_type ) { _type = p_type; };

		ChemDB::Atom::TYPE getAtomType() const;
		void			   setAtomType( const ChemDB::Atom::TYPE p_type );

		const char getInsertionCode() const { return _insertionCode; }
		void	   setInsertionCode( const char p_insertionCode ) { _insertionCode = p_insertionCode; }

		ChemDB::SecondaryStructure::TYPE getSecondaryStructure() const { return _ssType; }
		void setSecondaryStructure( const ChemDB::SecondaryStructure::TYPE p_ssType ) { _ssType = p_ssType; }

		AtomIndexRange			getAtomRange() const;
		Iterator::AtomContainer atoms() const;

		bool isVisible() const;
		bool isFullyVisible() const;

		void setVisible( const bool p_visible );

		void remove();

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
