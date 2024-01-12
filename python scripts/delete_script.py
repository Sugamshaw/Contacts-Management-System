import subprocess
import os
import csv
import random

add = "./add"
delete = "./delete"


def make_file(size):
    with open(f"{size}.csv","w",newline='') as output, open("contacts.csv","r",newline='') as input:
        reader = csv.reader(input)
        writer = csv.writer(output)
        for i in range(size):
            choice = random.choice([0,1])
            row = next(reader)
            writer.writerow([row[choice]])

with open("delete_result.csv","w",newline='') as output:
    for i in range(1,14):
        make_file(i)
        file = f"{i}.csv"
        result = subprocess.run([delete, "contacts.csv", file], capture_output=True, text=True)
        time = result.stdout
        output.write(f"{i}, {time}\n")
        subprocess.call([add,"contacts.csv",file])
        os.remove(file)