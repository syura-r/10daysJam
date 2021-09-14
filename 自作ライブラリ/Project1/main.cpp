#include"Game.h"
#include"ConsoleWindow.h"
// Windows アプリでのエントリーポイント(main 関数)
Game* Game::instance = nullptr;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#ifdef _DEBUG
	//デバッグレイヤーをオンに
	ComPtr < ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
	ConsoleWindow* consoleWindow = new ConsoleWindow();
	consoleWindow->CreateConsoleWindow();
#endif // _DEBUG
	srand(time(nullptr));
	Game* game = game->GetInstance();

	game->Initialize();
	game->Run();
	game->End();
	delete game;

#ifdef _DEBUG
	delete consoleWindow;
#endif // _DEBUG

	return 0;
}



