import os
import pandas as pd
import ast
import numpy as np
import warnings

def get_path_file(filename, subfolder=""):
    file_dir = os.path.dirname(os.path.abspath(__file__))
    file_path = os.path.join(file_dir, subfolder, filename)
    return file_path


def lambda_threshold(np_list, vector):
    vector.sort()
    np_list.sort()
    max_lambda = np.dot(vector,np_list)
    vector[::-1].sort()
    min_lambda = np.dot(vector,np_list)
    return [min_lambda, max_lambda]


def get_vdi(n):
    if n == 1:
        return np.array([1])
    vdi = np.array([2 ** i for i in range(n - 1, -1, -1)])
    vdi = vdi / (2 ** n - 1)
    return vdi


def get_vmm(n):
    if n == 1:
        return np.array([1])
    vmm = np.zeros(n)
    vmm[0] = 1
    return vmm


def get_vbi(gate_list):
    vbi = gate_list[1:]
    vbi = np.append(vbi, 1)
    s = np.sum(vbi)
    vbi = vbi / s
    return vbi


def calculate_lambda_thresholds(df):
    vdil = []
    vmml = []
    vdiil = []
    vmmil = []
    vbil = []
    df = df[~df['multilevel_endogamy'].str.contains('False')].copy()
    for index, row in df.iterrows():
        if index % 1e3 == 0:
            print(index, f'{100*index/len(df.index):.2f}')
        ml = np.array(
            ast.literal_eval(row['multilevel_endogamy']))

        n = len(ml)
        if n == 0:
            vdil.append(-1)
            continue

        vdi = get_vdi(n)
        vmm = get_vmm(n)
        vdil.append(lambda_threshold(ml, vdi))
        vmml.append(lambda_threshold(ml, vmm))

        mli = np.array(
            ast.literal_eval(row['multilevel_intertwined_endogamy']))
        gates = np.array(
            ast.literal_eval(row['gates_vector']))
        gates = gates[:-1]
        n = len(mli)
        if n == 0:
            assert False

        vdi = get_vdi(n)
        vmm = get_vmm(n)
        vbi = get_vbi(gates)
        vdiil.append(lambda_threshold(mli, vdi))
        vmmil.append(lambda_threshold(mli, vmm))
        vbil.append(lambda_threshold(mli, vbi))
    df = df[~df['multilevel_endogamy'].str.contains('False')].copy()
    print(len(vdil), len(df.index))
    df['direct_inverse_lambda_threshold'] = vdil
    df = df[~(df['direct_inverse_lambda_threshold'] == -1)].copy()
    print(len(df.index))
    df['minimum_maximum_lambda_threshold'] = vmml
    df['direct_inverse_intertwined_lambda_threshold'] = vdiil
    df['minimum_maximum_intertwined_lambda_threshold'] = vmmil
    df['biintertwined_lambda_threshold'] = vbil
    return df


if __name__ == "__main__":
    warnings.filterwarnings("error")
    # file_path = get_path_file('ml_endogamy.csv')
    # df = pd.read_csv(file_path, sep=';')
    # df = calculate_lambda_thresholds(df)
    # df = df.loc[:, df.columns.isin(['id', 'direct_inverse_lambda_threshold',
    #                                 'minimum_maximum_lambda_threshold',
    #                                 'direct_inverse_intertwined_lambda_threshold',
    #                                 'minimum_maximum_intertwined_lambda_threshold',
    #                                 'biintertwined_lambda_threshold'])].copy()
    # file_path = get_path_file('lambda_threshold.csv')
    # df.to_csv(file_path, index=False, encoding='utf-8', sep=";")

    file_path = get_path_file('ml_endogamy.csv', 'database/')
    df = pd.read_csv(file_path, sep=';')
    df = calculate_lambda_thresholds(df)
    df = df.loc[:, df.columns.isin(['id', 'direct_inverse_lambda_threshold',
                                    'minimum_maximum_lambda_threshold',
                                    'direct_inverse_intertwined_lambda_threshold',
                                    'minimum_maximum_intertwined_lambda_threshold',
                                    'biintertwined_lambda_threshold'])].copy()
    file_path = get_path_file('lambda_threshold.csv', 'database/')
    df.to_csv(file_path, index=False, encoding='utf-8', sep=";")