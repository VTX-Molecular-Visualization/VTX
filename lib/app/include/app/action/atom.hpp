#ifndef __VTX_APP_ACTION_ATOM__
#define __VTX_APP_ACTION_ATOM__

#include "app/action.hpp"
#include "app/action/visible.hpp"
#include "app/core/action/base_action.hpp"
#include "app/model/atom.hpp"
#include "app/old_app/color/rgba.hpp"

namespace VTX::App::Action::Atom
{
	class ChangeColor : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeColor( Model::Atom & p_atom, const Color::Rgba & p_color ) : _atom( p_atom ), _color( p_color )
		{
		}

		virtual void execute() override;

	  private:
		Model::Atom &	  _atom;
		const Color::Rgba _color;
	};

	class ChangeVisibility : public Visible::ChangeVisibility
	{
	  public:
		explicit ChangeVisibility( Model::Atom & p_atom, const VISIBILITY_MODE p_mode ) :
			Visible::ChangeVisibility( p_atom, p_mode )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;
	};

	class Delete : public App::Core::Action::BaseAction
	{
	  public:
		explicit Delete( Model::Atom & p_atom ) : _atom( p_atom )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		Model::Atom & _atom;
	};

	class Copy : public App::Core::Action::BaseAction
	{
	  public:
		explicit Copy( const Model::Atom & p_target ) : _target( p_target )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override;

	  private:
		const Model::Atom & _target;
	};

	class Extract : public App::Core::Action::BaseAction
	{
	  public:
		explicit Extract( const Model::Atom & p_target ) : _target( p_target )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override;

	  private:
		const Model::Atom & _target;
	};
} // namespace VTX::App::Action::Atom
#endif
