#ifndef __VTX_APP_COMPONENT_CHEMISTRY_BOND__
#define __VTX_APP_COMPONENT_CHEMISTRY_BOND__

#include "_fwd.hpp"
#include "app/core/model/base_model.hpp"
#include "app/internal/chemdb/bond.hpp"
#include "app/old_app/id.hpp"
#include <string>
#include <util/types.hpp>

namespace VTX::App::Component::Chemistry
{
	namespace ChemDB = VTX::App::Internal::ChemDB;

	class Bond : public App::Core::Model::BaseModel
	{
		VTX_MODEL

	  public:
		static bool comparer( const Bond & p_lhs, const Bond & p_rhs );

		inline uint getIndexFirstAtom() const { return _indexFirstAtom; }
		inline void setIndexFirstAtom( const uint p_index ) { _indexFirstAtom = p_index; }
		inline uint getIndexSecondAtom() const { return _indexSecondAtom; }
		inline void setIndexSecondAtom( const uint p_index ) { _indexSecondAtom = p_index; }

		inline ChemDB::Bond::ORDER getOrder() const { return _order; }
		inline void				   setOrder( const ChemDB::Bond::ORDER p_order ) { _order = p_order; }

		inline Molecule * const getMoleculePtr() const { return _moleculePtr; }
		inline void				setMoleculePtr( Molecule * const p_molecule ) { _moleculePtr = p_molecule; }

	  private:
		uint				_indexFirstAtom	 = 0;
		uint				_indexSecondAtom = 0;
		Molecule *			_moleculePtr	 = nullptr;
		ChemDB::Bond::ORDER _order			 = ChemDB::Bond::ORDER::UNKNOWN;

		Bond() : BaseModel( VTX::ID::Model::MODEL_BOND ) {};
	};

} // namespace VTX::App::Component::Chemistry
#endif
