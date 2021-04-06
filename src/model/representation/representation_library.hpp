#ifndef __VTX_MODEL_REPRESENTATION_LIBRARY__
#define __VTX_MODEL_REPRESENTATION_LIBRARY__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/base_model.hpp"
#include "representation.hpp"
#include <vector>

namespace VTX::Model::Representation
{
	class RepresentationLibrary : public BaseModel
	{
	  public:
		inline static RepresentationLibrary & get()
		{
			static RepresentationLibrary instance;
			return instance;
		};

		BaseRepresentation * const		 getRepresentation( const int p_index );
		const BaseRepresentation * const getRepresentation( const int p_index ) const;
		BaseRepresentation * const		 getRepresentationByName( const std::string & p_name );
		const BaseRepresentation * const getRepresentationByName( const std::string & p_name ) const;

		int getRepresentationIndex( const BaseRepresentation * const p_representation ) const;

		int getRepresentationCount() const;

		void addRepresentation( BaseRepresentation * const p_representation );
		void removeRepresentation( const int p_index );

	  private:
		inline RepresentationLibrary() : BaseModel( ID::Model::MODEL_REPRESENTATION_LIBRARY ) { _init(); };
		~RepresentationLibrary() = default;
		void _init();

		std::vector<BaseRepresentation *> _representations;
	};
} // namespace VTX::Model::Representation
#endif
