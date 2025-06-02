#include <stdio.h>
#include <stdlib.h>

int main() {
    // Открываем входной файл
    FILE *in_file = fopen("data.txt", "r");
    if (in_file == NULL) {
        // Если файл не открылся, пытаемся создать файл результата с ошибкой
        FILE *out_file = fopen("res.txt", "w");
        if (out_file != NULL) {
            fprintf(out_file, "ERROR\n");
            fclose(out_file);
        }
        return -1;
    }

    int M, L, K;
    // Считываем параметры M, L, K
    if (fscanf(in_file, "%d %d %d", &M, &L, &K) != 3) {
        fclose(in_file);
        // При ошибке чтения параметров записываем ERROR
        FILE *out_file = fopen("res.txt", "w");
        if (out_file != NULL) {
            fprintf(out_file, "ERROR\n");
            fclose(out_file);
        }
        return -1;
    }

    // Выделяем память под двумерный массив
    int **array = (int **)malloc(L * sizeof(int *));
    if (array == NULL) {
        fclose(in_file);
        FILE *out_file = fopen("res.txt", "w");
        if (out_file != NULL) {
            fprintf(out_file, "ERROR\n");
            fclose(out_file);
        }
        return -1;
    }
    for (int i = 0; i < L; i++) {
        // Выделяем память для каждой строки
        array[i] = (int *)malloc(K * sizeof(int));
        if (array[i] == NULL) {
            // Освобождаем ранее выделенную память при ошибке
            for (int j = 0; j < i; j++) {
                free(array[j]);
            }
            free(array);
            fclose(in_file);
            FILE *out_file = fopen("res.txt", "w");
            if (out_file != NULL) {
                fprintf(out_file, "ERROR\n");
                fclose(out_file);
            }
            return -1;
        }
        // Считываем элементы строки
        for (int j = 0; j < K; j++) {
            if (fscanf(in_file, "%d", &array[i][j]) != 1) {
                // Освобождаем память при ошибке чтения элемента
                for (int k = 0; k <= i; k++) {
                    free(array[k]);
                }
                free(array);
                fclose(in_file);
                FILE *out_file = fopen("res.txt", "w");
                if (out_file != NULL) {
                    fprintf(out_file, "ERROR\n");
                    fclose(out_file);
                }
                return -1;
            }
        }
    }
    fclose(in_file);

    // Создаем массив флагов для столбцов, принадлежащих множеству X
    int *is_in_X = (int *)calloc(K, sizeof(int));
    if (is_in_X == NULL) {
        // Освобождаем память массива при ошибке
        for (int i = 0; i < L; i++) {
            free(array[i]);
        }
        free(array);
        FILE *out_file = fopen("res.txt", "w");
        if (out_file != NULL) {
            fprintf(out_file, "ERROR\n");
            fclose(out_file);
        }
        return -1;
    }

    // Определяем столбцы, входящие в множество X
    for (int j = 0; j < K; j++) {
        for (int i = 0; i < L; i++) {
            // Проверяем остаток от деления на M
            if (array[i][j] % M == 0) {
                is_in_X[j] = 1; // Помечаем столбец
                break; // Достаточно одного элемента
            }
        }
    }

    // Создаем копию исходного массива для сохранения оригинальных значений
    int **orig = (int **)malloc(L * sizeof(int *));
    if (orig == NULL) {
        free(is_in_X);
        for (int i = 0; i < L; i++) {
            free(array[i]);
        }
        free(array);
        FILE *out_file = fopen("res.txt", "w");
        if (out_file != NULL) {
            fprintf(out_file, "ERROR\n");
            fclose(out_file);
        }
        return -1;
    }
    for (int i = 0; i < L; i++) {
        // Выделяем память для копии строки
        orig[i] = (int *)malloc(K * sizeof(int));
        if (orig[i] == NULL) {
            // Освобождаем память при ошибке
            for (int j = 0; j < i; j++) {
                free(orig[j]);
            }
            free(orig);
            free(is_in_X);
            for (int k = 0; k < L; k++) {
                free(array[k]);
            }
            free(array);
            FILE *out_file = fopen("res.txt", "w");
            if (out_file != NULL) {
                fprintf(out_file, "ERROR\n");
                fclose(out_file);
            }
            return -1;
        }
        // Копируем значения
        for (int j = 0; j < K; j++) {
            orig[i][j] = array[i][j];
        }
    }

    // Выполняем преобразование для столбцов из X (кроме первого)
    for (int j = 1; j < K; j++) {
        if (is_in_X[j]) {
            for (int i = 0; i < L; i++) {
                // Заменяем элемент на сумму с предыдущим столбцом
                array[i][j] = orig[i][j - 1] + orig[i][j];
            }
        }
    }

    // Записываем результат в файл
    FILE *out_file = fopen("res.txt", "w");
    if (out_file == NULL) {
        // Освобождаем память при ошибке открытия файла
        for (int i = 0; i < L; i++) {
            free(orig[i]);
            free(array[i]);
        }
        free(orig);
        free(array);
        free(is_in_X);
        return -1;
    }

    // Записываем параметры M, L, K
    fprintf(out_file, "%d %d %d\n", M, L, K);
    // Записываем преобразованный массив
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < K; j++) {
            fprintf(out_file, "%d", array[i][j]);
            if (j < K - 1) {
                fprintf(out_file, " ");
            }
        }
        fprintf(out_file, "\n");
    }

    // Освобождаем всю выделенную память
    for (int i = 0; i < L; i++) {
        free(orig[i]);
        free(array[i]);
    }
    free(orig);
    free(array);
    free(is_in_X);
    fclose(out_file);
    return 0;
}