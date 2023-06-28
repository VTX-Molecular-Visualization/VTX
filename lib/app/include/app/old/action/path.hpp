#ifndef __VTX_APP_OLD_ACTION_PATH__
#define __VTX_APP_OLD_ACTION_PATH__

#include "app/old/action.hpp"
#include "app/old/core/action/base_action.hpp"
#include "app/old/component/video/path.hpp"
#include "app/old/component/video/enum_path.hpp"
// #include "app/old/render/worker/snapshoter.hpp"

namespace VTX::App::Old::Action::Path
{
	class ChangeDuration : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeDuration( App::Old::Component::Video::Path & p_path, const float p_duration ) :
			_path( p_path ), _duration( p_duration )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
	 App::Old::Component::Video::Path & _path;
		const float	  _duration;
	};

	class ChangeDurationMode : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeDurationMode( App::Old::Component::Video::Path & p_path, const App::Old::Component::Video::PATH_DURATION_MODE p_mode ) :
			_path( p_path ), _mode( p_mode )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
	 App::Old::Component::Video::Path &				   _path;
		const App::Old::Component::Video::PATH_DURATION_MODE _mode;
	};

	class ChangeInterpolationMode : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeInterpolationMode( App::Old::Component::Video::Path & p_path, const App::Old::Component::Video::PATH_INTERPOLATION_MODE p_mode ) :
			_path( p_path ), _mode( p_mode )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
	 App::Old::Component::Video::Path &						_path;
		const App::Old::Component::Video::PATH_INTERPOLATION_MODE _mode;
	};

	class Play : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Play( App::Old::Component::Video::Path * const p_path ) : _path( p_path ) {}
		virtual void execute() override;

	  private:
	 App::Old::Component::Video::Path * const _path;
	};

	// class ExportVideo : public App::Old::Core::Action::BaseAction
	//{
	//   public:
	//	explicit ExportVideo( App::Old::Component::Video::Path * const p_path, Worker::Snapshoter::MODE p_mode ) :
	//		_path( p_path ), _mode( p_mode )
	//	{
	//	}

	//	virtual void execute() override;

	//  private:
	//	const Worker::Snapshoter::MODE _mode;
	// App::Old::Component::Video::Path * const			   _path;
	//};

	class ChangeIsLooping : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeIsLooping( App::Old::Component::Video::Path & p_path, const bool p_isLooping ) :
			_path( p_path ), _isLooping( p_isLooping )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
	 App::Old::Component::Video::Path & _path;
		const bool	  _isLooping;
	};

	class SetSelected : public App::Old::Core::Action::BaseAction
	{
	  public:
	  private:
	};

	class Clear : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Clear( App::Old::Component::Video::Path & p_path ) : _path( p_path )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
	 App::Old::Component::Video::Path & _path;
	};

} // namespace VTX::App::Old::Action::Path
#endif
