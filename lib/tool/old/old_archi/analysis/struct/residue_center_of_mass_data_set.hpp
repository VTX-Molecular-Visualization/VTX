#ifndef __VTX_ANALYSIS_STRUCT_RESIDUE_CENTER_OF_MASS_DATA_SET__
#define __VTX_ANALYSIS_STRUCT_RESIDUE_CENTER_OF_MASS_DATA_SET__

#include "residue_data_set.hpp"
#include <app/old/component/chemistry/_fwd.hpp>
#include <util/types.hpp>

namespace VTX::Analysis::Struct
{
	class ResidueCenterOfMassDataSet : public ResidueDataSet<Vec3f>
	{
	  public:
		ResidueCenterOfMassDataSet() : ResidueDataSet<Vec3f>() {}
		Vec3f getPositionInMolecule( const App::Old::Component::Chemistry::Residue & p_residue );

	  protected:
		Vec3f generateResidueData( const App::Old::Component::Chemistry::Residue & p_residue ) const override;
	};
} // namespace VTX::Analysis::Struct

#endif
