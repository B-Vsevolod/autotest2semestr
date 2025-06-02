#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int GM; // Глобальная переменная для хранения значения M (используется в функции сравнения)

// Функция сравнения для сортировки элементов:
// Сначала сравниваются остатки от деления на GM,
// при равенстве остатков сравниваются сами значения
static int comp(const void *a, const void *b) {
    int x = *(const int*)a;
    int y = *(const int*)b;
    int rem_x = x % GM; // Вычисляем остаток от деления x на GM
    int rem_y = y % GM; // Вычисляем остаток от деления y на GM
    if (rem_x != rem_y) {
        return rem_x - rem_y; // Сортируем по возрастанию остатка
    }
    return x - y; // При равных остатках сортируем по возрастанию значения
}

// Вспомогательная функция для записи "ERROR" в файл результатов
static void write_error() {
    FILE *f = fopen("res.txt", "w");
    if (f) {
        fprintf(f, "ERROR");
        fclose(f);
    }
}

int main() {
    // Открываем входной файл
    FILE *in = fopen("data.txt", "r");
    if (!in) {
        write_error();
        return -1;
    }

    int M, N, L, K;
    // Считываем основные параметры: M, N, L, K
    if (fscanf(in, "%d %d %d %d", &M, &N, &L, &K) != 4) {
        fclose(in);
        write_error();
        return -1;
    }

    // Проверка условия M > 1
    if (M <= 1) {
        fclose(in);
        write_error();
        return -1;
    }

    GM = M; // Инициализируем глобальную переменную для использования в функции сравнения

    // Выделение памяти под двумерный массив размера L x K
    int **arr = (int**)malloc(L * sizeof(int*));
    if (!arr) {
        fclose(in);
        write_error();
        return -1;
    }
    for (int i = 0; i < L; i++) {
        arr[i] = (int*)malloc(K * sizeof(int));
        if (!arr[i]) {
            // Освобождаем ранее выделенную память при ошибке
            for (int j = 0; j < i; j++) {
                free(arr[j]);
            }
            free(arr);
            fclose(in);
            write_error();
            return -1;
        }
    }

    // Чтение элементов массива из файла
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < K; j++) {
            if (fscanf(in, "%d", &arr[i][j]) != 1) {
                // Освобождаем память при ошибке чтения
                for (int ii = 0; ii < L; ii++) {
                    free(arr[ii]);
                }
                free(arr);
                fclose(in);
                write_error();
                return -1;
            }
        }
    }
    fclose(in); // Закрываем входной файл

    // Определение столбцов, которые содержат хотя бы один элемент в диапазоне [M, N]
    int *col_included = (int*)calloc(K, sizeof(int)); // Массив флагов (0/1) для столбцов
    if (!col_included) {
        for (int i = 0; i < L; i++) {
            free(arr[i]);
        }
        free(arr);
        write_error();
        return -1;
    }

    int col_count = 0; // Количество столбцов, удовлетворяющих условию
    for (int j = 0; j < K; j++) {
        for (int i = 0; i < L; i++) {
            if (arr[i][j] >= M && arr[i][j] <= N) {
                col_included[j] = 1; // Помечаем столбец
                col_count++; // Увеличиваем счетчик
                break; // Переходим к следующему столбцу
            }
        }
    }

    // Обработка каждой строки массива
    for (int i = 0; i < L; i++) {
        // Выделяем временный массив для элементов из отмеченных столбцов
        int *temp = (int*)malloc(col_count * sizeof(int));
        if (!temp) {
            // Освобождаем память при ошибке
            for (int ii = 0; ii < L; ii++) {
                free(arr[ii]);
            }
            free(arr);
            free(col_included);
            write_error();
            return -1;
        }
        
        // Копируем элементы из отмеченных столбцов текущей строки во временный массив
        int idx = 0;
        for (int j = 0; j < K; j++) {
            if (col_included[j]) {
                temp[idx++] = arr[i][j];
            }
        }

        // Сортируем временный массив по остаткам от деления на M
        qsort(temp, col_count, sizeof(int), comp);

        // Копируем отсортированные элементы обратно в исходные позиции
        idx = 0;
        for (int j = 0; j < K; j++) {
            if (col_included[j]) {
                arr[i][j] = temp[idx++];
            }
        }
        free(temp); // Освобождаем временный массив
    }

    free(col_included); // Освобождаем массив флагов столбцов

    // Открываем файл для записи результатов
    FILE *out = fopen("res.txt", "w");
    if (!out) {
        for (int i = 0; i < L; i++) {
            free(arr[i]);
        }
        free(arr);
        write_error();
        return -1;
    }

    // Записываем основные параметры в первую строку файла
    if (fprintf(out, "%d %d %d %d\n", M, N, L, K) < 0) {
        fclose(out);
        for (int i = 0; i < L; i++) {
            free(arr[i]);
        }
        free(arr);
        write_error();
        return -1;
    }

    // Записываем элементы массива
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < K; j++) {
            if (fprintf(out, "%d", arr[i][j]) < 0) {
                fclose(out);
                for (int ii = 0; ii < L; ii++) {
                    free(arr[ii]);
                }
                free(arr);
                write_error();
                return -1;
            }
            // Добавляем пробелы между элементами (кроме последнего)
            if (j < K-1) {
                if (fprintf(out, " ") < 0) {
                    fclose(out);
                    for (int ii = 0; ii < L; ii++) {
                        free(arr[ii]);
                    }
                    free(arr);
                    write_error();
                    return -1;
                }
            }
        }
        // Добавляем перевод строки после каждой строки массива
        if (fprintf(out, "\n") < 0) {
            fclose(out);
            for (int ii = 0; ii < L; ii++) {
                free(arr[ii]);
            }
            free(arr);
            write_error();
            return -1;
        }
    }

    fclose(out); // Закрываем файл результатов

    // Освобождаем память, выделенную под массив
    for (int i = 0; i < L; i++) {
        free(arr[i]);
    }
    free(arr);

    return 0; // Успешное завершение программы
}