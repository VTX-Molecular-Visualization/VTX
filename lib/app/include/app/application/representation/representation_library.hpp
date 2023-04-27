#ifndef __VTX_APPLICATION_REPRESENTATION_REPRESENTATION_LIBRARY__
#define __VTX_APPLICATION_REPRESENTATION_REPRESENTATION_LIBRARY__

#include "app/core/event/vtx_event.hpp"
#include "app/core/model/base_model.hpp"
#include "app/internal/chemdb/category.hpp"
#include "app/mvc.hpp"
#include "representation_preset.hpp"
#include <string>
#include <vector>

namespace VTX::App::Application::Representation
{
	class RepresentationLibrary : public App::Core::Model::BaseModel
	{
		VTX_MODEL

	  public:
		static RepresentationLibrary & get();

		RepresentationPreset * const				getRepresentation( const int p_index );
		const RepresentationPreset * const			getRepresentation( const int p_index ) const;
		RepresentationPreset * const				getRepresentationByName( const std::string & p_name );
		const RepresentationPreset * const			getRepresentationByName( const std::string & p_name ) const;
		const std::vector<RepresentationPreset *> & getRepresentations() { return _representations; };

		int getRepresentationIndex( const RepresentationPreset * const p_representation ) const;

		int getRepresentationCount() const;

		void addRepresentation( RepresentationPreset * const p_representation, const bool p_notify = true );
		void copyRepresentation( const int p_index, const bool p_notify = true );
		RepresentationPreset * removeRepresentation( const int p_index, const bool p_notify = true );
		bool				   canDeleteRepresentation( RepresentationPreset * const p_representation ) const;
		void				   deleteRepresentation( const int p_representation, const bool p_notify = true );
		void deleteRepresentation( const RepresentationPreset * const p_index, const bool p_notify = true );

		void				   setDefaultRepresentation( const int p_representationIndex, const bool p_notify = true );
		int					   getDefaultRepresentationIndex() const;
		RepresentationPreset * getDefaultRepresentation();
		RepresentationPreset * getDefaultRepresentation( const App::Internal::ChemDB::Category::TYPE & p_categoryEnum );

		std::string getValidName( const std::string & p_name ) const;
		bool		isValidName( const std::string & p_name ) const;
		bool		isExistingName( const std::string & p_name ) const;

		void clear( const bool p_notify = true );

		void applyDefault( const bool p_notify = true );

		void resetToDefault( const bool p_notify = false );

	  protected:
		RepresentationLibrary();
		~RepresentationLibrary();

		virtual void _onRepresentationChange( const App::Core::Event::VTXEvent * const p_event );

	  private:
		std::vector<RepresentationPreset *> _representations;
		RepresentationPreset *				_defaultRepresentation = nullptr;
	};
} // namespace VTX::App::Application::Representation
#endif
