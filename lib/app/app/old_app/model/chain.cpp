#include "chain.hpp"
#include "old_app/model/category.hpp"
#include "molecule.hpp"
#include "old_app/representation/representation_manager.hpp"
#include "residue.hpp"

namespace VTX
{
	namespace Model
	{
		const Color::Rgba Chain::CHAIN_ID_COLOR_ATOM[ Chain::NB_COLORS ] = {
			{ 192, 208, 255 }, // A, a,
			{ 176, 255, 176 }, // B, b,
			{ 255, 192, 200 }, // C, c,
			{ 255, 255, 128 }, // D, d,
			{ 255, 192, 255 }, // E, e,
			{ 176, 240, 240 }, // F, f,
			{ 255, 208, 112 }, // G, g,
			{ 240, 128, 128 }, // H, h,
			{ 245, 222, 179 }, // I, i,
			{ 0, 191, 255 },   // J, j,
			{ 205, 92, 92 },   // K, k,
			{ 102, 205, 170 }, // L, l,
			{ 154, 205, 50 },  // M, m,
			{ 238, 130, 238 }, // N, n,
			{ 0, 206, 209 },   // O, o,
			{ 0, 255, 127 },   // P, p, 0,
			{ 60, 179, 113 },  // Q, q, 1,
			{ 0, 0, 139 },	   // R, r, 2,
			{ 189, 183, 107 }, // S, s, 3,
			{ 0, 100, 0 },	   // T, t, 4,
			{ 128, 0, 0 },	   // U, u, 5,
			{ 128, 128, 0 },   // V, v, 6,
			{ 128, 0, 128 },   // W, w, 7,
			{ 0, 128, 128 },   // X, x, 8,
			{ 184, 134, 11 },  // Y, y, 9,
			{ 178, 34, 34 },   // Z, z
		};

		const Color::Rgba Chain::CHAIN_ID_COLOR_HETATM[ Chain::NB_COLORS ] = {
			{ 144, 160, 207 }, // A, a,
			{ 128, 207, 152 }, // B, b,
			{ 207, 144, 176 }, // C, c,
			{ 207, 207, 112 }, // D, d,
			{ 207, 144, 207 }, // E, e,
			{ 128, 192, 192 }, // F, f,
			{ 207, 160, 96 },  // G, g,
			{ 192, 80, 112 },  // H, h,
			{ 197, 174, 131 }, // I, i,
			{ 0, 167, 207 },   // J, j,
			{ 181, 76, 76 },   // K, k,
			{ 86, 181, 146 },  // L, l,
			{ 138, 181, 42 },  // M, m,
			{ 190, 114, 190 }, // N, n,
			{ 0, 182, 161 },   // O, o,
			{ 0, 207, 111 },   // P, p, 0,
			{ 52, 155, 97 },   // Q, q, 1,
			{ 0, 0, 187 },	   // R, r, 2,
			{ 165, 159, 91 },  // S, s, 3,
			{ 0, 148, 0 },	   // T, t, 4,
			{ 176, 0, 0 },	   // U, u, 5,
			{ 176, 176, 0 },   // V, v, 6,
			{ 176, 0, 176 },   // W, w, 7,
			{ 0, 176, 176 },   // X, x, 8,
			{ 232, 182, 19 },  // Y, y, 9,
			{ 194, 50, 50 },   // Z, z
		};

		const Color::Rgba Chain::CHAIN_ID_UNKNOWN_COLOR = Color::Rgba::WHITE;

		void Chain::setMoleculePtr( Molecule * const p_molecule )
		{
			_moleculePtr = p_molecule;

			initBaseRepresentable( this, p_molecule, p_molecule );
		}

		void Chain::setResidueCount( const uint p_count )
		{
			_residueCount	  = p_count;
			_realResidueCount = p_count;
		}
		void Chain::removeToResidues( const uint p_residueIndex )
		{
			if ( _indexFirstResidue == p_residueIndex )
			{
				while ( _residueCount > 0 && getMoleculePtr()->getResidue( _indexFirstResidue ) == nullptr )
				{
					_indexFirstResidue++;
					_residueCount--;
				}
			}
			else
			{
				uint lastResidueIndex = _indexFirstResidue + _residueCount - 1;
				if ( lastResidueIndex == p_residueIndex )
				{
					while ( _residueCount > 0 && getMoleculePtr()->getResidue( lastResidueIndex ) == nullptr )
					{
						_residueCount--;
						lastResidueIndex--;
					}
				}
			}

			_realResidueCount--;
		}

		uint Chain::computeRealAtomCount() const
		{
			uint realAtomCount = 0;

			for ( uint i = getIndexFirstResidue(); i <= getIndexLastResidue(); i++ )
			{
				const Model::Residue * const residue = _moleculePtr->getResidue( i );

				if ( residue == nullptr )
					continue;

				realAtomCount += residue->getRealAtomCount();
			}

			return realAtomCount;
		}

		Color::Rgba Chain::getChainIdColor( const std::string & p_chainId, const bool p_isHetAtm )
		{
			if ( p_chainId.empty() )
				return Chain::CHAIN_ID_UNKNOWN_COLOR;

			// chain id should be defined by one char
			const char c = static_cast<char>( std::toupper( static_cast<unsigned char>( p_chainId[ 0 ] ) ) );

			const int id = int( c ) - 65; // 65 is A
			if ( id < 0 || id > 26 )
				return Chain::CHAIN_ID_UNKNOWN_COLOR;

			return p_isHetAtm ? Chain::CHAIN_ID_COLOR_HETATM[ id ] : Chain::CHAIN_ID_COLOR_ATOM[ id ];
		}

		void Chain::setCategoryEnum( const CATEGORY_ENUM & p_categoryEnum )
		{
			if ( _categoryEnum != p_categoryEnum )
			{
				Model::Molecule * const moleculePtr = getMoleculePtr();
				if ( moleculePtr != nullptr )
				{
					moleculePtr->getCategory( _categoryEnum ).removeChain( _index );
					_categoryEnum = p_categoryEnum;
					moleculePtr->getCategory( _categoryEnum ).addChain( _index );
				}
			}
		}

		void Chain::setVisible( const bool p_visible )
		{
			const bool previousVisibleState = isVisible();

			BaseVisible::setVisible( p_visible );

			if ( previousVisibleState != p_visible )
			{
				_notifyViews( new Event::VTXEventValue<uint>( Event::Model::CHAIN_VISIBILITY, _index ) );
				_moleculePtr->propagateEventToViews(
					new Event::VTXEventValue<uint>( Event::Model::CHAIN_VISIBILITY, _index ) );
			}
		}

		void Chain::setVisible( const bool p_visible, const bool p_notify )
		{
			const bool previousVisibleState = isVisible();

			BaseVisible::setVisible( p_visible );

			if ( previousVisibleState != p_visible )
			{
				if ( p_notify )
				{
					_notifyViews( new Event::VTXEventValue<uint>( Event::Model::CHAIN_VISIBILITY, _index ) );
					_moleculePtr->propagateEventToViews(
						new Event::VTXEventValue<uint>( Event::Model::CHAIN_VISIBILITY, _index ) );
				}
			}
		}

		const Object3D::Helper::AABB Chain::getAABB() const
		{
			Object3D::Helper::AABB aabb = Object3D::Helper::AABB();

			for ( uint i = 0; i < _residueCount; ++i )
			{
				const Residue * const residue = _moleculePtr->getResidue( _indexFirstResidue + i );

				if ( residue == nullptr )
					continue;

				aabb.extend( residue->getAABB() );
			}

			return aabb;
		}
		const Object3D::Helper::AABB Chain::getWorldAABB() const
		{
			const Object3D::Helper::AABB aabb	   = getAABB();
			const Math::Transform &		 transform = getMoleculePtr()->getTransform();

			Object3D::Helper::AABB worldAabb   = Object3D::Helper::AABB();
			std::vector<Vec3f>	   aabbSummits = aabb.getSummits();

			for ( const Vec3f & summit : aabbSummits )
			{
				const Vec4f worldSummit = transform.get() * Vec4f( summit, 1 );
				worldAabb.extend( worldSummit );
			}

			return worldAabb;
		}

		void Chain::removeChildrenRepresentations()
		{
			for ( uint i = _indexFirstResidue; i < _indexFirstResidue + _residueCount; i++ )
			{
				Model::Residue * const residue = getMoleculePtr()->getResidue( i );

				if ( residue == nullptr )
					continue;

				VTX::Representation::RepresentationManager::get().removeInstantiatedRepresentation(
					*residue, false, false );
			}

			_callRepresentationChange();
		}

		void Chain::_onRepresentationChange()
		{
			_notifyViews( new Event::VTXEvent( Event::Model::REPRESENTATION_CHANGE ) );
		}
	} // namespace Model
} // namespace VTX
