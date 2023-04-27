#ifndef __VTX_APP_COMPONENT_CHEMISTRY_RESIDUE__
#define __VTX_APP_COMPONENT_CHEMISTRY_RESIDUE__

#include "_fwd.hpp"
#include "app/component/chemistry/secondary_structure.hpp"
#include "app/core/model/base_model.hpp"
#include "app/internal/chemdb/residue.hpp"
#include "app/internal/chemdb/secondary_structure.hpp"
#include "app/old_app/generic/base_colorable.hpp"
#include "app/application/representation/base_representable.hpp"
#include "app/old_app/generic/base_visible.hpp"
#include "app/old_app/id.hpp"
#include "app/old_app/object3d/helper/aabb.hpp"
#include "atom.hpp"
#include <map>
#include <string>
#include <util/types.hpp>

namespace VTX::App::Component::Chemistry
{
	namespace ChemDB = VTX::App::Internal::ChemDB;

	class Residue :
		public App::Core::Model::BaseModel,
		public Generic::BaseColorable,
		public Generic::BaseVisible,
		public App::Application::Representation::BaseRepresentable
	{
		VTX_MODEL

	  public:
		static const int SYMBOL_COUNT = int( ChemDB::Residue::SYMBOL::COUNT );

		static bool				 checkIfStandardFromName( const std::string & p_residueName );
		static const Color::Rgba getResidueColor( const Chemistry::Residue & p_residue );

		inline bool isStandardResidue() const
		{
			return int( ChemDB::Residue::SYMBOL::ALA ) <= _symbol && _symbol <= int( ChemDB::Residue::SYMBOL::PYL );
		}
		inline bool isWater() const
		{
			return ( _symbol == int( ChemDB::Residue::SYMBOL::WAT ) )
				   || ( _symbol == int( ChemDB::Residue::SYMBOL::HOH ) );
		}

		inline ChemDB::Residue::TYPE getType() const { return _type; }
		inline void					 setType( const ChemDB::Residue::TYPE p_type ) { _type = p_type; }
		inline uint					 getIndex() const { return _index; };
		inline void					 setIndex( const uint p_index ) { _index = p_index; };
		inline int					 getIndexInOriginalChain() const { return _indexInOriginalChain; };
		inline void					 setIndexInOriginalChain( const int p_index ) { _indexInOriginalChain = p_index; };
		Molecule * const			 getMoleculePtr() const;
		inline Chain * const		 getChainPtr() const { return _chainPtr; }
		void						 setChainPtr( Chain * const p_chain );

		inline const ChemDB::Residue::SYMBOL getSymbol() const { return ChemDB::Residue::SYMBOL( _symbol ); };
		const std::string &					 getSymbolStr() const;
		void								 setSymbol( const ChemDB::Residue::SYMBOL & p_symbol );
		void								 setSymbol( const int p_symbolValue );
		const std::string &					 getSymbolName() const;
		const std::string &					 getSymbolShort() const;

		inline uint				  getIndexFirstAtom() const { return _indexFirstAtom; };
		inline void				  setIndexFirstAtom( const uint p_id ) { _indexFirstAtom = p_id; };
		inline uint				  getAtomCount() const { return _atomCount; };
		void					  setAtomCount( const uint p_count );
		inline uint				  getIndexFirstBond() const { return _indexFirstBond; };
		inline void				  setIndexFirstBond( const uint p_id ) { _indexFirstBond = p_id; };
		inline uint				  getBondCount() const { return _bondCount; };
		inline void				  setBondCount( const uint p_count ) { _bondCount = p_count; };
		inline uint				  getRealAtomCount() const { return _realAtomCount; };
		void					  removeToAtoms( const uint p_atomIndex );
		inline char				  getInsertionCode() const { return _insertionCode; }
		inline void				  setInsertionCode( char p_insertionCode ) { _insertionCode = p_insertionCode; }
		inline bool				  hasInsertionCode() const { return _insertionCode != ' '; }
		inline ChemDB::Atom::TYPE getAtomType() const { return _atomType; }
		inline void				  setAtomType( const ChemDB::Atom::TYPE p_atomType ) { _atomType = p_atomType; }
		inline const ChemDB::SecondaryStructure::TYPE getSecondaryStructure() const { return _secondaryStructure; };
		inline void setSecondaryStructure( const ChemDB::SecondaryStructure::TYPE p_structure )
		{
			_secondaryStructure = p_structure;
		};
		const Atom * const findFirstAtomByName( const std::string & ) const;
		const uint		   findBondIndex( const uint p_firstAtomIndex, const uint p_secondAtomIndex ) const;

		const Atom * const getAlphaCarbon() const;

		// Mask BaseVisible::setVisible
		void setVisible( const bool p_visible );
		void setVisible( const bool p_visible, const bool p_notify );

		Object3D::Helper::AABB getAABB() const;
		Object3D::Helper::AABB getWorldAABB() const;

	  protected:
		void _onRepresentationChange() override;

	  private:
		ChemDB::Residue::TYPE _type					= ChemDB::Residue::TYPE::STANDARD;
		uint				  _index				= 0;
		int					  _indexInOriginalChain = INT_MIN;
		Chain *				  _chainPtr				= nullptr;

		int _symbol = int( ChemDB::Residue::SYMBOL::UNKNOWN );

		uint			   _indexFirstAtom = 0;
		uint			   _atomCount	   = 0;
		uint			   _realAtomCount  = 0;
		uint			   _indexFirstBond = 0;
		uint			   _bondCount	   = 0;
		ChemDB::Atom::TYPE _atomType	   = ChemDB::Atom::TYPE::NORMAL; // Set to solvent/ion only if full of it.
		ChemDB::SecondaryStructure::TYPE _secondaryStructure = ChemDB::SecondaryStructure::TYPE::COIL;
		char							 _insertionCode		 = ' ';

		Residue() : BaseModel( VTX::ID::Model::MODEL_RESIDUE ) {}
	};

} // namespace VTX::App::Component::Chemistry
#endif
