#ifndef __VTX_APP_OLD_APPLICATION_SELECTION_SELECTION_MANAGER__
#define __VTX_APP_OLD_APPLICATION_SELECTION_SELECTION_MANAGER__

#include "app/old/application/selection/_fwd.hpp"
#include <unordered_set>

namespace VTX::App::Old::Application::Selection
{
	class SelectionManager final
	{
	  public:
		inline static SelectionManager & get()
		{
			static SelectionManager instance;
			return instance;
		}

		inline const SelectionModel & getSelectionModel() const { return *_selectionModel; }
		inline SelectionModel &		  getSelectionModel() { return *_selectionModel; }
		void						  deleteModel();

		// For further feature: multiselecction.
		// void addSelectionObject();
		// void removeSelectionObject();

	  private:
		SelectionModel * _selectionModel;

		SelectionManager();
		SelectionManager( const SelectionManager & )			 = delete;
		SelectionManager & operator=( const SelectionManager & ) = delete;
		~SelectionManager();
	};
} // namespace VTX::App::Old::Application::Selection
#endif
