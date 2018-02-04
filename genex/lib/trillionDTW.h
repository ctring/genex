// TrillionDTW codebase. See README for attribution.
#ifndef TRILLIONDTW_H
#define TRILLIONDTW_H

#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <cmath>
#include <ctime>

#include "TimeSeries.hpp"

#define INF_TRILLION 1e20       //Pseudo Infitinte number for this code

using namespace std;
using namespace genex;

/// Finding the envelop of min and max value for LB_Keogh
/// Implementation idea is intoruduced by Danial Lemire in his paper
/// "Faster Retrieval with a Two-Pass Dynamic-Time-Warping Lower Bound", Pattern Recognition 42(9), 2009.
void lower_upper_lemire(data_t *t, int len, int r, data_t *l, data_t *u);
/// Calculate quick lower bound
/// Usually, LB_Kim take time O(m) for finding top,bottom,fist and last.
/// However, because of z-normalization the top and bottom cannot give siginifant benefits.
/// And using the first and last points can be computed in constant time.
/// The prunning power of LB_Kim is non-trivial, especially when the query is not long, say in length 128.
data_t lb_kim_hierarchy(data_t *t, data_t *q, int j, int len, data_t mean, data_t std, data_t bsf = INF_TRILLION);

/// LB_Keogh 1: Create Envelop for the query
/// Note that because the query is known, envelop can be created once at the begenining.
///
/// Variable Explanation,
/// order : sorted indices for the query.
/// uo, lo: upper and lower envelops for the query, which already sorted.
/// t     : a circular array keeping the current data.
/// j     : index of the starting location in t
/// cb    : (output) current bound at each position. It will be used later for early abandoning in DTW.
data_t lb_keogh_cumulative(int* order, data_t *t, data_t *uo, data_t *lo, data_t *cb, int j, int len, data_t mean, data_t std, data_t best_so_far = INF_TRILLION);

/// LB_Keogh 2: Create Envelop for the data
/// Note that the envelops have been created (in main function) when each data point has been read.
///
/// Variable Explanation,
/// tz: Z-normalized data
/// qo: sorted query
/// cb: (output) current bound at each position. Used later for early abandoning in DTW.
/// l,u: lower and upper envelop of the current data
data_t lb_keogh_data_cumulative(int* order, data_t *tz, data_t *qo, data_t *cb, data_t *l, data_t *u, int len, data_t mean, data_t std, data_t best_so_far = INF_TRILLION);

/// Calculate Dynamic Time Wrapping distance
/// A,B: data and query, respectively
/// cb : cummulative bound used for early abandoning
/// r  : size of Sakoe-Chiba warpping band
data_t dtw(data_t* A, data_t* B, data_t *cb, int m, int r, data_t bsf = INF_TRILLION);

/// Main Calculation Function
int calculate(const char *dataPath, const char *queryPath, int queryLength, int r=2);


/// Print function for debugging
void printArray(data_t *x, int len);

/// If expected error happens, teminated the program.
void error(int id);

#endif // TRILLIONDTW_H
