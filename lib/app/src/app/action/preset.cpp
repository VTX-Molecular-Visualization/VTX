#include "app/action/preset.hpp"
#include "app/session.hpp"

namespace VTX::App::Action::Preset
{
	void LoadAll::execute()
	{
		const Session & session = SESSION();
		ACTION().execute<Load<Renderer::Color::Layout>>(
			session.getDefaultColorLayoutsDir(), session.getColorLayoutsDir()
		);
		ACTION().execute<Load<Renderer::Representation>>(
			session.getDefaultRepresentationsDir(), session.getRepresentationsDir()
		);
		ACTION().execute<Load<Renderer::Material>>( session.getDefaultMaterialsDir(), session.getMaterialsDir() );
		ACTION().execute<Load<Renderer::GraphicsConfig>>(
			session.getDefaultGraphicsConfigsDir(), session.getGraphicsConfigsDir()
		);
	}
} // namespace VTX::App::Action::Preset
