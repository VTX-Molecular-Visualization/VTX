#ifndef __VTX_BASE_VIEW_3D_MOLECULE__
#define __VTX_BASE_VIEW_3D_MOLECULE__

#include "app/old_app/model/molecule.hpp"
#include "app/old_app/model/representation/representation.hpp"
#include "app/old_app/representation/representation_target.hpp"
#include "app/view/base_view_3d.hpp"
#include <map>

namespace VTX
{
	namespace View
	{
		class BaseView3DMolecule : public BaseView3D<Model::Molecule>
		{
			VTX_VIEW

		  protected:
			explicit BaseView3DMolecule( Model::Molecule * const p_model ) : BaseView3D( p_model ) {}
			virtual ~BaseView3DMolecule() {}
		};
	} // namespace View
} // namespace VTX
#endif
