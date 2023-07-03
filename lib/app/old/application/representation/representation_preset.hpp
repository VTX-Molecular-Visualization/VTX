#ifndef __VTX_APP_OLD_APPLICATION_REPRESENTATION_REPRESENTATION_PRESET__
#define __VTX_APP_OLD_APPLICATION_REPRESENTATION_REPRESENTATION_PRESET__

#include "app/old/application/representation/enum_representation.hpp"
#include "app/old/application/representation/representation_data.hpp"
#include "app/old/core/model/base_model.hpp"
#include <string>
#include <util/color/rgba.hpp>
#include <util/math.hpp>

namespace VTX::App::Old::Application::Representation
{
	class RepresentationPreset : public App::Old::Core::Model::BaseModel
	{
		VTX_MODEL

	  public:
		const std::string & getName() const { return _name; };
		void				setName( const std::string & p_name );

		const Util::Color::Rgba & getColor() const { return _color; }
		Util::Color::Rgba &		  getColor() { return _color; }
		void					  setColor( const Util::Color::Rgba & p_color );

		const bool hasQuickAccess() const { return _quickAccess; };
		void	   setQuickAccess( const bool p_quickAccess );

		void changeRepresentationType( const REPRESENTATION_ENUM & p_newType, const bool p_notify = true );

		const RepresentationData & getData() const { return *_data; }
		RepresentationData &	   getData() { return *_data; }

		const REPRESENTATION_ENUM & getRepresentationType() const { return _data->getRepresentationType(); }

		void copyDataFrom( const RepresentationPreset & p_source );

	  protected:
		RepresentationPreset( const REPRESENTATION_ENUM & p_type );
		RepresentationPreset();
		~RepresentationPreset();

	  private:
		std::string		  _name;
		Util::Color::Rgba _color;
		bool			  _quickAccess;

		RepresentationData * _data = nullptr;
	};

} // namespace VTX::App::Old::Application::Representation
#endif
