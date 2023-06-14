#ifndef __VTX_ACTION_MEASUREMENT__
#define __VTX_ACTION_MEASUREMENT__

#include "tool/old_tool/model/measurement/angle.hpp"
#include "tool/old_tool/model/measurement/dihedral_angle.hpp"
#include "tool/old_tool/model/measurement/distance.hpp"
#include <app/old/core/action/base_action.hpp>
#include <app/old/component/chemistry/atom.hpp>
#include <app/old/component/chemistry/molecule.hpp>
#include <app/old/mvc.hpp>
#include <app/old/application/scene.hpp>
#include <ui/old_ui/vtx_app.hpp>

namespace VTX::Action::Measurement
{
	class InstantiateDistanceLabel : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit InstantiateDistanceLabel( const App::Old::Component::Chemistry::Atom & p_firstAtom, const App::Old::Component::Chemistry::Atom & p_secondAtom ) :
			_firstAtom( p_firstAtom ), _secondAtom( p_secondAtom )
		{
		}

		virtual void execute() override
		{
			// TODO : perf test, to remove
			// const App::Old::Component::Chemistry::Molecule &				mol = *( App::Old::VTXApp::get().getScene().getMolecules().begin()->first );
			// std::default_random_engine			generator;
			// std::uniform_int_distribution<uint> distribution( 0, mol.getAtomCount() );
			// for ( int i = 0; i < 100; i++ )
			//{
			//	const uint firstAtomIndex  = distribution( generator );
			//	const uint secondAtomIndex = distribution( generator );

			//	Model::Measurement::Distance * const distanceModel
			//		= VTX::MVC_MANAGER().instantiateModel<Model::Measurement::Distance>(
			//			Model::Measurement::Distance::AtomPair( *mol.getAtom( firstAtomIndex ),
			//													*mol.getAtom( secondAtomIndex ) ) );

			//	App::Old::VTXApp::get().getScene().addLabel( distanceModel );
			//}

			// return;

			const Model::Measurement::Distance::AtomPair pair( _firstAtom, _secondAtom );
			Model::Measurement::Distance * const		 distanceModel
				= VTX::MVC_MANAGER().instantiateModel<Model::Measurement::Distance>( pair );

			App::Old::VTXApp::get().getScene().addLabel( distanceModel );
		}

	  private:
		const App::Old::Component::Chemistry::Atom & _firstAtom;
		const App::Old::Component::Chemistry::Atom & _secondAtom;
	};

	class InstantiateAngleLabel : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit InstantiateAngleLabel( const App::Old::Component::Chemistry::Atom & p_firstAtom,
										const App::Old::Component::Chemistry::Atom & p_secondAtom,
										const App::Old::Component::Chemistry::Atom & p_thirdAtom ) :
			_firstAtom( p_firstAtom ),
			_secondAtom( p_secondAtom ), _thirdAtom( p_thirdAtom )
		{
		}

		virtual void execute() override
		{
			const Model::Measurement::Angle::AtomTriplet triplet( _firstAtom, _secondAtom, _thirdAtom );
			Model::Measurement::Angle * const			 angleModel
				= VTX::MVC_MANAGER().instantiateModel<Model::Measurement::Angle>( triplet );

			App::Old::VTXApp::get().getScene().addLabel( angleModel );
		}

	  private:
		const App::Old::Component::Chemistry::Atom & _firstAtom;
		const App::Old::Component::Chemistry::Atom & _secondAtom;
		const App::Old::Component::Chemistry::Atom & _thirdAtom;
	};

	class InstantiateDihedralAngleLabel : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit InstantiateDihedralAngleLabel( const App::Old::Component::Chemistry::Atom & p_firstAtom,
												const App::Old::Component::Chemistry::Atom & p_secondAtom,
												const App::Old::Component::Chemistry::Atom & p_thirdAtom,
												const App::Old::Component::Chemistry::Atom & p_fourthAtom ) :
			_firstAtom( p_firstAtom ),
			_secondAtom( p_secondAtom ), _thirdAtom( p_thirdAtom ), _fourthAtom( p_fourthAtom )
		{
		}

		virtual void execute() override
		{
			const Model::Measurement::DihedralAngle::AtomQuadruplet quadruplet(
				_firstAtom, _secondAtom, _thirdAtom, _fourthAtom );

			Model::Measurement::DihedralAngle * const dihedralAngleModel
				= VTX::MVC_MANAGER().instantiateModel<Model::Measurement::DihedralAngle>( quadruplet );

			App::Old::VTXApp::get().getScene().addLabel( dihedralAngleModel );
		}

	  private:
		const App::Old::Component::Chemistry::Atom & _firstAtom;
		const App::Old::Component::Chemistry::Atom & _secondAtom;
		const App::Old::Component::Chemistry::Atom & _thirdAtom;
		const App::Old::Component::Chemistry::Atom & _fourthAtom;
	};

} // namespace VTX::Action::Measurement
#endif
