#include "GameApp.h"



int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE prevInstance,
	_In_ LPSTR cmdLine, _In_ int showCmd)
{
	// 这些参数不使用
	UNREFERENCED_PARAMETER(prevInstance);
	UNREFERENCED_PARAMETER(cmdLine);
	UNREFERENCED_PARAMETER(showCmd);

	GameApp theApp(hInstance);

	if (!theApp.Init())
		return 0;

	return theApp.Run();
}




