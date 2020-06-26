import os
import sys

import pandas as pd


def generate_database():

    file_id = 16
    file_path = get_path_file('data_in_csv.csv')
    dfvalid = pd.read_csv(file_path, sep=';')
    for file_id in range(0, 20):
        # try:
        #     df = pd.read_csv('prod8/data_in_csv' + str(file_id) + '.csv', sep=';')
        #     file_path = get_path_file('prod8/data_in_csv' + str(file_id) + '.csv')
        #     df.to_csv(file_path, index=False, encoding='utf-8', sep=";", header=(file_id==10000))
        #     continue
        # except FileNotFoundError:
        #     continue
        data = {"id": [], "circuit": []}
        #file = open("../../files_galvani/prod8/files/gates"+str(file_id)+".out")
        file = open("../random_circuits/palm_diamond"+str(file_id)+".out")
        lines = file.read().splitlines()
        i = 0
        n = len(lines)
        print(n)

        while i < len(lines) - 1:

            id = lines[i]
            i += 1
            circlines = []
            if i % (1e3) == 0:
                print(i, n, f'{i/n*100:.2f}')
            while lines[i] != "":
                circlines.append(lines[i])
                i += 1
            i += 1
            data['id'].append(id)
            data['circuit'].append(circlines)
            #{'name':'Geo', 'physics':87, 'chemistry':92, 'algebra':97}
        file.close()
        pddata = pd.DataFrame(data)
        pddata = pddata.loc[pddata['id'].isin(dfvalid['id'])].copy()
        file_path = get_path_file('willow_diamond/not_data_in_csv'+str(file_id)+'.csv')
        #file_path = get_path_file('data_in_csv.csv')
        pddata.to_csv(file_path, index=False, encoding='utf-8', sep=";")


def get_path_file(filename):
    file_dir = os.path.dirname(os.path.abspath(__file__))
    file_path = os.path.join(file_dir, filename)
    return file_path


if __name__ == "__main__":
    generate_database()
