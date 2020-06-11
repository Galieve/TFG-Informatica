import os
import pandas as pd
import ast
import numpy as np
import warnings


def get_path_file(filename, subfolder=""):
    file_dir = os.path.dirname(os.path.abspath(__file__))
    file_path = os.path.join(file_dir, subfolder, filename)
    return file_path


def get_representation_endogamy(list):
    nl = np.array(list)
    s = np.sum(nl)
    n = len(list)
    if(n <= 1): return 1
    else:
        try:
            r = (2 ** n - 1 - s) / (2 ** n - 1 - n)
        except RuntimeWarning:
            print(n, s)
        return (2**n - 1 - s) / (2**n - 1 -n)


def add_repr_endogamy(df):
    new_column = []

    for index, row in df.iterrows():
        vl = get_representation_endogamy(
            ast.literal_eval(row['gates_vector']))
        new_column.append(vl)
    df['representation_endogamy'] = new_column


def get_multilevel_endogamy(gates, times):
    assert(len(gates) == len(times))
    n = len(gates) - 1
    e = []

    for i in range(1,n+1):
        # l va desde 0 a n-1 <=> 0 a len(gates) -2
        l = n - i
        pl = gates[i]
        tl = times[i-1]
        el = 0
        for t in tl:
            el += np.log2(t) / np.log2(2**(n-(l+1)) + 1)
        e.append(el / pl)
        try:
            assert(e[-1] <= 1)
        except AssertionError:
            print(times)
            print(e)
            assert(False)
    return e


def get_multilevel_intertwined_endogamy(times, gates):
    n = len(times) - 1
    e = []

    for i in range(1,n+1):

        l = n - i
        tl = times[i-1]
        pl = gates[i]
        pl_ = gates[i-1]
        el = 0
        for t in tl:
            el += np.log2(t) / np.log2(2*pl_)
        e.append(el / pl)
        try:
            assert(e[-1] <= 1)
        except AssertionError:
            print(times)
            print(tl, pl, pl_, len(times[i]), len(times[i-1]))
            print(e)
            assert False
    return e


def add_ml_endogamy_vector_column(df):
    new_column = []

    for index, row in df.iterrows():
        vl = get_multilevel_endogamy(
            ast.literal_eval(row['gates_vector']),
            ast.literal_eval(row['times_vector']))
        new_column.append(vl)
    df['multilevel_endogamy'] = new_column


def add_ml_intertw_endogamy_vector_column(df):
    new_column = []

    for index, row in df.iterrows():
        vl = get_multilevel_intertwined_endogamy(
            ast.literal_eval(row['times_vector']),
            ast.literal_eval(row['gates_vector']))
        new_column.append(vl)
    df['multilevel_intertwined_endogamy'] = new_column


def generate_ml_endogamy_csv(in_filename, out_filename):
    file_path = get_path_file(in_filename)
    df = pd.read_csv(file_path, sep=';')
    print(in_filename+' readed')
    add_repr_endogamy(df)
    print('repr column generated')
    add_ml_endogamy_vector_column(df)
    print('ml column generated')
    add_ml_intertw_endogamy_vector_column(df)
    print('ml_intertw column generated')
    df = df.loc[:, df.columns.isin(['id', 'representation_endogamy',
                                    'multilevel_endogamy',
                                    'multilevel_intertwined_endogamy',
                                    'gates_vector'])].copy()
    file_path = get_path_file(out_filename)
    df.to_csv(file_path, index=False, encoding='utf-8', sep=";")


def minimum_collapse_endogamy(np_list):
    if len(np_list) == 0:
        return 1
    return np.min(np_list)


def maximum_collapse_endogamy(np_list):
    if len(np_list) == 0:
        return 1
    return np.max(np_list)


def direct_collapse_endogamy(np_list):
    # n es en realidad n-1, le falta el nivel de cableado.
    n = len(np_list)
    if n == 0:
        return 1
    elif n == 1:
        return np_list[0]
    else:
        vd = np.array([2**i for i in range(0,n)])
        vd = vd / (2**n - 1)
        return np.dot(np_list, vd)


def inverse_collapse_endogamy(np_list):
    # n es en realidad n-1, le falta el nivel de cableado.
    n = len(np_list)
    if n == 0:
        return 1
    elif n == 1:
        return np_list[0]
    else:
        vd = np.array([2**(n-i-1) for i in range(0, n)])
        vd = vd / (2**n - 1)
        assert(np.sum(vd)==1)
        try:
            assert(np.max(np_list) <= 1)
        except AssertionError:
            print(np_list)
            assert(False)
        return np.dot(np_list, vd)


def bi_intertwined_endogamy(np_list, gate_list):
    if len(np_list) == 0:
        return 1
    gate_list = gate_list[:-1]
    s = np.sum(gate_list)
    gate_list = gate_list / s
    return np.dot(np_list,gate_list)


def lambda_threshold(np_list, vector):
    vector.sort()
    np_list.sort()
    max_lambda = vector * np_list
    vector[::-1].sort()
    min_lambda = vector * np_list
    return min_lambda, max_lambda


def load_ml_endogamy_csv(filename):
    file_path = get_path_file(filename)
    return pd.read_csv(file_path, sep=';')


def generate_all_endogamies(df):
    mincel = []
    maxcel = []
    dcel = []
    icel = []
    dceil = []
    iceil = []
    bieil = []
    i = 0
    for index, row in df.iterrows():
        if i == 1000:
            print(index, len(df.index), f'{100*index / len(df.index):.2f}')
            i = 0
        ml = np.array(
            ast.literal_eval(row['multilevel_endogamy']))
        mli = np.array(
            ast.literal_eval(row['multilevel_intertwined_endogamy']))
        gates = np.array(
        ast.literal_eval(row['gates_vector']))
        mince = minimum_collapse_endogamy(ml)
        maxce = maximum_collapse_endogamy(ml)
        dce = direct_collapse_endogamy(ml)
        ice = inverse_collapse_endogamy(ml)
        dcei = direct_collapse_endogamy(mli)
        icei = inverse_collapse_endogamy(mli)
        biei = bi_intertwined_endogamy(mli,gates)

        mincel.append(mince)
        maxcel.append(maxce)
        dcel.append(dce)
        icel.append(ice)
        dceil.append(dcei)
        iceil.append(icei)
        bieil.append(biei)
        i += 1
    df['minimum_collapse_endogamy'] = mincel
    df['maximum_collapse_endogamy'] = maxcel
    df['direct_collapse_endogamy'] = dcel
    df['inverse_collapse_endogamy'] = icel
    df['direct_intertwined_collapse_endogamy'] = dceil
    df['inverse_intertwined_collapse_endogamy'] = iceil
    df['biintertwined_collapse_endogamy'] = bieil



def generate_all(subfolder):
    generate_ml_endogamy_csv(subfolder+'id.csv', subfolder+'ml_endogamy.csv')
    df = load_ml_endogamy_csv(subfolder+'ml_endogamy.csv')
    generate_all_endogamies(df)
    df = df.loc[:, df.columns.isin(['id', 'biintertwined_collapse_endogamy',
                                    'inverse_intertwined_collapse_endogamy',
                                    'direct_intertwined_collapse_endogamy',
                                    'inverse_collapse_endogamy',
                                    'direct_collapse_endogamy',
                                    'maximum_collapse_endogamy',
                                    'minimum_collapse_endogamy',
                                    'representation_endogamy'])].copy()
    file_path = get_path_file('endogamy.csv', subfolder)
    df.to_csv(file_path, index=False, encoding='utf-8', sep=";")



if __name__ == "__main__":
    warnings.filterwarnings("error")
    generate_all('database/')
    generate_all('diamond/')
    generate_all('willow_diamond/')
    generate_all('')




    # for i in range(0,10):
    #     generate_ml_endogamy_csv('database/id_gate_vector'+str(i)+'.csv',
    #                             'database/ml_endogamy'+str(i)+'.csv')
    # for i in range(0,10):
    #     df = load_ml_endogamy_csv('diamond/ml_endogamy'+str(i)+'.csv')
    #     generate_all_endogamies(df)
    #     df = df.loc[:, df.columns.isin(['id', 'biintertwined_collapse_endogamy',
    #                                     'inverse_intertwined_collapse_endogamy',
    #                                     'direct_intertwined_collapse_endogamy',
    #                                     'inverse_collapse_endogamy',
    #                                     'direct_collapse_endogamy',
    #                                     'maximum_collapse_endogamy',
    #                                     'minimum_collapse_endogamy',
    #                                     'representation_endogamy'])].copy()
    #     file_path = get_path_file('diamond/endogamy'+str(i)+'.csv')
    #     df.to_csv(file_path, header=(i==0), index=False, encoding='utf-8', sep=";")