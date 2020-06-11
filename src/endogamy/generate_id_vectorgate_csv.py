import os
import pandas as pd
import ast


def get_path_file(filename, subfolder=""):
    file_dir = os.path.dirname(os.path.abspath(__file__))
    file_path = os.path.join(file_dir, subfolder, filename)
    return file_path


def get_gate_vector(lines):
    times_vector = []
    gates_vector = []
    lines = ast.literal_eval(lines)
    for i in range(len(lines)):
        times = {}
        gates_vector.append(0)
        elements = lines[i].split()
        for c in elements:
            if c == 'o' or c == 'b' or c == 'a' or c == 'c':
                gates_vector[-1] += 1
            elif c.isdigit():
                s = int(c)
                if s in times:
                    times[s] += 1
                else:
                    times[s] = 1
        t_v = []
        t_v = list(times.values())
        # for key, val in times.items():
        #     t_v.append(val)
        times_vector.append(t_v)
    return gates_vector, times_vector


def add_gate_vector_column(df, dfvalid):
    time_col = []
    gates_col = []
    i = 0
    n = df.size
    df = df.loc[df['id'].isin(dfvalid['id'])].copy()
    for index, row in df.iterrows():
        if i % (1e3) == 0:
            print(2*i, n, f'{2*i/n*100:.2f}')
        gc, tc = get_gate_vector(row['circuit'])
        gates_col.append(gc)
        time_col.append(tc)
        i += 1
    print(len(time_col), df.size)
    df['gates_vector'] = gates_col
    df['times_vector'] = time_col
    return df


if __name__ == "__main__":

    pass
    # file_path = get_path_file('data_in_csv.csv')
    # df = pd.read_csv(file_path, sep=';')
    # df = add_gate_vector_column(df, df)
    # print('column generated')
    # df1 = df.loc[:, df.columns.isin(['id', 'gates_vector', 'times_vector'])]
    # file_path = get_path_file('id (test).csv', '')
    # df1.to_csv(file_path, header=True, index=False, encoding='utf-8', sep=";")

    # file_path = get_path_file('data_in_csv.csv')
    # dfvalid = pd.read_csv(file_path, sep=';')
    # for i in range(10000,10153):
    #     file_path = get_path_file('data_in_csv'+str(i)+'.csv', '')
    #     df = pd.read_csv(file_path, sep=';')
    #     print('file '+str(i)+' readed')
    #
    #
    #     df = add_gate_vector_column(df, dfvalid)
    #     print('column generated')
    #     df1 = df.loc[:, df.columns.isin(['id', 'gates_vector', 'times_vector'])]
    #     file_path = get_path_file('id_gate_vector'+str(i)+'.csv', 'database/')
    #     df1.to_csv(file_path, header=(i==0), index=False, encoding='utf-8', sep=";")
        #df1.to_csv(file_path, header=True, index=False, encoding='utf-8', sep=";")
    # file_path = get_path_file('data_in_csv.csv')
    # df = pd.read_csv(file_path, sep=';')
    # print('file readed')
    #
    #
    # df = add_gate_vector_column(df, dfvalid)
    # print('column generated')
    # df1 = df.loc[:, df.columns.isin(['id', 'gates_vector', 'times_vector'])]
    # file_path = get_path_file('id.csv')
    # df1.to_csv(file_path, header=True, index=False, encoding='utf-8', sep=";")
