#ifndef __VTX_MODEL_RESIDUE__
#define __VTX_MODEL_RESIDUE__

#include "atom.hpp"
#include "base_model.hpp"
#include <util/types.hpp>
#include "app/old_app/generic/base_colorable.hpp"
#include "app/old_app/generic/base_representable.hpp"
#include "app/old_app/generic/base_visible.hpp"
#include "app/old_app/id.hpp"
#include "app/old_app/model/secondary_structure.hpp"
#include "app/old_app/object3d/helper/aabb.hpp"
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
				SEC,
				PYL,
				ASX,
				GLX,
				C,
				G,
				A,
				U,
				I,
				DC,
				DG,
				DA,
				DU,
				DT,
				DI,
				WAT,
				HOH,
				COUNT
			};

			static bool IsStandard( const std::string & p_residueName );

			static const int SYMBOL_COUNT = int( SYMBOL::COUNT );

			// Static const mapping.
			static const std::string SYMBOL_STR[ (int)SYMBOL::COUNT ];
			static const std::string SYMBOL_SHORT_STR[ (int)SYMBOL::COUNT ];
			static const std::string SYMBOL_NAME[ (int)SYMBOL::COUNT ];
			static const Color::Rgba SYMBOL_COLOR[ (int)SYMBOL::COUNT ];

			static const Color::Rgba getResidueColor( const Model::Residue & p_residue );

			inline bool isStandardResidue() const
			{
				return int( SYMBOL::ALA ) <= _symbol && _symbol <= int( SYMBOL::PYL );
			}
			inline bool isWater() const
			{
				return ( _symbol == int( SYMBOL::WAT ) ) || ( _symbol == int( SYMBOL::HOH ) );
			}

			inline TYPE			 getType() const { return _type; }
			inline void			 setType( const TYPE p_type ) { _type = p_type; }
			inline uint			 getIndex() const { return _index; };
			inline void			 setIndex( const uint p_index ) { _index = p_index; };
			inline int			 getIndexInOriginalChain() const { return _indexInOriginalChain; };
			inline void			 setIndexInOriginalChain( const int p_index ) { _indexInOriginalChain = p_index; };
			Molecule * const	 getMoleculePtr() const;
			inline Chain * const getChainPtr() const { return _chainPtr; }
			void				 setChainPtr( Chain * const p_chain );

			inline const SYMBOL getSymbol() const { return SYMBOL( _symbol ); };
			const std::string & getSymbolStr() const;
			void				setSymbol( const SYMBOL & p_symbol );
			void				setSymbol( const int p_symbolValue );
			const std::string & getSymbolName() const;
			const std::string & getSymbolShort() const;

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
			TYPE	_type				  = TYPE::STANDARD;
			uint	_index				  = 0;
			int		_indexInOriginalChain = INT_MIN;
			Chain * _chainPtr			  = nullptr;

			int _symbol = int( SYMBOL::UNKNOWN );

			uint					 _indexFirstAtom	 = 0;
			uint					 _atomCount			 = 0;
			uint					 _realAtomCount		 = 0;
			uint					 _indexFirstBond	 = 0;
			uint					 _bondCount			 = 0;
			Atom::TYPE				 _atomType			 = Atom::TYPE::NORMAL; // Set to solvent/ion only if full of it.
			SecondaryStructure::TYPE _secondaryStructure = SecondaryStructure::TYPE::COIL;
			char					 _insertionCode		 = ' ';

			Residue() : BaseModel( VTX::ID::Model::MODEL_RESIDUE ) {}
		};

	} // namespace Model
} // namespace VTX
#endif
