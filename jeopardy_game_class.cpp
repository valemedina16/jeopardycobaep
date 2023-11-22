#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>

#include "main.h"
#include "TextTable.h"

using namespace std;

JeopardyGame::JeopardyGame(string nombreArchivo)
{
    this->m_puntuacion_total = 0;
    this->m_intentos_totales = 0;
    this->m_juego_en_ejecucion = true;
    if(nombreArchivo.empty())
        throw "Error: No se puede abrir un nombre de archivo vacío.";
    this->m_nombre_archivo = nombreArchivo;
    this->leerArchivo();
    this->m_ultimo_mensaje = CHAR_ESC+"[1;32m[ MENSAJE ]"+CHAR_ESC+"[0m - ¡Bienvenido a Jeopardy! Selecciona una pregunta escribiendo la categoría y el número (ej. A2)";
    this->iniciarJuego();
}

void JeopardyGame::iniciarJuego()
{
    int intentosDisponibles = this->m_num_categorias * this->m_num_preguntas_por_cat;

    while(this->m_intentos_totales < intentosDisponibles && this->m_juego_en_ejecucion)
    {
        system(LIMPIAR_PANTALLA); 
        string entradaUsuario;
        this->mostrarTitulo();
        this->mostrarUltimoMensaje();
        this->mostrarTablero();
        cout << endl;
        cout << CHAR_ESC+"[1;34m>>> "+CHAR_ESC+"[0m ";
        cin >> entradaUsuario;
        int valorRetorno = this->validarEntrada(entradaUsuario);
        if(!valorRetorno) 
        {
            continue;
        }
        else if(valorRetorno > 0) 
        {
            this->m_ultimo_mensaje = " "+CHAR_ESC+"[1;32m[ MENSAJE ]"+CHAR_ESC+"[0m  : ¡Correcto! ¿Cuál es tu siguiente elección?:";
            this->m_puntuacion_total += valorRetorno;
            this->m_intentos_totales++;
        }
        else if(valorRetorno == -1) 
        {
            this->m_ultimo_mensaje = " "+CHAR_ESC+"[1;33m[ MENSAJE ]"+CHAR_ESC+"[0m : ¡Incorrecto! Lo siento :( ¿Cuál es tu siguiente elección?:";
            this->m_intentos_totales++;
        }
        else if(valorRetorno == -2) 
        {
            this->m_ultimo_mensaje = " "+CHAR_ESC+"[1;31m[ MENSAJE ]"+CHAR_ESC+"[0m : Necesitas una entrada válida. Por favor, inténtalo de nuevo:";
        }
        else if(valorRetorno == -3) 
        {
            this->m_ultimo_mensaje = " "+CHAR_ESC+"[1;33m[ MENSAJE ]"+CHAR_ESC+"[0m : Ya intentaste esa pregunta.. Por favor, intenta otra:";
        }
    }
    system(LIMPIAR_PANTALLA);
    this->mostrarPantallaFinal(); 
}

void JeopardyGame::mostrarPantallaFinal() const
{
    this->mostrarTitulo();
    TextTable t( '~', '|', '+' );
    const string espacio = "              ";
    const string espacio2 = "                      ";

    t.add(espacio + "Puntuación Final:" + espacio);
    t.add(espacio + to_string(this->getPuntuacion()) + espacio);
    t.endOfRow();
    t.add(espacio + "Intentos Realizados:" + espacio);
    t.add(espacio + to_string(this->getIntentos()) + espacio);
    t.endOfRow();
    t.setAlignment( 2, TextTable::Alignment::LEFT );
    cout << endl;
    cout << t;
    cout << endl << endl << CHAR_ESC+"[1;45m" << espacio2 << "¡BUEN TRABAJO! ¡Gracias por jugar :)" << espacio2 << CHAR_ESC+"[0m\n\n\n";
}


void JeopardyGame::leerArchivo()
{
    string linea;
    int numDeCategorias = 0;

    this->m_archivo_juego.open(this->m_nombre_archivo);
    if(this->m_archivo_juego.fail())
        throw "Error: Fallo al abrir el archivo: " + this->m_nombre_archivo;

    while(!this->m_archivo_juego.eof()) // Lee el archivo y popula m_num_questions_per_cat y m_num_answers_per_ques para poder asignar memoria dinámicamente
    {
        getline(this->m_archivo_juego, linea);
        if(linea[0] == '#') // Número de categorías
            numDeCategorias++;
        else if(linea[0] == '^') // Número de preguntas por categoría
            this->m_num_preguntas_por_cat = stoi(linea.substr(2));
        else if(linea[0] == '$') // Número de respuestas por pregunta
            this->m_num_respuestas_por_preg = stoi(linea.substr(2));

        if(numDeCategorias > 7) // Limitación debido a que las letras de categoría m_cat_letters solo van de A a G porque H se usa como tecla de ayuda
            throw "Error: El juego solo soporta 7 categorías. Por favor, reduce el número de categorías e intenta nuevamente.";
    }
    const int NUM_DE_CATEGORIAS = numDeCategorias;
    const int NUM_DE_PREGUNTAS_POR_CAT = this->m_num_preguntas_por_cat;
    vector<string> vectorNombresCategorias (numDeCategorias);
    this->m_num_categorias = numDeCategorias;
    this->m_nombres_categorias = vectorNombresCategorias;
    this->m_elementos_juego = new JeopardyQuestion*[NUM_DE_CATEGORIAS];
    for(int i = 0; i < NUM_DE_CATEGORIAS; i++)
    {
        this->m_elementos_juego[i] = new JeopardyQuestion[NUM_DE_PREGUNTAS_POR_CAT];
    }

    this->m_archivo_juego.clear();
    this->m_archivo_juego.seekg(0, ios::beg);

    int elementoJuegoActual = 0;
    int valorPuntoActual = 0;
    int conteoNombreCategoria = 0;
    int conteoCategoriaActual = -1;
    int conteoPosiblesRespuestasActual = 0;
    int respuestaCorrectaActual = 0;
    string preguntaActual;
    vector<string> posiblesRespuestasActuales(this->m_num_preguntas_por_cat);

    while(!this->m_archivo_juego.eof()) // Lee el archivo para poblar objetos de pregunta con los datos apropiados
    {
        
        getline(this->m_archivo_juego, linea);
        if(linea[0] == '#') // Nombre de la categoría
        {
            this->m_nombres_categorias[conteoNombreCategoria] = linea.substr(2);
            conteoNombreCategoria++;
        }
        else if(linea[0] == '~') // Indicador de nueva categoría
        {
            elementoJuegoActual = 0;
            respuestaCorrectaActual = 0;
            conteoCategoriaActual++;
        }
        else if(linea[0] == '%') // Valor de puntos para la pregunta actual
        {
            valorPuntoActual = stoi(linea.substr(2));
        }
        else if(linea[0] == '*') // Indicador de nueva pregunta
        {
            preguntaActual = linea.substr(2);
            conteoPosiblesRespuestasActual = 0;
            posiblesRespuestasActuales.clear();
        }
        else if(linea[0] == '@') // una Opción incorrecta 
        {
            posiblesRespuestasActuales.push_back(linea.substr(2));
            conteoPosiblesRespuestasActual++;
            if(conteoPosiblesRespuestasActual == this->m_num_respuestas_por_preg) // Si es el final de las preguntas, configura el objeto de la pregunta e incrementa al siguiente elemento de juego
            {
                this->m_elementos_juego[conteoCategoriaActual][elementoJuegoActual].configurar(preguntaActual, posiblesRespuestasActuales, respuestaCorrectaActual, valorPuntoActual, conteoCategoriaActual);
                elementoJuegoActual++;
            }
        }
        else if(linea[0] == '&') // una Opción correcta 
        {
            posiblesRespuestasActuales.push_back(linea.substr(2));
            respuestaCorrectaActual = conteoPosiblesRespuestasActual;
            conteoPosiblesRespuestasActual++;
            if(conteoPosiblesRespuestasActual == this->m_num_respuestas_por_preg) // Si es el final de las preguntas, configura el objeto de la pregunta e incrementa al siguiente elemento de juego
            {
                this->m_elementos_juego[conteoCategoriaActual][elementoJuegoActual].configurar(preguntaActual, posiblesRespuestasActuales, respuestaCorrectaActual, valorPuntoActual, conteoCategoriaActual);
                elementoJuegoActual++;
            }
        }

        if(elementoJuegoActual > this->m_num_preguntas_por_cat) // Si el número de preguntas por categoría excede la cantidad especificada (posible desbordamiento/corrupción de memoria)
            throw "Error: Hay un problema con el archivo de texto de Jeopardy. Excediste la cantidad de preguntas por categoría que especificaste. Por favor, corrige e intenta de nuevo.";
        if(conteoPosiblesRespuestasActual > this->m_num_respuestas_por_preg)
            throw "Error: Hay un problema con el archivo de texto de Jeopardy. Excediste la cantidad de respuestas por pregunta que especificaste. Por favor, corrige e intenta de nuevo.";
    }
}


int JeopardyGame::obtenerPuntuacion() const
{
    return this->m_total_score;
}

int JeopardyGame::obtenerIntentos() const
{
    return this->m_total_tries;
}

void JeopardyGame::mostrarTitulo() const
{
    cout << endl
        << "  CCCCCC   OOOOO   BBBBB  AAAAA   EEEEE  PPPPP "
        << endl << " C      O O    O  B    B A     A  E      P    P"
        << endl << " C       O     O  BBBBB  AAAAAAA EEEEE  PPPPP "
        << endl << " C      O OOOOOO  B    B A     A E      P    "
        << endl << "  CCCCCC         BBBBB  A     A EEEEE  P     "
        << endl << endl;
}


void JeopardyGame::mostrarTablero() const
{
    /*
        Todo lo siguiente es código para mostrar el tablero correctamente
    */
    const string espacio = "      ";
    const string pequeño_espacio = "   ";
    const string relleno = "###################";
    const int intentosDisponibles = this->m_num_categories * this->m_num_questions_per_cat;
    TextTable t( '~', '|', '+' );
    t.add("Puntuación:" + to_string(this->obtenerPuntuacion())); // Muestra la puntuación en la parte superior de la tabla
    t.add(" ");
    t.add("Intentos Restantes: " + to_string(intentosDisponibles - this->obtenerIntentos())); // Muestra los intentos restantes en la parte superior de la tabla
    t.add(espacio + espacio + " Teclas:" ); // Teclas de ayuda
    t.add(" X=Salir  |  H=Ayuda");
    t.endOfRow();
    t.add(relleno);
    for(int i = 0; i < this->m_num_categories; i++) // Rellena la tabla con un símbolo "#"
    {
        t.add(relleno);
    }
    t.endOfRow();
    t.add(pequeño_espacio + "Categorías:" + pequeño_espacio);
    for(int i = 0; i < this->m_num_categories; i++) // Rellena la tabla con los nombres de las categorías
    {
        t.add(espacio + this->m_category_names[i] + espacio);
    }
    t.endOfRow();
    t.add(relleno);
    for(int i = 0; i < this->m_num_categories; i++)
    {
        t.add(relleno);
    }
    t.endOfRow();
    t.add(" ");
    for(int i = 0; i < this->m_num_categories; i++) // Rellena las letras de las categorías (por ejemplo, A, B, C, etc.)
    {
        t.add(espacio + "[ "+ this->m_cat_letters[i] +" ]" + espacio); 
    }
    t.endOfRow();
    for(int i = 0; i < this->m_num_questions_per_cat; i++) // Rellena los puntos de las preguntas en la tabla
    {
        t.add(espacio + to_string(i + 1) + espacio);
        for(int n = 0; n < this->m_num_categories; n++)
        {
            int _valorPt = this->m_game_elements[n][i].obtenerValorPunto();
            string valorPt;
            if(!_valorPt) // Si el valor del punto es 0 (en caso de que ya se haya intentado), muestra una X 
                valorPt = " X";
            else
                valorPt = to_string(_valorPt);
            t.add(espacio + valorPt + espacio);
        }
        t.endOfRow();
    }
    t.setAlignment( 2, TextTable::Alignment::LEFT );
    cout << endl;
    cout << t;
}


int JeopardyGame::validarEntrada(string userInput)
{
    if(!userInput.size()) // Si no se ingresó nada, no se devuelve nada
        return 0;
    else if(toupper(userInput[0]) == 'X') // El usuario indicó que quiere salir del juego.
    {
        this->finalizarJuego();
        return 0;
    }
    else if(toupper(userInput[0]) == 'H') // El usuario indicó que quiere ver la pantalla de ayuda
    {
        this->mostrarAyuda();
        return 0;
    }
    else if(userInput.size() != 2) // No es una entrada válida
        return -2;
    else 
    {
        for(int i = 0; i < this->m_num_categories; i++) 
        {
            if(toupper(userInput[0]) == this->m_cat_letters[i]) // Verificar si la letra ingresada pertenece a alguna categoría
            {
                for(int n = 0; n < this->m_num_questions_per_cat; n++)
                {
                    if((stoi(userInput.substr(1, 1)) - 1) == n) // Verificar si el número ingresado (después de la letra) pertenece a las preguntas
                    {
                        return this->intentarPregunta(i, n);
                    }
                }
            }
        }
    }
    return -2; // De lo contrario, se devuelve una entrada no válida
}

void JeopardyGame::finalizarJuego()
{
    this->m_run_game = false;
}


void JeopardyGame::mostrarAyuda() const
{
    system(LIMPIAR_P); // Limpiar pantalla
    string cualquierEntrada;
    this->mostrarTitulo();
    TextTable t( '-', '|', '+' );
    t.add(" Selección de una pregunta: ");
    t.add(" Utiliza la letra y el número de la pregunta que deseas seleccionar (por ejemplo, B3). ");
    t.endOfRow();
    t.add(" Intentos: ");
    t.add(" Solo se permite un intento por pregunta, así que elige sabiamente antes de seleccionar. ");
    t.endOfRow();
    t.add(" Suma/resta de puntos: ");
    t.add(" Obtienes puntos agregados por respuestas correctas, pero no pierdes puntos por respuestas incorrectas. ");
    t.endOfRow();
    t.setAlignment( 2, TextTable::Alignment::LEFT );
    cout << endl;
    cout << t;
    cout << endl << endl << CHAR_ESC+"[1;34mIngresa cualquier tecla y presiona enter para volver.. "+CHAR_ESC+"[0m" << endl;
    cin >> cualquierEntrada;
}

void JeopardyGame::mostrarPregunta(int cat, int ques, vector<string> &respuestas) const
{
    const string small_spacer = "        ";
    TextTable t( '-', '|', '+' );
    t.add("##  Pregunta de Jeopardy: " + small_spacer); // Mostrar la pregunta en sí
    t.endOfRow();
    t.add(small_spacer + this->m_game_elements[cat][ques].getQuestion() + small_spacer);
    t.endOfRow();
    t.add(" ");
    t.endOfRow();
    t.add("##  Respuestas Posibles:" + small_spacer);
    t.endOfRow();
    for(int i = 0; i < respuestas.size(); i++) // Mostrar cada respuesta posible
    {
        t.add(small_spacer + to_string(i+1) + ". " + respuestas[i] + small_spacer);
        t.endOfRow();
    }
    t.setAlignment( 2, TextTable::Alignment::LEFT );
    cout << endl;
    cout << t;
    cout << endl << CHAR_ESC+"[1;34m¿Cuál es tu respuesta:"+CHAR_ESC+"[0m ";
}

int JeopardyGame::intentarPregunta(int cat, int ques)
{
    vector<string> respuestas = this->m_game_elements[cat][ques].getRespuestasPosibles();
    system(LIMPIAR_P);
    if(!this->m_game_elements[cat][ques].getValorPuntos()) // La pregunta ya ha sido intentada
        return -3;
    int entradaUsuario = 0;
    this->mostrarTitulo();
    this->mostrarPregunta(cat, ques, respuestas);
    cin >> entradaUsuario;
    while(cin.fail() || entradaUsuario < 1 || (entradaUsuario - 1) >= respuestas.size()) // Si no es una entrada válida, sigue en un bucle hasta que el usuario proporcione una entrada válida.
    {
        system(LIMPIAR_P);
        this->m_ultimo_mensaje = " "+CHAR_ESC+"[1;31m[ MENSAJE ]"+CHAR_ESC+"[0m : Necesitas una entrada válida. Por favor, inténtalo de nuevo:";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(),'\n');
        this->mostrarTitulo();
        this->mostrarUltimoMensaje();
        this->mostrarPregunta(cat, ques, respuestas);
        cin >> entradaUsuario;
        if(entradaUsuario < 1 || (entradaUsuario - 1) >= respuestas.size())
            continue;
        if(!cin.fail())
            break;
    }

    int retVal = this->m_game_elements[cat][ques].responderPregunta(entradaUsuario-1);
    this->m_game_elements[cat][ques].marcarComoIntentada();
    return retVal;
}

void JeopardyGame::mostrarUltimoMensaje() // Muestra el último mensaje y luego establece m_ultimo_mensaje como vacío (porque ya ha sido mostrado)
{
    cout << this->m_ultimo_mensaje << endl;
    this->m_ultimo_mensaje = "";
}
