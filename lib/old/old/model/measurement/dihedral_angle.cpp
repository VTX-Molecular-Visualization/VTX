#include "dihedral_angle.hpp"
#include "event/event.hpp"
#include "event/event_manager.hpp"
#include "model/atom.hpp"
#include "model/molecule.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/scene.hpp"
#include "util/math.hpp"
#include "util/measurement.hpp"
#include <variant>

namespace VTX::Model::Measurement
{
	DihedralAngle::DihedralAngle() : Model::Label( VTX::ID::Model::MODEL_MEASUREMENT_DIHEDRAL_ANGLE )
	{
		_atoms.resize( 4, nullptr );
		_moleculeViews.resize( 4, nullptr );

		_registerEvent( Event::Global::MOLECULE_REMOVED );
		_registerEvent( Event::Global::ATOM_REMOVED );
		_registerEvent( Event::Global::LABEL_REMOVED );

		setAutoNaming( true, false );
	}

	DihedralAngle::DihedralAngle( const AtomQuadruplet & p_quadruplet ) : DihedralAngle()
	{
		_setAtomsInternal( std::get<0>( p_quadruplet ),
						   std::get<1>( p_quadruplet ),
						   std::get<2>( p_quadruplet ),
						   std::get<3>( p_quadruplet ),
						   false );
	}

	DihedralAngle ::~DihedralAngle() {}

	void DihedralAngle::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::ATOM_REMOVED )
		{
			const Event::VTXEventPtr<Model::Atom> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Atom> &>( p_event );

			if ( _isLinkedToAtom( castedEvent.ptr ) )
			{
				// TODO : Use a manager instead of managing scene from model
				VTXApp::get().getScene().removeLabel( this );
				_invalidate();
				VTXApp::get().deleteAtEndOfFrame( this );
			}
		}
		else if ( p_event.name == Event::Global::MOLECULE_REMOVED )
		{
			const Event::VTXEventPtr<Model::Molecule> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );

			if ( _isLinkedToMolecule( castedEvent.ptr ) )
			{
				// TODO : Use a manager instead of managing scene from model
				VTXApp::get().getScene().removeLabel( this );
				_invalidate();
				VTXApp::get().deleteAtEndOfFrame( this );
			}
		}
		else if ( p_event.name == Event::Global::LABEL_REMOVED )
		{
			const Event::VTXEventPtr<Model::Label> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Label> &>( p_event );

			if ( castedEvent.ptr == this )
				_cleanViews();
		}
	}

	void DihedralAngle::setAtoms( const Model::Atom & p_firstAtom,
								  const Model::Atom & p_secondAtom,
								  const Model::Atom & p_thirdAtom,
								  const Model::Atom & p_fourthAtom )
	{
		_setAtomsInternal( p_firstAtom, p_secondAtom, p_thirdAtom, p_fourthAtom );
	}

	bool DihedralAngle::isValid() const
	{
		for ( const Model::Atom * const atomPtr : _atoms )
			if ( atomPtr == nullptr )
				return false;

		return true;
	}

	void DihedralAngle::_invalidate()
	{
		for ( int i = 0; i < _atoms.size(); i++ )
			_atoms[ i ] = nullptr;
	}

	void DihedralAngle::_setAtomsInternal( const Model::Atom & p_firstAtom,
										   const Model::Atom & p_secondAtom,
										   const Model::Atom & p_thirdAtom,
										   const Model::Atom & p_fourthAtom,
										   const bool		   p_notify )
	{
		_cleanViews();

		_atoms[ 0 ] = &p_firstAtom;
		_atoms[ 1 ] = &p_secondAtom;
		_atoms[ 2 ] = &p_thirdAtom;
		_atoms[ 3 ] = &p_fourthAtom;

		const Model::Molecule * const firstAtomMolecule = p_firstAtom.getMoleculePtr();
		_isAllAtomsOnSameMolecule						= true;

		for ( int i = 1; i < _atoms.size(); i++ )
		{
			_isAllAtomsOnSameMolecule = _isAllAtomsOnSameMolecule && _atoms[ i ]->getMoleculePtr() == firstAtomMolecule;
		}

		_invalidateAABB();
		setPosition( ( _atoms[ 1 ]->getWorldPosition() + _atoms[ 2 ]->getWorldPosition() ) * 0.5f );

		_computeDihedralAngle( p_notify );

		_instantiateViewsOnMolecules();

		if ( hasAutoNaming() )
			_performAutoName( p_notify );
	}

	bool DihedralAngle::_isLinkedToAtom( const Model::Atom * const p_atom ) const
	{
		if ( !isValid() )
			return false;

		for ( const Model::Atom * const linkedAtom : _atoms )
			if ( linkedAtom == p_atom )
				return true;

		return false;
	}
	bool DihedralAngle::_isLinkedToMolecule( const Model::Molecule * const p_molecule ) const
	{
		if ( !isValid() )
			return false;

		for ( const Model::Atom * const linkedAtom : _atoms )
			if ( linkedAtom->getMoleculePtr() == p_molecule )
				return true;

		return false;
	}

	void DihedralAngle::_computeDihedralAngle( const bool p_notify )
	{
		const float newAngleRad = Util::Math::torsionalAngle( _atoms[ 0 ]->getWorldPosition(),
															  _atoms[ 1 ]->getWorldPosition(),
															  _atoms[ 2 ]->getWorldPosition(),
															  _atoms[ 3 ]->getWorldPosition() );

		const float newAngleDegree = Util::Math::degrees( newAngleRad );

		if ( _dihedralAngle != newAngleDegree )
		{
			_dihedralAngle = newAngleDegree;

			if ( p_notify )
				_notifyDataChanged();
		}
	}

	void DihedralAngle::_recomputeAABB( Object3D::Helper::AABB & p_aabb )
	{
		p_aabb = Object3D::Helper::AABB();

		for ( const Model::Atom * const atom : _atoms )
		{
			if ( atom != nullptr )
				p_aabb.extend( atom->getWorldAABB() );
		}
	}

	void DihedralAngle::_instantiateViewsOnMolecules()
	{
		std::vector<Model::Molecule *> viewedMolecules = std::vector<Model::Molecule *>();
		viewedMolecules.reserve( _moleculeViews.size() );

		for ( int i = 0; i < _atoms.size(); i++ )
		{
			if ( _atoms[ i ] != nullptr )
			{
				Model::Molecule * const molecule = _atoms[ i ]->getMoleculePtr();

				if ( std::find( viewedMolecules.begin(), viewedMolecules.end(), molecule ) == viewedMolecules.end() )
				{
					MoleculeView * const moleculeView
						= MVC::MvcManager::get().instantiateView<MoleculeView>( molecule, getViewID( i ) );

					moleculeView->setCallback( this, &DihedralAngle::_onMoleculeChange );
					_moleculeViews[ i ] = moleculeView;
					viewedMolecules.emplace_back( molecule );
				}
			}
			else
			{
				_moleculeViews[ i ] = nullptr;
			}
		}
	}

	void DihedralAngle::_cleanViews()
	{
		for ( int i = 0; i < _atoms.size(); i++ )
		{
			if ( _moleculeViews[ i ] != nullptr )
			{
				MVC::MvcManager::get().deleteView( _atoms[ i ]->getMoleculePtr(), getViewID( i ) );
				_moleculeViews[ i ] = nullptr;
			}
		}
	}

	void DihedralAngle::_onMoleculeChange( const Model::Molecule * const p_molecule,
										   const Event::VTXEvent * const p_event )
	{
		bool recomputeAngle = false;
		if ( p_event->name == Event::Model::TRANSFORM_CHANGE )
		{
			recomputeAngle = !_isAllAtomsOnSameMolecule;
			_invalidateAABB();
		}
		else if ( p_event->name == Event::Model::TRAJECTORY_FRAME_CHANGE )
		{
			recomputeAngle = true;
			_invalidateAABB();
		}

		if ( recomputeAngle )
		{
			_computeDihedralAngle( true );

			if ( hasAutoNaming() )
				_performAutoName();
		}
	}

	void DihedralAngle::_performAutoName( const bool p_notify )
	{
		std::string newName;

		if ( !isValid() )
		{
			newName = "...";
		}
		else
		{
			newName = _atoms[ 0 ]->getName() + "-" + _atoms[ 1 ]->getName() + '-' + _atoms[ 2 ]->getName() + '-'
					  + _atoms[ 3 ]->getName() + " : " + Util::Measurement::getDihedralAngleString( *this );
		}

		if ( p_notify )
		{
			setName( newName );
		}
		else
		{
			std::string & nameRef = _getName();
			nameRef				  = newName;
		}
	}

	VTX::ID::VTX_ID DihedralAngle::getViewID( const int p_atomPos ) const
	{
		return MVC::MvcManager::get().generateViewID( VTX::ID::View::MEASUREMENT_ON_MOLECULE,
													  std::to_string( getId() ) + '_' + std::to_string( p_atomPos ) );
	}

	void DihedralAngle::autoDelete() const { MVC::MvcManager::get().deleteModel( this ); }

} // namespace VTX::Model::Measurement
