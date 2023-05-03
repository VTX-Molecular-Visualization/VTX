// #include "tool/old_tool/model/measurement/distance_to_cycle.hpp"
// #include "event/event.hpp"
// #include "event/event_manager.hpp"
// #include "model/atom.hpp"
// #include "model/molecule.hpp"
// #include <app/event/global.hpp>
// #include "mvc/mvc_manager.hpp"
// #include "object3d/scene.hpp"
// #include <util/math.hpp>
// #include "util/measurement.hpp"
// #include <string>
//
//  namespace VTX::Model::Measurement
//{
//	DistanceToCycle::DistanceToCycle() : App::Component::Object3D::Label( App::ID::Model::MODEL_MEASUREMENT_DISTANCE_TO_CYCLE )
//	{
//		_registerEvent( VTX::App::Event::Global::MOLECULE_REMOVED );
//		_registerEvent( VTX::App::Event::Global::ATOM_REMOVED );
//		_registerEvent( VTX::App::Event::Global::LABEL_REMOVED );
//
//		setAutoNaming( true, false );
//		_moleculeViews.reserve( 2 );
//	}
//
//	DistanceToCycle::DistanceToCycle( const AtomPair & p_pair ) : DistanceToCycle()
//	{
//		_setAtomsInternal( p_pair.first, p_pair.second, false );
//	}
//
//	DistanceToCycle ::~DistanceToCycle() {}
//
//	void DistanceToCycle::receiveEvent( const App::Core::Event::VTXEvent & p_event )
//	{
//		if ( p_event.name == VTX::App::Event::Global::ATOM_REMOVED )
//		{
//			const Event::VTXEventPtr<App::Component::Chemistry::Atom> & castedEvent
//				= dynamic_cast<const App::Core::Event::VTXEventArg<App::Component::Chemistry::Atom*> &>( p_event );
//
//			if ( castedEvent.ptr == _firstAtom || castedEvent.ptr == _secondAtom )
//			{
//				// TODO : Use a manager instead of managing scene from model
//				VTXApp::get().getScene().removeLabel( this );
//				_invalidate();
//				VTXApp::get().deleteAtEndOfFrame( this );
//			}
//		}
//		else if ( p_event.name == VTX::App::Event::Global::MOLECULE_REMOVED )
//		{
//			const Event::VTXEventPtr<App::Component::Chemistry::Molecule> & castedEvent
//				= dynamic_cast<const App::Core::Event::VTXEventArg<App::Component::Chemistry::Molecule*> &>( p_event );
//
//			if ( castedEvent.ptr == _firstAtom->getMoleculePtr() || castedEvent.ptr == _secondAtom->getMoleculePtr() )
//			{
//				// TODO : Use a manager instead of managing scene from model
//				VTXApp::get().getScene().removeLabel( this );
//				_invalidate();
//				VTXApp::get().deleteAtEndOfFrame( this );
//			}
//		}
//		else if ( p_event.name == VTX::App::Event::Global::LABEL_REMOVED )
//		{
//			const Event::VTXEventPtr<App::Component::Object3D::Label> & castedEvent
//				= dynamic_cast<const App::Core::Event::VTXEventArg<App::Component::Object3D::Label*> &>( p_event );
//
//			// TODO : Use a manager instead of managing scene from model
//			if ( castedEvent.ptr == this )
//				_cleanViews();
//		}
//	}
//
//	void DistanceToCycle::setAtoms( const App::Component::Chemistry::Atom & p_firstAtom, const App::Component::Chemistry::Atom & p_secondAtom )
//	{
//		_setAtomsInternal( p_firstAtom, p_secondAtom );
//	}
//
//	void DistanceToCycle::_setAtomsInternal( const App::Component::Chemistry::Atom & p_firstAtom,
//											 const App::Component::Chemistry::Atom & p_secondAtom,
//											 const bool			 p_notify )
//	{
//		_cleanViews();
//
//		_firstAtom	= &p_firstAtom;
//		_secondAtom = &p_secondAtom;
//
//		_invalidateAABB();
//		setPosition( ( _firstAtom->getWorldPosition() + _secondAtom->getWorldPosition() ) * 0.5f );
//
//		_computeDistance( p_notify );
//
//		_instantiateViewsOnMolecules();
//
//		if ( hasAutoNaming() )
//			_performAutoName( p_notify );
//	}
//
//	void DistanceToCycle::_computeDistance( const bool p_notify )
//	{
//		const float newDistance
//			= Util::distance( _firstAtom->getWorldPosition(), _secondAtom->getWorldPosition() );
//
//		if ( _distance != newDistance )
//		{
//			_distance = newDistance;
//
//			if ( p_notify )
//				_notifyDataChanged();
//		}
//	}
//
//	void DistanceToCycle::_recomputeAABB( App::Component::Object3D::Helper::AABB & p_aabb )
//	{
//		p_aabb = App::Component::Object3D::Helper::AABB();
//		p_aabb.extend( _firstAtom->getWorldAABB() );
//		p_aabb.extend( _secondAtom->getWorldAABB() );
//	}
//
//	void DistanceToCycle::_instantiateViewsOnMolecules()
//	{
//		MoleculeView * const firstMoleculeView
//			= VTX::MVC_MANAGER().instantiateView<MoleculeView>( _firstAtom->getMoleculePtr(), getViewID(
// 0
//)
//); 		firstMoleculeView->setCallback( this, &DistanceToCycle::_onMoleculeChange );
//_moleculeViews.emplace_back( firstMoleculeView );
//
//		if ( _firstAtom->getMoleculePtr() != _secondAtom->getMoleculePtr() )
//		{
//			MoleculeView * const secondMoleculeView
//				= VTX::MVC_MANAGER().instantiateView<MoleculeView>( _secondAtom->getMoleculePtr(),
// getViewID( 1 ) ); 			secondMoleculeView->setCallback( this, &DistanceToCycle::_onMoleculeChange );
//_moleculeViews.emplace_back( secondMoleculeView );
//		}
//	}
//
//	void DistanceToCycle::_cleanViews()
//	{
//		if ( _moleculeViews.size() > 0 )
//		{
//			VTX::MVC_MANAGER().deleteView( _firstAtom->getMoleculePtr(), getViewID( 0 ) );
//		}
//
//		if ( _moleculeViews.size() > 1 )
//		{
//			VTX::MVC_MANAGER().deleteView( _secondAtom->getMoleculePtr(), getViewID( 1 ) );
//		}
//
//		_moleculeViews.clear();
//	}
//
//	void DistanceToCycle::_onMoleculeChange( const App::Component::Chemistry::Molecule * const p_molecule,
//											 const App::Core::Event::VTXEvent * const p_event )
//	{
//		bool recomputeDistance = false;
//		if ( p_event->name ==App::Event::Model::TRANSFORM_CHANGE )
//		{
//			// recompute only if the two atoms aren't in the same molecule
//			recomputeDistance = _firstAtom->getMoleculePtr() != _secondAtom->getMoleculePtr();
//			_invalidateAABB();
//		}
//		else if ( p_event->name ==App::Event::Model::TRAJECTORY_FRAME_CHANGE )
//		{
//			recomputeDistance = true;
//			_invalidateAABB();
//		}
//
//		if ( recomputeDistance )
//		{
//			_computeDistance( true );
//
//			if ( hasAutoNaming() )
//				_performAutoName();
//		}
//	}
//
//	void DistanceToCycle::_performAutoName( const bool p_notify )
//	{
//		std::string newName;
//
//		if ( _firstAtom == nullptr || _secondAtom == nullptr )
//		{
//			newName = "...";
//		}
//		else
//		{
//			newName = _firstAtom->getName() + "-" + _secondAtom->getName() + " : "
//					  + Util::Measurement::getDistanceString( *this );
//		}
//
//		if ( p_notify )
//		{
//			setName( newName );
//		}
//		else
//		{
//			std::string & nameRef = _getName();
//			nameRef				  = newName;
//		}
//	}
//
//	VTX::App::VTX_ID DistanceToCycle::getViewID( const int p_atomPos ) const
//	{
//		return VTX::MVC_MANAGER().generateViewID( VTX::App::ID::View::MEASUREMENT_ON_MOLECULE,
//													  std::to_string( getId() ) + '_' + std::to_string( p_atomPos ) );
//	}
//
//	void DistanceToCycle::autoDelete() const { VTX::MVC_MANAGER().deleteModel( this ); }
//
// } // namespace VTX::Model::Measurement
