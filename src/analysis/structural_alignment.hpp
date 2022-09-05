#ifndef __VTX_ANALYSIS_STRUCTURAL_ALIGNMENT__
#define __VTX_ANALYSIS_STRUCTURAL_ALIGNMENT__

#include "define.hpp"
#include "math/matrix.hpp"
#include "struct/residue_center_of_mass_data_set.hpp"
#include "tmp_external_libraries/Eigen/Geometry"
#include <utility>
#include <vector>

namespace VTX::Model
{
	class Molecule;
}

namespace VTX::Analysis
{
	class StructuralAlignment
	{
	  public:
		enum class AlignmentMethodEnum
		{
			CEAlign,
			CEAlign_Pymol,

			COUNT,
			Unknown,
		};
		inline static const std::vector<std::string> ALIGNMENT_METHOD_STRING = { "CE Align", "CE Align Pymol" };

		class AlignmentParameters
		{
		  public:
			AlignmentParameters();
			AlignmentParameters( const AlignmentMethodEnum & p_method );
			virtual ~AlignmentParameters() = default;

			const AlignmentMethodEnum method	 = AlignmentMethodEnum::Unknown;
			int					windowSize = 8;
			int					gapMax	   = 30;
			int					maxPath	   = 20;
		};

		class AlignmentResult
		{
		  public:
			AlignmentResult();
			float rmsd				   = -1.f;
			Mat4f transformationMatrix = MAT4F_ID;
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
		static void					 computeAlignment( const Model::Molecule &				  p_staticMolecule,
													   const std::vector<Model::Molecule *> & p_mobilesMolecules,
													   const AlignmentParameters &			  p_parameters );
	};

} // namespace VTX::Analysis

#endif
