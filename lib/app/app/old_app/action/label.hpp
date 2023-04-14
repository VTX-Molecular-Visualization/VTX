#ifndef __VTX_ACTION_LABEL__
#define __VTX_ACTION_LABEL__

#include "old_app/action/base_action.hpp"
#include "old_app/color/rgba.hpp"
#include "old_app/model/label.hpp"
#include "old_app/model/selection.hpp"
#include "old_app/mvc/mvc_manager.hpp"
#include "old_app/object3d/helper/aabb.hpp"
#include "old_app/object3d/scene.hpp"
#include "old_app/selection/selection_manager.hpp"
#include "old_app/vtx_app.hpp"
#include <unordered_set>
#include <vector>

namespace VTX::Action::Label
{
	class SetEnable : public BaseAction
	{
	  public:
		explicit SetEnable( Model::Label & p_label, const bool p_enabled ) :
			_labels( { &p_label } ), _enabled( p_enabled )
		{
		}
		explicit SetEnable( const std::unordered_set<Model::Label *> & p_labels, const bool p_enabled ) :
			_labels( p_labels ), _enabled( p_enabled )
		{
		}

		virtual void execute() override
		{
			for ( Model::Label * label : _labels )
				label->setEnable( _enabled );
		}

	  private:
		const bool						   _enabled;
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

	class ChangeColor : public BaseAction
	{
	  public:
		explicit ChangeColor( Model::Label & p_label, const Color::Rgba & p_color ) :
			_labels( { &p_label } ), _color( p_color )
		{
		}
		explicit ChangeColor( const std::unordered_set<Model::Label *> & p_labels, const Color::Rgba & p_color ) :
			_labels( p_labels ), _color( p_color )
		{
		}

		virtual void execute() override
		{
			for ( Model::Label * const label : _labels )
			{
				label->setColor( _color );
			}
		}

	  private:
		const std::unordered_set<Model::Label *> _labels;
		const Color::Rgba &						 _color;
	};

} // namespace VTX::Action::Label
#endif
