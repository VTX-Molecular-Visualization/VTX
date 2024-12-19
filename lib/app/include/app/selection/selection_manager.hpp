#ifndef __VTX_APP_SELECTION_SELECTION_MANAGER__
#define __VTX_APP_SELECTION_SELECTION_MANAGER__

#include "app/selection/selection.hpp"
#include <map>
#include <memory>
#include <string>

namespace VTX::App::Selection
{
	class SelectionManager
	{
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

		mutable std::map<std::string, std::unique_ptr<Selection>> _savedSelectionMap;
	};

} // namespace VTX::App::Selection

namespace VTX::App
{
	inline Selection::SelectionManager & SELECTION_MANAGER()
	{
		return Util::Singleton<Selection::SelectionManager>::get();
	}

	inline Selection::Selection & CURRENT_SELECTION() { return SELECTION_MANAGER().getCurrent(); }
} // namespace VTX::App
#endif
