import os
import pandas as pd
import ast
import numpy as np
import warnings
import matplotlib.pyplot as plt
from matplotlib import collections as matcoll
import matplotlib.ticker as tick
import seaborn as sns
from textwrap import wrap
from mpl_toolkits.axes_grid1 import make_axes_locatable

def get_path_file(filename, subfolder=""):
    file_dir = os.path.dirname(os.path.abspath(__file__))
    file_path = os.path.join(file_dir, subfolder, filename)
    return file_path


def load_csv(name, subfolder=""):
    file_path = get_path_file(name, subfolder)
    return pd.read_csv(file_path, sep=';')


def joint_df(subfolder):
    dfe = load_csv('endogamy.csv', subfolder)
    dfg = load_csv('id.csv', subfolder)
    listific = lambda x: np.array(ast.literal_eval(x))
    dfg['gates_vector'] = dfg['gates_vector'].copy().map(listific)
    gates = dfg['gates_vector'].to_numpy()
    gates = [np.sum(i) / len(i) for i in gates]
    dfe['gates'] = gates
    return dfe


if __name__ == "__main__":
    dfc = joint_df('')
    dfp = joint_df('database/')
    df = pd.merge(dfc, dfp, on="id", how='right').copy()
    df.dropna(how='any')
    df.dropna(how='all')
    print(dfc.index, dfp.index, df.index)


    df = df.loc[:, df.columns.isin(['biintertwined_collapse_endogamy_x',
                                    'biintertwined_collapse_endogamy_y',
                                    'gates_x',
                                    'gates_y'])].copy()
    df.to_csv('matrix.dat', header=False, index=False, encoding='utf-8', sep=",")
