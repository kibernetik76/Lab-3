import pandas as pd
import matplotlib.pyplot as plt

# Загружаем данные из CSV файла, который сгенерировал C++ код
try:
    df = pd.read_csv('benchmark_prng.csv')
except FileNotFoundError:
    print("Ошибка: Файл 'benchmark_prng.csv' не найден. Убедись, что C++ программа отработала корректно.")
    exit(1)

# Настраиваем размер графика
plt.figure(figsize=(10, 6))

# Строим линии для каждого метода
plt.plot(df['Size'], df['Quadratic'], marker='o', linewidth=2, label='Квадратичный')
plt.plot(df['Size'], df['SplitAdd'], marker='s', linewidth=2, label='Разделение/Умножение')
plt.plot(df['Size'], df['FibonacciWeyl'], marker='^', linewidth=2, label='Фибоначчи + Вейль')
plt.plot(df['Size'], df['StdRand'], marker='d', linestyle='--', linewidth=2, color='black', label='Стандартный rand()')

# Настраиваем оси
# Используем логарифмический масштаб, так как объем выборки прыгает от 1 000 до 1 000 000
plt.xscale('log')
plt.yscale('log')

# Подписи и заголовки
plt.title('Сравнение скорости работы генераторов ПСЧ', fontsize=14, pad=15)
plt.xlabel('Объем выборки (количество элементов)', fontsize=12)
plt.ylabel('Время генерации (миллисекунды)', fontsize=12)

# Включаем сетку для удобства чтения
plt.grid(True, which="both", linestyle="--", alpha=0.5)

# Добавляем легенду
plt.legend(fontsize=11)

# Убираем лишние отступы
plt.tight_layout()

# Сохраняем график в виде картинки (пригодится для отчета)
plt.savefig('benchmark_plot.png', dpi=300)

# Показываем график на экране
plt.show()