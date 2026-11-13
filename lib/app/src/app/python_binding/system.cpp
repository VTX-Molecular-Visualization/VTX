#include "app/python_binding/system.hpp"
#include "app/python_binding/topology/types.hpp"
#include <core/struct/topology.hpp>
#include <pybind11/stl.h>
#include <vector>

namespace VTX::App::PythonBinding
{
	namespace
	{
		std::vector<Topology::System> _getSystems()
		{
			std::vector<Topology::System> systems;
			for ( const Entity entity : REG().view<Core::Struct::Topology>() )
			{
				systems.push_back( Topology::System { entity } );
			}
			return systems;
		}
	} // namespace

	void SystemBinder::bind( Module & p_vtxModule )
	{ p_vtxModule.def( "getSystems", synchronized( &_getSystems ), "Return all loaded systems." ); }
} // namespace VTX::App::PythonBinding
