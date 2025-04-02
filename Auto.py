import csv
import matplotlib.pyplot as plt
import pandas as pd

def parse_csv(filename):
    with open(filename, "r", newline="", encoding="utf-8") as file:
        reader = csv.reader(file)
        data = list(reader)
    
    structured_data = {}
    
    # Process entries in a structured way
    for i in range(0, len(data), 3):  # Every directory has 2 associated values
        if i + 2 < len(data):
            directory = data[i][0]
            file_count = int(data[i + 1][0])
            size = int(float(data[i + 2][0]))  # Convert scientific notation to integer
            structured_data[directory] = {"file_count": file_count, "size": size}
    
    return structured_data

def display_table(data):
    fig, ax = plt.subplots(figsize=(8, 4))
    columns = ["Directory", "Time (ms)", "Size (bytes)"]
    table_data = [(directory, info['file_count'], info['size']) for directory, info in data.items()]
    df = pd.DataFrame(table_data, columns=columns)
    ax.axis('tight')
    ax.axis('off')
    ax.table(cellText=df.values, colLabels=df.columns, cellLoc='center', loc='center')
    plt.title("Run Time Performance Analysis")
    plt.show()

csv_file = "C:\\Users\\brade\\Documents\\GitHub\\Replica-Reaper\\build\\Desktop_Qt_6_8_2_MinGW_64_bit-Debug\\Performance.csv"
parsed_data = parse_csv(csv_file)
display_table(parsed_data)