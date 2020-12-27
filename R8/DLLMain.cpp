// Important headers
#include "Engine.h"
#include "CSignature.h"
#include "VMTHooks.h"
#include "Offsets.h"




//             ╔═══╦═══╗╔════╦═══╦═══╗             //
//             ║╔═╗║╔═╗║║╔╗╔╗║╔══╣╔═╗║             //
//             ║╚═╝║╚═╝║╚╝║║╚╣╚══╬╝╔╝║             //
//             ║╔╗╔╣╔═╗║──║║─║╔══╬═╝╔╝             //
//             ║║║╚╣╚═╝║──║║─║║──║║╚═╗             //
//             ╚╝╚═╩═══╝──╚╝─╚╝──╚═══╝             //





//#include "XorStr.h"

DWORD WINAPI dwMainThread( LPVOID lpArguments )
{

	if (gBase.Client != NULL) // Prevent repeat callings.
		return 0;

	gBase.GetInterfaces();
	/*const char* Version = gBase.Engine->GetProductVersionString();
	if (strcmp(Version, XorStr("4771545")))
	{
		gBase.Fatal(XorStr("Your copy is outdated!"), XorStr("Tell the developer to make an update!\nNew version: %s"), Version);
		return 1;
	}*/

	gOffsets.GetOffsets();
	gBase.HookInterfaces();

	return 0;
}

#include "Menu.h"
BOOL APIENTRY DllMain(HMODULE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	//If you manually map, make sure you setup this function properly.
	if (dwReason == DLL_PROCESS_ATTACH)
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)dwMainThread, NULL, 0, NULL);

	return true;
}