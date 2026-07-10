#include "app/python_binding/topology/collection.hpp"
#include "app/python_binding/topology/binding.hpp"

namespace VTX::App::PythonBinding::Topology
{
	void bindCollections( pybind11::module_ & p_module )
	{
		bindCollection<Chain>( p_module, "ChainCollection", SystemItem::CHAIN );
		bindCollection<Residue>( p_module, "ResidueCollection", SystemItem::RESIDUE );
		bindCollection<Atom>( p_module, "AtomCollection", SystemItem::ATOM );
		bindCollection<Bond>( p_module, "BondCollection" );
		bindCollection<Category>( p_module, "CategoryCollection", SystemItem::CATEGORY );
	}
} // namespace VTX::App::PythonBinding::Topology
