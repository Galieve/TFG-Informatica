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


def eps():
    return 1e-6

def get_path_file(filename, subfolder=""):
    file_dir = os.path.dirname(os.path.abspath(__file__))
    file_path = os.path.join(file_dir, subfolder, filename)
    return file_path


def load_csv(name, subfolder=""):
    file_path = get_path_file(name, subfolder)
    return pd.read_csv(file_path, sep=';')


def correlation_quotient(dfxy, variable_1, variable_2):
    e1 = dfxy[variable_1+'_x'].to_numpy()
    e2 = dfxy[variable_1+'_y'].to_numpy()
    t1 = dfxy[variable_2+'_x'].to_numpy()
    t2 = dfxy[variable_2+'_y'].to_numpy()
    e2 = np.where(e2 < eps(), eps(), e2)
    t2 = np.where(t2 < eps(), eps(), t2)
    return e1 * t2, e2 * t1


def correlation_gates_endogamy(dfxy, variable_1, variable_2):
    e1 = dfxy[variable_1+'_x'].to_numpy()
    e2 = dfxy[variable_1+'_y'].to_numpy()
    t1 = dfxy[variable_2+'_x'].to_numpy()
    t2 = dfxy[variable_2+'_y'].to_numpy()
    return t1, t2 * (1 - e2)


def correlation_gates_simple_endogamy(dfxy, variable_1, variable_2):
    e1 = dfxy[variable_1+'_x'].to_numpy()
    e2 = dfxy[variable_1+'_y'].to_numpy()
    t1 = dfxy[variable_2+'_x'].to_numpy()
    t2 = dfxy[variable_2+'_y'].to_numpy()
    return t1 * (1 - e1), t2 * (1 - e2)


def correlation_list(dfxy, variable_1, variable_2):
    corr = []
    a, b = correlation_gates_simple_endogamy(dfxy, variable_1, variable_2)
    c, d = correlation_gates_endogamy(dfxy, variable_1, variable_2)
    e, f = correlation_quotient(dfxy, variable_1, variable_2)
    e = np.nan_to_num(e)
    f = np.nan_to_num(f)

    df = pd.DataFrame({'Simple_x': a, 'Simple_y': b,
                       'Gates_x': c, 'Gates_y': d,
                       'Quotient_x': e, 'Quotient_y': f})
    corr.append(df['Simple_x'].corr(df['Simple_y']))
    corr.append(df['Gates_x'].corr(df['Gates_y']))
    corr.append(df['Quotient_x'].corr(df['Quotient_y']))
    return corr


def correlation_matrix(dfx, dfy, column_names):
    correlation_matrix = []
    gate_col = 'gates'
    for col in column_names:
        dfy.sort_values([col], ascending=[True], inplace=True)
        dfyc = dfy.copy()
        dfyc.drop_duplicates(subset=['id'], keep='first', inplace=True)
        print(len(dfyc.index), len(set(dfyc['id'])))
        dfxy = pd.merge(dfx, dfyc, left_on='id', right_on='id', how='left')
        # dfxy = pd.merge(dfx, dfy, on=['id'], how='left')
        dfxy.dropna(how='any', inplace=True)
        print('data '+col+'merged')
        correlation_matrix.append(correlation_list(dfxy, col, gate_col))
        print(col + ' done it')

    return correlation_matrix


def prepare_dataframe(subfolder):
    df1e = load_csv('endogamy.csv', subfolder)
    print(subfolder+'endogamy.csv'+' loaded')
    df1i = load_csv('id.csv', subfolder)
    print(subfolder+'id.csv'+' loaded')
    listific = lambda x: np.array(ast.literal_eval(x))
    df1i['gates_vector'] = df1i['gates_vector'].copy().map(listific)
    gates = df1i['gates_vector'].to_numpy()
    gates = [np.sum(i) for i in gates]
    df1e['gates'] = gates

    print('data in '+subfolder+' prepared')
    return df1e


def plot_histogram_correlations(corr_matrix, graphic_name, image_name, xlabels, ylabels):
    plt.clf()

    fig, ax = plt.subplots(figsize=(12, 12))

    im = ax.imshow(corr_matrix, cmap='Spectral', vmin=-1, vmax=1)
    divider = make_axes_locatable(ax)
    cax = divider.append_axes("right", size="5%", pad=0.1)
    fig.colorbar(im, cax=cax)
    ax.set_xticklabels(xlabels)
    # ymin, ymax = ax.get_ylim()
    # ylong = ymax - ymin
    # yticks = [(i*ylong)/4 + ymin for i in range(1,4)]
    # ax.set_yticks(yticks, minor=True)
    ax.set_yticklabels(ylabels)
    for tick in ax.get_xticklabels():
        tick.set_rotation(20)

    ax.set_title(graphic_name, y=1.1)

    plt.savefig(image_name)
    plt.clf()


def new_correlations(subfolder, suffix, othersubfolder=""):
    dfx = prepare_dataframe(othersubfolder)
    dfy = prepare_dataframe(subfolder)

    print('data merged')
    column_names = ['representation_endogamy',
                    'maximum_collapse_endogamy',
                    'minimum_collapse_endogamy',
                    'direct_collapse_endogamy',
                    'inverse_collapse_endogamy',
                    'biintertwined_collapse_endogamy',
                    'direct_intertwined_collapse_endogamy',
                    'inverse_intertwined_collapse_endogamy'
                    ]
    corr_matrix = correlation_matrix(dfx, dfy, column_names)
    corr_matrix = np.array(corr_matrix)
    corr_matrix = corr_matrix.T
    corr_matrix = np.nan_to_num(corr_matrix)
    print(corr_matrix)
    xlabels = ['End. representación',
               'End. colapso máximo',
               'End. colapso mínimo',
               'End. colapso directo',
               'End. colapso inverso',
               'End. colapso bientrelazado',
               'End. colapso entrelazado directo',
               'End. colapso entrelazado inverso',
               ]
    ylabels = ['','End. Reutilización simple','','End. Reutilización','','End. cociente','']
    plot_histogram_correlations(corr_matrix, 'Correlaciones nuevas endogamias - ' + suffix,
                                'Correlaciones nuevas endogamias - ' + suffix + '.png',
                                xlabels, ylabels)


if __name__ == "__main__":
    # new_correlations('diamond/', 'Circuito mínimo vs Romboides')
    # new_correlations('willow_diamond/', 'Circuito mínimo vs 4-Romboides')
    new_correlations('database/', 'Circuito mínimo vs - Gramáticas')
    # new_correlations('database/', 'Gramáticas vs Romboides', 'diamond/')
    # new_correlations('database/', 'Gramáticas vs 4-Romboides', 'willow_diamond/')

