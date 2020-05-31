import os
import pandas as pd
import ast
import numpy as np
import warnings
import matplotlib.pyplot as plt
from matplotlib import collections as matcoll


def get_path_file(filename, subfolder=""):
    file_dir = os.path.dirname(os.path.abspath(__file__))
    file_path = os.path.join(file_dir, subfolder, filename)
    return file_path


def load_csv(name, subfolder=""):
    file_path = get_path_file(name, subfolder)
    return pd.read_csv(file_path, sep=';')


def id_name_plot(df, column_name, graphic_name, image_name):
    plt.clf()
    plt.title(graphic_name)
    ell = []
    binidl = []
    cm = plt.cm.get_cmap('jet')

    for index, row in df.iterrows():
        el = row[column_name]
        id = row['id']
        if el != 1:
            binidl.append(int(id, 2))
            ell.append(el)
    plt.scatter(binidl, ell, s=0.001, c=ell, cmap=cm)
    plt.savefig(image_name)
    print(image_name + " finished")

def generate_all_graphics(csvname, subfolder=""):
    df = load_csv(csvname, subfolder)
    id_name_plot(df, 'maximum_collapse_endogamy', "Endogamia por colapso máximo",
                 subfolder+'Endogamia por colapso máximo.png')
    id_name_plot(df, 'minimum_collapse_endogamy', "Endogamia por colapso mínimo",
                 subfolder+'Endogamia por colapso mínimo.png')
    id_name_plot(df, 'direct_collapse_endogamy', "Endogamia por colapso directo",
                 subfolder+'Endogamia por colapso directo.png')
    id_name_plot(df, 'inverse_collapse_endogamy', "Endogamia por colapso inverso",
                 subfolder+'Endogamia por colapso inverso.png')
    id_name_plot(df, 'biintertwined_collapse_endogamy', 'Endogamia por colapso bientrelazado',
                 subfolder+'Endogamia por colapso bientrelazado.png')
    id_name_plot(df, 'direct_intertwined_collapse_endogamy', 'Endogamia por colapso entrelazado directo',
                 subfolder+'Endogamia por colapso entrelazado directo.png')
    id_name_plot(df, 'inverse_intertwined_collapse_endogamy', 'Endogamia por colapso entrelazado inverso',
                 subfolder+'Endogamia por colapso entrelazado inverso.png')


def plot_lambda_threshold(df, column_name, graphic_name, image_name):
    plt.clf()
    lines = []
    x = []
    y = []
    for index, row in df.iterrows():
        if index % 1e3 == 0:
            print(index, f'{100 * index / len(df.index):.2f}')

        x.append(row['id'])
        y.append(ast.literal_eval(row[column_name]))
        lines.append([(x[-1], y[-1][0]), (x[-1], y[-1][1])])


    linecoll = matcoll.LineCollection(lines, colors='k')

    print('linecoll created')

    fig, ax = plt.subplots()
    ax.plot(x, [i for (i, j) in y], 'rs', markersize=4)
    ax.plot(x, [j for (i, j) in y], 'bo', markersize=4)

    print('dots plotted')
    ax.add_collection(linecoll)

    print('linecoll plotted')
    ax.set_title(graphic_name)
    print('title set in '+image_name)
    #plt.show()
    fig.savefig(image_name)
    print(image_name + ' saved')


def generate_all_lambda_threshold(csvname, subfolder):
    df = load_csv(csvname, subfolder)
    column_names = ['direct_inverse_lambda_threshold',
             'minimum_maximum_lambda_threshold',
             'direct_inverse_intertwined_lambda_threshold',
             'minimum_maximum_intertwined_lambda_threshold',
             'biintertwined_lambda_threshold']
    graphic_names = ['Umbral directo-inverso',
                     'Umbral máximo-mínimo',
                     'Umbral directo-inverso entrelazado',
                     'Umbral máximo-mínimo entrelazado',
                     'Umbral bientrelazado']
    image_names = ['Umbral directo-inverso.png',
                     'Umbral máximo-mínimo.png',
                     'Umbral directo-inverso entrelazado.png',
                     'Umbral máximo-mínimo entrelazado.png',
                     'Umbral bientrelazado.png']
    for col, gr, im in zip(column_names, graphic_names, image_names):
        plot_lambda_threshold(df,col, gr,subfolder+im)
        print(subfolder+im+" finished")



if __name__ == "__main__":
    #generate_all_graphics('endogamy.csv')
    # generate_all_graphics('prod_endogamy.csv', 'database/')
    generate_all_lambda_threshold('lambda_threshold.csv',"")
    generate_all_lambda_threshold('lambda_threshold','database/')

