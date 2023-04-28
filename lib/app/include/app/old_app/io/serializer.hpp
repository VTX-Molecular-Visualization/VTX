#ifndef __VTX_SERIALIZER__
#define __VTX_SERIALIZER__

#include "app/application/render_effect/_fwd.hpp"
#include "app/application/representation/_fwd.hpp"
#include "app/component/chemistry/_fwd.hpp"
#include "app/component/object3d/_fwd.hpp"
#include "app/component/video/_fwd.hpp"
#include "app/core/model/base_model.hpp"
#include "app/old_app/io/writer/writer_chemfiles.hpp"
#include "app/old_app/math/transform.hpp"
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <tuple>
#include <util/math.hpp>

namespace VTX
{
	class VTXApp;

	namespace App::Application
	{
		class Setting;
	}

	namespace Worker
	{
		class BaseThread;
	}

	namespace Color
	{
		class Rgba;
	}

	namespace IO
	{
		class Serializer
		{
		  public:
			Serializer( const VTX::App::Core::Worker::BaseThread * const p_thread = nullptr );

			nlohmann::json serialize( const VTXApp & ) const;
			nlohmann::json serialize( const App::Application::Scene & ) const;
			nlohmann::json serialize( const App::Component::Chemistry::Molecule & ) const;
			nlohmann::json serialize( const App::Component::Video::Path & ) const;
			nlohmann::json serialize( const App::Component::Object3D::Viewpoint & ) const;
			nlohmann::json serialize( const App::Application::Representation::InstantiatedRepresentation & ) const;
			nlohmann::json serialize( const App::Component::Object3D::Label & ) const;

			nlohmann::json serialize( const App::Application::Representation::RepresentationPreset & ) const;
			nlohmann::json serialize( const App::Application::RenderEffect::RenderEffectPreset & ) const;

			nlohmann::json serialize( const Color::Rgba & ) const;
			nlohmann::json serialize( const Math::Transform & ) const;
			template<typename T, glm::qualifier Q>
			nlohmann::json serialize( const glm::vec<2, T, Q> & ) const;
			template<typename T, glm::qualifier Q>
			nlohmann::json serialize( const glm::vec<3, T, Q> & ) const;
			template<typename T, glm::qualifier Q>
			nlohmann::json serialize( const glm::vec<4, T, Q> & ) const;
			template<typename T, glm::qualifier Q>
			nlohmann::json serialize( const glm::qua<T, Q> & ) const;
			nlohmann::json serialize( const App::Application::Setting & ) const;

			nlohmann::json serializeAtomReference( const App::Component::Chemistry::Atom & ) const;

			void deserialize( const nlohmann::json &, const std::tuple<uint, uint, uint> &, VTXApp & ) const;
			void deserialize( const nlohmann::json &,
							  const std::tuple<uint, uint, uint> &,
							  App::Application::Scene & ) const;
			void deserialize( const nlohmann::json &,
							  const std::tuple<uint, uint, uint> &,
							  App::Component::Chemistry::Molecule & ) const;
			void deserialize( const nlohmann::json &, App::Component::Video::Path & ) const;
			void deserialize( const nlohmann::json &, App::Component::Object3D::Viewpoint & ) const;
			void deserialize( const nlohmann::json &,
							  const std::tuple<uint, uint, uint> &,
							  App::Application::Representation::InstantiatedRepresentation & ) const;

			void deserialize( const nlohmann::json &,
							  const std::tuple<uint, uint, uint> &,
							  App::Application::Representation::RepresentationPreset & ) const;
			void deserialize( const nlohmann::json &,
							  const std::tuple<uint, uint, uint> &,
							  App::Application::RenderEffect::RenderEffectPreset & ) const;

			void deserialize( const nlohmann::json &, Color::Rgba & ) const;
			void deserialize( const nlohmann::json &, Math::Transform & ) const;
			template<typename T, glm::qualifier Q>
			void deserialize( const nlohmann::json &, glm::vec<2, T, Q> & ) const;
			template<typename T, glm::qualifier Q>
			void deserialize( const nlohmann::json &, glm::vec<3, T, Q> & ) const;
			template<typename T, glm::qualifier Q>
			void deserialize( const nlohmann::json &, glm::vec<4, T, Q> & ) const;
			template<typename T, glm::qualifier Q>
			void deserialize( const nlohmann::json &, glm::qua<T, Q> & ) const;
			void deserialize( const nlohmann::json &,
							  const std::tuple<uint, uint, uint> &,
							  App::Application::Setting & ) const;

			const App::Component::Chemistry::Atom * deserializeAtomReference( const nlohmann::json & ) const;

			template<typename M, typename = std::enable_if<std::is_base_of<App::Core::Model::BaseModel, M>::value>>
			M * tryDeserializeModel( const nlohmann::json & p_json ) const
			{
				M * const model = VTX::MVC_MANAGER().instantiateModel<M>();

				try
				{
					deserialize( p_json, *model );
				}
				catch ( std::exception e )
				{
					VTX::MVC_MANAGER().deleteModel( model );
					throw e;
				}

				return model;
			}

		  private:
			void _migrate( const nlohmann::json &,
						   const std::tuple<uint, uint, uint> &,
						   App::Application::Representation::InstantiatedRepresentation & ) const;
			void _migrate( const nlohmann::json &,
						   const std::tuple<uint, uint, uint> &,
						   App::Application::Representation::RepresentationPreset & ) const;

			nlohmann::json _serializeMoleculeRepresentations( const App::Component::Chemistry::Molecule &,
															  const VTX::IO::Writer::ChemfilesWriter * ) const;
			nlohmann::json _serializeMoleculeVisibilities( const App::Component::Chemistry::Molecule &,
														   const VTX::IO::Writer::ChemfilesWriter * ) const;

			void _deserializeMoleculeRepresentations( const nlohmann::json &,
													  const std::tuple<uint, uint, uint> &,
													  App::Component::Chemistry::Molecule & ) const;
			void _deserializeMoleculeVisibilities( const nlohmann::json &,
												   App::Component::Chemistry::Molecule & ) const;

			template<typename T>
			T _get( const nlohmann::json & p_json, const std::string & p_key, const T & p_defaultValue = T() ) const
			{
				if ( p_json.contains( p_key ) )
					return p_json.at( p_key ).get<T>();
				else
					return p_defaultValue;
			}
			template<typename T>
			T _getEnum( const nlohmann::json & p_json, const std::string & p_key, const T & p_defaultValue = T() ) const
			{
				if ( p_json.contains( p_key ) )
				{
					try
					{
						std::string value	  = p_json.at( p_key ).get<std::string>();
						auto		valueEnum = magic_enum::enum_cast<T>( value );
						if ( valueEnum.has_value() )
						{
							return valueEnum.value();
						}
						else
						{
							return p_defaultValue;
						}
					}
					catch ( const std::exception & )
					{
						return p_defaultValue;
					}
				}
				else
				{
					return p_defaultValue;
				}
			}

			void _logError( const std::string & p_msg ) const;
			void _logWarning( const std::string & p_msg ) const;
			void _logInfo( const std::string & p_msg ) const;
			void _logDebug( const std::string & p_msg ) const;

			const VTX::App::Core::Worker::BaseThread * const _thread;
		};

	} // namespace IO
} // namespace VTX

#endif
