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
    return 0.01


def eps_id():
    return 1000


def eps_gates():
    return 1


def endogamy_fmt(x, y):
    return f'{x:.3f}'.format(x)


def get_path_file(filename, subfolder=""):
    file_dir = os.path.dirname(os.path.abspath(__file__))
    file_path = os.path.join(file_dir, subfolder, filename)
    return file_path


def load_csv(name, subfolder=""):
    file_path = get_path_file(name, subfolder)
    return pd.read_csv(file_path, sep=';')


def id_name_plot(df, column_name, graphic_name, image_name, subtitle=""):
    plt.clf()
    y = []
    x = []

    for index, row in df.iterrows():
        if index % 1e3 == 0:
            print(index, f'{100 * index / len(df.index):.2f}')
        el = row[column_name]
        id = row['id']

        # para mejor visualización
        if el != 1:
            x.append(int(id, 2))
            y.append(el)

    cm = plt.cm.get_cmap('jet')

    fig = plt.figure()
    ax = fig.add_axes((0.2, 0.2, 0.7, 0.7))

    ax.set_title(graphic_name)
    ax.set_xlabel('Id de la función en decimal')
    ax.set_ylabel('Valor de la endogamia')

    ax.yaxis.set_major_formatter(tick.FuncFormatter(endogamy_fmt))

    ax.scatter(x, y, s=0.05, c=y, cmap=cm)
    fig.text(.5, .05, subtitle, ha='center')
    ax.set_xlim([-eps_id(), np.max(np.array(x)) + eps_id()])
    ax.set_ylim([-eps() + np.min(np.array(y)), np.max(np.array(y)) + eps()])

    plt.savefig(image_name)
    print(image_name + " finished")


def generate_all_graphics(csvname, subfolder="", subtitle="", suffix=""):
    df = load_csv(csvname, subfolder)
    print(subfolder + csvname + ' loaded')
    column_names = ['representation_endogamy',
                    'maximum_collapse_endogamy',
                    'minimum_collapse_endogamy',
                    'direct_collapse_endogamy',
                    'inverse_collapse_endogamy',
                    'biintertwined_collapse_endogamy',
                    'direct_intertwined_collapse_endogamy',
                    'inverse_intertwined_collapse_endogamy'
                    ]
    graphic_names = ['Endogamia por representación',
                     'Endogamia por colapso máximo',
                     'Endogamia por colapso mínimo',
                     'Endogamia por colapso directo',
                     'Endogamia por colapso inverso',
                     'Endogamia por colapso bientrelazado',
                     'Endogamia por colapso entrelazado directo',
                     'Endogamia por colapso entrelazado inverso'
                     ]
    image_names = ['Endogamia por representación.png',
                   'Endogamia por colapso máximo.png',
                   'Endogamia por colapso mínimo.png',
                   'Endogamia por colapso directo.png',
                   'Endogamia por colapso inverso.png',
                   'Endogamia por colapso bientrelazado.png',
                   'Endogamia por colapso entrelazado directo.png',
                   'Endogamia por colapso entrelazado inverso.png'
                   ]

    for col, gr, im in zip(column_names, graphic_names, image_names):
        # id_name_plot(df, col, gr, subfolder + im, subtitle)
        print(im + " finished")

    show_correlation(df, subfolder + "Correlación entre endogamias" + suffix + ".png", subtitle)


def plot_lambda_threshold(df, column_name, graphic_name, image_name):
    plt.clf()
    lines = []
    x = []
    y0 = []
    y1 = []
    for index, row in df.iterrows():
        if index % 1e3 == 0:
            print(index, f'{100 * index / len(df.index):.2f}')

        x.append(int(row['id'], 2))
        lt = ast.literal_eval(row[column_name])
        y0.append(lt[0])
        y1.append(lt[1])

        lines.append([(x[-1], y0[-1]), (x[-1], y1[-1])])

    linecoll = matcoll.LineCollection(lines, colors='k')

    fig, ax = plt.subplots()

    print('linecoll created')
    plt.scatter(x, y0, s=0.05, c='r')
    plt.scatter(x, y1, s=0.05, c='b')

    # print('dots plotted')
    ax.add_collection(linecoll)
    ax.autoscale()

    print('linecoll plotted')
    plt.title(graphic_name)
    print('title set in ' + image_name)
    # plt.show()
    plt.savefig(image_name)
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
        plot_lambda_threshold(df, col, gr, subfolder + im)
        print(subfolder + im + " finished")


def compare_endogamies(dfc, dfp,
                       column_name, graphic_name, image_name, labels):
    plt.clf()
    dfc.sort_values([column_name], ascending=[True], inplace=True)
    dfc = dfc.copy()
    dfc.drop_duplicates(subset=['id'], keep='first', inplace=True)

    dfp.sort_values([column_name], ascending=[True], inplace=True)
    dfp = dfp.copy()
    dfp.drop_duplicates(subset=['id'], keep='first', inplace=True)

    dfc.rename(columns={column_name: column_name + '_x'})
    dfp.rename(columns={column_name: column_name + '_y'})
    df = pd.merge(dfc, dfp, on="id", how='left').copy()

    df = df[df[column_name + '_x'].notna()]
    df = df[df[column_name + '_y'].notna()]
    df.reset_index(drop=True)

    x = []
    y = []
    i = 0
    for index, row in df.iterrows():
        if i % 5e3 == 0:
            print(column_name, len(df.index), f'{100 * i / len(df.index):.2f}')
        if row[column_name + '_x'] != 1 and row[column_name + '_y'] != 1:
            x.append(row[column_name + '_x'])
            y.append(row[column_name + '_y'])
        i += 1

    # f'(subtitle_string_1}\n{subtitle_string_2}',

    corr_value = df[column_name + '_x'].corr(df[column_name + '_y'])
    if np.isnan(corr_value):
        corr_value = 0
    correlation_title = "Correlación entre los valores: " + f'{corr_value:.3f}'
    cm = plt.cm.get_cmap('jet')
    fig = plt.figure()
    ax = fig.add_axes((0.2, 0.2, 0.7, 0.7))

    xl, yl = labels

    ax.set_title(graphic_name)
    ax.set_xlabel(xl)
    ax.set_ylabel(yl)

    ax.yaxis.set_major_formatter(tick.FuncFormatter(endogamy_fmt))
    ax.xaxis.set_major_formatter(tick.FuncFormatter(endogamy_fmt))

    ax.scatter(x, y, s=0.05, c=y, cmap=cm)
    # subtitle
    fig.text(.5, .05, correlation_title, ha='center')
    ax.set_xlim([-eps(), np.max(np.array(x)) + eps()])
    ax.set_ylim([-eps() + np.min(np.array(y)), np.max(np.array(y)) + eps()])

    plt.savefig(image_name)
    print(image_name + " finished")


def all_endogamies_comparated(subfolder1, subfolder2, suffix, labels):
    dfc = load_csv('endogamy.csv', subfolder1)
    dfp = load_csv('endogamy.csv', subfolder2)
    extension = '.png'
    column_names = ['representation_endogamy',
                    'maximum_collapse_endogamy',
                    'minimum_collapse_endogamy',
                    'direct_collapse_endogamy',
                    'inverse_collapse_endogamy',
                    'biintertwined_collapse_endogamy',
                    'direct_intertwined_collapse_endogamy',
                    'inverse_intertwined_collapse_endogamy',
                    'id_decimal'
                    ]
    graphic_names = ['Comparativa endogamia por representación',
                     'Comparativa endogamia por colapso máximo',
                     'Comparativa endogamia por colapso mínimo',
                     'Comparativa endogamia por colapso directo',
                     'Comparativa endogamia por colapso inverso',
                     'Comparativa endogamia por colapso bientrelazado',
                     'Comparativa endogamia por colapso entrelazado directo',
                     'Comparativa endogamia por colapso entrelazado inverso'
                     ]
    image_names = ['Comparativa endogamia por representación',
                   'Comparativa endogamia por colapso máximo',
                   'Comparativa endogamia por colapso mínimo',
                   'Comparativa endogamia por colapso directo',
                   'Comparativa endogamia por colapso inverso',
                   'Comparativa endogamia por colapso bientrelazado',
                   'Comparativa endogamia por colapso entrelazado directo',
                   'Comparativa endogamia por colapso entrelazado inverso'
                   ]

    for col, gr, im in zip(column_names, graphic_names, image_names):
        image_path = subfolder2 + im + suffix + extension
        compare_endogamies(dfc, dfp, col, gr, image_path, labels)
        print(image_path + " finished")


def get_gates(list):
    list = ast.literal_eval(list)
    list = list[:-1]
    return np.sum(np.array(list))


def generate_graphic_gates(graphic_name, image_name, subtitle=""):
    dfc = load_csv('id.csv', "")
    dfp = load_csv('id.csv', 'database/')
    df = pd.merge(dfc.copy(), dfp.copy(), on="id", how='left').copy()
    df = df[df['gates_vector' + '_x'].notna()]
    df = df[df['gates_vector' + '_y'].notna()]
    i = 0
    n = len(df.index)
    x = []
    y = []
    for index, row in df.iterrows():
        if i % (5e3) == 0:
            print(i, n, f'{i / n * 100:.2f}')
        x.append(get_gates(row['gates_vector_x']))
        y.append(get_gates(row['gates_vector_y']))
        i += 1
    cm = plt.cm.get_cmap('jet')
    fig = plt.figure()
    ax = fig.add_axes((0.15, 0.2, 0.7, 0.6))

    ax.set_title(graphic_name)
    ax.set_xlabel('Número de puertas del circuito')
    ax.set_ylabel('Número de puertas de la gramática')

    # ,f'(subtitle_string_1}\n{subtitle_string_2}'
    df = pd.DataFrame({'gates_vector_x': x, 'gates_vector_y': y})
    corr_value = df['gates_vector_x'].corr(df['gates_vector_y'])
    if np.isnan(corr_value):
        corr_value = 0
    correlation_title = "Correlación entre los valores: " + f'{corr_value:.3f}'
    fig.text(.5, .05, f'\n{correlation_title}\n{subtitle}', ha='center')

    ax.scatter(x, y, s=1, c=y, cmap=cm)
    ax.set_xlim([-eps_gates(), np.max(np.array(x)) + eps_gates()])
    ax.set_ylim([-eps_gates() + np.min(np.array(y)), np.max(np.array(y)) + eps_gates()])

    plt.savefig(image_name)

    print(image_name + " finished")


def map_for_rename():
    keys = ['representation_endogamy',
            'maximum_collapse_endogamy',
            'minimum_collapse_endogamy',
            'direct_collapse_endogamy',
            'inverse_collapse_endogamy',
            'biintertwined_collapse_endogamy',
            'direct_intertwined_collapse_endogamy',
            'inverse_intertwined_collapse_endogamy',
            'id_decimal'
            ]
    values = ['End. representación',
              'End. colapso máximo',
              'End. colapso mínimo',
              'End. colapso directo',
              'End. colapso inverso',
              'End. colapso bientrelazado',
              'End. colapso entrelazado directo',
              'End. colapso entrelazado inverso',
              'Id'
              ]
    return dict(zip(keys, values))


def show_correlation(df, image_name, graphic_name):
    plt.clf()
    df = df.copy()
    df['id_decimal'] = df['id'].copy().map(lambda x: int(x, 2) / (2 ** 32 - 1))
    df = df.rename(columns=map_for_rename())
    corr = df.corr()
    corr = corr.fillna(0)
    for i in range(len(corr)):
        corr.iloc[i, i] = 1

    # print(corr)
    fig, ax = plt.subplots(figsize=(12, 12))
    # sns.heatmap(corr, ax=ax, cmap="YlGnBu", linewidths=0.1)
    # fig = plt.figure()
    # ax = fig.add_subplot(111)
    ylabels = ['\n'.join(wrap(l, 15)) for l in df.columns]
    xlabels = ['\n'.join(wrap(l, 15)) for l in df.columns]

    im = ax.imshow(corr, cmap='Spectral', vmin=-1, vmax=1)
    divider = make_axes_locatable(ax)
    cax = divider.append_axes("right", size="5%", pad=0.1)

    fig.colorbar(im, cax=cax)
    # ticks = np.arange(0, len(df.columns), 1)
    # ax.set_xticks(ticks)
    # ax.set_yticks(ticks)
    ax.set_xticklabels(xlabels)
    ax.set_yticklabels(ylabels)
    for tick in ax.get_xticklabels():
        tick.set_rotation(20)

    ax.set_title(graphic_name, y=1.1)

    plt.savefig(image_name)
    plt.clf()
    # sns.set(style="ticks", color_codes=True)
    # g = sns.pairplot(df)
    # plt.show()


if __name__ == "__main__":
    # generate_all_graphics('endogamy.csv', 'willow_diamond/',
    #                       "Circuitos con forma 4-romboidal", ' - 4-romboide')
    # generate_all_graphics('endogamy.csv', 'diamond/',
    #                       "Circuitos con forma romboidal", ' - Romboide')
    # generate_all_graphics('endogamy.csv', 'database/', 'Gramáticas', ' - Gramáticas')
    # generate_all_graphics('endogamy.csv', '',
    #                       'Circuitos con mínimo número de puertas.',
    #                       ' - Mínimo número de puertas')
    #
    # # generate_all_lambda_threshold('lambda_threshold.csv',"")
    # generate_all_lambda_threshold('lambda_threshold.csv','database/')
    # # generate_all_lambda_threshold('lambda_threshold.csv', 'diamond/')
    # min_dia = 'Valor de la endogamia del circuito mínimo', \
    #           'Valor de la endogamia del circuito romboidal'
    #
    # min_4dia = 'Valor de la endogamia del circuito mínimo', \
    #           'Valor de la endogamia del circuito 4-romboidal'
    # dia_4dia = 'Valor de la endogamia del circuito romboidal', \
    #            'Valor de la endogamia del circuito 4-romboidal'
    # min_gra = 'Valor de la endogamia del circuito mínimo', \
    #           'Valor de la endogamia de la gramática'
    #
    # all_endogamies_comparated('diamond/', 'willow_diamond/', ' - romboide vs 4-romboide', dia_4dia)
    # all_endogamies_comparated('', 'diamond/', ' - mínimo vs - romboide', min_dia)
    # all_endogamies_comparated('', 'willow_diamond/', ' - mínimo vs 4-romboide', min_4dia)
    # all_endogamies_comparated('', 'database/', ' - mínimo vs gramática', min_gra)
    generate_graphic_gates('Comparativa tamaño de puertas',
                           'database/Comparativa tamaño de puertas.png',
                           'Puertas del circuito mínimo frente a reglas de la gramática.')
    df1 = load_csv('endogamy.csv', '')
    df2 = load_csv('endogamy.csv', 'database/')
    df3 = load_csv('endogamy.csv', 'diamond/')
    df4 = load_csv('endogamy.csv', 'willow_diamond/')
    frames = [df1, df2, df3, df4]
    df = pd.concat(frames)
    # show_correlation(df, 'Correlación entre endogamias - Todos los circuitos.png',
    #                  'Todos los circuitos')

    # new_correlation_graphics(df, df)
