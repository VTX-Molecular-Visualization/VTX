#include "app/old_app/renderer/rt/primitives/molecule_rt.hpp"
#include "app/old_app/model/atom.hpp"
#include "app/old_app/model/bond.hpp"
#include "app/old_app/model/chain.hpp"
#include "app/old_app/model/representation/representation_enum.hpp"
#include "app/old_app/renderer/rt/materials/flat_color_material.hpp"
#include "app/old_app/renderer/rt/materials/matte.hpp"
#include "app/old_app/renderer/rt/materials/metal.hpp"
#include "app/old_app/renderer/rt/materials/phong_material.hpp"
#include "app/old_app/renderer/rt/primitives/cylinder.hpp"
#include "app/old_app/renderer/rt/primitives/sphere.hpp"

namespace VTX
{
	namespace Renderer
	{
		MoleculeRT::MoleculeRT( const Model::Molecule * p_molecule )
		{
			const Generic::REPRESENTATION rep = //
												// Generic::REPRESENTATION::SAS;
				Generic::REPRESENTATION::VAN_DER_WAALS;
			// Generic::REPRESENTATION::BALL_AND_STICK;
			// Generic::REPRESENTATION::STICK;
			// Setting::Rendering::representation;

			const uint nbAtoms = p_molecule->getAtomCount();
			const uint nbBonds = p_molecule->getBondCount();

			// show only what is visible
			// we assume that solvent and ions don't have bonds... chilled :-)
			std::vector<Renderer::BasePrimitive *> primitives;
			if ( rep == Generic::REPRESENTATION::BALL_AND_STICK || rep == Generic::REPRESENTATION::STICK )
			{
				primitives.reserve( nbAtoms + nbBonds );
			}
			else
			{
				primitives.reserve( nbAtoms );
			}

			// std::map<Model::Chain *, Vec3f> mapColors;
			// const std::vector<Vec3f>		predefColors = {
			//	   Vec3f( 0.145f, 0.886f, 0.906f ), // bleu clair
			//	   Vec3f( 1.f, 0.247f, 0.4f ),		// rouge clair
			//	   Vec3f( 0.969f, 0.772f, 0.172f )	// jaune ocre
			//};
			// =====================================
			// for arte video
			// =====================================
			// prot rouge
			// const std::vector<BaseMaterial *> predefMtls = {
			//	new PhongMaterial( 0.4f * jaune,
			//					   0.6f * jaune,
			//					   shininess ),									   // jaune ocre
			//	new MatteMaterial( rouge, roughness ), // rouge clair
			//	new MatteMaterial( blanc, 0.5f ),	   // blanc qui p�te
			//	new MatteMaterial( rouge, roughness ), // rouge clair
			//	new MatteMaterial( rouge, roughness )  // rouge clair
			//};
			// =====================================

			std::map<Model::Chain *, BaseMaterial *> mapMtls;

			float roughness = 0.3f;
			float shininess = 32.f;

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

			float radius = rep == Generic::REPRESENTATION::BALL_AND_STICK ? 0.4f : 0.25f;

			for ( uint i = 0; i < nbAtoms; ++i )
			{
				if ( p_molecule->isAtomVisible( i ) )
				{
					Model::Chain * chainPtr = p_molecule->getAtom( i )->getChainPtr();

					if ( mapMtls.find( chainPtr ) == mapMtls.end() )
					{
						_materials.emplace_back( //
							new MatteMaterial( chainPtr->getColor(), 0.3f ) );
						// new PhongMaterial( chainPtr->getColor() * 0.7f, chainPtr->getColor() * 0.3f, 64.f ) );
						mapMtls[ chainPtr ] = _materials.back();
					}

					primitives.emplace_back( new Renderer::Sphere(
						tAtomPositions[ i ],
						rep == Generic::REPRESENTATION::VAN_DER_WAALS ? p_molecule->getAtomRadius( i )
						: rep == Generic::REPRESENTATION::SAS		  ? p_molecule->getAtomRadius( i ) + 1.4f
																	  : radius,
						mapMtls[ chainPtr ] ) );
				}
			}

			if ( rep == Generic::REPRESENTATION::BALL_AND_STICK || rep == Generic::REPRESENTATION::STICK )
			{
				for ( uint i = 0; i < nbBonds; ++i )
				{
					const Model::Bond & bond = *p_molecule->getBond( i );
					const Vec3f &		a1	 = tAtomPositions[ bond.getIndexFirstAtom() ];
					const Vec3f &		a2	 = tAtomPositions[ bond.getIndexSecondAtom() ];

					primitives.emplace_back( new Renderer::Cylinder(
						a1, a2, 0.25f, mapMtls[ p_molecule->getAtom( bond.getIndexFirstAtom() )->getChainPtr() ] ) );
				}
			}

			primitives.shrink_to_fit();

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
			const Object3D::Helper::AABB & aabb		= _bvh.getAABB();
			const Vec3f					   centroid = aabb.centroid();
			std::cout << "centroid " << centroid.x << " - " << centroid.y << " - " << centroid.z << std::endl;
			std::cout << "min " << aabb.getMin().x << " - " << aabb.getMin().y << " - " << aabb.getMin().z << std::endl;
			std::cout << "max " << aabb.getMax().x << " - " << aabb.getMax().y << " - " << aabb.getMax().z << std::endl;
		}
	} // namespace Renderer
} // namespace VTX
