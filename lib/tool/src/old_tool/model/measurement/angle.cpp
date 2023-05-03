#include "tool/old_tool/model/measurement/angle.hpp"
#include "tool/old_tool/util/measurement.hpp"
#include <app/application/scene.hpp>
#include <app/component/chemistry/atom.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <app/core/event/vtx_event.hpp>
#include <app/event.hpp>
#include <app/event/global.hpp>
#include <app/mvc.hpp>
#include <ui/id.hpp>
#include <util/math.hpp>
#include <variant>

namespace VTX::Model::Measurement
{
	Angle::Angle() : App::Component::Object3D::Label( App::ID::Model::MODEL_MEASUREMENT_ANGLE )
	{
		_atoms.resize( 3, nullptr );
		_moleculeViews.resize( 3, nullptr );

		_registerEvent( VTX::App::Event::Global::MOLECULE_REMOVED );
		_registerEvent( VTX::App::Event::Global::ATOM_REMOVED );
		_registerEvent( VTX::App::Event::Global::LABEL_REMOVED );

		setAutoNaming( true, false );
	}

	Angle::Angle( const AtomTriplet & p_triplet ) : Angle()
	{
		_setAtomsInternal( std::get<0>( p_triplet ), std::get<1>( p_triplet ), std::get<2>( p_triplet ), false );
	}

	Angle ::~Angle() {}

	void Angle::receiveEvent( const App::Core::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::App::Event::Global::ATOM_REMOVED )
		{
			const VTX::App::Core::Event::VTXEventArg<App::Component::Chemistry::Atom *> & castedEvent
				= dynamic_cast<const VTX::App::Core::Event::VTXEventArg<App::Component::Chemistry::Atom *> &>(
					p_event );

			if ( _isLinkedToAtom( castedEvent.get() ) )
			{
				// TODO : Use a manager instead of managing scene from model
				VTXApp::get().getScene().removeLabel( this );
				_invalidate();
				VTXApp::get().deleteAtEndOfFrame( this );
			}
		}
		else if ( p_event.name == VTX::App::Event::Global::MOLECULE_REMOVED )
		{
			const VTX::App::Core::Event::VTXEventArg<App::Component::Chemistry::Molecule *> & castedEvent
				= dynamic_cast<const VTX::App::Core::Event::VTXEventArg<App::Component::Chemistry::Molecule *> &>(
					p_event );

			if ( _isLinkedToMolecule( castedEvent.get() ) )
			{
				// TODO : Use a manager instead of managing scene from model
				VTXApp::get().getScene().removeLabel( this );
				_invalidate();
				VTXApp::get().deleteAtEndOfFrame( this );
			}
		}
		else if ( p_event.name == VTX::App::Event::Global::LABEL_REMOVED )
		{
			const App::Core::Event::VTXEventArg<App::Component::Object3D::Label *> & castedEvent
				= dynamic_cast<const App::Core::Event::VTXEventArg<App::Component::Object3D::Label *> &>( p_event );

			// TODO : Use a manager instead of managing scene from model
			if ( castedEvent.get() == this )
				_cleanViews();
		}
	}

	void Angle::setAtoms( const App::Component::Chemistry::Atom & p_firstAtom,
						  const App::Component::Chemistry::Atom & p_secondAtom,
						  const App::Component::Chemistry::Atom & p_thirdAtom )
	{
		_setAtomsInternal( p_firstAtom, p_secondAtom, p_thirdAtom );
	}

	bool Angle::isValid() const
	{
		for ( const App::Component::Chemistry::Atom * const atomPtr : _atoms )
			if ( atomPtr == nullptr )
				return false;

		return true;
	}

	void Angle::_invalidate()
	{
		for ( int i = 0; i < _atoms.size(); i++ )
			_atoms[ i ] = nullptr;
	}

	void Angle::_setAtomsInternal( const App::Component::Chemistry::Atom & p_firstAtom,
								   const App::Component::Chemistry::Atom & p_secondAtom,
								   const App::Component::Chemistry::Atom & p_thirdAtom,
								   const bool							   p_notify )
	{
		_cleanViews();

		_atoms[ 0 ] = &p_firstAtom;
		_atoms[ 1 ] = &p_secondAtom;
		_atoms[ 2 ] = &p_thirdAtom;

		const App::Component::Chemistry::Molecule * const firstAtomMolecule = p_firstAtom.getMoleculePtr();
		_isAllAtomsOnSameMolecule											= true;

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

	bool Angle::_isLinkedToAtom( const App::Component::Chemistry::Atom * const p_atom ) const
	{
		if ( !isValid() )
			return false;

		for ( const App::Component::Chemistry::Atom * const linkedAtom : _atoms )
			if ( linkedAtom == p_atom )
				return true;

		return false;
	}
	bool Angle::_isLinkedToMolecule( const App::Component::Chemistry::Molecule * const p_molecule ) const
	{
		if ( !isValid() )
			return false;

		for ( const App::Component::Chemistry::Atom * const linkedAtom : _atoms )
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

	void Angle::_recomputeAABB( App::Component::Object3D::Helper::AABB & p_aabb )
	{
		p_aabb = App::Component::Object3D::Helper::AABB();

		for ( const App::Component::Chemistry::Atom * const atom : _atoms )
		{
			if ( atom != nullptr )
				p_aabb.extend( atom->getWorldAABB() );
		}
	}

	void Angle::_instantiateViewsOnMolecules()
	{
		std::vector<App::Component::Chemistry::Molecule *> viewedMolecules
			= std::vector<App::Component::Chemistry::Molecule *>();
		viewedMolecules.reserve( _moleculeViews.size() );

		for ( int i = 0; i < _atoms.size(); i++ )
		{
			if ( _atoms[ i ] != nullptr )
			{
				App::Component::Chemistry::Molecule * const molecule = _atoms[ i ]->getMoleculePtr();

				if ( std::find( viewedMolecules.begin(), viewedMolecules.end(), molecule ) == viewedMolecules.end() )
				{
					MoleculeView * const moleculeView
						= VTX::MVC_MANAGER().instantiateView<MoleculeView>( molecule, getViewID( i ) );

					moleculeView->setCallback( this, &Angle::_onMoleculeChange );
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

	void Angle::_cleanViews()
	{
		for ( int i = 0; i < _atoms.size(); i++ )
		{
			if ( _moleculeViews[ i ] != nullptr )
			{
				VTX::MVC_MANAGER().deleteView( _atoms[ i ]->getMoleculePtr(), getViewID( i ) );
				_moleculeViews[ i ] = nullptr;
			}
		}
	}

	void Angle::_onMoleculeChange( const App::Component::Chemistry::Molecule * const p_molecule,
								   const VTX::App::Core::Event::VTXEvent * const	 p_event )
	{
		bool recomputeAngle = false;
		if ( p_event->name == App::Event::Model::TRANSFORM_CHANGE )
		{
			recomputeAngle = !_isAllAtomsOnSameMolecule;
			_invalidateAABB();
		}
		else if ( p_event->name == App::Event::Model::TRAJECTORY_FRAME_CHANGE )
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

	VTX::App::VTX_ID Angle::getViewID( const int p_atomPos ) const
	{
		return VTX::MVC_MANAGER().generateViewID( UI::ID::View::MEASUREMENT_ON_MOLECULE,
												  std::to_string( getId() ) + '_' + std::to_string( p_atomPos ) );
	}

	void Angle::autoDelete() const { VTX::MVC_MANAGER().deleteModel( this ); }

} // namespace VTX::Model::Measurement
