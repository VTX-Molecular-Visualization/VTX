#ifndef __VTX_APP_PYTHON_BINDING_PRESET_HELPERS__
#define __VTX_APP_PYTHON_BINDING_PRESET_HELPERS__

#include "app/generic/name.hpp"
#include "app/python_binding/preset/types.hpp"
#include <pybind11/pybind11.h>

namespace VTX::App::PythonBinding::Preset
{
	template<typename T>
	void validate( const Handle<T> & p_preset )
	{
		if ( not REG().valid( p_preset.entity ) || not REG().all_of<App::Generic::Name, T>( p_preset.entity ) )
		{
			throw pybind11::value_error( "Invalid preset." );
		}
	}

	template<typename T>
	const T & getData( const Handle<T> & p_preset )
	{
		validate( p_preset );
		return REG().get<T>( p_preset.entity );
	}
}

#endif
