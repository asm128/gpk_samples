#include "draw.h"

int main(int /*argc*/, char ** /*argv*/)
{
#if defined(NWOL_DEBUG_ENABLED)
	int tmp = _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF|_CRTDBG_DELAY_FREE_MEM_DF);
	// Clear the upper 16 bits and OR in the desired freqency
	tmp = (tmp & 0x0000FFFF) | _CRTDBG_CHECK_EVERY_16_DF;
	// Set the new bits
	_CrtSetDbgFlag(tmp);
#endif

	::klib::initASCIIScreen(klib::SGlobalDisplay::Width, klib::SGlobalDisplay::Depth);
	
	::klib::SGame* pInstancedGame	= new klib::SGame;
	::klib::SGame& instanceGame		= *pInstancedGame;

	::klib::initGame(instanceGame);
	while(instanceGame.Flags & klib::GAME_FLAGS_RUNNING) {
		::klib::pollInput(instanceGame.FrameInput);
		::klib::SASCIITarget							target;
		::klib::getASCIIBackBuffer						(target);
		::klib::clearASCIIBackBuffer(' ', COLOR_WHITE);
 		::klib::drawAndPresentGame(instanceGame, target);
		::klib::presentASCIIBackBuffer();
	}

	if(pInstancedGame)
		delete(pInstancedGame);

	::klib::shutdownASCIIScreen();

	return 0;
}


int WINAPI WinMain 
(    _In_		HINSTANCE	//hInstance
,    _In_opt_	HINSTANCE	//hPrevInstance
,    _In_		LPSTR		//lpCmdLine
,    _In_		int			//nShowCmd
)
{
	if(0 > ::main(__argc, __argv))
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
};