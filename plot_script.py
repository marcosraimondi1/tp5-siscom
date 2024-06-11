import csv
import matplotlib.pyplot as plt

def plot_gpio_data(csv_filename):
    times_A = []
    values_A = []
    times_B = []
    values_B = []

    with open(csv_filename, mode='r') as csvfile:
        csv_reader = csv.reader(csvfile)
        _ = next(csv_reader)  # Skip the header row
        for row in csv_reader:
            relative_time = float(row[0])
            gpio = row[1]
            value = int(row[2])
            
            if gpio == 'A':
                times_A.append(relative_time)
                values_A.append(value)
            elif gpio == 'B':
                times_B.append(relative_time)
                values_B.append(value)

    plt.figure(figsize=(10, 6))
    plt.plot(times_A, values_A, label='GPIO A')
    plt.plot(times_B, values_B, label='GPIO B')
    
    plt.title("GPIO OUTPUT")
    plt.xlabel("Time (s)")
    plt.ylabel("Value")
    plt.legend()
    plt.grid(True)
    plt.show()

if __name__ == "__main__":
    plot_gpio_data("gpio_output.csv")
