#include "app/application/representation/representation_data_generator.hpp"
#include "app/application/representation/data/ball_and_stick.hpp"
#include "app/application/representation/data/ball_stick_and_cartoon.hpp"
#include "app/application/representation/data/cartoon.hpp"
#include "app/application/representation/data/sas.hpp"
#include "app/application/representation/data/ses.hpp"
#include "app/application/representation/data/stick.hpp"
#include "app/application/representation/data/stick_and_cartoon.hpp"
#include "app/application/representation/data/trace.hpp"
#include "app/application/representation/data/van_der_waals.hpp"
#include "app/application/representation/representation_data.hpp"

namespace VTX::App::Application::Representation
{
	RepresentationData * RepresentationDataGenerator::instantiate( const REPRESENTATION_ENUM & p_representationEnum )
	{
		RepresentationData * res;

		switch ( p_representationEnum )
		{
		case REPRESENTATION_ENUM::STICK: res = new Data::Stick(); break;
		case REPRESENTATION_ENUM::STICK_AND_CARTOON: res = new Data::StickAndCartoon(); break;
		case REPRESENTATION_ENUM::BALL_AND_STICK: res = new Data::BallAndStick(); break;
		case REPRESENTATION_ENUM::BALL_AND_STICK_AND_CARTOON: res = new Data::BallStickAndCartoon(); break;
		case REPRESENTATION_ENUM::VAN_DER_WAALS: res = new Data::VanDerWaals(); break;
		case REPRESENTATION_ENUM::SAS:
			res = new Data::Sas();
			break;
			// !V0.1
			// case REPRESENTATION_ENUM::TRACE: res = new Trace();
			// break;
		case REPRESENTATION_ENUM::CARTOON: res = new Data::Cartoon(); break;
		case REPRESENTATION_ENUM::SES: res = new Data::Ses(); break;
		default: res = nullptr; break;
		}

		return res;
	}
} // namespace VTX::App::Application::Representation
