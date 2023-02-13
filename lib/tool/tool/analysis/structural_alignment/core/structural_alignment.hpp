#ifndef __VTX_TOOL_ANALYSIS_STRUCTURAL_ALIGNMENT__
#define __VTX_TOOL_ANALYSIS_STRUCTURAL_ALIGNMENT__

#include "struct/residue_center_of_mass_data_set.hpp"
#include <Eigen/Geometry>
#include <old/math/matrix.hpp>
#include <util/constants.hpp>
#include <util/types.hpp>
#include <utility>
#include <vector>

namespace VTX::Model
{
	class Molecule;
}

namespace VTX::Tool::Analysis::StructuralAlignment::Core
{
	class StructuralAlignment
	{
	  public:
		enum class AlignmentMethodEnum
		{
			CEAlign,

			COUNT,
			Unknown,
		};
		inline static const std::vector<std::string> ALIGNMENT_METHOD_STRING = { "CE Align" };

		class AlignmentParameters
		{
		  public:
			AlignmentParameters();
			AlignmentParameters( const AlignmentMethodEnum & p_method );
			virtual ~AlignmentParameters() = default;

			const AlignmentMethodEnum method	 = AlignmentMethodEnum::Unknown;
			int						  windowSize = 8;
			int						  gapMax	 = 30;
			int						  maxPath	 = 20;

			bool considerWater		   = false;
			bool considerHiddenResidue = true;
		};

		class AlignmentResult
		{
		  public:
			AlignmentResult( const Model::Molecule * const p_staticMolecule,
							 const Model::Molecule * const p_mobileMolecule );

			const Model::Molecule * const staticMolecule = nullptr;
			const Model::Molecule * const mobileMolecule = nullptr;
			float						  alignedResidueCount;
			float						  alignedResiduesRMSD  = -1.f;
			Mat4f						  transformationMatrix = MAT4F_ID;
		};

		class AlignmentMethod
		{
		  public:
			AlignmentMethod();
			~AlignmentMethod() = default;

			virtual AlignmentResult compute( const Model::Molecule &	 p_staticMolecule,
											 Model::Molecule &			 p_mobilesMolecules,
											 const AlignmentParameters & p_parameters )
				= 0;
		};

	  public:
		static AlignmentParameters * instantiateDefaultParameters( const AlignmentMethodEnum & p_methodEnum );
		static void					 computeAlignment( const Model::Molecule * const		  p_staticMolecule,
													   const std::vector<Model::Molecule *> & p_mobilesMolecules,
													   const AlignmentParameters &			  p_parameters );
	};

} // namespace VTX::Tool::Analysis::StructuralAlignment::Core

#endif
