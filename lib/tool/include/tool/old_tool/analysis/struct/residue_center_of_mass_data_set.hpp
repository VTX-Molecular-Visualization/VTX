#ifndef __VTX_ANALYSIS_STRUCT_RESIDUE_CENTER_OF_MASS_DATA_SET__
#define __VTX_ANALYSIS_STRUCT_RESIDUE_CENTER_OF_MASS_DATA_SET__

#include <util/types.hpp>
#include "residue_data_set.hpp"

namespace VTX::Model
{
	class Residue;
} // namespace VTX::Model

namespace VTX::Analysis::Struct
{
	class ResidueCenterOfMassDataSet : public ResidueDataSet<Vec3f>
	{
	  public:
		ResidueCenterOfMassDataSet() : ResidueDataSet<Vec3f>() {}
		Vec3f getPositionInMolecule( const Model::Residue & p_residue );

	  protected:
		Vec3f generateResidueData( const Model::Residue & p_residue ) const override;
	};
} // namespace VTX::Analysis::Struct

#endif
