#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <cstdlib> 
#include <ctime>   
#include <set>     

using namespace std;



// 1
unsigned int kvadrat(unsigned int& state) {
    state = 13 * state * state + 17 * state + 12345;
    return state;
}

// 2
unsigned int drob(unsigned int& state) {
    unsigned int lower = state % 65536; 
    unsigned int upper = state / 65536; 
    state = lower * 12345 + upper * 54321; 
    return state;
}

// 3
unsigned int custom_Fib(unsigned int& prev1, unsigned int& prev2, unsigned int& weyl) {
    // Фибоначчи
    unsigned int current_fib = prev1 + prev2;
    prev1 = prev2;
    prev2 = current_fib;
    
    // Улучшение
    weyl += 12345;
    
   
    return current_fib + weyl;
}


void printStatistics(const vector<unsigned int>& arr, const string& name) {
    double sum = 0;
    int n = arr.size();
    
    // Среднее значение
    for (int i = 0; i < n; i++) sum += arr[i];
    double mean = sum / n;
    
    // Стандартное отклонение
    double varianceSum = 0;
    for (int i = 0; i < n; i++) {
        varianceSum += (arr[i] - mean) * (arr[i] - mean);
    }
    double stdDev = sqrt(varianceSum / n);
    
    // Коэффициент вариации
    double cv = stdDev / mean;
    
    // Критерий Хи-квадрат 
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

    cout << "[" << name << "] Среднее: " << mean 
         << " | Отклонение: " << stdDev 
         << " | CV: " << cv 
         << " | Хи-квадрат: " << chiSquare << endl;
}



void runSimpleTests(const vector<unsigned int>& arr, const string& name) {
    int evenCount = 0, oddCount = 0;
    int onesCount = 0, zerosCount = 0;
    int changesCount = 0, extremesCount = 0;

    for (int i = 0; i < arr.size(); i++) {
        unsigned int num = arr[i];
        
        if (num % 2 == 0) evenCount++;
        else oddCount++;
        
        if (num < 500 || num > 4500) extremesCount++;

        
        unsigned int tempNum = num;
        int lastBit = tempNum % 2; 
        
        for (int b = 0; b < 32; b++) {
            int currentBit = tempNum % 2; 
            if (currentBit == 1) onesCount++;
            else zerosCount++;
            
            if (currentBit != lastBit) changesCount++;
            lastBit = currentBit;
            tempNum = tempNum / 2; 
        }
    }
    
    cout << "--- Тесты для " << name << " ---" << endl;
    cout << "Четные/Нечетные: " << evenCount << " / " << oddCount << endl;
    cout << "Единицы/Нули: " << onesCount << " / " << zerosCount << endl;
    cout << "Смены битов: " << changesCount << " | Экстремумы: " << extremesCount << endl;
}



int main() {
    
    unsigned int state_qcg = 42;
    unsigned int state_split = 42;
    
    // 3 метод
    unsigned int fw_prev1 = 42;
    unsigned int fw_prev2 = 100;
    unsigned int fw_weyl = 0;

    
    
    vector<unsigned int> sampleQCG(1000);
    vector<unsigned int> sampleSplit(1000);
    vector<unsigned int> sampleFW(1000);

    
    set<unsigned int> rbt_storage; 

    for (int s = 0; s < 20; s++) {
        for (int i = 0; i < 1000; i++) {
            sampleQCG[i]   = kvadrat(state_qcg) % 5000;
            sampleSplit[i] = drob(state_split) % 5000;
            sampleFW[i]    = custom_Fib(fw_prev1, fw_prev2, fw_weyl) % 5000;
            
            
            rbt_storage.insert(sampleQCG[i]);
        }
        
        if (s == 0) {
            cout << "Статистика Выборки №1:" << endl;
            printStatistics(sampleQCG, "Квадратичный");
            printStatistics(sampleSplit, "Разделение/Умножение");
            printStatistics(sampleFW, "Фибоначчи+Вейль");
            cout << endl;
            
            runSimpleTests(sampleQCG, "Квадратичный");
            runSimpleTests(sampleSplit, "Разделение/Умножение");
            runSimpleTests(sampleFW, "Фибоначчи+Вейль");
            cout << endl;
        }
    }

    cout << "Размер Красно-Черного Дерева (уникальных чисел): " << rbt_storage.size() << endl << endl;

    
    
    int sizes[] = {1000, 5000, 10000, 50000, 100000, 500000, 1000000};
    ofstream results("benchmark_prng.csv");
    results << "Size,Quadratic,SplitAdd,FibonacciWeyl,StdRand\n";

    for (int n : sizes) {
        cout << "Тест N = " << n << "..." << endl;
        
        
        unsigned long long sum = 0; 
        
        // 1. 
        clock_t start = clock();
        for (int i = 0; i < n; i++) sum += kvadrat(state_qcg) % 5000;
        clock_t end = clock();
        double timeQCG = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;

        // 2. 
        start = clock();
        for (int i = 0; i < n; i++) sum += drob(state_split) % 5000;
        end = clock();
        double timeSplit = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;

        // 3. 
        start = clock();
        for (int i = 0; i < n; i++) sum += custom_Fib(fw_prev1, fw_prev2, fw_weyl) % 5000;
        end = clock();
        double timeFW = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;

        // 4. rand
        start = clock();
        for (int i = 0; i < n; i++) sum += rand() % 5000;
        end = clock();
        double timeRand = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;

        results << n << "," << timeQCG << "," << timeSplit << "," << timeFW << "," << timeRand << "\n";
    }

    results.close();
    return 0;
}