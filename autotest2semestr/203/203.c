#include <stdio.h>
#include <stdlib.h>

int main() {
    // Открываем входной файл
    FILE *fin = fopen("data.txt", "r");
    if (fin == NULL) {  // Если файл не открылся
        // Пытаемся открыть выходной файл
        FILE *fout = fopen("res.txt", "w");
        if (fout) {  // Если открыть удалось
            fprintf(fout, "FROM");  // Записываем "FROM"
            fclose(fout);
        }
        return -1;  // Возвращаем -1
    }

    int M, L, K;
    // Читаем три натуральных числа: M, L, K
    if (fscanf(fin, "%d %d %d", &M, &L, &K) != 3) {
        fclose(fin);  // Закрываем входной файл
        // Пытаемся открыть выходной файл
        FILE *fout = fopen("res.txt", "w");
        if (fout) {
            fprintf(fout, "FROM");  // Записываем "FROM"
            fclose(fout);
        }
        return -1;  // Возвращаем -1
    }

    // Выделяем память под двумерный массив (L строк)
    int **A = (int **)malloc(L * sizeof(int *));
    if (A == NULL) {  // Если не удалось выделить память
        fclose(fin);
        FILE *fout = fopen("res.txt", "w");
        if (fout) {
            fprintf(fout, "FROM");
            fclose(fout);
        }
        return -1;
    }
    
    // Выделяем память для каждой строки массива (K столбцов)
    for (int i = 0; i < L; i++) {
        A[i] = (int *)malloc(K * sizeof(int));
        if (A[i] == NULL) {  // Если не удалось выделить память
            // Освобождаем ранее выделенную память
            for (int j = 0; j < i; j++) {
                free(A[j]);
            }
            free(A);
            fclose(fin);
            FILE *fout = fopen("res.txt", "w");
            if (fout) {
                fprintf(fout, "FROM");
                fclose(fout);
            }
            return -1;
        }
    }

    // Читаем элементы массива из файла
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < K; j++) {
            if (fscanf(fin, "%d", &A[i][j]) != 1) {  // Если чтение не удалось
                // Освобождаем память
                for (int k = 0; k < L; k++) {
                    free(A[k]);
                }
                free(A);
                fclose(fin);
                FILE *fout = fopen("res.txt", "w");
                if (fout) {
                    fprintf(fout, "FROM");
                    fclose(fout);
                }
                return -1;
            }
        }
    }
    fclose(fin);  // Закрываем входной файл

    // Создаем копию исходного массива для сохранения оригинальных значений
    int **orig = (int **)malloc(L * sizeof(int *));
    if (orig == NULL) {
        // Освобождаем память массива A
        for (int i = 0; i < L; i++) {
            free(A[i]);
        }
        free(A);
        FILE *fout = fopen("res.txt", "w");
        if (fout) {
            fprintf(fout, "FROM");
            fclose(fout);
        }
        return -1;
    }
    // Выделяем память и копируем данные
    for (int i = 0; i < L; i++) {
        orig[i] = (int *)malloc(K * sizeof(int));
        if (orig[i] == NULL) {
            // Освобождаем ранее выделенную память
            for (int j = 0; j < i; j++) {
                free(orig[j]);
            }
            free(orig);
            for (int k = 0; k < L; k++) {
                free(A[k]);
            }
            free(A);
            FILE *fout = fopen("res.txt", "w");
            if (fout) {
                fprintf(fout, "FROM");
                fclose(fout);
            }
            return -1;
        }
        for (int j = 0; j < K; j++) {
            orig[i][j] = A[i][j];  // Копируем элемент
        }
    }

    // Определяем столбцы, где все элементы делятся на M без остатка
    int *valid_col = (int *)malloc(K * sizeof(int));
    if (valid_col == NULL) {
        // Освобождаем память
        for (int i = 0; i < L; i++) {
            free(orig[i]);
            free(A[i]);
        }
        free(orig);
        free(A);
        FILE *fout = fopen("res.txt", "w");
        if (fout) {
            fprintf(fout, "FROM");
            fclose(fout);
        }
        return -1;
    }

    // Проверяем каждый столбец
    for (int j = 0; j < K; j++) {
        valid_col[j] = 1;  // Предполагаем, что столбец подходит
        for (int i = 0; i < L; i++) {
            if (orig[i][j] % M != 0) {  // Если элемент не делится на M
                valid_col[j] = 0;  // Помечаем столбец как неподходящий
                break;
            }
        }
    }

    // Обрабатываем только валидные столбцы
    for (int j = 0; j < K; j++) {
        if (valid_col[j]) {  // Если столбец валиден
            for (int i = 0; i < L; i++) {  // Для каждой строки
                int count = 0;
                // Считаем элементы в строке i до столбца j, меньшие orig[i][j]
                for (int col = 0; col < j; col++) {
                    if (orig[i][col] < orig[i][j]) {
                        count++;
                    }
                }
                A[i][j] = count;  // Заменяем значение
            }
        }
    }

    // Записываем результат в файл res.txt
    FILE *fout = fopen("res.txt", "w");
    if (fout == NULL) {  // Если не удалось открыть файл
        // Пытаемся открыть снова
        fout = fopen("res.txt", "w");
        if (fout) {
            fprintf(fout, "FROM");
            fclose(fout);
        }
        // Освобождаем память
        for (int i = 0; i < L; i++) {
            free(orig[i]);
            free(A[i]);
        }
        free(orig);
        free(A);
        free(valid_col);
        return -1;
    }

    // Записываем M, L, K в первой строке
    fprintf(fout, "%d %d %d\n", M, L, K);
    // Записываем массив
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < K; j++) {
            fprintf(fout, "%d", A[i][j]);
            if (j < K - 1) {
                fputc(' ', fout);  // Ставим пробел между элементами
            }
        }
        fputc('\n', fout);  // Переход на новую строку после каждой строки массива
    }
    fclose(fout);  // Закрываем выходной файл

    // Освобождаем всю выделенную память
    for (int i = 0; i < L; i++) {
        free(orig[i]);
        free(A[i]);
    }
    free(orig);
    free(A);
    free(valid_col);

    return 0;  // Успешное завершение
}