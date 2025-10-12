#ifndef __VTX_APP_ACTION_LIBRARY__
#define __VTX_APP_ACTION_LIBRARY__

#include "app/action/base_action.hpp"
#include "app/library/library_manager.hpp"
#include "app/services.hpp"

namespace VTX::App::Action::Library
{
	/**
	 * @brief Base class for library actions.
	 */
	template<typename T>
	class BaseActionLibrary : public BaseAction
	{
	  public:
		virtual ~BaseActionLibrary() = default;

	  protected:
		App::Library::BaseLibrary<T> * const _library = LIBRARY().getLibrary<T>();
	};

	/**
	 * @brief Base class for preset actions that can retrieve a preset by its name.
	 */
	template<typename T>
	class BaseActionPreset : public BaseActionLibrary<T>
	{
	  public:
		BaseActionPreset( T * const p_preset ) : _preset( p_preset ) {}
		BaseActionPreset( const std::string_view p_name ) :
			_preset( BaseActionLibrary<T>::_library->getPreset( p_name ) )
		{
		}
		virtual ~BaseActionPreset() = default;

	  protected:
		T * const _preset;
	};

	template<typename T>
	class RenamePreset final : public BaseActionLibrary<T>
	{
	  public:
		RenamePreset( const std::string_view p_src, const std::string_view p_dest ) : _src( p_src ), _dest( p_dest ) {}

		void execute() override { this->_library->renamePreset( _src, _dest ); }

	  private:
		const std::string _src;
		const std::string _dest;
	};

	template<typename T>
	class AddPreset final : public BaseActionLibrary<T>
	{
	  public:
		AddPreset( const std::optional<std::string_view> p_name = std::nullopt ) : _name( p_name ) {}

		void execute() override { this->_library->createPreset( _name ); }

	  private:
		const std::optional<std::string> _name;
	};

	template<typename T>
	class DuplicatePreset final : public BaseActionLibrary<T>
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
	class DeletePreset final : public BaseActionLibrary<T>
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
	class SavePreset final : public BaseActionLibrary<T>
	{
	};
	*/

} // namespace VTX::App::Action::Library
#endif
