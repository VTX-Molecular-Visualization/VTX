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
	class LoadMolecule final : public App::Core::Action::BaseAction
	{
	  public:
		explicit LoadMolecule( const FilePath & p_path ) { _paths.emplace_back( p_path ); }
		// explicit Open( const std::vector<FilePath> & p_paths ) : _paths( p_paths ) {}
		// explicit Open( const std::map<FilePath, std::string *> & p_buffers ) : _buffers( p_buffers ) {}
		// explicit Open( const FilePath & p_trajectoryPath, Model::Chemistry::Molecule & p_target )
		//{
		//	_trajectoryTargets.emplace_back( &p_target );
		//	_paths.emplace_back( p_trajectoryPath );
		// }
		// explicit Open( const FilePath &									 p_trajectoryPath,
		//			   const std::vector<Model::Chemistry::Molecule *> & p_targets ) :
		//	_trajectoryTargets( p_targets )
		//{
		//	_paths.emplace_back( p_trajectoryPath );
		// }

		void execute() override;

	  private:
		std::vector<FilePath>			  _paths   = std::vector<FilePath>();
		std::map<FilePath, std::string *> _buffers = std::map<FilePath, std::string *>();

		std::vector<Component::Chemistry::Molecule *> _trajectoryTargets
			= std::vector<Component::Chemistry::Molecule *>();
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
