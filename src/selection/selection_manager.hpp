#ifndef __VTX_SELECTION_MANAGER__
#define __VTX_SELECTION_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include <unordered_set>

namespace VTX
{
	namespace Model
	{
		class Selection;
	}
	namespace Selection
	{
		class SelectionManager final
		{
		  public:
			inline static SelectionManager & get()
			{
				static SelectionManager instance;
				return instance;
			}

			inline const Model::Selection & getSelectionModel() const { return *_selectionModel; }
			inline Model::Selection &		getSelectionModel() { return *_selectionModel; }
			void							deleteModel();

			// For further feature: multiselecction.
			// void addSelectionObject();
			// void removeSelectionObject();

		  private:
			Model::Selection * _selectionModel;

			SelectionManager();
			SelectionManager( const SelectionManager & ) = delete;
			SelectionManager & operator=( const SelectionManager & ) = delete;
			~SelectionManager();
		};
	} // namespace Selection
} // namespace VTX
#endif
