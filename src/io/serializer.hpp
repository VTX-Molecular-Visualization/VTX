#ifndef __VTX_SERIALIZER__
#define __VTX_SERIALIZER__

#ifdef _MSC_VER
#pragma once
#endif

#include "util/math.hpp"
#include <nlohmann/json.hpp>

namespace VTX
{
	class VTXApp;
	class Setting;

	namespace Object3D
	{
		class Scene;
	}

	namespace Model
	{
		class Molecule;
		class Path;
		class Viewpoint;
	} // namespace Model

	namespace Color
	{
		class Rgb;
	}

	namespace IO
	{
		class Serializer
		{
		  public:
			nlohmann::json serialize( const VTXApp & ) const;
			nlohmann::json serialize( const Object3D::Scene & ) const;
			nlohmann::json serialize( const Model::Molecule & ) const;
			nlohmann::json serialize( const Model::Path & ) const;
			nlohmann::json serialize( const Model::Viewpoint & ) const;
			nlohmann::json serialize( const Color::Rgb & ) const;
			template<int L, typename T, glm::qualifier Q>
			nlohmann::json serialize( const glm::vec<L, T, Q> & ) const;
			template<typename T, glm::qualifier Q>
			nlohmann::json serialize( const glm::qua<T, Q> & ) const;
			nlohmann::json serialize( const Setting & ) const;

			void deserialize( const nlohmann::json &, VTXApp & ) const;
			void deserialize( const nlohmann::json &, Object3D::Scene & ) const;
			void deserialize( const nlohmann::json &, Model::Molecule & ) const;
			void deserialize( const nlohmann::json &, Model::Path & ) const;
			void deserialize( const nlohmann::json &, Model::Viewpoint & ) const;
			void deserialize( const nlohmann::json &, Color::Rgb & ) const;
			template<int L, typename T, glm::qualifier Q>
			void deserialize( const nlohmann::json &, glm::vec<L, T, Q> & ) const;
			template<typename T, glm::qualifier Q>
			void deserialize( const nlohmann::json &, glm::qua<T, Q> & ) const;
			void deserialize( const nlohmann::json &, Setting & ) const;
		};

	} // namespace IO
} // namespace VTX

#endif
