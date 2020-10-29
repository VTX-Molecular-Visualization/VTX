#ifndef __VTX_SELECTION_MANAGER__
#define __VTX_SELECTION_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/atom.hpp"
#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include "model/selection.hpp"
#include <unordered_set>

namespace VTX
{
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
