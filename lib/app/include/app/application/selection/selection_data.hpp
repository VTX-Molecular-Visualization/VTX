#ifndef __VTX_APP_APPLICATION_SELECTION_SELECTION_DATA__
#define __VTX_APP_APPLICATION_SELECTION_SELECTION_DATA__

#include "app/component/scene/_fwd.hpp"
#include <memory>
#include <string>

namespace VTX::App::Application::Selection
{
	class SelectionData
	{
	  public:
		SelectionData( const Component::Scene::Selectable & p_selectionComponent );
		virtual ~SelectionData() {};

		virtual bool isEqualsTo( const SelectionData & p_other ) const;

		std::unique_ptr<SelectionData> clone() const { return std::move( _cloneImpl() ); }

		const Component::Scene::Selectable & getSelectionComponent() const;
		virtual std::string					 toString() const;

		inline virtual bool isValid() const { return _valid; }

		virtual void selectAll();

		virtual void			set( const SelectionData & p_other );
		virtual SelectionData & add( const SelectionData & p_other );
		virtual SelectionData & remove( const SelectionData & p_other );
		virtual SelectionData & intersect( const SelectionData & p_other );
		virtual SelectionData & exclude( const SelectionData & p_other );

	  protected:
		virtual std::unique_ptr<SelectionData> _cloneImpl() const;

	  private:
		const Component::Scene::Selectable * const _selectionComponent = nullptr;
		bool									   _valid			   = true;
	};
} // namespace VTX::App::Application::Selection
#endif
