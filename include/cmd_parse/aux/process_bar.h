#pragma once
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int progress_line = 6;

// Функция для получения текущей позиции курсора
bool getCursorPosition(int& row, int& col);

class pBar {
public:
    pBar(){}

    void setline(int pline){
        lineNumber = pline;
    }

    void update(double newProgress) {
        currentProgress = newProgress;
        amountOfFiller = (int)((currentProgress / neededProgress) * (double)pBarLength);
    }

    void print();

    std::string firstPartOfpBar = "[", // Начало прогресс-бара
                lastPartOfpBar = "]",  // Конец прогресс-бара
                pBarFiller = "|",      // Символ заполнения
                pBarUpdater = "/-\\|"; // Анимация

private:
    int amountOfFiller,
        pBarLength = 50, // Длина прогресс-бара
        currUpdateVal = 0, // Текущее значение анимации
        lineNumber = 0; // Номер строки для вывода
    double currentProgress = 0, // Текущий прогресс
           neededProgress = 100; // Общий прогресс
};