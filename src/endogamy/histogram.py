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



def save_times_histogram():
    file = open('../../files_galvani/circuit/times_log.log')
    lines = file.read().splitlines()
    i = 0
    n = len(lines)
    print(n)
    idl = []
    timesl = []
    n = len(lines)
    while lines[i] != "":
        if i % (1e3) == 0:
            print(i, n, f'{i / n * 100:.2f}')
        arr = lines[i].split()
        idl.append(arr[0])
        timesl.append(arr[1])
        i += 1

        # {'name':'Geo', 'physics':87, 'chemistry':92, 'algebra':97}
    file.close()
    pddata = pd.DataFrame(list(zip(idl, timesl)), columns=['id', 'times'])
    file_path = get_path_file('times_log.csv')
    # file_path = get_path_file('data_in_csv.csv')
    pddata.to_csv(file_path, index=False, encoding='utf-8', sep=";")


def set_font(names):
    plt.rcParams["font.family"] = names

def plot_histogram(suffix=""):

    df = load_csv('times'+suffix+'.csv')

    toint = lambda x: int(x, 2)
    tolog = lambda x: np.log10(x)
    df['id'] = df['id'].map(toint)
    df['times'] = df['times'].map(tolog)

    lines = []
    z = []
    for i, t in zip(df['id'], df['times']):
        lines.append([(i, 0), (i, t)])
        z.append(t)


    lines = lines
    z = np.array(z)
    print('lines generated')
    linecoll = matcoll.LineCollection(lines, array=z, cmap=plt.cm.Spectral, linewidths=0.5)
    print('linecoll generated')

    fig = plt.figure()
    ax = fig.add_axes((0.2, 0.2, 0.7, 0.7))



    plt.rc('figure', titlesize=20)
    ax.set_xlabel('Id de la función en decimal')
    ax.set_ylabel('Apariciones (en escala logarítmica)')

    ax.add_collection(linecoll)
    fig.colorbar(linecoll)

    ax.autoscale()

    print('linecoll plotted')
    ax.set_title('Histograma de funciones', fontsize='large')
    title = 'Porcentaje de funciones analizadas: ' + f'{len(df.index)*100/(2**32):.3f}'+'\%'
    fig.text(.5, .05, title, ha='center')


    print('Saving...')
    fig.savefig('eps/histogram/histogram'+suffix+'.eps', format='eps')

    print('Saved! :)')


if __name__ == "__main__":
    # save_times_histogram()
    plt.rc('text', usetex=True)
    # font = {'family':'serif','size':16}
    font = {'family': 'serif', 'size': 10, 'serif': ['computer modern roman']}
    plt.rc('font', **font)
    plt.rc('legend', **{'fontsize': 12})
    plot_histogram("")
    plot_histogram("_log")