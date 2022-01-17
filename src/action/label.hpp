#ifndef __VTX_ACTION_LABEL__
#define __VTX_ACTION_LABEL__

#include "action/base_action.hpp"
#include "math/aabb.hpp"
#include "model/label.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/scene.hpp"
#include "selection/selection_manager.hpp"
#include "state/state_machine.hpp"
#include "state/visualization.hpp"
#include "vtx_app.hpp"
#include <unordered_set>
#include <vector>

namespace VTX::Action::Label
{
	class Orient : public BaseAction
	{
	  public:
		explicit Orient( Model::Label * const p_label ) : _labels( { p_label } ) {}
		explicit Orient( const std::unordered_set<Model::Label *> & p_labels ) : _labels( p_labels ) {}

		virtual void execute() override
		{
			Math::AABB aabb = Math::AABB();

			for ( Model::Label * label : _labels )
				aabb.extend( label->getAABB() );

			VTXApp::get()
				.getStateMachine()
				.getState<State::Visualization>( ID::State::VISUALIZATION )
				->orientCameraController( aabb );
		}

	  private:
		std::unordered_set<Model::Label *> _labels;
	};

	class Delete : public BaseAction
	{
	  public:
		explicit Delete( Model::Label * const p_label ) : _labels( { p_label } ) {}
		explicit Delete( const std::unordered_set<Model::Label *> & p_labels )
		{
			_labels.reserve( p_labels.size() );
			for ( Model::Label * label : p_labels )
				_labels.emplace_back( label );
		}

		virtual void execute() override
		{
			VTX::Selection::SelectionManager::get().getSelectionModel().unselectModels<Model::Label>( _labels );

			for ( Model::Label * label : _labels )
			{
				VTXApp::get().getScene().removeLabel( label );
				MVC::MvcManager::get().deleteModel<Model::Label>( label );
			}
		}

	  private:
		std::vector<Model::Label *> _labels;
	};
	class EnableAutoNaming : public BaseAction
	{
	  public:
		explicit EnableAutoNaming( Model::Label & p_label, const bool p_enable ) :
			_labels( { &p_label } ), _enable( p_enable )
		{
		}
		explicit EnableAutoNaming( const std::unordered_set<Model::Label *> & p_labels, const bool p_enable ) :
			_labels( p_labels ), _enable( p_enable )
		{
		}

		virtual void execute() override
		{
			for ( Model::Label * const label : _labels )
				label->setAutoNaming( _enable );
		}

	  private:
		const std::unordered_set<Model::Label *> & _labels;
		const bool								   _enable;
	};

	class Rename : public BaseAction
	{
	  public:
		explicit Rename( Model::Label & p_label, const std::string & p_newName ) :
			_labels( { &p_label } ), _name( p_newName )
		{
		}
		explicit Rename( const std::unordered_set<Model::Label *> & p_labels, const std::string & p_name ) :
			_labels( p_labels ), _name( p_name )
		{
		}

		virtual void execute() override
		{
			for ( Model::Label * const label : _labels )
			{
				label->setAutoNaming( false );
				label->setName( _name );
			}
		}

	  private:
		const std::unordered_set<Model::Label *> _labels;
		const std::string						 _name;
	};
} // namespace VTX::Action::Label
#endif
