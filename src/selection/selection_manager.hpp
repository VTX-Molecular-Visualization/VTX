#ifndef __VTX_SELECTION_MANAGER__
#define __VTX_SELECTION_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/selection.hpp"
#include "selection/base_selectable.hpp"
#include <unordered_set>

namespace VTX
{
	namespace Selection
	{
		class SelectionManager final
		{
		  public:
			using SetSelectablePtr = std::unordered_set<BaseSelectable *>;

			inline static SelectionManager & get()
			{
				static SelectionManager instance;
				return instance;
			}

			void select( BaseSelectable * const );
			void unselect( BaseSelectable * const );
			void clear();

			inline const SetSelectablePtr & getSelection() const { return _selected; };

		  private:
			SetSelectablePtr   _selected = SetSelectablePtr();
			Model::Selection * _selectionModel;

			SelectionManager();
			SelectionManager( const SelectionManager & ) = delete;
			SelectionManager & operator=( const SelectionManager & ) = delete;
			~SelectionManager();
		};
	} // namespace Selection
} // namespace VTX
#endif
