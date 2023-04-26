#ifndef __VTX_SERIALIZER__
#define __VTX_SERIALIZER__

#include "app/old_app/io/writer/writer_chemfiles.hpp"
#include "app/old_app/math/transform.hpp"
#include "app/core/model/base_model.hpp"
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <tuple>
#include <util/math.hpp>

namespace VTX
{
	class VTXApp;
	class Setting;

	namespace Object3D
	{
		class Scene;
	}

	namespace Worker
	{
		class BaseThread;
	}

	namespace Model
	{
		class Molecule;
		class Atom;
		class Label;
		class Path;
		class Viewpoint;
		namespace Representation
		{
			class InstantiatedRepresentation;
			class Representation;
		} // namespace Representation
		namespace Renderer
		{
			class RenderEffectPreset;
		} // namespace Renderer

	}	  // namespace Model

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
			nlohmann::json serialize( const Object3D::Scene & ) const;
			nlohmann::json serialize( const Model::Molecule & ) const;
			nlohmann::json serialize( const Model::Path & ) const;
			nlohmann::json serialize( const Model::Viewpoint & ) const;
			nlohmann::json serialize( const Model::Representation::InstantiatedRepresentation & ) const;
			nlohmann::json serialize( const Model::Label & ) const;

			nlohmann::json serialize( const Model::Representation::Representation & ) const;
			nlohmann::json serialize( const Model::Renderer::RenderEffectPreset & ) const;

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
			nlohmann::json serialize( const Setting & ) const;

			nlohmann::json serializeAtomReference( const Model::Atom & ) const;

			void deserialize( const nlohmann::json &, const std::tuple<uint, uint, uint> &, VTXApp & ) const;
			void deserialize( const nlohmann::json &, const std::tuple<uint, uint, uint> &, Object3D::Scene & ) const;
			void deserialize( const nlohmann::json &, const std::tuple<uint, uint, uint> &, Model::Molecule & ) const;
			void deserialize( const nlohmann::json &, Model::Path & ) const;
			void deserialize( const nlohmann::json &, Model::Viewpoint & ) const;
			void deserialize( const nlohmann::json &,
							  const std::tuple<uint, uint, uint> &,
							  Model::Representation::InstantiatedRepresentation & ) const;

			void deserialize( const nlohmann::json &,
							  const std::tuple<uint, uint, uint> &,
							  Model::Representation::Representation & ) const;
			void deserialize( const nlohmann::json &,
							  const std::tuple<uint, uint, uint> &,
							  Model::Renderer::RenderEffectPreset & ) const;

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
			void deserialize( const nlohmann::json &, const std::tuple<uint, uint, uint> &, Setting & ) const;

			const Model::Atom * deserializeAtomReference( const nlohmann::json & ) const;

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
						   Model::Representation::InstantiatedRepresentation & ) const;
			void _migrate( const nlohmann::json &,
						   const std::tuple<uint, uint, uint> &,
						   Model::Representation::Representation & ) const;

			nlohmann::json _serializeMoleculeRepresentations( const Model::Molecule &,
															  const VTX::IO::Writer::ChemfilesWriter * ) const;
			nlohmann::json _serializeMoleculeVisibilities( const Model::Molecule &,
														   const VTX::IO::Writer::ChemfilesWriter * ) const;

			void _deserializeMoleculeRepresentations( const nlohmann::json &,
													  const std::tuple<uint, uint, uint> &,
													  Model::Molecule & ) const;
			void _deserializeMoleculeVisibilities( const nlohmann::json &, Model::Molecule & ) const;

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
