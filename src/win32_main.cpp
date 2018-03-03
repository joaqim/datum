
#define WINVER 0x0501 // Allow use of features specific to Windows XP or later.
#define _WIN32_WINNT 0x0501
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib, "User32.lib")

#include "Application.h"

// Attach output of application to parent console
static bool attachOutputToConsole(FILE** stream)
{
    HANDLE consoleHandleOut, consoleHandleError;
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        // Redirect unbuffered STDOUT to the console
        consoleHandleOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (consoleHandleOut != INVALID_HANDLE_VALUE) {
            freopen_s(stream, "CONOUT$", "w", stdout);
            setvbuf(stdout, NULL, _IONBF, 0);
			//fclose(stream);
        }
        else {
			return false;
        }
        // Redirect unbuffered STDERR to the console
        consoleHandleError = GetStdHandle(STD_ERROR_HANDLE);
        if (consoleHandleError != INVALID_HANDLE_VALUE) {
            freopen_s(stream,"CONOUT$", "w", stderr);
            setvbuf(stderr, NULL, _IONBF, 0);
			//fclose(stream);
        }
        else {
            return false;
        }
		return true;
    }
    //Not a console application
    return false;
}

// Send the "enter" to the console to release the command prompt
// on the parent console
static void sendEnterKey(void)
{
    INPUT ip;
    // Set up a generic keyboard event.
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0; // hardware scan code for key
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

    // Send the "Enter" key
    ip.ki.wVk = 0x0D; // virtual-key code for the "Enter" key
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));

    // Release the "Enter" key
    ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, &ip, sizeof(INPUT));
}
#include <gl/glew.h>

int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{	
	int argc = __argc;
	char **argv = __argv;
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);
    FILE* console;
	bool hasConsole;
    int i;
	
	


    //Is the program running as console or GUI application
	//if (!AllocConsole())
	//	MessageBox(NULL, "The console window was not created", NULL, MB_ICONEXCLAMATION);
	//AttachConsole(ATTACH_PARENT_PROCESS);
    hasConsole = attachOutputToConsole(&console);

    if (hasConsole) {
        // Print to stdout
        printf("Program running as console application\n");
        for (i = 0; i < argc; i++) {
            printf("argv[%d] %s\n", i, argv[i]);
        }

        // Print to stderr
        fprintf(stderr, "Output to stderr\n");
	}
#ifdef _DEBUG
    else {
		printf("Attempting to open a console");
		AllocConsole();
		
		AttachConsole(ATTACH_PARENT_PROCESS);
		//FILE* pCout;
		freopen_s(&console, "conout$", "w", stdout);
		setvbuf(stdout, NULL, _IONBF, 0);

		freopen_s(&console,"CONOUT$", "w", stderr);
        setvbuf(stderr, NULL, _IONBF, 0);
		hasConsole = true;
		
		//hasConsole = attachOutputToConsole(&console);
        //MessageBox(NULL, "Program running as Windows GUI application", "Windows GUI Application", MB_OK | MB_SETFOREGROUND);
    }
#endif

    // Send "enter" to release application from the console
    // This is a hack, but if not used the console doesn't know the application has
    // returned. The "enter" key only sent if the console window is in focus.
    if (hasConsole && (GetConsoleWindow() == GetForegroundWindow())) { sendEnterKey(); } 
	char path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, (LPSTR)path);
	printf("%s\n", path);
	
	//Game game;
	//game.RunGame();
	
	
	Application app;
	app.initialize();
	app.run();
	app.exit();
    
	if(hasConsole) {
		fclose(console);
		if (!FreeConsole())
			MessageBox(NULL, "Failed to free the console!", NULL, MB_ICONEXCLAMATION);
	}
	
    return 0;
}
