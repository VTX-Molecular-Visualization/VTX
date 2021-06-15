#ifndef __VTX_MODEL_RESIDUE__
#define __VTX_MODEL_RESIDUE__

#include "atom.hpp"
#include "base_model.hpp"
#include "define.hpp"
#include "generic/base_colorable.hpp"
#include "generic/base_representable.hpp"
#include "generic/base_visible.hpp"
#include "id.hpp"
#include "math/aabb.hpp"
#include "model/secondary_structure.hpp"
#include <map>

namespace VTX
{
	namespace Model
	{
		class Molecule;
		class Chain;
		class Residue :
			public BaseModel,
			public Generic::BaseColorable,
			public Generic::BaseVisible,
			public Generic::BaseRepresentable
		{
			VTX_MODEL

		  public:
			enum class TYPE : int
			{
				STANDARD,
				NON_STANDARD
			};

			enum class SYMBOL : int
			{
				UNKNOWN,
				ALA,
				ARG,
				ASN,
				ASP,
				CYS,
				GLN,
				GLU,
				GLY,
				HIS,
				ILE,
				LEU,
				LYS,
				MET,
				PHE,
				PRO,
				SER,
				THR,
				TRP,
				TYR,
				VAL,
				ASX,
				GLX,
				WAT,
				HOH,
				HEM,
				PO4,
				MEL,
				COUNT
			};

			// Static const mapping.
			static const std::string SYMBOL_STR[ (int)SYMBOL::COUNT ];
			static const std::string SYMBOL_SHORT_STR[ (int)SYMBOL::COUNT ];
			static const std::string SYMBOL_NAME[ (int)SYMBOL::COUNT ];
			static const Color::Rgb	 SYMBOL_COLOR[ (int)SYMBOL::COUNT ];

			inline TYPE			 getType() const { return _type; }
			inline void			 setType( const TYPE p_type ) { _type = p_type; }
			inline uint			 getIndex() const { return _index; };
			inline void			 setIndex( const uint p_index ) { _index = p_index; };
			inline int			 getIndexInOriginalChain() const { return _indexInOriginalChain; };
			inline void			 setIndexInOriginalChain( const int p_index ) { _indexInOriginalChain = p_index; };
			Molecule * const	 getMoleculePtr() const;
			inline Chain * const getChainPtr() const { return _chainPtr; }
			void				 setChainPtr( Chain * const p_chain );

			inline const SYMBOL		   getSymbol() const { return _symbol; };
			inline const std::string & getSymbolStr() const { return SYMBOL_STR[ (int)_symbol ]; }
			inline void				   setSymbol( const SYMBOL p_type )
			{
				_symbol = p_type;
				BaseModel::setDefaultName( &getSymbolName() );
			};
			inline const std::string & getSymbolName() const { return SYMBOL_NAME[ (int)_symbol ]; }
			inline const std::string & getSymbolShort() const { return SYMBOL_SHORT_STR[ (int)_symbol ]; }

			inline uint		  getIndexFirstAtom() const { return _indexFirstAtom; };
			inline void		  setIndexFirstAtom( const uint p_id ) { _indexFirstAtom = p_id; };
			inline uint		  getAtomCount() const { return _atomCount; };
			void			  setAtomCount( const uint p_count );
			inline uint		  getIndexFirstBond() const { return _indexFirstBond; };
			inline void		  setIndexFirstBond( const uint p_id ) { _indexFirstBond = p_id; };
			inline uint		  getBondCount() const { return _bondCount; };
			inline void		  setBondCount( const uint p_count ) { _bondCount = p_count; };
			inline uint		  getRealAtomCount() const { return _realAtomCount; };
			void			  removeToAtoms( const uint p_atomIndex );
			inline char		  getInsertionCode() const { return _insertionCode; }
			inline void		  setInsertionCode( char p_insertionCode ) { _insertionCode = p_insertionCode; }
			inline bool		  hasInsertionCode() const { return _insertionCode != ' '; }
			inline Atom::TYPE getAtomType() const { return _atomType; }
			inline void		  setAtomType( const Atom::TYPE p_atomType ) { _atomType = p_atomType; }
			inline const SecondaryStructure::TYPE getSecondaryStructure() const { return _secondaryStructure; };
			inline void							  setSecondaryStructure( const SecondaryStructure::TYPE p_structure )
			{
				_secondaryStructure = p_structure;
			};
			const Atom * const findFirstAtomByName( const std::string & ) const;

			// Mask BaseVisible::setVisible
			void setVisible( const bool p_visible );

			const Math::AABB getAABB() const;
			const Math::AABB getWorldAABB() const;

		  protected:
			void _onRepresentationChange() override;

		  private:
			TYPE	_type				  = TYPE::STANDARD;
			uint	_index				  = 0;
			int		_indexInOriginalChain = INT_MIN;
			Chain * _chainPtr			  = nullptr;

			SYMBOL _symbol = SYMBOL::UNKNOWN;

			uint					 _indexFirstAtom	 = 0;
			uint					 _atomCount			 = 0;
			uint					 _realAtomCount		 = 0;
			uint					 _indexFirstBond	 = 0;
			uint					 _bondCount			 = 0;
			Atom::TYPE				 _atomType			 = Atom::TYPE::NORMAL; // Set to solvent/ion only if full of it.
			SecondaryStructure::TYPE _secondaryStructure = SecondaryStructure::TYPE::COIL;
			char					 _insertionCode		 = ' ';

			Residue() : BaseModel( ID::Model::MODEL_RESIDUE ) {}
		};

	} // namespace Model
} // namespace VTX
#endif
