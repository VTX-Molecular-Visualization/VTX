#ifndef VTX_TOOL_MDPREP_UI_GROMACS_HPP
#define VTX_TOOL_MDPREP_UI_GROMACS_HPP

#include <cstdint>
#include <memory>

namespace VTX::Tool::Mdprep::Gateway
{
	class EngineJobManager;
	struct EngineSpecificCommonInformation;
} // namespace VTX::Tool::Mdprep::Gateway

namespace VTX::Tool::Mdprep::ui
{
	class MdEngineFieldPlacer;
	class MdEngineSpecificFieldPlacer;
	enum class E_FIELD_SECTION : uint8_t;

	/**
	 * @brief Class responsible for providing gromacs specific ui objects
	 */
	class Gromacs
	{
	  public:
		Gromacs();

		void get( MdEngineFieldPlacer & p_out );
		void get( const E_FIELD_SECTION &, MdEngineSpecificFieldPlacer & );
		void get( Gateway::EngineJobManager & );

	  private:
		class Impl;

		struct Del
		{
			void operator()( Impl * ) noexcept;
		};

		std::unique_ptr<Impl, Del> _impl = nullptr;
	};

	void get( const Gromacs &, Gateway::EngineSpecificCommonInformation & );
} // namespace VTX::Tool::Mdprep::ui

#endif
