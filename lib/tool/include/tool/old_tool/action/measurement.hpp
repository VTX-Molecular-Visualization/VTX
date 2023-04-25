#ifndef __VTX_ACTION_MEASUREMENT__
#define __VTX_ACTION_MEASUREMENT__

#include "tool/old_tool/model/measurement/angle.hpp"
#include "tool/old_tool/model/measurement/dihedral_angle.hpp"
#include "tool/old_tool/model/measurement/distance.hpp"
#include <app/core/action/base_action.hpp>
#include <app/model/atom.hpp>
#include <app/model/molecule.hpp>
#include <app/mvc.hpp>
#include <app/old_app/object3d/scene.hpp>
#include <ui/old_ui/vtx_app.hpp>

namespace VTX::Action::Measurement
{
	class InstantiateDistanceLabel : public App::Core::Action::BaseAction
	{
	  public:
		explicit InstantiateDistanceLabel( const Model::Atom & p_firstAtom, const Model::Atom & p_secondAtom ) :
			_firstAtom( p_firstAtom ), _secondAtom( p_secondAtom )
		{
		}

		virtual void execute() override
		{
			// TODO : perf test, to remove
			// const Model::Molecule &				mol = *( VTXApp::get().getScene().getMolecules().begin()->first );
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

			//	VTXApp::get().getScene().addLabel( distanceModel );
			//}

			// return;

			const Model::Measurement::Distance::AtomPair pair( _firstAtom, _secondAtom );
			Model::Measurement::Distance * const		 distanceModel
				= VTX::MVC_MANAGER().instantiateModel<Model::Measurement::Distance>( pair );

			VTXApp::get().getScene().addLabel( distanceModel );
		}

	  private:
		const Model::Atom & _firstAtom;
		const Model::Atom & _secondAtom;
	};

	class InstantiateAngleLabel : public App::Core::Action::BaseAction
	{
	  public:
		explicit InstantiateAngleLabel( const Model::Atom & p_firstAtom,
										const Model::Atom & p_secondAtom,
										const Model::Atom & p_thirdAtom ) :
			_firstAtom( p_firstAtom ),
			_secondAtom( p_secondAtom ), _thirdAtom( p_thirdAtom )
		{
		}

		virtual void execute() override
		{
			const Model::Measurement::Angle::AtomTriplet triplet( _firstAtom, _secondAtom, _thirdAtom );
			Model::Measurement::Angle * const			 angleModel
				= VTX::MVC_MANAGER().instantiateModel<Model::Measurement::Angle>( triplet );

			VTXApp::get().getScene().addLabel( angleModel );
		}

	  private:
		const Model::Atom & _firstAtom;
		const Model::Atom & _secondAtom;
		const Model::Atom & _thirdAtom;
	};

	class InstantiateDihedralAngleLabel : public App::Core::Action::BaseAction
	{
	  public:
		explicit InstantiateDihedralAngleLabel( const Model::Atom & p_firstAtom,
												const Model::Atom & p_secondAtom,
												const Model::Atom & p_thirdAtom,
												const Model::Atom & p_fourthAtom ) :
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

			VTXApp::get().getScene().addLabel( dihedralAngleModel );
		}

	  private:
		const Model::Atom & _firstAtom;
		const Model::Atom & _secondAtom;
		const Model::Atom & _thirdAtom;
		const Model::Atom & _fourthAtom;
	};

} // namespace VTX::Action::Measurement
#endif
