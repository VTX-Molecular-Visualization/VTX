#ifndef __VTX_APP_ACTION_SCENE__
#define __VTX_APP_ACTION_SCENE__

#include "app/component/chemistry/_fwd.hpp"
#include "app/component/render/_fwd.hpp"
#include "app/core/action/base_action.hpp"
#include <map>
#include <string>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Action::Scene
{
	class LoadSystem final : public App::Core::Action::BaseAction
	{
	  public:
		explicit LoadSystem( const FilePath & p_path ) : _path( p_path ) {}
		explicit LoadSystem( const FilePath & p_path, std::string * const p_buffer ) :
			_path( p_path ), _buffer( p_buffer )
		{
		}

		void execute() override;

	  private:
		const FilePath		_path;
		std::string * const _buffer = nullptr;

		std::vector<Component::Chemistry::System *> _trajectoryTargets;
	};

	// TODO: move to other file?
	class DownloadSystem final : public App::Core::Action::BaseAction
	{
	  public:
		explicit DownloadSystem( const std::string_view p_url, const FilePath & p_filename ) :
			_url( p_url ), _filename( p_filename.filename() )
		{
		}

		void execute() override;

	  private:
		const std::string_view _url;
		const FilePath		   _filename;
	};

	class CreateViewpoint final : public App::Core::Action::BaseAction
	{
	  public:
		explicit CreateViewpoint();
		explicit CreateViewpoint( const Component::Render::Camera & p_fromCamera );
		explicit CreateViewpoint( const Vec3f & p_position, const Quatf & p_rotation ) :
			_position( p_position ), _rotation( p_rotation )
		{
		}

		void execute() override;

	  private:
		Vec3f _position;
		Quatf _rotation;
	};

} // namespace VTX::App::Action::Scene
#endif
