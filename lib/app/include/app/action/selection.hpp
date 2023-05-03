#ifndef __VTX_APP_ACTION_SELECTION__
#define __VTX_APP_ACTION_SELECTION__

#include "app/action.hpp"
#include "app/action/visible.hpp"
#include "app/application/selection/selection.hpp"
#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/category.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/generated_molecule.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/component/object3d/viewpoint.hpp"
#include "app/core/action/base_action.hpp"
#include "app/id.hpp"
#include <vector>

namespace VTX::App::Action::Selection
{
	class SelectAll : public App::Core::Action::BaseAction
	{
	  public:
		explicit SelectAll() {}

		virtual void execute() override;
	};
	class ClearSelection : public App::Core::Action::BaseAction
	{
	  public:
		explicit ClearSelection( App::Application::Selection::SelectionModel & p_selection ) : _selection( p_selection )
		{
		}

		virtual void execute() override;

	  private:
		App::Application::Selection::SelectionModel & _selection;
	};

	class SelectModels : public App::Core::Action::BaseAction
	{
	  public:
		explicit SelectModels( App::Application::Selection::SelectionModel & p_selection,
							   const std::vector<App::Core::Model::ID> &	 p_models,
							   const bool									 p_appendToSelection = false ) :
			_selection( p_selection ),
			_appendToSelection( p_appendToSelection )
		{
			_models.resize( p_models.size() );
			for ( int i = 0; i < _models.size(); i++ )
				_models[ i ] = p_models[ i ];
		}

		virtual void execute() override;

	  private:
		App::Application::Selection::SelectionModel & _selection;
		std::vector<App::Core::Model::ID>			  _models = std::vector<App::Core::Model::ID>();
		const bool									  _appendToSelection;
	};
	class UnselectModels : public App::Core::Action::BaseAction
	{
	  public:
		explicit UnselectModels( App::Application::Selection::SelectionModel & p_selection,
								 const std::vector<App::Core::Model::ID> &	   p_models ) :
			_selection( p_selection )
		{
			_models.resize( p_models.size() );
			for ( int i = 0; i < _models.size(); i++ )
				_models[ i ] = p_models[ i ];
		}

		virtual void execute() override;

	  private:
		App::Application::Selection::SelectionModel & _selection;
		std::vector<App::Core::Model::ID>			  _models = std::vector<App::Core::Model::ID>();
	};

	class SelectMolecule : public App::Core::Action::BaseAction
	{
	  public:
		explicit SelectMolecule( App::Application::Selection::SelectionModel & p_selection,
								 App::Component::Chemistry::Molecule &		   p_molecule,
								 const bool									   p_appendToSelection = false ) :
			_selection( p_selection ),
			_appendToSelection( p_appendToSelection )
		{
			_molecules.emplace_back( &p_molecule );
		}
		explicit SelectMolecule( App::Application::Selection::SelectionModel &				p_selection,
								 const std::vector<App::Component::Chemistry::Molecule *> & p_molecules,
								 const bool p_appendToSelection = false ) :
			_selection( p_selection ),
			_appendToSelection( p_appendToSelection )
		{
			_molecules.resize( p_molecules.size() );
			for ( int i = 0; i < _molecules.size(); i++ )
				_molecules[ i ] = p_molecules[ i ];
		}

		virtual void execute() override;

	  private:
		App::Application::Selection::SelectionModel &	   _selection;
		std::vector<App::Component::Chemistry::Molecule *> _molecules
			= std::vector<App::Component::Chemistry::Molecule *>();
		const bool _appendToSelection;
	};
	class SelectCategory : public App::Core::Action::BaseAction
	{
	  public:
		explicit SelectCategory( App::Application::Selection::SelectionModel & p_selection,
								 App::Component::Chemistry::Category &		   p_category,
								 const bool									   p_appendToSelection = false ) :
			_selection( p_selection ),
			_categories { &p_category }, _appendToSelection( p_appendToSelection )
		{
		}

		explicit SelectCategory( App::Application::Selection::SelectionModel &				p_selection,
								 const std::vector<App::Component::Chemistry::Category *> & p_categories,
								 const bool p_appendToSelection = false ) :
			_selection( p_selection ),
			_categories( p_categories ), _appendToSelection( p_appendToSelection )
		{
		}

		virtual void execute() override;

	  private:
		App::Application::Selection::SelectionModel &	   _selection;
		std::vector<App::Component::Chemistry::Category *> _categories;
		const bool										   _appendToSelection;
	};
	class SelectChain : public App::Core::Action::BaseAction
	{
	  public:
		explicit SelectChain( App::Application::Selection::SelectionModel & p_selection,
							  App::Component::Chemistry::Chain &			p_chain,
							  const bool									p_appendToSelection = false ) :
			_selection( p_selection ),
			_chains { &p_chain }, _appendToSelection( p_appendToSelection )
		{
		}

		explicit SelectChain( App::Application::Selection::SelectionModel &			  p_selection,
							  const std::vector<App::Component::Chemistry::Chain *> & p_chains,
							  const bool											  p_appendToSelection = false ) :
			_selection( p_selection ),
			_chains( p_chains ), _appendToSelection( p_appendToSelection )
		{
		}

		virtual void execute() override;

	  private:
		App::Application::Selection::SelectionModel &	_selection;
		std::vector<App::Component::Chemistry::Chain *> _chains;
		const bool										_appendToSelection;
	};
	class SelectResidue : public App::Core::Action::BaseAction
	{
	  public:
		explicit SelectResidue( App::Application::Selection::SelectionModel & p_selection,
								App::Component::Chemistry::Residue &		  p_residue,
								const bool									  p_appendToSelection = false ) :
			_selection( p_selection ),
			_appendToSelection( p_appendToSelection )
		{
			_residues.emplace_back( &p_residue );
		}
		explicit SelectResidue( App::Application::Selection::SelectionModel &			  p_selection,
								const std::vector<App::Component::Chemistry::Residue *> & p_residues,
								const bool p_appendToSelection = false ) :
			_selection( p_selection ),
			_appendToSelection( p_appendToSelection )
		{
			_residues.resize( p_residues.size() );
			for ( int i = 0; i < _residues.size(); i++ )
				_residues[ i ] = p_residues[ i ];
		}

		virtual void execute() override;

	  private:
		App::Application::Selection::SelectionModel &	  _selection;
		std::vector<App::Component::Chemistry::Residue *> _residues
			= std::vector<App::Component::Chemistry::Residue *>();
		const bool _appendToSelection;
	};
	class SelectAtom : public App::Core::Action::BaseAction
	{
	  public:
		explicit SelectAtom( App::Application::Selection::SelectionModel & p_selection,
							 App::Component::Chemistry::Atom &			   p_atom,
							 const bool									   p_appendToSelection = false ) :
			_selection( p_selection ),
			_appendToSelection( p_appendToSelection )
		{
			_atoms.emplace_back( &p_atom );
		}

		explicit SelectAtom( App::Application::Selection::SelectionModel &			p_selection,
							 const std::vector<App::Component::Chemistry::Atom *> & p_atoms,
							 const bool												p_appendToSelection = false ) :
			_selection( p_selection ),
			_atoms( p_atoms ), _appendToSelection( p_appendToSelection )
		{
		}

		virtual void execute() override;

	  private:
		App::Application::Selection::SelectionModel &  _selection;
		std::vector<App::Component::Chemistry::Atom *> _atoms = std::vector<App::Component::Chemistry::Atom *>();
		const bool									   _appendToSelection;
	};

	class UnselectMolecule : public App::Core::Action::BaseAction
	{
	  public:
		explicit UnselectMolecule( App::Application::Selection::SelectionModel & p_selection,
								   App::Component::Chemistry::Molecule &		 p_molecule,
								   bool											 p_check = false ) :
			_selection( p_selection ),
			_check( p_check )
		{
			_molecules.emplace_back( &p_molecule );
		}
		explicit UnselectMolecule( App::Application::Selection::SelectionModel &			  p_selection,
								   const std::vector<App::Component::Chemistry::Molecule *> & p_molecules,
								   bool														  p_check = false ) :
			_selection( p_selection ),
			_check( p_check )
		{
			_molecules.resize( p_molecules.size() );
			for ( int i = 0; i < _molecules.size(); i++ )
				_molecules[ i ] = p_molecules[ i ];
		}

		virtual void execute() override;

	  private:
		App::Application::Selection::SelectionModel &	   _selection;
		std::vector<App::Component::Chemistry::Molecule *> _molecules
			= std::vector<App::Component::Chemistry::Molecule *>();
		const bool _check;
	};
	class UnselectCategory : public App::Core::Action::BaseAction
	{
	  public:
		explicit UnselectCategory( App::Application::Selection::SelectionModel & p_selection,
								   App::Component::Chemistry::Category &		 p_category,
								   bool											 p_check = false ) :
			_selection( p_selection ),
			_check( p_check )
		{
			_categories.emplace_back( &p_category );
		}
		explicit UnselectCategory( App::Application::Selection::SelectionModel &			  p_selection,
								   const std::vector<App::Component::Chemistry::Category *> & p_categories,
								   bool														  p_check = false ) :
			_selection( p_selection ),
			_check( p_check )
		{
			_categories.resize( p_categories.size() );
			for ( int i = 0; i < p_categories.size(); i++ )
				_categories[ i ] = p_categories[ i ];
		}

		virtual void execute() override;

	  private:
		App::Application::Selection::SelectionModel &	   _selection;
		std::vector<App::Component::Chemistry::Category *> _categories
			= std::vector<App::Component::Chemistry::Category *>();
		const bool _check;
	};
	class UnselectChain : public App::Core::Action::BaseAction
	{
	  public:
		explicit UnselectChain( App::Application::Selection::SelectionModel & p_selection,
								App::Component::Chemistry::Chain &			  p_chain,
								bool										  p_check = false ) :
			_selection( p_selection ),
			_check( p_check )
		{
			_chains.emplace_back( &p_chain );
		}
		explicit UnselectChain( App::Application::Selection::SelectionModel &			p_selection,
								const std::vector<App::Component::Chemistry::Chain *> & p_chains,
								bool													p_check = false ) :
			_selection( p_selection ),
			_check( p_check )
		{
			_chains.resize( p_chains.size() );
			for ( int i = 0; i < p_chains.size(); i++ )
				_chains[ i ] = p_chains[ i ];
		}

		virtual void execute() override;

	  private:
		App::Application::Selection::SelectionModel &	_selection;
		std::vector<App::Component::Chemistry::Chain *> _chains = std::vector<App::Component::Chemistry::Chain *>();
		const bool										_check;
	};
	class UnselectResidue : public App::Core::Action::BaseAction
	{
	  public:
		explicit UnselectResidue( App::Application::Selection::SelectionModel & p_selection,
								  App::Component::Chemistry::Residue &			p_residue,
								  bool											p_check = false ) :
			_selection( p_selection ),
			_check( p_check )
		{
			_residues.emplace_back( &p_residue );
		}
		explicit UnselectResidue( App::Application::Selection::SelectionModel &				p_selection,
								  const std::vector<App::Component::Chemistry::Residue *> & p_residues,
								  bool														p_check = false ) :
			_selection( p_selection ),
			_check( p_check )
		{
			_residues.resize( p_residues.size() );
			for ( int i = 0; i < p_residues.size(); i++ )
				_residues[ i ] = p_residues[ i ];
		}

		virtual void execute() override;

	  private:
		App::Application::Selection::SelectionModel &	  _selection;
		std::vector<App::Component::Chemistry::Residue *> _residues
			= std::vector<App::Component::Chemistry::Residue *>();
		const bool _check;
	};
	class UnselectAtom : public App::Core::Action::BaseAction
	{
	  public:
		explicit UnselectAtom( App::Application::Selection::SelectionModel & p_selection,
							   App::Component::Chemistry::Atom &			 p_atom,
							   bool											 p_check = false ) :
			_selection( p_selection ),
			_check( p_check )
		{
			_atoms.emplace_back( &p_atom );
		}
		explicit UnselectAtom( App::Application::Selection::SelectionModel &		  p_selection,
							   const std::vector<App::Component::Chemistry::Atom *> & p_atoms,
							   bool													  p_check = false ) :
			_selection( p_selection ),
			_atoms( p_atoms ), _check( p_check )
		{
		}
		virtual void execute() override;

	  private:
		App::Application::Selection::SelectionModel &  _selection;
		std::vector<App::Component::Chemistry::Atom *> _atoms = std::vector<App::Component::Chemistry::Atom *>();
		const bool									   _check;
	};

	class SelectViewpoint : public App::Core::Action::BaseAction
	{
	  public:
		explicit SelectViewpoint( App::Application::Selection::SelectionModel & p_selection,
								  App::Component::Object3D::Viewpoint &			p_viewpoint,
								  const bool									p_appendToSelection = false ) :
			_selection( p_selection ),
			_appendToSelection( p_appendToSelection )
		{
			_viewpoints.emplace_back( &p_viewpoint );
		}
		explicit SelectViewpoint( App::Application::Selection::SelectionModel &		   p_selection,
								  std::vector<App::Component::Object3D::Viewpoint *> & p_viewpoints,
								  const bool										   p_appendToSelection = false ) :
			_selection( p_selection ),
			_appendToSelection( p_appendToSelection )
		{
			_viewpoints.resize( p_viewpoints.size() );
			for ( int i = 0; i < _viewpoints.size(); i++ )
				_viewpoints[ i ] = p_viewpoints[ i ];
		}

		virtual void execute() override;

	  private:
		App::Application::Selection::SelectionModel &	   _selection;
		std::vector<App::Component::Object3D::Viewpoint *> _viewpoints
			= std::vector<App::Component::Object3D::Viewpoint *>();
		const bool _appendToSelection;
	};
	class UnselectViewpoint : public App::Core::Action::BaseAction
	{
	  public:
		explicit UnselectViewpoint( App::Application::Selection::SelectionModel & p_selection,
									App::Component::Object3D::Viewpoint &		  p_viewpoint,
									bool										  p_check = false ) :
			_selection( p_selection ),
			_check( p_check )
		{
			_viewpoints.emplace_back( &p_viewpoint );
		}
		explicit UnselectViewpoint( App::Application::Selection::SelectionModel &		 p_selection,
									std::vector<App::Component::Object3D::Viewpoint *> & p_viewpoints,
									bool												 p_check = false ) :
			_selection( p_selection ),
			_check( p_check )
		{
			_viewpoints.resize( p_viewpoints.size() );
			for ( int i = 0; i < p_viewpoints.size(); i++ )
				_viewpoints[ i ] = p_viewpoints[ i ];
		}

		virtual void execute() override;

	  private:
		App::Application::Selection::SelectionModel &	   _selection;
		std::vector<App::Component::Object3D::Viewpoint *> _viewpoints
			= std::vector<App::Component::Object3D::Viewpoint *>();
		const bool _check;
	};

	///////////////////////////// ACTION ON SELECTION ///////////////////////////////
	class ChangeVisibility : public Visible::ChangeVisibility
	{
	  public:
		explicit ChangeVisibility( const App::Application::Selection::SelectionModel & p_selection,
								   const Component::Generic::BaseVisible &						   p_objReference,
								   const App::VTX_ID &								   p_objRefTypeId,
								   const VISIBILITY_MODE							   p_mode ) :
			Visible::ChangeVisibility( p_mode ),
			_selection( p_selection ), _objRefTypeId( p_objRefTypeId )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		explicit ChangeVisibility( const App::Application::Selection::SelectionModel & p_selection,
								   const VISIBILITY_MODE							   p_mode ) :
			Visible::ChangeVisibility( p_mode ),
			_selection( p_selection ), _objRefTypeId( "" )
		{
		}

		virtual void execute() override;

		void show( const bool p_show );
		void solo();

	  private:
		const App::Application::Selection::SelectionModel & _selection;
		const App::VTX_ID									_objRefTypeId;
	};

	class ToggleWatersVisibility : public App::Core::Action::BaseAction
	{
	  public:
		explicit ToggleWatersVisibility( App::Application::Selection::SelectionModel & p_selection ) :
			_selection( p_selection )
		{
		}

		virtual void execute() override;

	  private:
		App::Application::Selection::SelectionModel & _selection;
	};

	class ToggleSolventVisibility : public App::Core::Action::BaseAction
	{
	  public:
		explicit ToggleSolventVisibility( App::Application::Selection::SelectionModel & p_selection ) :
			_selection( p_selection )
		{
		}

		virtual void execute() override;

	  private:
		App::Application::Selection::SelectionModel & _selection;
	};

	class ToggleHydrogensVisibility : public App::Core::Action::BaseAction
	{
	  public:
		explicit ToggleHydrogensVisibility( App::Application::Selection::SelectionModel & p_selection ) :
			_selection( p_selection )
		{
		}

		virtual void execute() override;

	  private:
		App::Application::Selection::SelectionModel & _selection;
	};

	class ToggleIonsVisibility : public App::Core::Action::BaseAction
	{
	  public:
		explicit ToggleIonsVisibility( App::Application::Selection::SelectionModel & p_selection ) :
			_selection( p_selection )
		{
		}

		virtual void execute() override;

	  private:
		App::Application::Selection::SelectionModel & _selection;
	};

	class ToggleTrajectoryPlaying : public App::Core::Action::BaseAction
	{
	  public:
		explicit ToggleTrajectoryPlaying( App::Application::Selection::SelectionModel & p_selection ) :
			_selection( p_selection )
		{
		}

		virtual void execute() override;

	  private:
		App::Application::Selection::SelectionModel & _selection;
	};

	class ChangeRepresentationPreset : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeRepresentationPreset( App::Application::Selection::SelectionModel & p_selection,
											 const int									   p_indexPreset ) :
			_selection( p_selection ),
			_indexPreset( p_indexPreset )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		App::Application::Selection::SelectionModel & _selection;
		const int									  _indexPreset;
	};

	class Copy : public App::Core::Action::BaseAction
	{
	  public:
		explicit Copy( const App::Application::Selection::SelectionModel & p_source,
					   const int p_frame = App::Component::Chemistry::GeneratedMolecule::ALL_FRAMES_INDEX ) :
			_selection( p_source ),
			_frame( p_frame )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override;

	  protected:
		void _copyFrame( const App::Component::Chemistry::Molecule &		 p_source,
						 const App::Application::Selection::SelectionModel & p_selection,
						 const int											 p_frame );

	  private:
		const App::Application::Selection::SelectionModel & _selection;
		const int											_frame;
	};

	class Extract : public App::Core::Action::BaseAction
	{
	  public:
		explicit Extract( App::Application::Selection::SelectionModel & p_source ) : _selection( p_source )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override;

	  private:
		App::Application::Selection::SelectionModel & _selection;
	};
	class Delete : public App::Core::Action::BaseAction
	{
	  public:
		explicit Delete( App::Application::Selection::SelectionModel & p_selection ) : _selection( p_selection )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		App::Application::Selection::SelectionModel & _selection;
	};
	////////////////////////////////////////////////////////////////////////////////////////
} // namespace VTX::App::Action::Selection
#endif
