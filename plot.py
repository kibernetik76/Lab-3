import pandas as pd
import matplotlib.pyplot as plt


try:
    df = pd.read_csv('benchmark_prng.csv')
except FileNotFoundError:
    print("Ошибка: Файл 'benchmark_prng.csv' не найден.")
    exit(1)


cols = ['Quadratic', 'SplitAdd', 'FibonacciWeyl', 'StdRand']
for col in cols:
    df[col] = df[col].replace(0, 1)



plt.figure(figsize=(10, 6))


plt.plot(df['Size'], df['Quadratic'], marker='o', linewidth=2, label='Квадратичный')
plt.plot(df['Size'], df['SplitAdd'], marker='s', linewidth=2, label='Разделение/Умножение')
plt.plot(df['Size'], df['FibonacciWeyl'], marker='^', linewidth=2, label='Фибоначчи + Вейль')
plt.plot(df['Size'], df['StdRand'], marker='d', linestyle='--', linewidth=2, color='black', label='Стандартный rand()')


plt.xscale('log')
plt.yscale('log')


plt.title('Сравнение скорости работы генераторов ПСЧ', fontsize=14, pad=15)
plt.xlabel('Объем выборки (количество элементов)', fontsize=12)
plt.ylabel('Время генерации (микросекунды)', fontsize=12) # Исправил подпись


plt.grid(True, which="both", linestyle="--", alpha=0.5)


plt.legend(fontsize=11)


plt.tight_layout()


plt.savefig('benchmark_plot.png', dpi=300)


plt.show()