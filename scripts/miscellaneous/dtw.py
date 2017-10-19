import math
import numpy as np
import matplotlib.pyplot as plt

def dist(a, b):
  return (a - b) ** 2

def dtw(a, b):
  m = len(a)
  n = len(b)
  f = [[0] * n for i in range(m)]
  trace = [[0] * n for i in range(m)]

  f[0][0] = dist(a[0], b[0])
  trace[0][0] = (-1, -1)

  for i in range(1, m):
    f[i][0] = f[i - 1][0] + dist(a[i], b[0])
    trace[i][0] = (i - 1, 0)

  for i in range(1, n):
    f[0][i] = f[0][i - 1] + dist(a[0], b[i])
    trace[0][i] = (0, i - 1)

  for i in range(1, m):
    for j in range(1, n):
      if f[i - 1][j] < f[i][j - 1] and f[i - 1][j] < f[i - 1][j - 1]:
        f[i][j] = f[i - 1][j]
        trace[i][j] = (i - 1, j)
      elif f[i][j - 1] < f[i - 1][j] and f[i][j - 1] < f[i - 1][j - 1]:
        f[i][j] = f[i][j - 1]
        trace[i][j] = (i, j - 1)
      else:
        f[i][j] = f[i - 1][j - 1]
        trace[i][j] = (i - 1, j - 1)
      f[i][j] += dist(a[i], b[j])

  i, j = (m - 1, n - 1)
  path = []

  while not (i == -1):
    path.append((i, j))
    (i, j) = trace[i][j]

#  print "DTW dist = ", math.sqrt(f[m - 1][n - 1])
#  print "Path: ", path
#  valDist = 0
#  for i in range(len(path)):
#      valDist += dist(a[path[i][0]], b[path[i][1]])
#  print "Val dist = ", math.sqrt(valDist)
  return math.sqrt(f[m - 1][n - 1]), path

def plot_dtw(a, b, alabel, blabel, path, distance, offset=1):
  na = len(a)
  nb = len(b)

  xa = range(0, na)
  xb = np.arange(0, nb) + offset

  pairs = []
  path_x = []
  path_y = []
  for t in path:
    pairs.append([xa[t[0]], xb[t[1]]])
    pairs.append([a[t[0]], b[t[1]]])
    path_x.append(t[0])
    path_y.append(t[1])

  plt.plot(color='k', linestyle='--', linewidth=1, *pairs)
  plt.plot(xa, a, color='b', linestyle='-.', marker='o', linewidth=2, label=alabel)
  plt.plot(xb, b, color='r', linewidth=2, marker='x', label=blabel)
  plt.title('DTW distance = %.2f' % distance, fontsize=18)
  plt.legend(fontsize=13)

