#include <stdio.h>
#include <stdlib.h>

int main() {
    // Открываем входной файл
    FILE *in = fopen("data.txt", "r");
    if (!in) {
        // Если файл не открылся, создаем файл результата с ERROR
        FILE *out = fopen("res.txt", "w");
        if (out) {
            fprintf(out, "ERROR");
            fclose(out);
        }
        return -1;
    }

    // Читаем параметры M, L, K
    int M, L, K;
    if (fscanf(in, "%d %d %d", &M, &L, &K) != 3) {
        fclose(in);
        // Обработка ошибки чтения параметров
        FILE *out = fopen("res.txt", "w");
        if (out) {
            fprintf(out, "ERROR");
            fclose(out);
        }
        return -1;
    }

    // Выделение памяти под двумерный массив
    int **arr = (int **)malloc(L * sizeof(int *));
    if (!arr) {
        fclose(in);
        // Обработка ошибки выделения памяти
        FILE *out = fopen("res.txt", "w");
        if (out) {
            fprintf(out, "ERROR");
            fclose(out);
        }
        return -1;
    }
    for (int i = 0; i < L; i++) {
        arr[i] = (int *)malloc(K * sizeof(int));
        if (!arr[i]) {
            // Освобождаем ранее выделенную память при ошибке
            for (int j = 0; j < i; j++) free(arr[j]);
            free(arr);
            fclose(in);
            FILE *out = fopen("res.txt", "w");
            if (out) {
                fprintf(out, "ERROR");
                fclose(out);
            }
            return -1;
        }
    }

    // Чтение элементов массива
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < K; j++) {
            if (fscanf(in, "%d", &arr[i][j]) != 1) {
                // Обработка ошибки чтения элемента
                fclose(in);
                for (int ii = 0; ii < L; ii++) free(arr[ii]);
                free(arr);
                FILE *out = fopen("res.txt", "w");
                if (out) {
                    fprintf(out, "ERROR");
                    fclose(out);
                }
                return -1;
            }
        }
    }
    fclose(in);

    // Создаем копию исходного массива (для сохранения оригинальных значений)
    int **orig_arr = (int **)malloc(L * sizeof(int *));
    if (!orig_arr) {
        // Обработка ошибки выделения памяти
        for (int i = 0; i < L; i++) free(arr[i]);
        free(arr);
        FILE *out = fopen("res.txt", "w");
        if (out) {
            fprintf(out, "ERROR");
            fclose(out);
        }
        return -1;
    }
    for (int i = 0; i < L; i++) {
        orig_arr[i] = (int *)malloc(K * sizeof(int));
        if (!orig_arr[i]) {
            // Освобождаем память при ошибке
            for (int j = 0; j < i; j++) free(orig_arr[j]);
            free(orig_arr);
            for (int i = 0; i < L; i++) free(arr[i]);
            free(arr);
            FILE *out = fopen("res.txt", "w");
            if (out) {
                fprintf(out, "ERROR");
                fclose(out);
            }
            return -1;
        }
        // Копируем данные из исходного массива
        for (int j = 0; j < K; j++) {
            orig_arr[i][j] = arr[i][j];
        }
    }

    // Определение столбцов, входящих в множество X
    int *isX = (int *)calloc(K, sizeof(int));
    if (!isX) {
        // Обработка ошибки выделения памяти
        for (int i = 0; i < L; i++) {
            free(orig_arr[i]);
            free(arr[i]);
        }
        free(orig_arr);
        free(arr);
        FILE *out = fopen("res.txt", "w");
        if (out) {
            fprintf(out, "ERROR");
            fclose(out);
        }
        return -1;
    }

    // Проверка каждого столбца на соответствие условию
    for (int j = 0; j < K; j++) {
        int flag = 1;  // Предполагаем, что столбец входит в X
        for (int i = 0; i < L; i++) {
            // Проверяем делимость на M (учитываем отрицательные числа)
            if (orig_arr[i][j] % M != 0) {
                flag = 0;
                break;
            }
        }
        isX[j] = flag;  // Помечаем столбец (1 - в X, 0 - не в X)
    }

    // Создаем список индексов столбцов из X
    int *colsX = (int *)malloc(K * sizeof(int));
    if (!colsX) {
        free(isX);
        for (int i = 0; i < L; i++) {
            free(orig_arr[i]);
            free(arr[i]);
        }
        free(orig_arr);
        free(arr);
        FILE *out = fopen("res.txt", "w");
        if (out) {
            fprintf(out, "ERROR");
            fclose(out);
        }
        return -1;
    }
    int countX = 0;  // Количество столбцов в X
    for (int j = 0; j < K; j++) {
        if (isX[j]) {
            colsX[countX++] = j;  // Сохраняем индекс столбца
        }
    }

    // Обработка столбцов из множества X
    if (countX > 0) {
        // Проходим по всем столбцам в X, начиная со второго (индекс 1)
        for (int idx = 1; idx < countX; idx++) {
            int j = colsX[idx];       // Текущий столбец
            int j_prev = colsX[idx - 1];  // Предыдущий столбец в X
            for (int i = 0; i < L; i++) {
                // Заменяем элемент на сумму текущего и предыдущего из X
                arr[i][j] = orig_arr[i][j] + orig_arr[i][j_prev];
            }
        }
    }

    // Освобождаем вспомогательные ресурсы
    free(isX);
    free(colsX);
    for (int i = 0; i < L; i++) {
        free(orig_arr[i]);
    }
    free(orig_arr);

    // Запись результатов в файл
    FILE *out = fopen("res.txt", "w");
    if (!out) {
        // Обработка ошибки открытия файла результата
        for (int i = 0; i < L; i++) free(arr[i]);
        free(arr);
        return -1;
    }

    // Записываем параметры M, L, K
    if (fprintf(out, "%d %d %d\n", M, L, K) < 0) {
        // Обработка ошибки записи
        fclose(out);
        for (int i = 0; i < L; i++) free(arr[i]);
        free(arr);
        return -1;
    }

    // Записываем элементы массива
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < K; j++) {
            if (fprintf(out, "%d", arr[i][j]) < 0) {
                // Обработка ошибки записи элемента
                fclose(out);
                for (int i = 0; i < L; i++) free(arr[i]);
                free(arr);
                return -1;
            }
            // Добавляем пробелы между элементами (кроме последнего)
            if (j < K - 1) {
                if (fprintf(out, " ") < 0) {
                    fclose(out);
                    for (int i = 0; i < L; i++) free(arr[i]);
                    free(arr);
                    return -1;
                }
            }
        }
        // Переход на новую строку после каждой строки массива
        if (fprintf(out, "\n") < 0) {
            fclose(out);
            for (int i = 0; i < L; i++) free(arr[i]);
            free(arr);
            return -1;
        }
    }

    // Финализация: закрываем файл и освобождаем память
    fclose(out);
    for (int i = 0; i < L; i++) free(arr[i]);
    free(arr);
    return 0;  // Успешное завершение
}