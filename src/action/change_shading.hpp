#ifndef __VTX_ACTION_CHANGE_SHADING__
#define __VTX_ACTION_CHANGE_SHADING__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ChangeShading : public BaseAction
		{
		  public:
			explicit ChangeShading() {}
			explicit ChangeShading( const Renderer::SHADING p_shading ) : _shading( p_shading ) {}

			virtual void setParameters( const std::vector<std::string> & p_parameters ) override
			{
				_shading = magic_enum::enum_cast<Renderer::SHADING>( p_parameters.at( 1 ) ).value();
			}

			virtual void execute() override
			{
				Setting::Rendering::shading = _shading;
				VTXApp::get().getRendererGL().setShading();
			};

			virtual void displayUsage() override { VTX_INFO( "DIFFUSE|GLOSSY|TOON|FLAT_COLOR" ); }

		  private:
			Renderer::SHADING _shading = Setting::Rendering::shading;
		};
	} // namespace Action
} // namespace VTX
#endif
