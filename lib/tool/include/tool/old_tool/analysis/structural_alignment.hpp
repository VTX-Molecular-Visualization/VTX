#ifndef __VTX_ANALYSIS_STRUCTURAL_ALIGNMENT__
#define __VTX_ANALYSIS_STRUCTURAL_ALIGNMENT__

#include "matrix.hpp"
#include "struct/residue_center_of_mass_data_set.hpp"
#include <app/old/component/chemistry/_fwd.hpp>
#include <util/constants.hpp>
#include <util/types.hpp>
#include <utility>
#include <vector>

namespace VTX::Analysis
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
			AlignmentResult( const App::Old::Component::Chemistry::Molecule * const p_staticMolecule,
							 const App::Old::Component::Chemistry::Molecule * const p_mobileMolecule );

			const App::Old::Component::Chemistry::Molecule * const staticMolecule = nullptr;
			const App::Old::Component::Chemistry::Molecule * const mobileMolecule = nullptr;
			size_t											  alignedResidueCount;
			float											  alignedResiduesRMSD  = -1.f;
			Mat4f											  transformationMatrix = MAT4F_ID;
		};

		class AlignmentMethod
		{
		  public:
			AlignmentMethod();
			~AlignmentMethod() = default;

			virtual AlignmentResult compute( const App::Old::Component::Chemistry::Molecule & p_staticMolecule,
											 App::Old::Component::Chemistry::Molecule &		 p_mobilesMolecules,
											 const AlignmentParameters &				 p_parameters )
				= 0;
		};

	  public:
		static AlignmentParameters * instantiateDefaultParameters( const AlignmentMethodEnum & p_methodEnum );
		static void computeAlignment( const App::Old::Component::Chemistry::Molecule * const			 p_staticMolecule,
									  const std::vector<App::Old::Component::Chemistry::Molecule *> & p_mobilesMolecules,
									  const AlignmentParameters &								 p_parameters );
	};

} // namespace VTX::Analysis

#endif
