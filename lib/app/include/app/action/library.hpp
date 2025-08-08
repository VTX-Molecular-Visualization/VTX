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
	  public:
		RenamePreset( const std::string_view p_src, const std::string_view p_dest ) : _src( p_src ), _dest( p_dest ) {}

		void execute() override { this->_library->renamePreset( _src, _dest ); }

	  private:
		const std::string _src;
		const std::string _dest;
	};

	template<typename T>
	class AddPreset final : public BaseActionPreset<T>
	{
	  public:
		AddPreset( const std::optional<std::string_view> p_name = std::nullopt ) : _name( p_name ) {}

		void execute() override { this->_library->createPreset( _name ); }

	  private:
		const std::optional<std::string> _name;
	};

	template<typename T>
	class DuplicatePreset final : public BaseActionPreset<T>
	{
	  public:
		DuplicatePreset( const std::string_view p_src, const std::optional<std::string_view> p_dest = std::nullopt ) :
			_src( p_src ), _dest( p_dest )
		{
		}

		void execute() override { this->_library->copyPreset( _src, _dest ); }

	  private:
		const std::string				 _src;
		const std::optional<std::string> _dest;
	};

	template<typename T>
	class DeletePreset final : public BaseActionPreset<T>
	{
	  public:
		DeletePreset( const std::string_view p_name ) : _name( p_name ) {}

		void execute() override
		{
			// TODO: check if used.
			this->_library->removePreset( _name );
		}

	  private:
		const std::string _name;
	};

	/*
	template<typename T>
	class SavePreset final : public BaseActionPreset<T>
	{
	};
	*/

} // namespace VTX::App::Action::Library
#endif
