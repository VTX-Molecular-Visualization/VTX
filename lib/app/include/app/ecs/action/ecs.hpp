#ifndef __VTX_APP_ECS_ACTION_TEST__
#define __VTX_APP_ECS_ACTION_TEST__

#include "app/model/chemistry/molecule.hpp"
#include "app/old/core/action/base_action.hpp"
#include <core/old/struct/molecule.hpp>
#include <map>
#include <string>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::ECS::Action
{
	class Open : public App::Old::Core::Action::BaseAction
	{
	  private:
		// class LoadSceneClass
		//{
		//   public:
		//	LoadSceneClass( const std::vector<FilePath> & p_paths ) : _paths( p_paths ) {};
		//	void _loadScene();

		//  private:
		//	std::vector<FilePath> _paths;
		//};

	  public:
		explicit Open( const FilePath & p_path ) { _paths.emplace_back( p_path ); }
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

		virtual void execute() override;

	  private:
		std::vector<FilePath>			  _paths = std::vector<FilePath>();
		std::map<FilePath, std::string *> _buffers;

		std::vector<Model::Chemistry::Molecule *> _trajectoryTargets = std::vector<Model::Chemistry::Molecule *>();
	};

} // namespace VTX::App::ECS::Action
#endif
