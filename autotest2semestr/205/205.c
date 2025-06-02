#include <stdio.h>
#include <stdlib.h>

// Функция сравнения для qsort
int compare_ints(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

int main() {
    // Открытие файлов
    FILE *fin = fopen("data.txt", "r");
    FILE *fout = fopen("res.txt", "w");
    
    // Проверка открытия файлов
    if (fin == NULL || fout == NULL) {
        if (fout) fprintf(fout, "ERROR\n");
        else {
            // Попытка создать файл для записи ошибки
            fout = fopen("res.txt", "w");
            if (fout) fprintf(fout, "ERROR\n");
        }
        if (fin) fclose(fin);
        if (fout) fclose(fout);
        return -1;
    }

    int M, N, L, K;
    // Чтение основных параметров
    if (fscanf(fin, "%d %d %d %d", &M, &N, &L, &K) != 4) {
        fprintf(fout, "ERROR\n");
        fclose(fin);
        fclose(fout);
        return -1;
    }

    // Выделение памяти для основного массива
    int **A = (int **)malloc(L * sizeof(int *));
    if (A == NULL) {
        fprintf(fout, "ERROR\n");
        fclose(fin);
        fclose(fout);
        return -1;
    }
    
    // Выделение памяти для каждой строки
    for (int i = 0; i < L; i++) {
        A[i] = (int *)malloc(K * sizeof(int));
        if (A[i] == NULL) {
            // Освобождение ранее выделенной памяти при ошибке
            for (int j = 0; j < i; j++) free(A[j]);
            free(A);
            fprintf(fout, "ERROR\n");
            fclose(fin);
            fclose(fout);
            return -1;
        }
    }

    // Чтение данных массива из файла
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < K; j++) {
            if (fscanf(fin, "%d", &A[i][j]) != 1) {
                // Освобождение памяти при ошибке чтения
                for (int ii = 0; ii < L; ii++) free(A[ii]);
                free(A);
                fprintf(fout, "ERROR\n");
                fclose(fin);
                fclose(fout);
                return -1;
            }
        }
    }

    // Создание копии исходного массива
    int **B = (int **)malloc(L * sizeof(int *));
    if (B == NULL) {
        for (int i = 0; i < L; i++) free(A[i]);
        free(A);
        fprintf(fout, "ERROR\n");
        fclose(fin);
        fclose(fout);
        return -1;
    }
    
    // Копирование данных
    for (int i = 0; i < L; i++) {
        B[i] = (int *)malloc(K * sizeof(int));
        if (B[i] == NULL) {
            // Освобождение памяти при ошибке
            for (int j = 0; j < i; j++) free(B[j]);
            free(B);
            for (int j = 0; j < L; j++) free(A[j]);
            free(A);
            fprintf(fout, "ERROR\n");
            fclose(fin);
            fclose(fout);
            return -1;
        }
        // Копирование строки
        for (int j = 0; j < K; j++) {
            B[i][j] = A[i][j];
        }
    }

    // Поиск столбцов, входящих в множество X
    int *x_cols = (int *)malloc(K * sizeof(int));
    int countX = 0; // Количество столбцов в X
    
    if (x_cols == NULL) {
        // Освобождение памяти при ошибке
        for (int i = 0; i < L; i++) {
            free(A[i]);
            free(B[i]);
        }
        free(A);
        free(B);
        fprintf(fout, "ERROR\n");
        fclose(fin);
        fclose(fout);
        return -1;
    }

    // Проверка каждого столбца
    for (int j = 0; j < K; j++) {
        int found = 0; // Флаг наличия элемента с остатком N
        for (int i = 0; i < L; i++) {
            // Вычисление остатка с обработкой отрицательных чисел
            int r = A[i][j] % M;
            if (r < 0) r += M; // Корректировка отрицательного остатка
            if (r == N) {
                found = 1;
                break;
            }
        }
        if (found) {
            x_cols[countX] = j; // Сохранение индекса столбца
            countX++;
        }
    }

    // Сортировка индексов столбцов из X
    qsort(x_cols, countX, sizeof(int), compare_ints);

    // Обработка столбцов (кроме первого и последнего в X)
    if (countX >= 3) {
        for (int idx = 1; idx < countX - 1; idx++) {
            int j = x_cols[idx]; // Текущий столбец
            int left_col = x_cols[idx - 1]; // Предыдущий столбец в X
            int right_col = x_cols[idx + 1]; // Следующий столбец в X
            for (int i = 0; i < L; i++) {
                // Замена на сумму элементов из исходного массива
                A[i][j] = B[i][left_col] + B[i][right_col];
            }
        }
    }

    // Запись результатов
    fprintf(fout, "%d %d %d %d\n", M, N, L, K);
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < K; j++) {
            fprintf(fout, "%d", A[i][j]);
            if (j < K - 1) fprintf(fout, " "); // Пробел между элементами
        }
        fprintf(fout, "\n"); // Переход на новую строку
    }

    // Освобождение памяти
    free(x_cols);
    for (int i = 0; i < L; i++) {
        free(A[i]);
        free(B[i]);
    }
    free(A);
    free(B);
    
    // Закрытие файлов
    fclose(fin);
    fclose(fout);
    return 0;
}