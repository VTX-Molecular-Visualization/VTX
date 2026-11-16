#ifndef VTX_APP_PYTHON_BINDING_IDENTITY_HPP
#define VTX_APP_PYTHON_BINDING_IDENTITY_HPP

#include "app/ecs.hpp"
#include <cstddef>
#include <functional>
#include <pybind11/pybind11.h>
#include <type_traits>
#include <utility>

namespace VTX::App::PythonBinding
{
	namespace Detail
	{
		template<typename T>
		auto identity( const T & p_object )
		{
			if constexpr ( requires { p_object.index; } )
			{
				return std::pair { p_object.entity, p_object.index };
			}
			else
			{
				return p_object.entity;
			}
		}

		template<typename T>
		std::size_t identityHash( const T & p_object )
		{
			const std::size_t entityHash = std::hash<Entity> {}( p_object.entity );
			if constexpr ( requires { p_object.index; } )
			{
				const std::size_t indexHash
					= std::hash<std::remove_cvref_t<decltype( p_object.index )>> {}( p_object.index );
				return entityHash ^ ( indexHash + 0x9e3779b9 + ( entityHash << 6 ) + ( entityHash >> 2 ) );
			}
			else
			{
				return entityHash;
			}
		}
	} // namespace Detail

	template<typename T>
	pybind11::class_<T> bindIdentity( pybind11::class_<T> p_class )
	{
		p_class
			.def(
				"__eq__",
				[]( const T & p_left, const T & p_right )
				{ return Detail::identity( p_left ) == Detail::identity( p_right ); },
				pybind11::is_operator()
			)
			.def( "__hash__", []( const T & p_object ) { return Detail::identityHash( p_object ); } );
		return p_class;
	}
} // namespace VTX::App::PythonBinding

#endif
