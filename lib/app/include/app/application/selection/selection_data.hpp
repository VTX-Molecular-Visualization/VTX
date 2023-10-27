#ifndef __VTX_APP_APPLICATION_SELECTION_SELECTION_DATA__
#define __VTX_APP_APPLICATION_SELECTION_SELECTION_DATA__

#include "app/component/scene/_fwd.hpp"

namespace VTX::App::Application::Selection
{
	class SelectionData
	{
	  public:
		SelectionData( const Component::Scene::Selectable & p_selectionComponent );
		virtual ~SelectionData() {};

		const Component::Scene::Selectable & getSelectionComponent() const;

	  private:
		const Component::Scene::Selectable * const _selectionComponent = nullptr;
	};
} // namespace VTX::App::Application::Selection
#endif
