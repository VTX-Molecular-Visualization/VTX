#ifndef __VTX_APP_OLD_ACTION_LABEL__
#define __VTX_APP_OLD_ACTION_LABEL__

#include "app/old/action.hpp"
#include "app/old/core/action/base_action.hpp"
#include "app/old/component/object3d/label.hpp"
#include <util/color/rgba.hpp>
#include <string>
#include <unordered_set>
#include <vector>

namespace VTX::App::Old::Action::Label
{
	class SetEnable : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit SetEnable( App::Old::Component::Object3D::Label & p_label, const bool p_enabled ) :
			_labels( { &p_label } ), _enabled( p_enabled )
		{
		}
		explicit SetEnable( const std::unordered_set<App::Old::Component::Object3D::Label *> & p_labels, const bool p_enabled ) :
			_labels( p_labels ), _enabled( p_enabled )
		{
		}

		virtual void execute() override;

	  private:
		const bool						   _enabled;
		std::unordered_set<App::Old::Component::Object3D::Label *> _labels;
	};

	class Delete : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Delete( App::Old::Component::Object3D::Label * const p_label ) : _labels( { p_label } ) {}
		explicit Delete( const std::unordered_set<App::Old::Component::Object3D::Label *> & p_labels )
		{
			_labels.reserve( p_labels.size() );
			for ( App::Old::Component::Object3D::Label * label : p_labels )
				_labels.emplace_back( label );
		}

		virtual void execute() override;

	  private:
		std::vector<App::Old::Component::Object3D::Label *> _labels;
	};
	class EnableAutoNaming : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit EnableAutoNaming( App::Old::Component::Object3D::Label & p_label, const bool p_enable ) :
			_labels( { &p_label } ), _enable( p_enable )
		{
		}
		explicit EnableAutoNaming( const std::unordered_set<App::Old::Component::Object3D::Label *> & p_labels, const bool p_enable ) :
			_labels( p_labels ), _enable( p_enable )
		{
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<App::Old::Component::Object3D::Label *> & _labels;
		const bool								   _enable;
	};

	class Rename : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Rename( App::Old::Component::Object3D::Label & p_label, const std::string & p_newName ) :
			_labels( { &p_label } ), _name( p_newName )
		{
		}
		explicit Rename( const std::unordered_set<App::Old::Component::Object3D::Label *> & p_labels, const std::string & p_name ) :
			_labels( p_labels ), _name( p_name )
		{
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<App::Old::Component::Object3D::Label *> _labels;
		const std::string						 _name;
	};

	class ChangeColor : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeColor( App::Old::Component::Object3D::Label & p_label, const Util::Color::Rgba & p_color ) :
			_labels( { &p_label } ), _color( p_color )
		{
		}
		explicit ChangeColor( const std::unordered_set<App::Old::Component::Object3D::Label *> & p_labels, const Util::Color::Rgba & p_color ) :
			_labels( p_labels ), _color( p_color )
		{
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<App::Old::Component::Object3D::Label *> _labels;
		const Util::Color::Rgba &						 _color;
	};

} // namespace VTX::App::Old::Action::Label
#endif
