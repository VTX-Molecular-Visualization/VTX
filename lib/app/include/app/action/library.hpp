#ifndef __VTX_APP_ACTION_LIBRARY__
#define __VTX_APP_ACTION_LIBRARY__

#include "app/library/library_manager.hpp"
#include "app/services.hpp"

namespace VTX::App::Action::Library
{
	/**
	 * @brief Rename a preset in a library.
	 */
	template<typename T>
	struct RenamePreset
	{
		void execute( const std::string_view p_src, const std::string_view p_dest )
		{
			LIBRARY().getLibrary<T>().renamePreset( p_src, p_dest );
		}
	};

	/**
	 * @brief Add a new preset to a library.
	 */
	template<typename T>
	struct AddPreset
	{
	  public:
		void execute( const std::optional<std::string_view> p_name = std::nullopt )
		{
			LIBRARY().getLibrary<T>().createPreset( p_name );
		}
	};

	/**
	 * @brief Duplicate a preset in a library.
	 */
	template<typename T>
	struct DuplicatePreset
	{
		void execute( const std::string_view p_src, const std::optional<std::string_view> p_dest = std::nullopt )
		{
			LIBRARY().getLibrary<T>().copyPreset( p_src, p_dest );
		}
	};

	/**
	 * @brief Delete a preset from a library.
	 */
	template<typename T>
	struct DeletePreset
	{
		void execute( const std::string_view p_name )
		{
			// TODO: check if used.
			LIBRARY().getLibrary<T>().removePreset( p_name );
		}
	};

} // namespace VTX::App::Action::Library
#endif
