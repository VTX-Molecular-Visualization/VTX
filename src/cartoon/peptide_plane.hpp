#ifndef __PEPTIDE_PLANE__
#define __PEPTIDE_PLANE__

#include "define.hpp"
#include "model/atom.hpp"
#include "model/residue.hpp"
#include "util/math.hpp"

// TODO: move in Object3D but without model ref.
// Pass directly atom positions in constructor.
namespace VTX
{
	namespace Cartoon
	{
		class PeptidePlane
		{
		  public:
			PeptidePlane( const Model::Residue & p_residue1,
						  const Model::Residue & p_residue2,
						  const Model::Residue & p_residue3 ) :
				_residue1( p_residue1 ),
				_residue2( p_residue2 ), _residue3( p_residue3 )
			{
				const Model::Atom * const CA1 = p_residue1.findFirstAtomByName( "CA" );
				const Model::Atom * const O1  = p_residue1.findFirstAtomByName( "O" );
				const Model::Atom * const CA2 = p_residue1.findFirstAtomByName( "CA" );

				// What to do?
				if ( CA1 == nullptr || O1 == nullptr || CA2 == nullptr )
				{
					VTX_ERROR( "Failed to get atoms for peptide plane" );
					return;
				}

				const Model::Molecule * const molecule	  = p_residue1.getMoleculePtr();
				Vec3f						  positionCA1 = molecule->getAtomPositionFrame( 0 )[ CA1->getIndex() ];
				Vec3f						  positionO1  = molecule->getAtomPositionFrame( 0 )[ O1->getIndex() ];
				Vec3f						  positionCA2 = molecule->getAtomPositionFrame( 0 )[ CA2->getIndex() ];

				_position = ( positionCA1 + positionCA2 ) / 2.0f;
				_forward  = Util::Math::normalize( positionCA2 - positionCA1 );
				_normal	  = Util::Math::normalize(
					  Util::Math::cross( _forward, Util::Math::normalize( positionO1 - positionCA1 ) ) );
				_side	 = Util::Math::normalize( Util::Math::cross( _normal, _forward ) );
				_isValid = true;
			}

			const Vec3f getPostion() const { return _position; }
			void		setPostion( const Vec3f & p_position ) { _position = p_position; }
			const Vec3f getSide() const { return _side; }
			void		setSide( const Vec3f & p_side ) { _side = p_side; }
			const bool	isFlipped() const { return _isFlipped; }
			const bool	isValid() const { return _isValid; }

			void flip()
			{
				_side *= -1.f;
				_normal *= -1.f;
				_isFlipped = !_isFlipped;
			}

		  private:
			const Model::Residue & _residue1;
			const Model::Residue & _residue2;
			const Model::Residue & _residue3;

			Vec3f _position;
			Vec3f _normal;
			Vec3f _forward;
			Vec3f _side;
			bool  _isFlipped = false;
			bool  _isValid	 = false;
		};

	} // namespace Cartoon
} // namespace VTX

#endif
