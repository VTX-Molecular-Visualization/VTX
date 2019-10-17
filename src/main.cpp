#include "vtx_app.hpp"
#include <chrono>
#include <thread>

using namespace VTX;

int main()
{
	VTXApp & app = VTXApp::INSTANCE();

	app.start();
	std::this_thread::sleep_for( std::chrono::milliseconds( 10000 ) );
	app.stop();

	return 0;
}
