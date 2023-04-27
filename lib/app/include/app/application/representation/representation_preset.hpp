#ifndef __VTX_APP_APPLICATION_REPRESENTATION_REPRESENTATION_PRESET__
#define __VTX_APP_APPLICATION_REPRESENTATION_REPRESENTATION_PRESET__

#include "app/application/representation/enum_representation.hpp"
#include "app/application/representation/representation_data.hpp"
#include "app/core/model/base_model.hpp"
#include "app/old_app/color/rgba.hpp"
#include "app/old_app/generic/base_colorable.hpp"
#include <string>
#include <util/math.hpp>

namespace VTX::App::Application::Representation
{
	class RepresentationPreset : public App::Core::Model::BaseModel
	{
		VTX_MODEL

	  public:
		const std::string & getName() const { return _name; };
		void				setName( const std::string & p_name );

		const Color::Rgba & getColor() const { return _color; }
		Color::Rgba &		getColor() { return _color; }
		void				setColor( const Color::Rgba & p_color );

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
		std::string _name;
		Color::Rgba _color;
		bool		_quickAccess;

		RepresentationData * _data = nullptr;
	};

} // namespace VTX::App::Application::Representation
#endif