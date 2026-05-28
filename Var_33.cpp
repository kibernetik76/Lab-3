#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <cstdlib> 
#include <ctime>   
#include <set>     
#include <iomanip>
#include <chrono>

using namespace std;

// 1. 
unsigned int kvadrat(unsigned int& num) {
    num = 13 * num * num + 19 * num + 12345;
    return num;
}

//2.
unsigned int drob(unsigned int& num) {
    unsigned int low = num % 100000; 
    
    unsigned int up = num / 100000; 

    num = low * 123456 + up * 335298; 
    
    return num;
}

//3.
unsigned int fib(unsigned int& prev1, unsigned int& prev2, unsigned int& b) {
    unsigned int current_fib = prev1 + prev2;
    prev1 = prev2;
    prev2 = current_fib;
    
    b += 12345;
    
    return current_fib + b;
}

// Стандартные тесты
void printStatistics(const vector<unsigned int>& arr, const string& name, bool print_out) {
    //Среднее
    double sum = 0;
    int n = arr.size();
    for (int i = 0; i < n; i++) sum += arr[i];
    double mean = sum / n;

    //Стандартное отклонение
    double varianceSum = 0;
    for (int i = 0; i < n; i++) {
        varianceSum += (arr[i] - mean) * (arr[i] - mean);
    }
    double stdDev = sqrt(varianceSum / n);
    
    //Коэф вариации
    double cv = stdDev / mean;

    //10 интервалов
    int buckets[10] = {0};
    for (int i = 0; i < n; i++) {
        int index = (arr[i] * 10) / 5000; 
        if (index >= 10){ 
             index = 9;
        }
        buckets[index]++;
    }
    
    //Хи квадарат
    double expected = n / 10.0;
    double chiSquare = 0;
    for (int i = 0; i < 10; i++) {
        chiSquare += ((buckets[i] - expected) * (buckets[i] - expected)) / expected;
    }


    if (print_out) {
        cout  << name << "\n";
        cout << "Среднее: " << fixed << setprecision(2) << mean 
             << ", Откл: " << stdDev 
             << ", Коэф.вар: " << cv << "\n";
             
        cout << "Хи-квадрат: " << chiSquare 
             << (chiSquare <= 16.92 ? " (Равномерно)" : " (Не равномерно)") << "\n";
    }
}

// Битовые тесты
void runAdvancedTests(const vector<unsigned int>& arr, const string& name, bool print_out) {
    vector<int> bits;
    for (unsigned int num : arr) {
        for (int b = 0; b < 8; b++) {
            bits.push_back((num >> b) & 1);
        }
    }
    
    int n = bits.size(); 
    int passed = 0;
    
    if (print_out) cout << "Тесты NIST/Diehard:\n";

    // 1. Частотный (Monobit)
    int ones = 0;
    for (int b : bits) ones += b;
    int zeros = n - ones;
    double s_obs = abs(ones - zeros) / sqrt(n);
    double p_monobit = erfc(s_obs / sqrt(2.0));
    bool pass_monobit = (p_monobit > 0.01);
    if (pass_monobit) passed++;
    if (print_out) cout << "  - Monobit: " << (pass_monobit ? "OK" : "Провал") << "\n";

    // 2. Серии (Runs)
    double pi = (double)ones / n;
    bool pass_runs = false;
    if (abs(pi - 0.5) >= (2.0 / sqrt(n))) {
        if (print_out) cout << "  - Runs: Провал (перекос битов)\n";
    } else {
        int v_obs = 1;
        for (int i = 0; i < n - 1; i++) {
            if (bits[i] != bits[i+1]) v_obs++;
        }
        double num_r = abs(v_obs - 2.0 * n * pi * (1.0 - pi));
        double den_r = 2.0 * sqrt(2.0 * n) * pi * (1.0 - pi);
        double p_runs = erfc(num_r / den_r);
        pass_runs = (p_runs > 0.01);
        if (pass_runs) passed++;
        if (print_out) cout << "  - Runs: " << (pass_runs ? "OK" : "Провал") << "\n";
    }

    // 3. Пары (Serial)
    int pairs[4] = {0};
    for (int i = 0; i < n - 1; i++) pairs[(bits[i] << 1) | bits[i+1]]++;
    double chi2_serial = 0;
    double exp_serial = (n - 1) / 4.0;
    for (int i = 0; i < 4; i++) chi2_serial += pow(pairs[i] - exp_serial, 2) / exp_serial;
    bool pass_serial = (chi2_serial <= 11.34);
    if (pass_serial) passed++;
    if (print_out) cout << "  - Serial: " << (pass_serial ? "OK" : "Провал") << "\n";

    // 4. Тетрады (Poker)
    int poker_buckets[16] = {0};
    int k = n / 4;
    for (int i = 0; i < k; i++) {
        int val = (bits[i*4]<<3) | (bits[i*4+1]<<2) | (bits[i*4+2]<<1) | bits[i*4+3];
        poker_buckets[val]++;
    }
    double chi2_poker = 0;
    double exp_poker = k / 16.0;
    for (int i = 0; i < 16; i++) chi2_poker += pow(poker_buckets[i] - exp_poker, 2) / exp_poker;
    bool pass_poker = (chi2_poker <= 30.58);
    if (pass_poker) passed++;
    if (print_out) cout << "  - Poker: " << (pass_poker ? "OK" : "Провал") << "\n";

    // 5. Четность (Parity)
    int even_parity = 0;
    int words = n / 8;
    for (int i = 0; i < words; i++) {
        int w_ones = 0;
        for (int j = 0; j < 8; j++) w_ones += bits[i*8+j];
        if (w_ones % 2 == 0) even_parity++;
    }
    double s_parity = abs(even_parity - words / 2.0) / sqrt(words / 4.0);
    double p_parity = erfc(s_parity / sqrt(2.0));
    bool pass_parity = (p_parity > 0.01);
    if (pass_parity) passed++;
    if (print_out) cout << "  - Parity: " << (pass_parity ? "OK" : "Провал") << "\n";

    if (print_out) cout << "Итог: пройдено " << passed << "/5\n\n";
}

int main() {
    unsigned int state_qcg = 42;
    unsigned int state_split = 42;
    unsigned int fw_prev1 = 42;
    unsigned int fw_prev2 = 100;
    unsigned int fw_weyl = 0;

    vector<unsigned int> sampleQCG(1000);
    vector<unsigned int> sampleSplit(1000);
    vector<unsigned int> sampleFW(1000);
    set<unsigned int> rbt_storage; 

    cout << "Сбор 20 выборок по 1000 элементов...\n\n";

    for (int s = 0; s < 20; s++) {
        for (int i = 0; i < 1000; i++) {
            sampleQCG[i]   = kvadrat(state_qcg) % 5000;
            sampleSplit[i] = drob(state_split) % 5000;
            sampleFW[i]    = fib(fw_prev1, fw_prev2, fw_weyl) % 5000;
            
            rbt_storage.insert(sampleQCG[i]);
        }
        
        // Выводим в консоль только результаты первой выборки, но считаем для всех
        bool print_this_round = (s == 0);
        
        if (print_this_round) {
            cout << "--- Результаты (Выборка 1) ---\n";
        }
        
        printStatistics(sampleQCG, "Квадратичный", print_this_round);
        runAdvancedTests(sampleQCG, "Квадратичный", print_this_round);
        
        printStatistics(sampleSplit, "Разделение", print_this_round);
        runAdvancedTests(sampleSplit, "Разделение", print_this_round);
        
        printStatistics(sampleFW, "Фибоначчи+Вейль", print_this_round);
        runAdvancedTests(sampleFW, "Фибоначчи+Вейль", print_this_round);
    }

    cout << "Уникальных чисел в Квадратичном методе: " << rbt_storage.size() << "\n\n";

    // Бенчмарк
    cout << "Запуск бенчмарка...\n";
    
    int sizes[] = {10000, 50000, 100000, 500000, 1000000, 5000000};
    ofstream results("benchmark_prng.csv");
    results << "Size,Quadratic,SplitAdd,FibonacciWeyl,StdRand\n";

    // Сумма, чтобы компилятор при оптимизации не вырезал наши пустые циклы
    unsigned long long global_sum = 0; 
    const int RUNS = 50; 

    for (int n : sizes) {
        cout << "Обработка N = " << n << "\n";
        
        // 1
        auto start = chrono::high_resolution_clock::now();
        for (int r = 0; r < RUNS; r++) {
            for (int i = 0; i < n; i++) global_sum += kvadrat(state_qcg) % 5000;
        }
        auto end = chrono::high_resolution_clock::now();
        long long timeQCG = chrono::duration_cast<chrono::microseconds>(end - start).count() / RUNS;

        // 2
        start = chrono::high_resolution_clock::now();
        for (int r = 0; r < RUNS; r++) {
            for (int i = 0; i < n; i++) global_sum += drob(state_split) % 5000;
        }
        end = chrono::high_resolution_clock::now();
        long long timeSplit = chrono::duration_cast<chrono::microseconds>(end - start).count() / RUNS;

        // 3
        start = chrono::high_resolution_clock::now();
        for (int r = 0; r < RUNS; r++) {
            for (int i = 0; i < n; i++) global_sum += fib(fw_prev1, fw_prev2, fw_weyl) % 5000;
        }
        end = chrono::high_resolution_clock::now();
        long long timeFW = chrono::duration_cast<chrono::microseconds>(end - start).count() / RUNS;

        // 4
        start = chrono::high_resolution_clock::now();
        for (int r = 0; r < RUNS; r++) {
            for (int i = 0; i < n; i++) global_sum += rand() % 5000;
        }
        end = chrono::high_resolution_clock::now();
        long long timeRand = chrono::duration_cast<chrono::microseconds>(end - start).count() / RUNS;

        results << n << "," << timeQCG << "," << timeSplit << "," << timeFW << "," << timeRand << "\n";
    }

    results.close();
    cout << "\nГотово. Контрольная сумма: " << global_sum << "\n";
    return 0;
}