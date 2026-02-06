#ifndef __VTX_PYTHON_BINDING_ENTITY_CASTER__
#define __VTX_PYTHON_BINDING_ENTITY_CASTER__

#include <entt/entt.hpp>
#include <pybind11/pybind11.h>

namespace pybind11::detail
{
	template<>
	struct type_caster<entt::entity>
	{
		PYBIND11_TYPE_CASTER( entt::entity, const_name( "Entity" ) );

		// Python int -> entt::entity
		bool load( handle src, bool )
		{
			PyObject * tmp = PyNumber_Long( src.ptr() );
			if ( !tmp )
				return false;

			value = static_cast<entt::entity>( PyLong_AsUnsignedLong( tmp ) );
			Py_DECREF( tmp );
			return !PyErr_Occurred();
		}

		// entt::entity -> Python int
		static handle cast( entt::entity src, return_value_policy, handle )
		{
			return PyLong_FromUnsignedLong( static_cast<std::uint32_t>( src ) );
		}
	};
} // namespace pybind11::detail

#endif
