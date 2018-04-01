import argparse
import logging
import os
import re
from pprint import pprint

import matplotlib.cm
import matplotlib.patches
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import squarify
from matplotlib.collections import PatchCollection

COUNT_PCT = 0.25
MAX_RECT = 600

def read_groups_size(path):
    groups = []
    num_ts = length = 0
    if os.path.exists(path):
        with open(path, 'r') as f:
            _, _, num_ts, length = f.readline().strip().split(' ')
            from_length, to_length = map(int, f.readline().strip().split(' '))
            f.readline() # Ignore distance info
            for l in range(from_length, to_length):
                count = int(f.readline())
                group_sizes = map(int, f.readline().strip().split(' '))
                for s in group_sizes:
                    groups.append({'length': l, 'size': s})
    subseq = int(num_ts) * int(length) * (int(length) - 1) / 2
    return pd.DataFrame(groups), subseq


def parse_file_name(file_name):
    try:
        pattern = '(.+)_GROUP_SIZES_([0-9.]+)'
        m = re.match(pattern, file_name)
        return m.group(1, 2)
    except IndexError as e:
        print('Invalid file name:', file_name)
        return None


def plot_groups_heatmap(ax, groups_df, count_limit,
                        width=5, height=10, padded=False):
    if groups_df.empty:
        return None
    # these values define the coordinate system for the returned rectangles
    # the values will range from x to x + width and y to y + height
    x = 0.
    y = 0.
    
    groups_df = groups_df.sort_values('size', ascending=False)
    values = groups_df['size'].values
    cumsum_val = np.cumsum(values)
    cutoff = min(max(np.argmax(cumsum_val >= count_limit), 1), MAX_RECT)
    values = values[:cutoff]
    colors = groups_df['length'].values
    colors = colors[:cutoff]

    # the sum of the values must equal the total area to be laid out
    # i.e., sum(values) == width * height
    values = squarify.normalize_sizes(values, width, height)

    if padded:
        rects = squarify.padded_squarify(values, x, y, width, height)
    else:
        rects = squarify.squarify(values, x, y, width, height)

    ax.set_xlim(0, width)
    ax.set_ylim(0, height)
    
    def to_patch(rect):
        return matplotlib.patches.Rectangle((rect['x'], rect['y']), rect['dx'], rect['dy'])
    
    patches = map(to_patch, rects)
    collection = PatchCollection(patches, cmap=matplotlib.cm.plasma, alpha=0.9)
    collection.set_array(colors)
    ax.add_collection(collection)
    ax.set_yticklabels([])
    ax.set_xticklabels([])
    return collection


if __name__ == '__main__':
    logging.basicConfig(format='%(asctime)s [%(levelname)s] %(name)s: %(message)s', 
                    level=logging.INFO)
    parser = argparse.ArgumentParser('Plot heat map of groups distribution')
    parser.add_argument('path', help=
            'path to a group size file or a folder containing the files.        \
            If a file is provided, its name must be <dataset>_GROUP_SIZES_<ST>. \
            If a folder is provided, its structure must be                      \
            <dataset>/<distance>/<dataset>_GROUP_SIZES_<ST> ')
    parser.add_argument('--st', nargs='+', required=True,
                        help='list of similarity threshold to plot')
    parser.add_argument('--distance', default='euclidean',
                        help='distance to plot')
    parser.add_argument('--width', type=int, help='width of each plot', default=5)
    parser.add_argument('--height', type=int, help='height of each plot', default=5)
    parser.add_argument('--dataset-per-fig', default=10, type=int,
                        help='number of datasets to plot per figure')
    parser.add_argument('--save-dir', default='',
                        help='if provided, the result figures will be saved here')
    parser.add_argument('--save-name', default='groups',
                        help='base name for the saved files (e.g. If this argument\
                              is "groups", the files will be: groups1.png,        \
                              groups2.png, ...')
    parser.add_argument('--save-ext', default='png',
                        help='extension of the saved file (png, jpg, or eps)')
    parser.add_argument('--no-show', action='store_true',
                        help='the figure will not be shown')

    args = parser.parse_args()
    logging.info(args)

    all_figs = []
    if os.path.isdir(args.path):
        ds_dirs = [d for d in os.listdir(args.path) if not '.' in d]
        logging.info('Found %d datasets', len(ds_dirs))
        logging.info(ds_dirs)
        next_start = 0
        for i, ds in enumerate(ds_dirs):
            if i == next_start:
                logging.info('Creating new figure')
                remaining_ds = len(ds_dirs) - i
                fig, axes = plt.subplots(min(args.dataset_per_fig, remaining_ds),
                                         len(args.st),
                                         squeeze=False,
                                         figsize=(args.width, args.height))
                if not args.no_show:
                    all_figs.append(fig)

            for j, st in enumerate(args.st):
                ax = axes[i - next_start, j]

                # Set axes titles
                if i == next_start:
                    ax.set_title('ST = ' + st, fontsize=9)
            
                if j == 0:
                    ax.set_ylabel(ds, fontsize=9)

                group_file_path = os.path.join(args.path, ds,
                                               args.distance,
                                               ds + '_GROUP_SIZES_' + st)
                if os.path.exists(group_file_path):
                    logging.info('Plotting heat map for %s', group_file_path)
                    groups_df, subseq = read_groups_size(group_file_path)
                    col = plot_groups_heatmap(ax, groups_df, COUNT_PCT*subseq,
                                              width=args.width, height=args.height)
                    
                    # Plot the colorbar
                    if col is not None:
                        fig.colorbar(col, ax=ax)
                else:
                    logging.error('Cannot find group file %s', group_file_path)
            
            if i == next_start + args.dataset_per_fig - 1 or i == len(ds_dirs) - 1:
                next_start = i + 1
                plt.tight_layout()
                fig_index = i // args.dataset_per_fig
                save_path = os.path.join(args.save_dir,
                                         args.save_name+str(fig_index)+'.'+args.save_ext)
                logging.info('Saving the figure to %s', save_path)
                fig.savefig(save_path)

    else:
        file_name = os.path.basename(args.path)
        dsname, st = parse_file_name(file_name)
        
        groups_df, subseq = read_groups_size(args.path)
        fig, ax = plt.subplots(1, 1, figsize=(args.width, args.height))
        if not args.no_show:
            all_figs.append(fig)

        plot_groups_heatmap(ax, groups_df, COUNT_PCT*subseq,
                            width=args.width, height=args.height)
        if args.save_dir:
            plt.tight_layout()    
            save_path = os.path.join(args.save_dir,
                                     args.save_name+str(i + 1)+'.'+args.save_ext)
            logging.info('Saving figure to %s', save_path)
            fig.savefig(save_path)

    if not args.no_show:
        logging.info('Showing the figures...')
        plt.show()
