#ifndef __VTX_ACTION_ATOM__
#define __VTX_ACTION_ATOM__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/atom.hpp"
#include "state/state_machine.hpp"
#include "state/visualization.hpp"
#include "util/molecule.hpp"
#include "visible.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		namespace Atom
		{
			class ChangeColor : public BaseAction
			{
			  public:
				explicit ChangeColor( Model::Atom & p_atom, const Color::Rgb & p_color ) : _atom( p_atom ), _color( p_color ) {}

				virtual void execute() override
				{
					//_atom.setColor( _color );
					//_atom.getMoleculePtr()->setColorMode();
				}

			  private:
				Model::Atom &	 _atom;
				const Color::Rgb _color;
			};

			class ChangeVisibility : public Visible::ChangeVisibility
			{
			  public:
				explicit ChangeVisibility( Model::Atom & p_atom, const VISIBILITY_MODE p_mode ) : Visible::ChangeVisibility( p_atom, p_mode ) {}

				virtual void execute() override
				{
					const Model::Atom & atom = ( (Model::Atom &)_visible );
					Visible::ChangeVisibility::execute();

					if ( _mode == VISIBILITY_MODE::ALL || _mode == VISIBILITY_MODE::SOLO )
					{
						for ( uint i = 0; i < atom.getResiduePtr()->getAtomCount(); ++i )
						{
							atom.getMoleculePtr()
								->getAtom( atom.getResiduePtr()->getIndexFirstAtom() + i )
								.setVisible( _mode == VISIBILITY_MODE::ALL
											 || ( _mode == VISIBILITY_MODE::SOLO && atom.getResiduePtr()->getIndexFirstAtom() + i == atom.getIndex() ) );
						}
					}

					Util::Molecule::refreshRepresentationState( *atom.getMoleculePtr() );
				}
			};

			class Focus : public BaseAction
			{
			  public:
				explicit Focus( Model::Atom & p_atom ) : _atom( p_atom ) {}

				virtual void execute() override
				{
					VTXApp::get().getStateMachine().getItem<State::Visualization>( ID::State::VISUALIZATION )->getCurrentCameraController()->focus( _atom.getAABB() );
				}

			  private:
				Model::Atom & _atom;
			};
		} // namespace Atom
	}	  // namespace Action
} // namespace VTX
#endif
