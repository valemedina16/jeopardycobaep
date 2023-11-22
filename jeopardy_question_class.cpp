#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "main.h"

using namespace std;

JeopardyQuestion::JeopardyQuestion()
{
    vector<string> emptyVec;
    this->m_question = "";
    this->m_possible_answers = emptyVec;
    this->m_correct_answer = 0;
    this->m_point_value = 0;
    this->m_category = 0;
}

JeopardyQuestion::JeopardyQuestion(string pregunta, vector<string> respuestasPosibles, 
                                    int respuestaCorrecta, int valorPuntos, int categoria)
{
    this->m_question = pregunta;
    this->m_possible_answers = respuestasPosibles;
    this->m_correct_answer = respuestaCorrecta;
    this->m_point_value = valorPuntos;
    this->m_category = categoria;

}

// Configura el objeto si no se inicializa con los datos correctos al ser asignado
void JeopardyQuestion::setup(string pregunta, vector<string> respuestasPosibles, 
                                    int respuestaCorrecta, int valorPuntos, int categoria)
{
    this->m_question = pregunta;
    this->m_possible_answers = respuestasPosibles;
    this->m_correct_answer = respuestaCorrecta;
    this->m_point_value = valorPuntos;
    this->m_category = categoria;
}

string JeopardyQuestion::getQuestion() const
{
    return this->m_question;
}

vector<string> JeopardyQuestion::getPossibleAnswers() const
{
    return this->m_possible_answers;
}

int JeopardyQuestion::answerQuestion(int respuestaSeleccionada) // Verificar si la respuesta es correcta o no
{
    if(respuestaSeleccionada == this->m_correct_answer)
        return this->m_point_value;
    else
        return -1;
}

int JeopardyQuestion::getPointVal() const
{
    return this->m_point_value;
}

void JeopardyQuestion::setToTried()
{
    this->m_point_value = 0;
}
