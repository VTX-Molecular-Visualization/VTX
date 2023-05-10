#ifndef __VTX_CORE_STRUCT_MOLECULE__
#define __VTX_CORE_STRUCT_MOLECULE__

#include "_fwd.hpp"
#include "core/chemdb/unknown_residue_data.hpp"
#include <map>
#include <string>
#include <unordered_set>
#include <util/color/rgba.hpp>
#include <util/types.hpp>
#include <utility>
#include <vector>

namespace VTX::Core::Struct
{
	class Molecule
	{
	  public:
		using AtomPositionsFrame = std::vector<Vec3f>;

	  public:
		Molecule();
		virtual ~Molecule();

		// Models.
		inline const std::string & getName() const { return _name; }
		inline void				   setName( const std::string & p_name ) { _name = p_name; }

		Chain &								addChain();
		inline Chain * const				getChain( const uint p_idx ) { return _chains[ p_idx ]; }
		inline const Chain * const			getChain( const uint p_idx ) const { return _chains[ p_idx ]; }
		Chain *								getFirstChain();
		const Chain * const					getFirstChain() const;
		const Chain * const					getPreviousChain( const uint p_idBaseChain ) const;
		Chain * const						getPreviousChain( const uint p_idBaseChain );
		const Chain * const					getNextChain( const uint p_idBaseChain ) const;
		Chain * const						getNextChain( const uint p_idBaseChain );
		inline std::vector<Chain *> &		getChains() { return _chains; }
		inline const std::vector<Chain *> & getChains() const { return _chains; }
		uint								getRealChainCount() const { return _realChainCount; };
		void removeChain( const uint p_id, const bool p_delete = true, const bool p_recursive = true );

		Residue &							  addResidue();
		inline Residue * const				  getResidue( const uint p_idx ) { return _residues[ p_idx ]; }
		inline const Residue * const		  getResidue( const uint p_idx ) const { return _residues[ p_idx ]; }
		const Residue * const				  getPreviousResidue( const uint p_idBaseResidue ) const;
		Residue * const						  getPreviousResidue( const uint p_idBaseResidue );
		const Residue * const				  getNextResidue( const uint p_idBaseResidue ) const;
		Residue * const						  getNextResidue( const uint p_idBaseResidue );
		inline std::vector<Residue *> &		  getResidues() { return _residues; }
		inline const std::vector<Residue *> & getResidues() const { return _residues; }
		int									  getRealResidueCount() const;
		void								  removeResidue( const uint p_id,
															 const bool p_delete			= true,
															 const bool p_recursive			= true,
															 const bool p_checkParentUpdate = true );

		Atom &							   addAtom();
		inline Atom * const				   getAtom( const uint p_idx ) { return _atoms[ p_idx ]; }
		inline const Atom * const		   getAtom( const uint p_idx ) const { return _atoms[ p_idx ]; }
		inline std::vector<Atom *> &	   getAtoms() { return _atoms; }
		inline const std::vector<Atom *> & getAtoms() const { return _atoms; }
		int								   getRealAtomCount() const;
		void							   removeAtom( const uint p_id,
													   const bool p_delete			  = true,
													   const bool p_checkBonds		  = true,
													   const bool p_checkParentUpdate = true );

		Bond &							   addBond();
		inline Bond * const				   getBond( const uint p_idx ) { return _bonds[ p_idx ]; }
		inline const Bond * const		   getBond( const uint p_idx ) const { return _bonds[ p_idx ]; }
		inline std::vector<Bond *> &	   getBonds() { return _bonds; }
		inline const std::vector<Bond *> & getBonds() const { return _bonds; }
		void							   removeBond( const uint p_id, const bool p_delete = true );

		bool isEmpty();

		inline const std::vector<ChemDB::UnknownResidueData *> & getUnknownResidueSymbols() const
		{
			return _unknownResidueSymbol;
		}
		inline const std::unordered_set<std::string> & getUnknownAtomSymbols() const { return _unknownAtomSymbol; }

		int								   getUnknownResidueSymbolIndex( const std::string & p_symbol ) const;
		ChemDB::UnknownResidueData * const getUnknownResidueSymbol( const uint p_symbolIndex ) const;
		ChemDB::UnknownResidueData * const getUnknownResidueSymbol( const std::string & p_symbol ) const;
		int								   addUnknownResidueSymbol( ChemDB::UnknownResidueData * const p_data );
		inline void addUnknownAtomSymbol( const std::string & p_symbol ) { _unknownAtomSymbol.emplace( p_symbol ); }

		inline AtomPositionsFrame & addAtomPositionFrame()
		{
			_atomPositionsFrames.emplace_back();
			return _atomPositionsFrames.back();
		}
		inline void addAtomPositionFrame( const AtomPositionsFrame & p_frame )
		{
			_atomPositionsFrames.emplace_back( p_frame );
		}

		inline void setAtomPositionFrames( const std::vector<AtomPositionsFrame> & p_frame )
		{
			_atomPositionsFrames.clear();
			_atomPositionsFrames = p_frame;
		}

		inline const AtomPositionsFrame & getAtomPositionFrame( const uint p_frame ) const
		{
			return _atomPositionsFrames[ p_frame ];
		}
		inline AtomPositionsFrame & getAtomPositionFrame( const uint p_frame )
		{
			return _atomPositionsFrames[ p_frame ];
		}
		inline const std::vector<AtomPositionsFrame> & getAtomPositionFrames() const { return _atomPositionsFrames; }
		inline std::vector<AtomPositionsFrame> &	   getAtomPositionFrames() { return _atomPositionsFrames; }

		inline const uint getChainCount() const { return uint( _chains.size() ); }
		inline const uint getResidueCount() const { return uint( _residues.size() ); }
		inline const uint getAtomCount() const { return uint( _atoms.size() ); }
		inline const uint getBondCount() const { return uint( _bonds.size() ); }

		inline bool									   hasTrajectory() { return _atomPositionsFrames.size() >= 2; }
		inline std::vector<AtomPositionsFrame> &	   getFrames() { return _atomPositionsFrames; }
		inline const std::vector<AtomPositionsFrame> & getFrames() const { return _atomPositionsFrames; }
		inline const uint getFrameCount() const { return uint( _atomPositionsFrames.size() ); }

		// At least one residue
		inline bool hasTopology() const { return getResidueCount() > 1; }
		inline bool hasDynamic() const { return getFrameCount() > 1; }

		void print() const;

		bool mergeTopology( const Molecule & );

	  protected:
		void _addChain( Chain * const p_chain );

	  private:
		// Models.
		std::string						_name						= "unknown";
		std::vector<Chain *>			_chains						= std::vector<Chain *>();
		std::vector<Residue *>			_residues					= std::vector<Residue *>();
		std::vector<Atom *>				_atoms						= std::vector<Atom *>();
		std::vector<Bond *>				_bonds						= std::vector<Bond *>();
		std::vector<AtomPositionsFrame> _atomPositionsFrames		= std::vector<AtomPositionsFrame>();
		uint							_indexFirstBondExtraResidue = 0;

		uint _realChainCount = 0;

		// Missing symbols.
		std::vector<ChemDB::UnknownResidueData *> _unknownResidueSymbol = std::vector<ChemDB::UnknownResidueData *>();
		std::unordered_set<std::string>			  _unknownAtomSymbol	= std::unordered_set<std::string>();

#ifdef _DEBUG
	  public:
		// Validation.
		uint chainCount	  = 0;
		uint residueCount = 0;
		uint atomCount	  = 0;
		uint bondCount	  = 0;
#endif
	};
} // namespace VTX::Core::Struct

#endif
