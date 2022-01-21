#include "angle.hpp"
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
	Angle::Angle() : Model::Label( VTX::ID::Model::MODEL_MEASUREMENT_ANGLE )
	{
		_atoms.resize( 3, nullptr );
		_moleculeViews.resize( 3, nullptr );

		_registerEvent( Event::Global::MOLECULE_REMOVED );
		_registerEvent( Event::Global::ATOM_REMOVED );
		_registerEvent( Event::Global::LABEL_REMOVED );

		setAutoNaming( true, false );
	}

	Angle::Angle( const AtomTriplet & p_triplet ) : Angle()
	{
		_setAtomsInternal( std::get<0>( p_triplet ), std::get<1>( p_triplet ), std::get<2>( p_triplet ), false );
	}

	Angle ::~Angle() {}

	void Angle::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::ATOM_REMOVED )
		{
			const Event::VTXEventPtr<Model::Atom> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Atom> &>( p_event );

			if ( _isLinkedToAtom( castedEvent.ptr ) )
			{
				// TODO : Use a manager instead of managing scene from model
				VTXApp::get().getScene().removeLabel( this );
				MVC::MvcManager::get().deleteModel( this );
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
				MVC::MvcManager::get().deleteModel( this );
			}
		}
		else if ( p_event.name == Event::Global::LABEL_REMOVED )
		{
			const Event::VTXEventPtr<Model::Label> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Label> &>( p_event );

			// TODO : Use a manager instead of managing scene from model
			if ( castedEvent.ptr == this )
				_cleanViews();
		}
	}

	void Angle::setAtoms( const Model::Atom & p_firstAtom,
						  const Model::Atom & p_secondAtom,
						  const Model::Atom & p_thirdAtom )
	{
		_setAtomsInternal( p_firstAtom, p_secondAtom, p_thirdAtom );
	}

	bool Angle::isValid() const
	{
		for ( const Model::Atom * const atomPtr : _atoms )
			if ( atomPtr == nullptr )
				return false;

		return true;
	}

	void Angle::_setAtomsInternal( const Model::Atom & p_firstAtom,
								   const Model::Atom & p_secondAtom,
								   const Model::Atom & p_thirdAtom,
								   const bool		   p_notify )
	{
		_cleanViews();

		_atoms[ 0 ] = &p_firstAtom;
		_atoms[ 1 ] = &p_secondAtom;
		_atoms[ 2 ] = &p_thirdAtom;

		const Model::Molecule * const firstAtomMolecule = p_firstAtom.getMoleculePtr();
		_isAllAtomsOnSameMolecule						= true;

		for ( int i = 1; i < _atoms.size(); i++ )
		{
			_isAllAtomsOnSameMolecule = _isAllAtomsOnSameMolecule && _atoms[ i ]->getMoleculePtr() == firstAtomMolecule;
		}

		_invalidateAABB();
		setPosition( _atoms[ 1 ]->getWorldPosition() );

		_computeAngle( p_notify );

		_instantiateViewsOnMolecules();

		if ( hasAutoNaming() )
			_performAutoName( p_notify );
	}

	bool Angle::_isLinkedToAtom( const Model::Atom * const p_atom ) const
	{
		if ( !isValid() )
			return false;

		for ( const Model::Atom * const linkedAtom : _atoms )
			if ( linkedAtom == p_atom )
				return true;

		return false;
	}
	bool Angle::_isLinkedToMolecule( const Model::Molecule * const p_molecule ) const
	{
		if ( !isValid() )
			return false;

		for ( const Model::Atom * const linkedAtom : _atoms )
			if ( linkedAtom->getMoleculePtr() == p_molecule )
				return true;

		return false;
	}

	void Angle::_computeAngle( const bool p_notify )
	{
		float newAngle = 0;

		if ( isValid() )
		{
			const Vec3f vec1
				= Util::Math::normalize( _atoms[ 0 ]->getWorldPosition() - _atoms[ 1 ]->getWorldPosition() );
			const Vec3f vec2
				= Util::Math::normalize( _atoms[ 2 ]->getWorldPosition() - _atoms[ 1 ]->getWorldPosition() );

			newAngle = Util::Math::degrees( Util::Math::angle( vec1, vec2 ) );
		}

		if ( _angle != newAngle )
		{
			_angle = newAngle;

			if ( p_notify )
				_notifyDataChanged();
		}
	}

	void Angle::_recomputeAABB( Math::AABB & p_aabb )
	{
		p_aabb = Math::AABB();

		for ( const Model::Atom * const atom : _atoms )
		{
			if ( atom != nullptr )
				p_aabb.extend( atom->getWorldAABB() );
		}
	}

	void Angle::_instantiateViewsOnMolecules()
	{
		for ( int i = 0; i < _atoms.size(); i++ )
		{
			if ( _atoms[ i ] != nullptr )
			{
				MoleculeView * const moleculeView = MVC::MvcManager::get().instantiateView<MoleculeView>(
					_atoms[ i ]->getMoleculePtr(), getViewID( i ) );
				moleculeView->setCallback( this, &Angle::_onMoleculeChange );
				_moleculeViews[ i ] = moleculeView;
			}
		}
	}

	void Angle::_cleanViews()
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

	void Angle::_onMoleculeChange( const Model::Molecule * const p_molecule, const Event::VTXEvent * const p_event )
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
			_computeAngle( true );

			if ( hasAutoNaming() )
				_performAutoName();
		}
	}

	void Angle::_performAutoName( const bool p_notify )
	{
		std::string newName;

		if ( !isValid() )
		{
			newName = "...";
		}
		else
		{
			newName = _atoms[ 0 ]->getName() + "-" + _atoms[ 1 ]->getName() + '-' + _atoms[ 2 ]->getName() + " : "
					  + Util::Measurement::getAngleString( *this );
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

	VTX::ID::VTX_ID Angle::getViewID( const int p_atomPos ) const
	{
		return MVC::MvcManager::get().generateViewID( VTX::ID::View::MEASUREMENT_ON_MOLECULE,
													  std::to_string( getId() ) + '_' + std::to_string( p_atomPos ) );
	}

} // namespace VTX::Model::Measurement
