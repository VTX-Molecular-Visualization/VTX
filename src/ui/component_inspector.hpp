#ifndef __VTX_COMPONENT_INSPECTOR__
#define __VTX_COMPONENT_INSPECTOR__

#ifdef _MSC_VER
#pragma once
#endif

#include "localization/language.hpp"
#include "model/model_molecule.hpp"
#include "view/base_view.hpp"
#include "base_component.hpp"

namespace VTX
{
	namespace UI
	{
		class ComponentInspector : public BaseComponent
		{
		  public:
			explicit ComponentInspector( bool * const );

			virtual std::string getName() const override { return ID::UI::INSPECTOR; }

		  protected:
			virtual void _addItems() override {};
			virtual void _draw() override;

		  private:
		};
	} // namespace UI
} // namespace VTX
#endif
