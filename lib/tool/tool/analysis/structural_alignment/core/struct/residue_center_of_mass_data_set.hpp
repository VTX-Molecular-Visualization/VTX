#ifndef __VTX_TOOL_ANALYSIS_STRUCTURAL_ALIGNMENT_CORE_STRUCT_RESIDUE_CENTER_OF_MASS_DATA_SET__
#define __VTX_TOOL_ANALYSIS_STRUCTURAL_ALIGNMENT_CORE_STRUCT_RESIDUE_CENTER_OF_MASS_DATA_SET__

#include "residue_data_set.hpp"
#include <util/types.hpp>

namespace VTX::Model
{
	class Residue;
} // namespace VTX::Model

namespace VTX::Tool::Analysis::StructuralAlignment::Core::Struct
{
	class ResidueCenterOfMassDataSet : public ResidueDataSet<Vec3f>
	{
	  public:
		ResidueCenterOfMassDataSet() : ResidueDataSet<Vec3f>() {}
		Vec3f getPositionInMolecule( const Model::Residue & p_residue );

	  protected:
		Vec3f generateResidueData( const Model::Residue & p_residue ) const override;
	};
} // namespace VTX::Tool::Analysis::StructuralAlignment::Core::Struct

#endif
