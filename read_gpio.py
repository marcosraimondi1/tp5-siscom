from datetime import datetime 
import csv
import time

GPIO_A = "0"
GPIO_B = "1"

def main():
    while True:
        chosen_gpio = input("Choose GPIO (A or B): ").upper()
        if chosen_gpio not in ["A", "B"]:
            print("Invalid GPIO")
            continue
        break

    with open("/dev/my_module", "w") as f:
        # select gpio
        if chosen_gpio == "A":
            f.write(GPIO_A)
        else:
            f.write(GPIO_B)

    csv_filename = "gpio_output.csv"

    # Create CSV file and write the header if it doesn't exist
    with open(csv_filename, mode='a', newline='') as csvfile:
        csv_writer = csv.writer(csvfile)
        if csvfile.tell() == 0:  # File is empty, write header
            csv_writer.writerow(["timestamp", "gpio", "value"])

    start_time = time.time()  # Record the start time

    while True:
        with open("/dev/my_module", "r") as f:
            value = f.read(1).strip()
        
        # Calculate the relative time
        relative_time = time.time() - start_time
        
        # Write the data to the CSV file
        with open(csv_filename, mode='a', newline='') as csvfile:
            csv_writer = csv.writer(csvfile)
            csv_writer.writerow([relative_time, chosen_gpio, value])
        
        time.sleep(0.5)

if __name__ == "__main__":
    main()
