#ifndef __VTX_APP_MODEL_CHEMISTRY_ATOM__
#define __VTX_APP_MODEL_CHEMISTRY_ATOM__

#include "_fwd.hpp"
#include "define.hpp"
#include <core/chemdb/atom.hpp>
#include <core/new_struct/atom.hpp>

namespace VTX::App::Model::Chemistry
{
	namespace ChemDB = VTX::Core::ChemDB;

	class Atom : public AtomCore
	{
	  public:
		Atom();
		Atom( Residue * const p_residue );
		Atom( Molecule * const p_molecule );
		Atom( Molecule * const p_molecule, const size_t p_index );

		void updateData();

		ChemDB::Atom::TYPE getType() const { return _type; };
		void			   setType( const ChemDB::Atom::TYPE p_type ) { _type = p_type; };

	  private:
		ChemDB::Atom::TYPE _type = ChemDB::Atom::TYPE::NORMAL;
	};

} // namespace VTX::App::Model::Chemistry
#endif
