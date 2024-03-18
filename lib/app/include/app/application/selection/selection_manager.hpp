#ifndef __VTX_APP_APPLICATION_SELECTION_SELECTION_MANAGER__
#define __VTX_APP_APPLICATION_SELECTION_SELECTION_MANAGER__

#include "app/application/selection/selection.hpp"
#include "app/application/system/system_registration.hpp"
#include "app/core/system/base_system.hpp"
#include <map>
#include <memory>
#include <string>

namespace VTX::App::Application::Selection
{
	class SelectionManager : public Core::System::BaseSystem
	{
	  public:
		inline static const System::SystemRegistration<SelectionManager> SYSTEM
			= System::SystemRegistration<SelectionManager>();

	  public:
		SelectionManager();
		~SelectionManager();

		Selection &		  getCurrent();
		const Selection & getCurrent() const;

		void save( const std::string & p_name, std::unique_ptr<Selection> p_selection );
		void erase( const std::string & p_name );

		void			  apply( const Selection & p_selection );
		void			  apply( const std::string & p_selectionName );
		bool			  hasSaved( const std::string & p_name ) const;
		const Selection & getSaved( const std::string & p_name ) const;
		Selection &		  getSaved( const std::string & p_name );

	  private:
		void _setCurrentSelection( const Selection & p_selection );
		void _linkCallbackToSelectables();

		std::unique_ptr<Selection> _currentSelection = std::make_unique<Selection>();

		mutable std::map<std::string, std::unique_ptr<Selection>> _savedSelectionMap
			= std::map<std::string, std::unique_ptr<Selection>>();
	};

} // namespace VTX::App::Application::Selection

namespace VTX::App
{
	Application::Selection::SelectionManager & SELECTION_MANAGER();
	Application::Selection::Selection &		   CURRENT_SELECTION();
} // namespace VTX::App
#endif
