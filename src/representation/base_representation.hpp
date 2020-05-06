#ifndef __VTX_BASE_REPRESENTATION__
#define __VTX_BASE_REPRESENTATION__

#ifdef _MSC_VER
#pragma once
#endif

#include "generic/base_collectionable.hpp"
#include "generic/has_collection.hpp"
#include "view/base_view_3d_molecule.hpp"

namespace VTX
{
	namespace Representation
	{
		class BaseRepresentation :
			public Generic::HasCollection<View::BaseView3DMolecule>, // Reprensation is a collection of views.
			public View::BaseView3DMolecule							 // And is also a view.
		{
		  public:
			BaseRepresentation()		  = default;
			virtual ~BaseRepresentation() = default;
		};
	} // namespace Representation
} // namespace VTX
#endif
