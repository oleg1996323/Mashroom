#include "cmd_parse/aux/process_bar.h"

// Функция для получения текущей позиции курсора
bool getCursorPosition(int& row, int& col) {
    // Отправляем запрос позиции курсора
    std::cout << "\033[6n" << std::flush<<'\r'<<std::flush;

    // Читаем ответ терминала
    char buf[32];
    int i = 0;
    while (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
        if (buf[i] == 'R') break; // Конец ответа
        i++;
    }
    buf[i] = '\0';

    // Парсим ответ (формат: \033[<row>;<col>R)
    if (buf[0] != '\033' || buf[1] != '[') return false;
    if (sscanf(&buf[2], "%d;%d", &row, &col) != 2) return false;

    return true;
}

void pBar::print() {
    currUpdateVal %= pBarUpdater.length();
    std::cout << "\033[" << lineNumber<< ";0H"; // Переместить курсор на нужную строку
    std::cout << firstPartOfpBar; // Начало прогресс-бара
    for (int a = 0; a < amountOfFiller; a++) { // Заполнение прогресса
        std::cout << pBarFiller;
    }
    std::cout << pBarUpdater[currUpdateVal];
    for (int b = 0; b < pBarLength - amountOfFiller; b++) { // Пробелы
        std::cout << " ";
    }
    std::cout << lastPartOfpBar // Конец прогресс-бара
              << " Progress: " << (int)(100 * (currentProgress / neededProgress)) << "% " // Процент
              << std::flush;
    currUpdateVal += 1;
}