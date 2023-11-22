#include <iostream>
#include <string>

#include "main.h"

using namespace std;

#if defined(_WIN64)
    /* Microsoft Windows (64 bits). -- */
    const char SYS_PAUSE[6] = "pause";
    const char CLEAR_S[4] = "CLS";
    const string CHAR_ESC = "\x1b";
#elif defined(_WIN32)
    /* Microsoft Windows (32 bits). -- */
    const char SYS_PAUSE[6] = "pause";
    const char CLEAR_S[4] = "CLS";
    const string CHAR_ESC = "\x1b";
#else
    const char SYS_PAUSE[41] = "read -p 'Presiona Enter para continuar...' var";
    const char CLEAR_S[6] = "clear";
    const string CHAR_ESC = "\033";
#endif

int main()
{   
    try
    {
        // Ejecutar el juego
        JeopardyGame juego1("jeopardy_game_file.txt");
        system(SYS_PAUSE);
    }
    catch(const char * e)
    {
        cout << e << endl;
        exit(-1);
    }
    catch(...)
    {
        cout << "Excepción no controlada. Saliendo." << endl;
        exit(-1);
    }
    return 0;
}
