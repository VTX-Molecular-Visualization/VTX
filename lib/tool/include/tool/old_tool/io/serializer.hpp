#ifndef __VTX_SERIALIZER__
#define __VTX_SERIALIZER__

#include <nlohmann/json.hpp>

namespace VTX
{
	namespace Model
	{
		namespace Measurement
		{
			class Distance;
			class DistanceToCycle;
			class Angle;
			class DihedralAngle;
		} // namespace Measurement

	} // namespace Model

	namespace IO
	{
		class Serializer
		{
		  public:
			nlohmann::json serialize( const Model::Measurement::Distance & ) const;
			nlohmann::json serialize( const Model::Measurement::DistanceToCycle & ) const;
			nlohmann::json serialize( const Model::Measurement::Angle & ) const;
			nlohmann::json serialize( const Model::Measurement::DihedralAngle & ) const;

			void deserialize( const nlohmann::json &, Model::Measurement::Distance & ) const;
			void deserialize( const nlohmann::json &, Model::Measurement::DistanceToCycle & ) const;
			void deserialize( const nlohmann::json &, Model::Measurement::Angle & ) const;
			void deserialize( const nlohmann::json &, Model::Measurement::DihedralAngle & ) const;
		};

	} // namespace IO
} // namespace VTX

#endif
