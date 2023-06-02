#ifndef __VTX_CORE_STRUCT_ATOM__
#define __VTX_CORE_STRUCT_ATOM__

#include "_fwd.hpp"
#include "core/chemdb/atom.hpp"
#include <string>
#include <util/types.hpp>

namespace VTX::Core::Struct
{
	class Atom
	{
	  public:
		Atom() {}

		inline const uint	   getIndex() const { return _index; };
		inline void			   setIndex( const uint p_index ) { _index = p_index; };
		Molecule * const	   getMoleculePtr() const;
		Chain * const		   getChainPtr() const;
		inline Residue * const getResiduePtr() const { return _residuePtr; }
		inline void			   setResiduePtr( Residue * const p_residue ) { _residuePtr = p_residue; }

		inline const Util::Color::Rgba & getColor() const { return ChemDB::Atom::SYMBOL_COLOR[ int( _symbol ) ]; };

		inline const ChemDB::Atom::SYMBOL getSymbol() const { return _symbol; };
		inline const std::string &		  getSymbolStr() const { return ChemDB::Atom::SYMBOL_STR[ int( _symbol ) ]; };
		inline void						  setSymbol( const ChemDB::Atom::SYMBOL p_symbol ) { _symbol = p_symbol; };
		inline const std::string &		  getSymbolName() const { return ChemDB::Atom::SYMBOL_NAME[ int( _symbol ) ]; }
		inline const uint				  getAtomicNumber() const { return uint( _symbol ); }
		inline const float		   getVdwRadius() const { return ChemDB::Atom::SYMBOL_VDW_RADIUS[ int( _symbol ) ]; }
		inline ChemDB::Atom::TYPE  getType() const { return _type; }
		inline void				   setType( const ChemDB::Atom::TYPE p_type ) { _type = p_type; }
		inline const std::string & getName() const { return _name; };
		inline void				   setName( const std::string & p_name ) { _name = p_name; };

		// const Vec3f & getLocalPosition() const;
		const Vec3f & getLocalPosition( const uint & p_frameIndex ) const;

		const Vec3f getWorldPosition() const;

	  private:
		uint				 _index		 = 0;
		Residue *			 _residuePtr = nullptr;
		ChemDB::Atom::TYPE	 _type		 = ChemDB::Atom::TYPE::NORMAL;
		ChemDB::Atom::SYMBOL _symbol	 = ChemDB::Atom::SYMBOL::UNKNOWN;
		// /!\ Names PDB != MMTF (CA and C1 for alpha carbon).
		std::string _name = "";
	};

} // namespace VTX::Core::Struct
#endif
