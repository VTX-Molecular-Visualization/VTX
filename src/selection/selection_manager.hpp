#ifndef __VTX_SELECTION_MANAGER__
#define __VTX_SELECTION_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "selection/base_selectable.hpp"
#include <unordered_set>

namespace VTX
{
	namespace Selection
	{
		class SelectionManager
		{
		  public:
			using SetSelectablePtr = std::unordered_set<Generic::BaseSelectable *>;

			void select( Generic::BaseSelectable * const );
			void unselect( Generic::BaseSelectable * const );

		  private:
			SetSelectablePtr _selected = SetSelectablePtr();
		};
	} // namespace Selection
} // namespace VTX
#endif
