#ifndef __VTX_APP_ACTION_LIBRARY__
#define __VTX_APP_ACTION_LIBRARY__

#include "app/core/action/base_action.hpp"
#include <app/core/library/library_system.hpp>

namespace VTX::App::Action::Library
{
	/**
	 * @brief Base class for library actions.
	 */
	template<typename T>
	class BaseActionPreset : public App::Core::Action::BaseAction
	{
	  protected:
		Core::Library::Library<T> * const _library = LIBRARY_SYSTEM().getLibrary<T>();
	};

	template<typename T>
	class RenamePreset final : public BaseActionPreset<T>
	{
	};

	template<typename T>
	class AddPreset final : public BaseActionPreset<T>
	{
	  public:
		AddPreset( const std::optional<std::string_view> p_name = std::nullopt ) : _name( p_name ) {}

		void execute() override { this->_library->createItem( _name ); }

	  private:
		const std::optional<std::string_view> _name;
	};

	template<typename T>
	class DuplicatePreset final : public BaseActionPreset<T>
	{
	};

	template<typename T>
	class DeletePreset final : public BaseActionPreset<T>
	{
		// TODO: check if used before deleting.
	};

	/*
	template<typename T>
	class SavePreset final : public BaseActionPreset<T>
	{
	};
	*/

} // namespace VTX::App::Action::Library
#endif
