#ifndef VTX_APP_PYTHON_BINDING_PRESET_HELPERS_HPP
#define VTX_APP_PYTHON_BINDING_PRESET_HELPERS_HPP

#include "app/generic/name.hpp"
#include "app/python_binding/module.hpp"
#include "app/python_binding/preset/types.hpp"
#include <pybind11/pybind11.h>
#include <utility>

namespace VTX::App::PythonBinding::Preset
{
	/**
	 * @brief Verify existence.
	 */
	template<typename T>
	void validate( const Handle<T> & p_preset )
	{
		synchronizeCopy(
			[ &p_preset ]()
			{
				if ( not REG().valid( p_preset.entity ) || not REG().all_of<App::Generic::Name, T>( p_preset.entity ) )
				{
					throw pybind11::value_error( "Invalid preset." );
				}
			}
		);
	}

	/**
	 * @brief Execute action.
	 */
	template<typename Action, typename T, typename... Args>
	void executePresetAction( const Handle<T> & p_preset, Args &&... p_args )
	{
		THREAD().synchronize(
			[ & ]()
			{
				validate( p_preset );
				Action action;
				Util::ActionManager::run( action, Entity( p_preset.entity ), std::forward<Args>( p_args )... );
			}
		);
	}

	/**
	 * @brief Get a preset copy.
	 */
	template<typename T>
	T getData( const Handle<T> & p_preset )
	{
		return synchronizeCopy(
			[ &p_preset ]()
			{
				validate( p_preset );
				return REG().get<T>( p_preset.entity );
			}
		);
	}
} // namespace VTX::App::PythonBinding::Preset

#endif
