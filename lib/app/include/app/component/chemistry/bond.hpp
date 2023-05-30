#ifndef __VTX_APP_COMPONENT_CHEMISTRY_BOND__
#define __VTX_APP_COMPONENT_CHEMISTRY_BOND__

#include "_fwd.hpp"
#include "app/core/model/base_model.hpp"
#include "app/id.hpp"
#include <core/chemdb/bond.hpp>
#include <core/struct/bond.hpp>
#include <string>
#include <util/types.hpp>

namespace VTX::App::Component::Chemistry
{
	namespace ChemDB = VTX::Core::ChemDB;

	class Bond : public App::Core::Model::BaseModel
	{
		VTX_MODEL

	  public:
		static bool comparer( const Bond & p_lhs, const Bond & p_rhs );

		inline VTX::Core::Struct::Bond &	   getBondStruct() { return *_bondStruct; }
		inline const VTX::Core::Struct::Bond & getBondStruct() const { return *_bondStruct; }

		inline uint getIndexFirstAtom() const { return _bondStruct->getIndexFirstAtom(); }
		inline void setIndexFirstAtom( const uint p_index ) { _bondStruct->setIndexFirstAtom( p_index ); }
		inline uint getIndexSecondAtom() const { return _bondStruct->getIndexSecondAtom(); }
		inline void setIndexSecondAtom( const uint p_index ) { _bondStruct->setIndexSecondAtom( p_index ); }

		inline ChemDB::Bond::ORDER getOrder() const { return _bondStruct->getOrder(); }
		inline void				   setOrder( const ChemDB::Bond::ORDER p_order ) { _bondStruct->setOrder( p_order ); }

		inline Molecule * const getMoleculePtr() const { return _moleculePtr; }
		void					setMoleculePtr( Molecule * const p_molecule );

	  protected:
		Bond() : BaseModel( App::ID::Model::MODEL_BOND ) {};

	  private:
		VTX::Core::Struct::Bond * _bondStruct = nullptr;

		// TODO manage access to Model::Molecule from Struct::Molecule with EnTT.
		Molecule * _moleculePtr = nullptr;
	};

} // namespace VTX::App::Component::Chemistry
#endif
