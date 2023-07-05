#ifndef __VTX_APP_COMPONENT_CHEMISTRY_ATOM__
#define __VTX_APP_COMPONENT_CHEMISTRY_ATOM__

#include "_fwd.hpp"
#include <core/chemdb/atom.hpp>
#include <core/struct/atom.hpp>

namespace VTX::App::Component::Chemistry
{
	namespace ChemDB = VTX::Core::ChemDB;

	class Atom : public Core::Struct::Atom
	{
	  public:
		Atom();
		Atom( Residue * const p_residue );
		Atom( Molecule * const p_molecule );
		Atom( Molecule * const p_molecule, const size_t p_index );

		const Molecule * const getMoleculePtr() const;
		Molecule * const	   getMoleculePtr();

		void updateData();

		ChemDB::Atom::TYPE getType() const { return _type; };
		void			   setType( const ChemDB::Atom::TYPE p_type ) { _type = p_type; };

	  private:
		ChemDB::Atom::TYPE _type = ChemDB::Atom::TYPE::NORMAL;
	};

} // namespace VTX::App::Component::Chemistry
#endif
