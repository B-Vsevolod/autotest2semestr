#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *fdata, *fres;  // Указатели на файлы данных и результатов
    int M, N, L, K;      // Параметры задачи
    int **a;             // Основной двумерный массив
    int *x_set = NULL;   // Массив флагов для столбцов (1 - в множестве X, 0 - нет)
    int **orig = NULL;   // Копия исходного массива для сохранения оригинальных значений

    // Открытие файла данных для чтения
    fdata = fopen("data.txt", "r");
    if (fdata == NULL) {
        // Если файл не открылся, создаем файл ошибки
        fres = fopen("res.txt", "w");
        if (fres) fprintf(fres, "ERROR");
        if (fres) fclose(fres);
        return -1;  // Возвращаем -1 при ошибке
    }

    // Считывание основных параметров: M, N, L, K
    if (fscanf(fdata, "%d%d%d%d", &M, &N, &L, &K) != 4) {
        fclose(fdata);  // Закрываем файл при ошибке чтения
        fres = fopen("res.txt", "w");
        if (fres) fprintf(fres, "ERROR");
        if (fres) fclose(fres);
        return -1;
    }

    // Выделение памяти под двумерный массив (L строк)
    a = (int **)malloc(L * sizeof(int *));
    for (int i = 0; i < L; i++) {
        // Выделение памяти для каждой строки (K столбцов)
        a[i] = (int *)malloc(K * sizeof(int));
    }

    // Чтение элементов массива из файла
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < K; j++) {
            if (fscanf(fdata, "%d", &a[i][j]) != 1) {
                // Очистка памяти при ошибке чтения
                fclose(fdata);
                for (int i2 = 0; i2 < L; i2++) free(a[i2]);
                free(a);
                fres = fopen("res.txt", "w");
                if (fres) fprintf(fres, "ERROR");
                if (fres) fclose(fres);
                return -1;
            }
        }
    }
    fclose(fdata);  // Закрываем файл данных после чтения

    // Создание копии исходного массива для безопасных вычислений
    orig = (int **)malloc(L * sizeof(int *));
    for (int i = 0; i < L; i++) {
        orig[i] = (int *)malloc(K * sizeof(int));
        for (int j = 0; j < K; j++) {
            orig[i][j] = a[i][j];  // Копируем значения
        }
    }

    // Определение столбцов для замены (множество X)
    x_set = (int *)calloc(K, sizeof(int));  // Выделяем и инициализируем нулями
    for (int j = 0; j < K; j++) {           // Проверяем каждый столбец
        for (int i = 0; i < L; i++) {       // Проверяем каждый элемент в столбце
            // Проверка условия: элемент с остатком N при делении на M
            if (orig[i][j] % M == N) {
                x_set[j] = 1;  // Помечаем столбец
                break;          // Достаточно одного подходящего элемента
            }
        }
    }

    // Выполнение замены элементов в столбцах множества X
    for (int j = 1; j < K-1; j++) {  // Пропускаем первый и последний столбцы
        if (x_set[j]) {               // Если столбец в множестве X
            for (int i = 0; i < L; i++) {
                // Заменяем элемент суммой соседей из оригинального массива
                a[i][j] = orig[i][j-1] + orig[i][j+1];
            }
        }
    }

    // Запись результатов в файл res.txt
    fres = fopen("res.txt", "w");
    if (fres == NULL) {
        // Обработка ошибки открытия файла результатов
        for (int i = 0; i < L; i++) {
            free(a[i]);
            free(orig[i]);
        }
        free(a);
        free(orig);
        free(x_set);
        // Пытаемся создать файл для записи ERROR
        fres = fopen("res.txt", "w");
        if (fres) fprintf(fres, "ERROR");
        if (fres) fclose(fres);
        return -1;
    }

    // Вывод параметров в первой строке файла
    fprintf(fres, "%d %d %d %d\n", M, N, L, K);
    // Вывод преобразованного массива
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < K; j++) {
            fprintf(fres, "%d", a[i][j]);
            if (j < K-1) fprintf(fres, " ");  // Пробелы между элементами
        }
        fprintf(fres, "\n");  // Переход на новую строку после каждой строки массива
    }
    fclose(fres);  // Закрываем файл результатов

    // Освобождение выделенной памяти
    for (int i = 0; i < L; i++) {
        free(a[i]);
        free(orig[i]);
    }
    free(a);
    free(orig);
    free(x_set);

    return 0;  // Успешное завершение программы
}