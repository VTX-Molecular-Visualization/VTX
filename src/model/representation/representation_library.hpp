#ifndef __VTX_MODEL_REPRESENTATION_LIBRARY__
#define __VTX_MODEL_REPRESENTATION_LIBRARY__

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

		void			 addRepresentation( Representation * const p_representation, const bool p_notify = true );
		void			 copyRepresentation( const int p_index, const bool p_notify = true );
		Representation * removeRepresentation( const int p_index, const bool p_notify = true );
		bool			 canDeleteRepresentation( Representation * const p_representation ) const;
		void			 deleteRepresentation( const int p_representation, const bool p_notify = true );
		void			 deleteRepresentation( const Representation * const p_index, const bool p_notify = true );

		void			 setDefaultRepresentation( const int p_representationIndex, const bool p_notify = true );
		int				 getDefaultRepresentationIndex() const;
		Representation * getDefaultRepresentation();

		std::string getValidName( const std::string & p_name ) const;
		bool		isValidName( const std::string & p_name ) const;
		bool		isExistingName( const std::string & p_name ) const;

		void clear( const bool p_notify = true );

		void applyDefault( const bool p_notify = true );

		void resetToDefault( const bool p_notify = false );

	  protected:
		RepresentationLibrary();
		~RepresentationLibrary();

		virtual void _onRepresentationChange( const Event::VTXEvent * const p_event );

	  private:
		std::vector<Representation *> _representations;
		Representation *			  _defaultRepresentation = nullptr;
	};
} // namespace VTX::Model::Representation
#endif
