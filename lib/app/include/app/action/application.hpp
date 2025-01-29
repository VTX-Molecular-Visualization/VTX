#ifndef __VTX_APP_ACTION_APPLICATION__
#define __VTX_APP_ACTION_APPLICATION__

#include "app/core/action/base_action.hpp"
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Action::Application
{
	class NewScene final : public Core::Action::BaseAction
	{
	  public:
		void execute() override;
	};
	class OpenScene final : public Core::Action::BaseAction
	{
	  public:
		explicit OpenScene( const FilePath & p_path ) : _path( p_path ) {}
		void execute() override;

	  private:
		FilePath _path = FilePath();
	};
	class SaveScene final : public Core::Action::BaseAction
	{
	  public:
		explicit SaveScene() : _path( "" ) /*, _callback( nullptr )*/ {}
		explicit SaveScene( const FilePath & p_path ) : _path( p_path ) /*, _callback( nullptr )*/ {}
		// explicit SaveScene( const FilePath & p_path, VTX::App::Old::Core::Worker::CallbackThread * const p_callback )
		// : 	_path( p_path ), _callback( p_callback )
		//{
		// }

		void execute() override;

	  private:
		const FilePath _path;
		// VTX::App::Old::Core::Worker::CallbackThread * const _callback;
	};
	class ClearScene final : public Core::Action::BaseAction
	{
	  public:
		void execute() override;
	};

	class LoadSettings final : public Core::Action::BaseAction
	{
	  public:
		explicit LoadSettings();
		explicit LoadSettings( const FilePath & p_path ) : _path( p_path ) {}

		void execute() override;

	  private:
		const FilePath _path;
	};
	class SaveSettings final : public Core::Action::BaseAction
	{
	  public:
		explicit SaveSettings();
		explicit SaveSettings( const FilePath & p_path ) : _path( p_path ) {}

		void execute() override;

	  private:
		const FilePath _path;
	};
	class ReloadSettings final : public Core::Action::BaseAction
	{
	  public:
		explicit ReloadSettings() {}
		void execute() override;
	};
	class ResetSettings final : public Core::Action::BaseAction
	{
	  public:
		explicit ResetSettings() {}
		void execute() override;
	};

	class Open final : public Core::Action::BaseAction
	{
	  public:
		explicit Open( const FilePath & p_path ) { _paths.emplace_back( p_path ); }
		// explicit Open( const std::vector<FilePath> & p_paths ) : _paths( p_paths ) {}
		// explicit Open( const std::map<FilePath, std::string *> & p_buffers ) : _buffers( p_buffers ) {}
		// explicit Open( const FilePath & p_trajectoryPath, Model::Chemistry::System & p_target )
		//{
		//	_trajectoryTargets.emplace_back( &p_target );
		//	_paths.emplace_back( p_trajectoryPath );
		// }
		// explicit Open( const FilePath &									 p_trajectoryPath,
		//			   const std::vector<Model::Chemistry::System *> & p_targets ) :
		//	_trajectoryTargets( p_targets )
		//{
		//	_paths.emplace_back( p_trajectoryPath );
		// }

		void execute() override;

	  private:
		std::vector<FilePath> _paths = std::vector<FilePath>();
		// std::map<FilePath, std::string *> _buffers = std::map<FilePath, std::string *>();

		// std::vector<Component::Chemistry::System *> _trajectoryTargets
		//	= std::vector<Component::Chemistry::System *>();
	};

	class Quit final : public Core::Action::BaseAction
	{
	  public:
		Quit() {};
		void execute() override;
	};

	class Resize final : public Core::Action::BaseAction
	{
	  public:
		Resize( const size_t p_width, const size_t p_height ) : _width( p_width ), _height( p_height ) {}
		void execute() override;

	  private:
		const size_t _width;
		const size_t _height;
	};

	/**
	 * @brief Move the camera in a straight line from the current position to the one in arguments
	 */
	class MoveCameraPosition final : public Core::Action::BaseAction
	{
	  public:
		inline MoveCameraPosition( float p_x, float p_y, float p_z, float p_travelTime = 1.f ) :
			_x( std::move( p_x ) ), _y( std::move( p_y ) ), _z( std::move( p_z ) ),
			_travelTime( std::move( p_travelTime ) )
		{
		}
		void execute() override;

	  private:
		float _x		  = 0.f;
		float _y		  = 0.f;
		float _z		  = 0.f;
		float _travelTime = 1.f;
	};

	/**
	 * @brief Set the camera position to the target location, effectively "teleporting" the point of view
	 */
	class SetCameraPosition final : public Core::Action::BaseAction
	{
	  public:
		inline SetCameraPosition( float p_x, float p_y, float p_z ) :
			_x( std::move( p_x ) ), _y( std::move( p_y ) ), _z( std::move( p_z ) )
		{
		}
		void execute() override;

	  private:
		float _x = 0.f;
		float _y = 0.f;
		float _z = 0.f;
	};

} // namespace VTX::App::Action::Application
#endif
