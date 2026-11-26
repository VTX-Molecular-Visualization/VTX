#include "app/extractor/mesh.hpp"
#include "app/action/camera.hpp"
#include "app/events.hpp"
#include "app/generic/name.hpp"
#include "app/helper/aabb.hpp"
#include "app/services.hpp"
#include <core/struct/mesh.hpp>
#include <fmt/format.h>
#include <io/mesh_reader.hpp>
#include <util/action_manager.hpp>
#include <util/event_hub.hpp>
#include <util/math/aabb.hpp>
#include <util/math/transform.hpp>
#include <util/thread/thread_manager.hpp>

namespace VTX::App::Extractor
{
	namespace
	{
		struct Pending
		{
			FilePath						sourcePath;
			std::vector<Core::Struct::Mesh> meshes;
		};

		void deliver( Pending & p_data )
		{
			auto & reg = REG();

			Util::Math::AABB sceneAabb;

			for ( size_t meshIndex = 0; meshIndex < p_data.meshes.size(); ++meshIndex )
			{
				Core::Struct::Mesh & mesh = p_data.meshes[ meshIndex ];
				Util::Math::AABB	 aabb;
				for ( const Vec3f & vertex : mesh.vertices )
				{
					aabb.extend( vertex );
					sceneAabb.extend( vertex );
				}

				const Entity entity = reg.create();
				reg.emplace<Core::Struct::Mesh>( entity, std::move( mesh ) );
				const std::string name = p_data.meshes.size() == 1
											 ? p_data.sourcePath.stem().string()
											 : fmt::format( "{}_{}", p_data.sourcePath.stem().string(), meshIndex + 1 );
				reg.emplace<Generic::Name>( entity, name );
				reg.emplace<Util::Math::Transform>( entity );
				reg.emplace<Util::Math::AABB>( entity, aabb );
				Helper::AABB::invalidateScene();

				// Trigger mesh load.
				HUB().trigger<Events::MeshLoad>( { entity } );
			}

			ACTION().execute<Action::Camera::Orient>( sceneAabb );
		}
	} // namespace

	Mesh::Mesh( FilePath p_path ) : _sourcePath( std::move( p_path ) ) {}

	uint Mesh::operator()(
		const Util::Thread::StopToken &				p_stopToken,
		const Util::Thread::OptionalThreadReference p_thread
	)
	{
		if ( p_thread )
		{
			p_thread->get().setProgressText( fmt::format( "Reading {}...", _sourcePath.filename().string() ) );
		}

		Pending pendingData { .sourcePath = std::move( _sourcePath ) };
		IO::MeshReader( pendingData.sourcePath, p_stopToken ).get( pendingData.meshes );
		if ( p_stopToken.stop_requested() )
		{
			return 0;
		}

		THREAD().synchronize( [ &pendingData ]() { deliver( pendingData ); } );
		return 0;
	}
} // namespace VTX::App::Extractor
