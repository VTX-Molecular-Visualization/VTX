#ifndef __VTX_MODEL_REPRESENTATION_LIBRARY__
#define __VTX_MODEL_REPRESENTATION_LIBRARY__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/event.hpp"
#include "model/base_model.hpp"
#include "mvc/mvc_manager.hpp"
#include "representation.hpp"
#include <vector>

namespace VTX::Model::Representation
{
	class RepresentationLibrary : public BaseModel
	{
		VTX_MODEL

	  public:
		static RepresentationLibrary & get();

		Representation * const				  getRepresentation( const int p_index );
		const Representation * const		  getRepresentation( const int p_index ) const;
		Representation * const				  getRepresentationByName( const std::string & p_name );
		const Representation * const		  getRepresentationByName( const std::string & p_name ) const;
		const std::vector<Representation *> & getRepresentations() { return _representations; };

		int getRepresentationIndex( const Representation * const p_representation ) const;

		int getRepresentationCount() const;

		void addRepresentation( Representation * const p_representation, const bool p_notify = true );
		void removeRepresentation( const int p_index, const bool p_notify = true );

	  protected:
		inline RepresentationLibrary() : BaseModel( ID::Model::MODEL_REPRESENTATION_LIBRARY ) { _init(); };
		~RepresentationLibrary() = default;

		virtual void _onRepresentationChange( const Event::VTXEvent * const p_event );

	  private:
		void						  _init();
		std::vector<Representation *> _representations;
	};
} // namespace VTX::Model::Representation
#endif
