#include "app/action/preset.hpp"
#include "app/session.hpp"

namespace VTX::App::Action::Preset
{
	void LoadAll::execute()
	{
		const Session & session = SESSION();
		ACTION().execute<Load<Renderer::Color::Layout>>(
			session.getDefaultColorLayoutsDir(), session.getColorLayoutsDir()
		);
		ACTION().execute<Load<Renderer::Representation>>(
			session.getDefaultRepresentationsDir(), session.getRepresentationsDir()
		);
		ACTION().execute<Load<Renderer::Material>>( session.getDefaultMaterialsDir(), session.getMaterialsDir() );
		ACTION().execute<Load<Renderer::GraphicsConfig>>(
			session.getDefaultGraphicsConfigsDir(), session.getGraphicsConfigsDir()
		);
	}

	void Import::execute( const FilePath & p_path )
	{
		try
		{
			Util::Json::Document document = Util::Json::read( p_path );
			const std::string	 type	  = document.at( "type" ).get<std::string>();
			const Session &		 session  = SESSION();

			if ( type == Helper::Preset::TYPE_NAME<Renderer::Representation> )
			{
				_import<Renderer::Representation>( document, p_path, session.getRepresentationsDir() );
			}
			else if ( type == Helper::Preset::TYPE_NAME<Renderer::Color::Layout> )
			{
				_import<Renderer::Color::Layout>( document, p_path, session.getColorLayoutsDir() );
			}
			else if ( type == Helper::Preset::TYPE_NAME<Renderer::GraphicsConfig> )
			{
				_import<Renderer::GraphicsConfig>( document, p_path, session.getGraphicsConfigsDir() );
			}
			else if ( type == Helper::Preset::TYPE_NAME<Renderer::Material> )
			{
				_import<Renderer::Material>( document, p_path, session.getMaterialsDir() );
			}
			else
			{
				throw VTXException( "unknown preset type \"{}\"", type );
			}
		}
		catch ( const std::exception & p_e )
		{
			throw VTXException( "Can not import preset {}: {}", p_path.string(), p_e.what() );
		}
	}

	template<typename T>
	void Import::_import(
		Util::Json::Document &			p_document,
		const FilePath &				p_path,
		[[maybe_unused]] const FilePath & p_userDirectory
	)
	{
		T data = Detail::read<T>( p_document );

		const std::string name = p_document.at( "name" ).get<std::string>();

		// std::filesystem::create_directories( p_userDirectory );
		// FilePath destination = p_userDirectory / p_path.filename();
		// Util::Filesystem::generateUniqueFileName( destination );
		// Util::Json::write( destination, p_document );
		// VTX_INFO( "Preset \"{}\" imported to {}", name, destination.string() );

		// const Entity entity = Detail::create<T>( name, std::move( data ), destination, false );
		const Entity entity = Detail::create<T>( name, std::move( data ), std::nullopt, false );
		VTX_INFO( "Preset \"{}\" imported from {}", name, p_path.string() );
		ACTION().execute<Apply<T>>( entity );
	}
} // namespace VTX::App::Action::Preset
