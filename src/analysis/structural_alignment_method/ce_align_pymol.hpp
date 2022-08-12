#ifndef __VTX_ANALYSIS_STRUCTURAL_ALIGNMENT_METHOD_CE_ALIGN_PYMOL__
#define __VTX_ANALYSIS_STRUCTURAL_ALIGNMENT_METHOD_CE_ALIGN_PYMOL__

#include "analysis/struct/residue_center_of_mass_data_set.hpp"
#include "analysis/structural_alignment.hpp"
#include "define.hpp"
#include "tmp_external_libraries/Eigen/Geometry"
#include "tmp_external_libraries/tnt/tnt.h"
#include <vector>

namespace VTX::Model
{
	class Molecule;
}

namespace VTX::Analysis::StructuralAlignmentMethod
{
	class CEAlignPymol : public StructuralAlignment::AlignmentMethod
	{
	  private:
		using TA1 = TNT::Array1D<double>;
		using TA2 = TNT::Array2D<double>;

		using cePoint  = Vec3d;
		using pcePoint = cePoint *;

		/*
		// An AFP (aligned fragment pair), and list/pointer
		*/
		class afp
		{
		  public:
			int first;
			int second;
		};

		using path		= afp *;
		using pathCache = path *;

		class Result
		{
		  public:
			int				 bestLen;
			double			 bestRMSD;
			Mat4f			 u;
			std::vector<int> pathA;
			std::vector<int> pathB;
		};

	  public:
		class CustomParameters : public StructuralAlignment::AlignmentParameters
		{
		  public:
			CustomParameters();

			float D0 = 3.f;
			float D1 = 4.f;
		};

		StructuralAlignment::AlignmentResult compute(
			const Model::Molecule &							 p_staticMolecule,
			Model::Molecule &								 p_mobileMolecule,
			const StructuralAlignment::AlignmentParameters & p_parameters ) override;

	  private:
		static Result ExecutiveCEAlign( /* PyMOLGlobals * G,*/
										const std::vector<Vec3f> & listA,
										const std::vector<Vec3f> & listB,
										int						   lenA,
										int						   lenB,
										float					   d0,
										float					   d1,
										int						   windowSize,
										int						   gapMax );

		static std::vector<Vec3f> _generateResiduePositionsVector( const Model::Molecule & p_molecule );

		/////////////////////////////////////////////////////////////////////////////
		// Function Declarations
		/////////////////////////////////////////////////////////////////////////////
		// Calculates the CE Similarity Matrix
		static double ** calcS( double ** d1, double ** d2, int lenA, int lenB, int wSize );

		// calculates a simple distance matrix
		static double ** calcDM( pcePoint coords, int len );

		// Converter: Python Object -> C Structs
		static pcePoint getCoords( const std::vector<Vec3f> & L, int len );

		// Optimal path finding algorithm (CE).
		static pathCache findPath( double ** S,
								   double ** dA,
								   double ** dB,
								   int		 lenA,
								   int		 lenB,
								   float	 D0,
								   float	 D1,
								   int		 winSize,
								   int		 gapMax,
								   int *	 bufferSize );

		// filter through the results and find the best
		static Result findBest( pcePoint  coordsA,
								pcePoint  coordsB,
								pathCache paths,
								int		  bufferSize,
								int		  smaller,
								int		  winSize );

		// tranpose of a matrix
		static TA2 transpose( const TA2 & v );

		static Eigen::Affine3d _find3DAffineTransform( Eigen::Matrix3Xd in, Eigen::Matrix3Xd out );

		static Struct::ResidueCenterOfMassDataSet _residuePositionsDataSet;
	};

} // namespace VTX::Analysis::StructuralAlignmentMethod

#endif
