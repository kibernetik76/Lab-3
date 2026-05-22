#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <cstdlib> 
#include <ctime>   
#include <set>     
#include <iomanip>
#include <chrono> // Добавлена библиотека для точного времени

using namespace std;

// 1. 
unsigned int kvadrat(unsigned int& state) {
    state = 13 * state * state + 17 * state + 12345;
    return state;
}

// 2. 
unsigned int drob(unsigned int& state) {
    unsigned int lower = state % 65536; 
    unsigned int upper = state / 65536; 
    state = lower * 12345 + upper * 54321; 
    return state;
}

// 3. 
unsigned int custom_Fib(unsigned int& prev1, unsigned int& prev2, unsigned int& b) {
    unsigned int current_fib = prev1 + prev2;
    prev1 = prev2;
    prev2 = current_fib;
    
    b += 12345;
    
    return current_fib + b;
}


void printStatistics(const vector<unsigned int>& arr, const string& name) {
    double sum = 0;
    int n = arr.size();
    
    for (int i = 0; i < n; i++) sum += arr[i];
    double mean = sum / n;
    
    double varianceSum = 0;
    for (int i = 0; i < n; i++) {
        varianceSum += (arr[i] - mean) * (arr[i] - mean);
    }
    double stdDev = sqrt(varianceSum / n);
    double cv = stdDev / mean;
    
    // Хи-квадрат для 10 интервалов
    int buckets[10] = {0};
    for (int i = 0; i < n; i++) {
        int index = (arr[i] * 10) / 5000; 
        if (index >= 10) index = 9; 
        buckets[index]++;
    }
    
    double expected = n / 10.0;
    double chiSquare = 0;
    for (int i = 0; i < 10; i++) {
        chiSquare += ((buckets[i] - expected) * (buckets[i] - expected)) / expected;
    }

    cout << "Генератор: " << name << "\n";
    cout << "  Среднее: " << fixed << setprecision(2) << mean 
         << " | Отклонение: " << stdDev 
         << " | Коэфф. вариации: " << cv << "\n";
         
    cout << "  Хи-квадрат: " << chiSquare;
    if (chiSquare <= 16.92) {
        cout << " (<= 16.92, распределение равномерное)\n";
    } else {
        cout << " (> 16.92, распределение НЕ равномерное)\n";
    }
}

// Углубленные битовые тесты (NIST / Diehard)
void runAdvancedTests(const vector<unsigned int>& arr, const string& name) {
    // Вытаскиваем 8 младших битов (старшие искажены из-за % 5000)
    vector<int> bits;
    for (unsigned int num : arr) {
        for (int b = 0; b < 8; b++) {
            bits.push_back((num >> b) & 1);
        }
    }
    
    int n = bits.size(); 
    int passed = 0;
    
    cout << "  Битовые тесты:\n";

    // 1. Monobit Test
    int ones = 0;
    for (int b : bits) ones += b;
    int zeros = n - ones;
    double s_obs = abs(ones - zeros) / sqrt(n);
    double p_monobit = erfc(s_obs / sqrt(2.0));
    cout << "    - Частотный (Monobit): p-value = " << p_monobit 
         << (p_monobit > 0.01 ? " [Пройден]\n" : " [Провален]\n");
    if (p_monobit > 0.01) passed++;

    // 2. Runs Test
    double pi = (double)ones / n;
    double p_runs = 0.0;
    if (abs(pi - 0.5) >= (2.0 / sqrt(n))) {
        cout << "    - Серии (Runs):        [Провален] (сильный перекос битов)\n";
    } else {
        int v_obs = 1;
        for (int i = 0; i < n - 1; i++) {
            if (bits[i] != bits[i+1]) v_obs++;
        }
        double num_r = abs(v_obs - 2.0 * n * pi * (1.0 - pi));
        double den_r = 2.0 * sqrt(2.0 * n) * pi * (1.0 - pi);
        p_runs = erfc(num_r / den_r);
        cout << "    - Серии (Runs):        p-value = " << p_runs 
             << (p_runs > 0.01 ? " [Пройден]\n" : " [Провален]\n");
        if (p_runs > 0.01) passed++;
    }

    // 3. Serial Test (2-bit)
    int pairs[4] = {0};
    for (int i = 0; i < n - 1; i++) pairs[(bits[i] << 1) | bits[i+1]]++;
    double chi2_serial = 0;
    double exp_serial = (n - 1) / 4.0;
    for (int i = 0; i < 4; i++) chi2_serial += pow(pairs[i] - exp_serial, 2) / exp_serial;
    cout << "    - Пары (Serial):       Хи-квадрат = " << chi2_serial 
         << (chi2_serial <= 11.34 ? " [Пройден]\n" : " [Провален]\n");
    if (chi2_serial <= 11.34) passed++;

    // 4. Poker Test (4-bit)
    int buckets[16] = {0};
    int k = n / 4;
    for (int i = 0; i < k; i++) {
        int val = (bits[i*4]<<3) | (bits[i*4+1]<<2) | (bits[i*4+2]<<1) | bits[i*4+3];
        buckets[val]++;
    }
    double chi2_poker = 0;
    double exp_poker = k / 16.0;
    for (int i = 0; i < 16; i++) chi2_poker += pow(buckets[i] - exp_poker, 2) / exp_poker;
    cout << "    - Тетрады (Poker):     Хи-квадрат = " << chi2_poker 
         << (chi2_poker <= 30.58 ? " [Пройден]\n" : " [Провален]\n");
    if (chi2_poker <= 30.58) passed++;

    // 5. Word Parity Test
    int even_parity = 0;
    int words = n / 8;
    for (int i = 0; i < words; i++) {
        int w_ones = 0;
        for (int j = 0; j < 8; j++) w_ones += bits[i*8+j];
        if (w_ones % 2 == 0) even_parity++;
    }
    double s_parity = abs(even_parity - words / 2.0) / sqrt(words / 4.0);
    double p_parity = erfc(s_parity / sqrt(2.0));
    cout << "    - Четность (Parity):   p-value = " << p_parity 
         << (p_parity > 0.01 ? " [Пройден]\n" : " [Провален]\n");
    if (p_parity > 0.01) passed++;

    
    cout << "  Итого пройдено тестов: " << passed << " из 5.\n";
    cout << "  Вывод: ";
    
    
    double chiSquareUniformity = 0;
    int dist_buckets[10] = {0};
    for (unsigned int val : arr) {
        int index = (val * 10) / 5000;
        if (index >= 10) index = 9;
        dist_buckets[index]++;
    }
    double expected_dist = arr.size() / 10.0;
    for (int i = 0; i < 10; i++) chiSquareUniformity += pow(dist_buckets[i] - expected_dist, 2) / expected_dist;
    bool isUniform = (chiSquareUniformity <= 16.92);

    if (isUniform && passed >= 4) {
        cout << "Генератор показывает хорошие результаты как по равномерности, так и по случайности битов.\n";
    } else if (isUniform && passed < 4) {
        cout << "Распределение равномерное, но битовая последовательность предсказуема (тесты завалены).\n";
    } else if (!isUniform && passed >= 4) {
        cout << "Генератор не равномерен по диапазону, но структура битов близка к случайной.\n";
    } else {
        cout << "Генератор не прошел проверки: отсутствует и равномерность, и битовая случайность.\n";
    }
    
}

int main() {
    // Инициализация состояний
    unsigned int state_qcg = 42;
    unsigned int state_split = 42;
    unsigned int fw_prev1 = 42;
    unsigned int fw_prev2 = 100;
    unsigned int fw_weyl = 0;

    vector<unsigned int> sampleQCG(1000);
    vector<unsigned int> sampleSplit(1000);
    vector<unsigned int> sampleFW(1000);
    set<unsigned int> rbt_storage; 

    // Запускаем сбор 20 выборок, как требует задание
    cout << "Сбор 20 выборок по 1000 элементов...\n\n";

    for (int s = 0; s < 20; s++) {
        for (int i = 0; i < 1000; i++) {
            sampleQCG[i]   = kvadrat(state_qcg) % 5000;
            sampleSplit[i] = drob(state_split) % 5000;
            sampleFW[i]    = custom_Fib(fw_prev1, fw_prev2, fw_weyl) % 5000;
            
            rbt_storage.insert(sampleQCG[i]);
        }
        
        // Выводим детальную статистику только для первой выборки, чтобы не засорять консоль
        if (s == 0) {
            cout << "=== Подробный анализ Выборки №1 ===\n\n";
            
            printStatistics(sampleQCG, "Квадратичный");
            runAdvancedTests(sampleQCG, "Квадратичный");
            
            printStatistics(sampleSplit, "Разделение/Умножение");
            runAdvancedTests(sampleSplit, "Разделение/Умножение");
            
            printStatistics(sampleFW, "Фибоначчи+Вейль");
            runAdvancedTests(sampleFW, "Фибоначчи+Вейль");
        }
    }

    cout << "\nРазмер Красно-Черного Дерева (уникальных чисел в Квадратичном методе): " 
         << rbt_storage.size() << "\n\n";

    // Бенчмарк времени (Замеры в микросекундах)
    cout << "Запуск бенчмарка производительности (сохранение в benchmark_prng.csv)...\n";
    
    // Слегка увеличим размеры, 1000 чисел процессор щелкает за наносекунды
    int sizes[] = {10000, 50000, 100000, 500000, 1000000, 5000000};
    ofstream results("benchmark_prng.csv");
    results << "Size,Quadratic,SplitAdd,FibonacciWeyl,StdRand\n";

    // ГЛОБАЛЬНАЯ СУММА: Вынесем её сюда и распечатаем в конце.
    // Теперь компилятор не имеет права удалять наши вычисления!
    unsigned long long global_sum = 0; 
    
    // Количество повторений каждого теста, чтобы сгладить "тики" таймера Windows
    const int RUNS = 50; 

    for (int n : sizes) {
        cout << "  Обработка N = " << n << "...\n";
        
        // 1. Квадратичный
        auto start = chrono::high_resolution_clock::now();
        for (int r = 0; r < RUNS; r++) {
            for (int i = 0; i < n; i++) global_sum += kvadrat(state_qcg) % 5000;
        }
        auto end = chrono::high_resolution_clock::now();
        long long timeQCG = chrono::duration_cast<chrono::microseconds>(end - start).count() / RUNS;

        // 2. Разделение
        start = chrono::high_resolution_clock::now();
        for (int r = 0; r < RUNS; r++) {
            for (int i = 0; i < n; i++) global_sum += drob(state_split) % 5000;
        }
        end = chrono::high_resolution_clock::now();
        long long timeSplit = chrono::duration_cast<chrono::microseconds>(end - start).count() / RUNS;

        // 3. Фибоначчи + Вейль
        start = chrono::high_resolution_clock::now();
        for (int r = 0; r < RUNS; r++) {
            for (int i = 0; i < n; i++) global_sum += custom_Fib(fw_prev1, fw_prev2, fw_weyl) % 5000;
        }
        end = chrono::high_resolution_clock::now();
        long long timeFW = chrono::duration_cast<chrono::microseconds>(end - start).count() / RUNS;

        // 4. rand() из <cstdlib>
        start = chrono::high_resolution_clock::now();
        for (int r = 0; r < RUNS; r++) {
            for (int i = 0; i < n; i++) global_sum += rand() % 5000;
        }
        end = chrono::high_resolution_clock::now();
        long long timeRand = chrono::duration_cast<chrono::microseconds>(end - start).count() / RUNS;

        results << n << "," << timeQCG << "," << timeSplit << "," << timeFW << "," << timeRand << "\n";
    }

    results.close();
    
    // ПЕЧАТАЕМ СУММУ В САМОМ КОНЦЕ, чтобы компилятор не вырезал циклы
    cout << "\nБенчмарк завершен. Контрольная сумма: " << global_sum << "\n";
    return 0;
}
