#ifndef __VTX_APP_COMPONENT_CHEMISTRY_RESIDUE__
#define __VTX_APP_COMPONENT_CHEMISTRY_RESIDUE__

#include "_fwd.hpp"
#include "app/application/representation/base_representable.hpp"
#include "app/component/generic/base_colorable.hpp"
#include "app/component/generic/base_visible.hpp"
#include "app/component/object3d/helper/aabb.hpp"
#include "app/core/model/base_model.hpp"
#include "app/id.hpp"
#include "atom.hpp"
#include <core/chemdb/residue.hpp>
#include <core/chemdb/secondary_structure.hpp>
#include <core/struct/residue.hpp>
#include <map>
#include <string>
#include <util/constants.hpp>
#include <util/types.hpp>

namespace VTX::App::Component::Chemistry
{
	namespace ChemDB = VTX::Core::ChemDB;

	class Residue :
		public App::Core::Model::BaseModel,
		public Component::Generic::BaseColorable,
		public Component::Generic::BaseVisible,
		public App::Application::Representation::BaseRepresentable
	{
		VTX_MODEL

	  public:
		static const int SYMBOL_COUNT = int( ChemDB::Residue::SYMBOL::COUNT );

		static bool					   checkIfStandardFromName( const std::string & p_residueName );
		static const Util::Color::Rgba getResidueColor( const Chemistry::Residue & p_residue );

	  public:
		inline VTX::Core::Struct::Residue &		  getResidueStruct() { return *_residueStruct; }
		inline const VTX::Core::Struct::Residue & getResidueStruct() const { return *_residueStruct; }

		inline bool isStandardResidue() const { return _residueStruct->isStandardResidue(); }
		inline bool isWater() const { return _residueStruct->isWater(); }

		inline ChemDB::Residue::TYPE getType() const { return _residueStruct->getType(); }
		inline void setType( const ChemDB::Residue::TYPE p_type ) { _residueStruct->setType( p_type ); }
		inline uint getIndex() const { return _residueStruct->getIndex(); };
		inline void setIndex( const uint p_index ) { _residueStruct->setIndex( p_index ); };
		inline int	getIndexInOriginalChain() const { return _residueStruct->getIndexInOriginalChain(); };
		inline void setIndexInOriginalChain( const int p_index )
		{
			_residueStruct->setIndexInOriginalChain( p_index );
		};

		Molecule * const	 getMoleculePtr() const;
		inline Chain * const getChainPtr() const { return _chainPtr; }
		void				 setChainPtr( Chain * const p_chain );

		inline const ChemDB::Residue::SYMBOL getSymbol() const { return _residueStruct->getSymbol(); };
		inline const std::string &			 getSymbolStr() const { return _residueStruct->getSymbolStr(); };
		inline void setSymbol( const ChemDB::Residue::SYMBOL & p_symbol ) { _residueStruct->setSymbol( p_symbol ); };
		inline void setSymbol( const int p_symbolValue ) { _residueStruct->setSymbol( p_symbolValue ); };
		inline const std::string & getSymbolName() const { return _residueStruct->getSymbolName(); };
		inline const std::string & getSymbolShort() const { return _residueStruct->getSymbolShort(); };

		inline uint getIndexFirstAtom() const { return _residueStruct->getIndexFirstAtom(); };
		inline void setIndexFirstAtom( const uint p_id ) { _residueStruct->setIndexFirstAtom( p_id ); };
		inline uint getAtomCount() const { return _residueStruct->getAtomCount(); };
		inline void setAtomCount( const uint p_count ) { _residueStruct->setAtomCount( p_count ); }
		inline uint getIndexFirstBond() const { return _residueStruct->getIndexFirstBond(); };
		inline void setIndexFirstBond( const uint p_id ) { _residueStruct->setIndexFirstBond( p_id ); };
		inline uint getBondCount() const { return _residueStruct->getBondCount(); };
		inline void setBondCount( const uint p_count ) { _residueStruct->setBondCount( p_count ); };
		inline uint getRealAtomCount() const { return _residueStruct->getRealAtomCount(); };
		inline void removeToAtom( const uint p_atomIndex ) { _residueStruct->removeToAtom( p_atomIndex ); }
		inline char getInsertionCode() const { return _residueStruct->getInsertionCode(); }
		inline void setInsertionCode( char p_insertionCode ) { _residueStruct->setInsertionCode( p_insertionCode ); }
		inline bool hasInsertionCode() const { return _residueStruct->hasInsertionCode(); }
		inline ChemDB::Atom::TYPE getAtomType() const { return _residueStruct->getAtomType(); }
		inline void setAtomType( const ChemDB::Atom::TYPE p_atomType ) { _residueStruct->setAtomType( p_atomType ); }
		inline const ChemDB::SecondaryStructure::TYPE getSecondaryStructure() const
		{
			return _residueStruct->getSecondaryStructure();
		};
		inline void setSecondaryStructure( const ChemDB::SecondaryStructure::TYPE p_structure )
		{
			_residueStruct->setSecondaryStructure( p_structure );
		};
		inline const Atom * const findFirstAtomByName( const std::string & p_name ) const
		{
			const VTX::Core::Struct::Atom * const atomStruct = _residueStruct->findFirstAtomByName( p_name );
			return nullptr;
		}
		inline const uint findBondIndex( const uint p_firstAtomIndex, const uint p_secondAtomIndex ) const
		{
			return _residueStruct->findBondIndex( p_firstAtomIndex, p_secondAtomIndex );
		}

		const Atom * const getAlphaCarbon() const
		{
			const VTX::Core::Struct::Atom * const atomStruct = _residueStruct->getAlphaCarbon();
			return nullptr;
		}

		// Mask BaseVisible::setVisible
		void setVisible( const bool p_visible );
		void setVisible( const bool p_visible, const bool p_notify );

		App::Component::Object3D::Helper::AABB getAABB() const;
		App::Component::Object3D::Helper::AABB getWorldAABB() const;

	  protected:
		Residue() : BaseModel( App::ID::Model::MODEL_RESIDUE ) {}
		void _onRepresentationChange() override;

	  private:
		VTX::Core::Struct::Residue * _residueStruct = nullptr;
		Chain *						 _chainPtr		= nullptr;
	};

} // namespace VTX::App::Component::Chemistry
#endif
