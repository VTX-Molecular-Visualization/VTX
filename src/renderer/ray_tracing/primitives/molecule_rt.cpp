#include "molecule_rt.hpp"
#include "../materials/flat_color_material.hpp"
#include "../materials/matte.hpp"
#include "../materials/phong_material.hpp"
#include "cylinder.hpp"
#include "setting.hpp"
#include "sphere.hpp"

namespace VTX
{
	namespace Renderer
	{
		MoleculeRT::MoleculeRT( const Model::Molecule * p_molecule )
		{
			const View::MOLECULE_REPRESENTATION rep = Setting::Rendering::representation;

			const uint nbAtoms = p_molecule->getAtomCount();
			const uint nbBonds = p_molecule->getBondCount();

			std::vector<Renderer::BasePrimitive *> primitives;
			if ( rep == View::MOLECULE_REPRESENTATION::BALL_AND_STICK || rep == View::MOLECULE_REPRESENTATION::STICK )
			{ primitives.resize( nbAtoms + nbBonds ); }
			else
			{
				primitives.resize( nbAtoms );
			}

			uint idPrimitive = 0;
			uint cptAtoms	 = 0;
			uint cptBonds	 = 0;

			_materials.emplace_back( new MatteMaterial( VEC3F_XYZ ) );
			//_materials.emplace_back( new PhongMaterial( Vec3f( 0.2f, 0.f, 0.f ), Vec3f( 0.8f, 0.f, 0.f ) ) );
			//_materials.emplace_back( new DiffuseMaterial( Vec3f( 0.8f, 0.f, 0.f ) ) );

			std::map<Model::Chain *, Vec3f> mapColors;
			const std::vector<Vec3f>		predefColors = {
				   Vec3f( 0.145f, 0.886f, 0.906f ), // bleu clair
				   Vec3f( 1.f, 0.247f, 0.4f ),		// rouge clair
				   Vec3f( 0.969f, 0.772f, 0.172f )	// jaune ocre
			};
			uint idColor = 0;

			const std::vector<Vec3f> & atomPositions = p_molecule->getAtomPositionFrame( p_molecule->getFrame() );
			const Mat4f &			   transform	 = p_molecule->getTransform().get();
			std::vector<Vec3f>		   tAtomPositions;
			tAtomPositions.resize( nbAtoms );
			for ( uint i = 0; i < nbAtoms; ++i )
			{
				const Vec4f tPos	= transform * Vec4f( atomPositions[ i ], 1.f );
				tAtomPositions[ i ] = Vec3f( tPos.x, tPos.y, tPos.z );
			}

			float radius = rep == View::MOLECULE_REPRESENTATION::BALL_AND_STICK ? 0.4f : 0.15f;

			for ( uint i = 0; i < nbAtoms; ++i )
			{
				Model::Chain * chainPtr = p_molecule->getAtom( i ).getChainPtr();

				if ( mapColors.find( chainPtr ) == mapColors.end() )
				{
					if ( idColor < uint( predefColors.size() ) )
					{
						mapColors[ chainPtr ] = predefColors[ idColor++ ];
						if ( idColor < 1 ) idColor++;
					}
					else
					{
						mapColors[ chainPtr ] = chainPtr->getColor();
					}
				}
				const Vec3f & color		  = mapColors[ chainPtr ];
				primitives[ idPrimitive ] = new Renderer::Sphere(
					tAtomPositions[ i ],
					rep == View::MOLECULE_REPRESENTATION::VAN_DER_WAALS ? p_molecule->getAtomRadius( i ) : radius,
					new MatteMaterial( color, 0.3f ) );
				// new PhongMaterial( 0.4f * color, 0.6f * color ) );
				// new FlatColorMaterial( color ) );
				idPrimitive++;
			}

			if ( rep == View::MOLECULE_REPRESENTATION::BALL_AND_STICK || rep == View::MOLECULE_REPRESENTATION::STICK )
			{
				for ( uint i = 0; i < nbBonds; ++i )
				{
					const Model::Bond & bond = p_molecule->getBond( i );
					const Vec3f &		a1	 = tAtomPositions[ bond.getIndexFirstAtom() ];
					const Vec3f &		a2	 = tAtomPositions[ bond.getIndexSecondAtom() ];

					primitives[ idPrimitive ] = new Renderer::Cylinder( a1, a2, 0.15f, _materials.front() );
					idPrimitive++;
				}
			}

			// TODO: we don't have bond per residu... :-(

			// Add atoms and bonds of each residue
			// for ( uint i = 0; i < p_molecule->getResidueCount(); ++i )
			//{
			//	// TODO: remove material duplication + only allow chain colors
			//	const Model::Residue & r = p_molecule->getResidue( i );
			//	_materials.emplace_back( new DiffuseMaterial( r.getChainPtr()->getColor() ) );

			//	const uint idFirstAtomRes = r.getIdFirstAtom();
			//	const uint nbAtomsRes	  = r.getAtomCount();
			//	const uint idFirstBondRes = r.getIdFirstBond();
			//	const uint nbBondsRes	  = r.getBondCount();

			//	for ( uint j = idFirstAtomRes; j < idFirstAtomRes + nbAtomsRes; ++j )
			//	{
			//		primitives[ idPrimitive ]
			//			= new Renderer::Sphere( positions[ j ], p_molecule->getAtomRadius( j ), _materials.back() );
			//		idPrimitive++;
			//		cptAtoms++;
			//	}
			//	for ( uint j = idFirstBondRes; j < idFirstBondRes + nbBondsRes * 2; j += 2 )
			//	{
			//		const Vec3f & a1 = positions[ p_molecule->getBond( j ) ];
			//		const Vec3f & a2 = positions[ p_molecule->getBond( j + 1 ) ];

			//		primitives[ idPrimitive ] = new Renderer::Cylinder( a1, a2, 0.2f, _materials.front() );
			//		idPrimitive++;
			//		cptBonds++;
			//	}
			//}

			//// Add bonds out of residues
			// for ( ; cptBonds < nbBonds; cptBonds++ )
			//{
			//	const Vec3f & a1 = positions[ p_molecule->getBond( cptBonds * 2 ) ];
			//	const Vec3f & a2 = positions[ p_molecule->getBond( cptBonds * 2 + 1 ) ];

			//	primitives[ idPrimitive ] = new Renderer::Cylinder( a1, a2, 0.2f, _materials.front() );
			//	idPrimitive++;
			//}

			// TODO: add options for splitMethod and maxPrimsLeaf
			const uint maxPrimsLeaf = 8;
			_bvh.build( primitives, maxPrimsLeaf, BVH::SplitMethod::SAH );
			const Math::AABB & aabb		= _bvh.getAABB();
			const Vec3f		   centroid = aabb.centroid();
			std::cout << "centroid " << centroid.x << " - " << centroid.y << " - " << centroid.z << std::endl;
			std::cout << "min " << aabb._min.x << " - " << aabb._min.y << " - " << aabb._min.z << std::endl;
			std::cout << "max " << aabb._max.x << " - " << aabb._max.y << " - " << aabb._max.z << std::endl;
		}
	} // namespace Renderer
} // namespace VTX
