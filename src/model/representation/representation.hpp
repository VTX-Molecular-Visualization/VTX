#ifndef __VTX_MODEL_REPRESENTATION__
#define __VTX_MODEL_REPRESENTATION__

#ifdef _MSC_VER
#pragma once
#endif

#include "color/rgb.hpp"
#include "generic/base_colorable.hpp"
#include "model/base_model.hpp"
#include "representation_data.hpp"
#include "representation_enum.hpp"
#include "util/math.hpp"

namespace VTX::Model::Representation
{
	class Representation : public BaseModel
	{
		VTX_MODEL

	  public:
		const std::string & getName() const { return _name; };
		void				setName( const std::string & p_name );

		const std::string & getIconPath() const { return _iconPath; };
		void				setIconPath( const std::string & p_iconPath );

		const Color::Rgb & getColor() const { return _color; }
		Color::Rgb &	   getColor() { return _color; }
		void			   setColor( const Color::Rgb & p_color );

		const bool hasQuickAccess() const { return _quickAccess; };
		void	   setQuickAccess( const bool p_quickAccess );

		void changeRepresentationType( const Generic::REPRESENTATION & p_newType, const bool p_notify = true );

		const RepresentationData & getData() const { return *_data; }
		RepresentationData &	   getData() { return *_data; }

		const Generic::REPRESENTATION & getRepresentationType() const { return _data->getRepresentationType(); }
		
		void copyDataFrom( const Representation & p_source );


	  protected:
		Representation( const Generic::REPRESENTATION & p_type );

	  private:
		std::string _name;
		std::string _iconPath;
		Color::Rgb	_color;
		bool		_quickAccess;

		RepresentationData * _data = nullptr;
	};

} // namespace VTX::Model::Representation
#endif
