#include <stdio.h>
#include <stdlib.h>

// Функция для освобождения памяти, выделенной под двумерный массив
void free_matrix(int **matrix, int rows) {
    if (matrix) {
        for (int i = 0; i < rows; i++) {
            free(matrix[i]);  // Освобождаем каждую строку
        }
        free(matrix);         // Освобождаем массив указателей
    }
}

int main() {
    // Шаг 1: Открытие входного файла
    FILE *fin = fopen("data.txt", "r");
    if (!fin) {
        // Обработка ошибки: если файл не открылся, записываем ERROR в res.txt
        FILE *ferr = fopen("res.txt", "w");
        if (ferr) {
            fprintf(ferr, "ERROR");
            fclose(ferr);
        }
        return -1;
    }

    // Шаг 2: Чтение параметров M, N, L, K
    int M, N, L, K;
    if (fscanf(fin, "%d %d %d %d", &M, &N, &L, &K) != 4) {
        fclose(fin);  // Закрываем файл при ошибке чтения
        FILE *ferr = fopen("res.txt", "w");
        if (ferr) {
            fprintf(ferr, "ERROR");
            fclose(ferr);
        }
        return -1;
    }

    // Шаг 3: Выделение памяти под двумерный массив
    int **a = (int **)malloc(L * sizeof(int *));
    if (!a) {  // Проверка выделения памяти
        fclose(fin);
        FILE *ferr = fopen("res.txt", "w");
        if (ferr) {
            fprintf(ferr, "ERROR");
            fclose(ferr);
        }
        return -1;
    }

    int allocated_rows = 0;  // Счетчик успешно выделенных строк
    for (int i = 0; i < L; i++) {
        a[i] = (int *)malloc(K * sizeof(int));
        if (!a[i]) {
            // Освобождаем уже выделенную память при ошибке
            free_matrix(a, allocated_rows);
            fclose(fin);
            FILE *ferr = fopen("res.txt", "w");
            if (ferr) {
                fprintf(ferr, "ERROR");
                fclose(ferr);
            }
            return -1;
        }
        allocated_rows++;
    }

    // Шаг 4: Чтение элементов массива из файла
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < K; j++) {
            if (fscanf(fin, "%d", &a[i][j]) != 1) {
                // Обработка ошибки чтения элемента
                free_matrix(a, L);
                fclose(fin);
                FILE *ferr = fopen("res.txt", "w");
                if (ferr) {
                    fprintf(ferr, "ERROR");
                    fclose(ferr);
                }
                return -1;
            }
        }
    }
    fclose(fin);  // Закрываем входной файл

    // Шаг 5: Определение столбцов, входящих в множество X
    int *in_x = (int *)calloc(K, sizeof(int));  // Инициализация нулями
    if (!in_x) {
        free_matrix(a, L);
        FILE *ferr = fopen("res.txt", "w");
        if (ferr) {
            fprintf(ferr, "ERROR");
            fclose(ferr);
        }
        return -1;
    }

    int count_x = 0;  // Количество столбцов в множестве X
    for (int j = 0; j < K; j++) {
        for (int i = 0; i < L; i++) {
            if (a[i][j] >= M && a[i][j] <= N) {
                in_x[j] = 1;  // Помечаем столбец как принадлежащий X
                count_x++;
                break;  // Достаточно одного элемента в диапазоне
            }
        }
    }

    // Шаг 6: Преобразование элементов в столбцах множества X
    if (count_x > 0) {
        // Создаем массив индексов столбцов, входящих в X
        int *cols = (int *)malloc(count_x * sizeof(int));
        // Создаем временный массив для хранения исходных значений
        int *temp = (int *)malloc(count_x * sizeof(int));
        
        if (!cols || !temp) {
            // Освобождаем ресурсы при ошибке выделения памяти
            free(in_x);
            free(cols);
            free(temp);
            free_matrix(a, L);
            FILE *ferr = fopen("res.txt", "w");
            if (ferr) {
                fprintf(ferr, "ERROR");
                fclose(ferr);
            }
            return -1;
        }

        // Заполняем массив индексов столбцов из X
        int idx = 0;
        for (int j = 0; j < K; j++) {
            if (in_x[j]) {
                cols[idx++] = j;
            }
        }

        // Обработка каждой строки
        for (int i = 0; i < L; i++) {
            // Копируем исходные значения столбцов X во временный массив
            for (int p = 0; p < count_x; p++) {
                int j = cols[p];
                temp[p] = a[i][j];
            }
            
            // Заменяем элементы (кроме последнего в X) суммами
            for (int p = 0; p < count_x - 1; p++) {
                int j = cols[p];  // Индекс текущего столбца в X
                // Сумма текущего и следующего элемента во временном массиве
                a[i][j] = temp[p] + temp[p + 1];
            }
        }

        // Освобождаем временные массивы
        free(temp);
        free(cols);
    }
    free(in_x);  // Освобождаем массив меток

    // Шаг 7: Запись результатов в файл res.txt
    FILE *fout = fopen("res.txt", "w");
    if (!fout) {
        free_matrix(a, L);
        FILE *ferr = fopen("res.txt", "w");
        if (ferr) {
            fprintf(ferr, "ERROR");
            fclose(ferr);
        }
        return -1;
    }

    // Записываем параметры M, N, L, K
    if (fprintf(fout, "%d %d %d %d", M, N, L, K) < 0) {
        fclose(fout);
        free_matrix(a, L);
        FILE *ferr = fopen("res.txt", "w");
        if (ferr) {
            fprintf(ferr, "ERROR");
            fclose(ferr);
        }
        return -1;
    }

    // Записываем элементы массива
    for (int i = 0; i < L; i++) {
        fprintf(fout, "\n");  // Новая строка для каждой строки массива
        for (int j = 0; j < K; j++) {
            // Форматированный вывод: пробел перед всеми элементами кроме первого
            if (j > 0) {
                if (fprintf(fout, " %d", a[i][j]) < 0) {
                    fclose(fout);
                    free_matrix(a, L);
                    FILE *ferr = fopen("res.txt", "w");
                    if (ferr) {
                        fprintf(ferr, "ERROR");
                        fclose(ferr);
                    }
                    return -1;
                }
            } else {
                if (fprintf(fout, "%d", a[i][j]) < 0) {
                    fclose(fout);
                    free_matrix(a, L);
                    FILE *ferr = fopen("res.txt", "w");
                    if (ferr) {
                        fprintf(ferr, "ERROR");
                        fclose(ferr);
                    }
                    return -1;
                }
            }
        }
    }

    // Шаг 8: Завершение программы
    fclose(fout);
    free_matrix(a, L);  // Освобождение памяти массива
    return 0;           // Успешное завершение
}