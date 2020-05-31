import os
import sys

import pandas as pd




def generate_database():

    file_id = 16
    #for file_id in range(368):
    data = {"id": [], "circuit": []}
    file = open("../../files_galvani/circuit/gates"+str(file_id)+".out")
    lines = file.read().splitlines()
    i = 0
    print(file_id)

    while i < len(lines) - 1:

        id = lines[i]
        i += 1
        circlines = []
        while lines[i] != "":
            circlines.append(lines[i])
            i += 1
        i += 1
        data['id'].append(id)
        data['circuit'].append(circlines)
        #{'name':'Geo', 'physics':87, 'chemistry':92, 'algebra':97}
    file.close()
    pddata = pd.DataFrame(data)
    #file_path = get_path_file('data_in_csv'+str(file_id)+'.csv')
    file_path = get_path_file('data_in_csv.csv')
    pddata.to_csv(file_path, index=False, encoding='utf-8', sep=";")


def get_path_file(filename):
    file_dir = os.path.dirname(os.path.abspath(__file__))
    file_path = os.path.join(file_dir, filename)
    return file_path


if __name__ == "__main__":
    generate_database()
