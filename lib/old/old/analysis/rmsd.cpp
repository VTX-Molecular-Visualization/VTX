#include "rmsd.hpp"
#include "event/event.hpp"
#include "event/event_manager.hpp"
#include "model/molecule.hpp"
#include "model/selection.hpp"
#include "util/analysis.hpp"
#include <cmath>

namespace VTX::Analysis
{
	void RMSD::callRMSDComputation( const Model::Molecule * const p_firstMolecule,
									const Model::Molecule * const p_secondMolecule,
									const bool					  p_considerTransform )
	{
		const double rmsd = computeRMSD( *p_firstMolecule, *p_secondMolecule, p_considerTransform );

		RMSDData data = RMSDData();

		data.setFirstMolecule( p_firstMolecule );
		data.setSecondMolecule( p_secondMolecule );
		data.setRMSD( rmsd );

		VTX_EVENT( new Event::VTXEventRef<const RMSDData>( Event::Global::RMSD_COMPUTED, data ) );
	}

	void RMSD::callRMSDComputation( const Model::Selection & p_selection, const bool p_considerTransform )
	{
		const Model::Selection::MapMoleculeIds & selectedMolecules = p_selection.getMoleculesMap();

		const Model::Molecule *				 targetMolecule = nullptr;
		std::vector<const Model::Molecule *> otherMolecules = std::vector<const Model::Molecule *>();
		otherMolecules.reserve( p_selection.getMoleculeSelectedCount() - 1 );

		Util::Analysis::pickTargetAndComparersFromSelection( p_selection, targetMolecule, otherMolecules );

		std::vector<Vec3f> targetAtomPositions = std::vector<Vec3f>();
		Util::Analysis::getAtomPositions( p_selection, targetMolecule, targetAtomPositions );

		const Mat4f targetTransform = p_considerTransform ? targetMolecule->getTransform().get() : MAT4F_ID;

		for ( const Model::Molecule * otherMolecule : otherMolecules )
		{
			std::vector<Vec3f> otherAtomPositions = std::vector<Vec3f>();
			Util::Analysis::getAtomPositions( p_selection, otherMolecule, otherAtomPositions );

			const Mat4f otherTransform = p_considerTransform ? otherMolecule->getTransform().get() : MAT4F_ID;

			const double rmsd = computeRMSD( targetAtomPositions, otherAtomPositions, targetTransform, otherTransform );

			RMSDData data = RMSDData();

			data.setFirstMolecule( targetMolecule );
			data.setSecondMolecule( otherMolecule );
			data.setRMSD( rmsd );
			const uint atomCount = uint(targetAtomPositions.size() < otherAtomPositions.size()
				? targetAtomPositions.size() : otherAtomPositions.size());
			data.setAtomCount( atomCount );

			VTX_EVENT( new Event::VTXEventRef<const RMSDData>( Event::Global::RMSD_COMPUTED, data ) );
		}
	}

	double RMSD::computeRMSD( const std::vector<Vec3f> & p_vectorPositionsA,
							  const std::vector<Vec3f> & p_vectorPositionsB,
							  const Mat4f &				 p_transformA,
							  const Mat4f &				 p_transformB )
	{
		const size_t minAtomLength = p_vectorPositionsA.size() < p_vectorPositionsB.size() ? p_vectorPositionsA.size()
																						   : p_vectorPositionsB.size();

		double rmsd = 0;

		for ( uint i = 0; i < minAtomLength; i++ )
		{
			const Vec4f point1Vec4f = p_transformA * Vec4f( p_vectorPositionsA[ i ], 1 );
			const Vec4f point2Vec4f = p_transformB * Vec4f( p_vectorPositionsB[ i ], 1 );

			const Vec3f point1Vec3f = { point1Vec4f.x, point1Vec4f.y, point1Vec4f.z };
			const Vec3f point2Vec3f = { point2Vec4f.x, point2Vec4f.y, point2Vec4f.z };

			rmsd += Util::distance( point1Vec3f, point2Vec3f ) / minAtomLength;
		}

		rmsd = sqrt( rmsd );

		return rmsd;
	}

	double RMSD::computeRMSD( const Model::Molecule & p_firstMolecule,
							  const Model::Molecule & p_secondMolecule,
							  const bool			  p_considerTransform )
	{
		const size_t minAtomLength = p_firstMolecule.getAtomCount() < p_secondMolecule.getAtomCount()
										 ? p_firstMolecule.getAtomCount()
										 : p_secondMolecule.getAtomCount();

		double rmsd = 0;

		const Model::Molecule::AtomPositionsFrame & frame1
			= p_firstMolecule.getAtomPositionFrame( p_firstMolecule.getFrame() );
		const Model::Molecule::AtomPositionsFrame & frame2
			= p_secondMolecule.getAtomPositionFrame( p_secondMolecule.getFrame() );

		if ( p_considerTransform )
		{
			const Mat4f & transform1 = p_firstMolecule.getTransform().get();
			const Mat4f & transform2 = p_secondMolecule.getTransform().get();

			size_t atomCount = minAtomLength;

			for ( uint i = 0; i < minAtomLength; i++ )
			{
				if ( p_firstMolecule.getAtom( i ) == nullptr || p_secondMolecule.getAtom( i ) == nullptr )
				{
					atomCount--;
					continue;
				}

				const Vec4f point1Vec4f = transform1 * Vec4f( frame1[ i ], 1 );
				const Vec4f point2Vec4f = transform2 * Vec4f( frame2[ i ], 1 );

				const Vec3f point1Vec3f = { point1Vec4f.x, point1Vec4f.y, point1Vec4f.z };
				const Vec3f point2Vec3f = { point2Vec4f.x, point2Vec4f.y, point2Vec4f.z };

				rmsd += Util::distance( point1Vec3f, point2Vec3f ) / atomCount;
			}
		}
		else
		{
			size_t atomCount = minAtomLength;

			for ( uint i = 0; i < minAtomLength; i++ )
			{
				if ( p_firstMolecule.getAtom( i ) == nullptr || p_secondMolecule.getAtom( i ) == nullptr )
				{
					atomCount--;
					continue;
				}

				rmsd += Util::distance( frame1[ i ], frame2[ i ] ) / atomCount;
			}
		}

		rmsd = sqrt( rmsd );

		return rmsd;
	}

} // namespace VTX::Analysis
