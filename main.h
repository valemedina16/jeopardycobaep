#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

extern const char CLEAR_S[];
extern const string CHAR_ESC;

class PreguntaJeopardy {
private:
    string m_pregunta;
    vector<string> m_respuestas_posibles;
    int m_respuesta_correcta;
    int m_categoria;
    int m_valor_puntos;

public:
    PreguntaJeopardy();
    PreguntaJeopardy(string pregunta, vector<string> respuestasPosibles, int respuestaCorrecta, int valorPuntos, int categoria);
    void configurar(string pregunta, vector<string> respuestasPosibles, int respuestaCorrecta, int valorPuntos, int categoria);
    string obtenerPregunta() const;
    vector<string> obtenerRespuestasPosibles() const;
    int responderPregunta(int respuestaSeleccionada);
    int obtenerValorPuntos() const;
    void marcarComoIntentada();
};

class JuegoJeopardy {
private:
    const char m_letras_cat[8] = "ABCDEFG";
    string m_nombre_archivo;
    int m_num_categorias;
    vector<string> m_nombres_categorias;
    ifstream m_archivo_juego;
    PreguntaJeopardy **m_elementos_juego;
    int m_puntaje_total;
    int m_intentos_totales;
    int m_num_preguntas_por_cat;
    int m_num_respuestas_por_preg;
    bool m_ejecutar_juego;
    string m_ultimo_mensaje;
    void leerArchivo();
    void ejecutarJuego();
    void finalizarJuego();
    void mostrarAyuda() const;
    void mostrarUltimoMensaje();
    void mostrarTitulo() const;
    void mostrarTablero() const;
    int validarEntrada(string entradaUsuario);
    int obtenerPuntaje() const;
    int obtenerIntentos() const;
    void mostrarPregunta(int cat, int preg, vector<string> &respuestas) const;
    void mostrarPantallaFinal() const;
    int intentarPregunta(int categoria, int pregunta);

public:
    JuegoJeopardy(string nombreArchivo);
};


