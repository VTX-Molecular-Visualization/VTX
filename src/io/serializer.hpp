#ifndef __VTX_SERIALIZER__
#define __VTX_SERIALIZER__

#include "io/writer/writer_chemfiles.hpp"
#include "math/transform.hpp"
#include "util/math.hpp"
#include <nlohmann/json.hpp>
#include <string>

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
		class Rgb;
	}

	namespace IO
	{
		class Serializer
		{
		  public:
			Serializer( const Worker::BaseThread * const p_thread = nullptr );

			nlohmann::json serialize( const VTXApp & ) const;
			nlohmann::json serialize( const Object3D::Scene & ) const;
			nlohmann::json serialize( const Model::Molecule & ) const;
			nlohmann::json serialize( const Model::Path & ) const;
			nlohmann::json serialize( const Model::Viewpoint & ) const;
			nlohmann::json serialize( const Model::Representation::InstantiatedRepresentation & ) const;

			nlohmann::json serialize( const Model::Representation::Representation & ) const;
			nlohmann::json serialize( const Model::Renderer::RenderEffectPreset & ) const;

			nlohmann::json serialize( const Color::Rgb & ) const;
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

			void deserialize( const nlohmann::json &, VTXApp & ) const;
			void deserialize( const nlohmann::json &, Object3D::Scene & ) const;
			void deserialize( const nlohmann::json &, Model::Molecule & ) const;
			void deserialize( const nlohmann::json &, Model::Path & ) const;
			void deserialize( const nlohmann::json &, Model::Viewpoint & ) const;
			void deserialize( const nlohmann::json &, Model::Representation::InstantiatedRepresentation & ) const;

			void deserialize( const nlohmann::json &, Model::Representation::Representation & ) const;
			void deserialize( const nlohmann::json &, Model::Renderer::RenderEffectPreset & ) const;

			void deserialize( const nlohmann::json &, Color::Rgb & ) const;
			void deserialize( const nlohmann::json &, Math::Transform & ) const;
			template<typename T, glm::qualifier Q>
			void deserialize( const nlohmann::json &, glm::vec<2, T, Q> & ) const;
			template<typename T, glm::qualifier Q>
			void deserialize( const nlohmann::json &, glm::vec<3, T, Q> & ) const;
			template<typename T, glm::qualifier Q>
			void deserialize( const nlohmann::json &, glm::vec<4, T, Q> & ) const;
			template<typename T, glm::qualifier Q>
			void deserialize( const nlohmann::json &, glm::qua<T, Q> & ) const;
			void deserialize( const nlohmann::json &, Setting & ) const;

		  private:
			nlohmann::json _serializeMoleculeRepresentations( const Model::Molecule &,
															  const VTX::IO::Writer::ChemfilesWriter * ) const;
			nlohmann::json _serializeMoleculeVisibilities( const Model::Molecule &,
														   const VTX::IO::Writer::ChemfilesWriter * ) const;

			void _deserializeMoleculeRepresentations( const nlohmann::json &, Model::Molecule & ) const;
			void _deserializeMoleculeVisibilities( const nlohmann::json &, Model::Molecule & ) const;

			template<typename T>
			T _get( const nlohmann::json & p_json, const std::string & p_key, const T & p_defaultValue = T() ) const
			{
				if ( p_json.contains( p_key ) )
					return p_json.at( p_key ).get<T>();
				else
					return p_defaultValue;
			}

			void _logError( const std::string & p_msg ) const;
			void _logWarning( const std::string & p_msg ) const;
			void _logInfo( const std::string & p_msg ) const;
			void _logDebug( const std::string & p_msg ) const;

			const Worker::BaseThread * const _thread;
		};

	} // namespace IO
} // namespace VTX

#endif
