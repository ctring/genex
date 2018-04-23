#include <boost/python.hpp>

#include "GenexAPI.hpp"

using namespace genex;
namespace py = boost::python;

GenexAPI genexAPI;

/* Helpers for converting C++ types to Python types */

py::list timeSeriesToPythonList(const TimeSeries & ts)
{
  py::list pl;
  for (int i = 0; i < ts.getLength(); i++)
  {
    pl.append(ts[i]);
  }
  return pl;
}

py::dict timeSeriesToPythonDict(const TimeSeries & ts)
{
  py::dict pd;
  pd["index"] = ts.getIndex();
  pd["start"] = ts.getStart();
  pd["end"] = ts.getEnd();
  return pd;
}

py::dict candidateTimeSeriesToPythonDict(const candidate_time_series_t & cts)
{
  py::dict pd;
  pd["dist"] = cts.dist;
  pd["data"] = timeSeriesToPythonDict(cts.data);
  return pd;
}

/* End helpers */

/**
 *  @brief loads data from a text file to the memory
 *
 *  Values in the text file have to be arranged in a table format. The number of
 *  values (a.k.a columns) is inferred from the first line. All lines must have
 *  the same number of columns. If the number of lines exceeds maxNumRow, only
 *  maxNumRow lines are read and the rest is discarded. If maxNumRow is larger than 
 *  or equal to the actual number of lines, or maxNumRow is not positive, all lines 
 *  are read.
 *
 *  @param name name of the dataset
 *  @param filePath path to a text file
 *  @param separator a string containing possible separator characters for values
 *         in a line
 *  @param maxNumRow maximum number of rows to be read. If this value is not positive,
 *         all lines are read
 *  @param startCol columns before startCol, in 0-based index, are discarded
 *  @param hasNameCol whether the first column (starting from startCol) is the one
 *         with names for each time series
 *  @return a dict containing information of the loaded dataset:
 *			{ 
 *			  "name": <dataset name>,
 *			  "count": <number of time series>,
 *			  "length": <length of each time series>
 *			}
 *
 *  @throw GenexException if cannot read from the given file or the dataset name 
 *                        has already been used.
 */
py::dict loadDataset(const string& name
                   , const string& path
                   , const string& separators
                   , int maxNumRow
                   , int startCol
                   , bool hasNameCol)
{
  dataset_metadata_t info = genexAPI.loadDataset(name
                                                , path
                                                , separators
                                                , maxNumRow
                                                , startCol
                                                , hasNameCol);
  py::dict pinfo;
  pinfo["name"] = info.name;
  pinfo["count"] = info.itemCount;
  pinfo["length"] = info.itemLength;
  return pinfo;
}

/**
*  @brief unloads a dataset at given name
*
*  @param name name of the dataset being unloaded
*/
void unloadDataset(const string& name)
{
  genexAPI.unloadDataset(name);
}

/*
 *  @brief saves data from memory to a file
 *
 *  @param name name of the dataset
 *  @param filePath path to a text file
 *  @param separator a character to separate entries in the file
 *
 *  @throw GenexException if cannot read from the given file
 */
void saveDataset(const string& name
                 , const string& filePath
                 , const string& separator)
{
  genexAPI.saveDataset(name, filePath, separator[0]);
}

/**
 *  @brief normalizes the dataset
 *
 *  Each value in the dataset is transformed by the following formula:
 *    d = (d - min) / (max - min)
 *  Where min and max are respectively the minimum and maximum values
 *  across the whole dataset.
 *
 *  @param name name of the dataset to be normalized
 *  @return a tuple (min, max) - the minimum and maximum value across
 *          the whole dataset before being normalized.
 */
py::tuple normalize(const string& name)
{
  std::pair<data_t, data_t> val = genexAPI.normalizeDataset(name);
  return py::make_tuple(val.first, val.second);
}

/**
 *  @brief gets name of a time series in a dataset
 *
 *  @param name name of the dataset
 *  @param index index of the time series
 *  @return name of the specified time series.
 */
string getTimeSeriesName(const string& name, int index)
{
  return genexAPI.getTimeSeriesName(name, index);
}

/**
 *  @brief groups a dataset
 *
 *  @param name name of the dataset to be grouped
 *  @param threshold the threshold to use when creating the group
 *  @param distance_name the distance to use when grouping the data
 *  @return the number of groups created
 */
int groupDataset(const string& name
                 , data_t threshold
                 , const string& distanceName
                 , int numThreads)
{
  return genexAPI.groupDataset(name, threshold, distanceName, numThreads);
}

/**
 *  @brief saves all groups of a dataset to a file
 *  
 *  This function saves all groups of a dataset to a file. The dataset must be
 *  grouped before this function is used. The dimension of the dataset (e.g. size, length)
 *  is also included in the file so that compatibility can be checked later.
 * 
 *  @param name name of the dataset
 *  @param path path to the saved group file
 */
void saveGroups(const string& name, const string& path)
{
  genexAPI.saveGroups(name, path);
}

/**
 *  @brief saves all group sizes to a file
 *  
 *  @param name name of the dataset
 *  @param path path to the saved group file
 */
void saveGroupsSize(const string& name, const string& path)
{
  genexAPI.saveGroupsOld(name, path, true);
}

/**
 *  @brief loads all groups of a dataset from a file.
 *
 *  This function load all saved groups from a file to a dataset. The dimension of
 *  the dataset (e.g. size, lenth) must match the dimensions saved in the group file. 
 *
 *  @param name name of the dataset
 *  @param path path to the group file
 *  @return number of groups
 */
int loadGroups(const string& name, const string& path)
{
  return genexAPI.loadGroups(name, path);
}


/**
 *  @brief computes the distance between 2 time series.
 * 
 *  @param name1 dataset name of the first time series
 *  @param idx1 index of the first time series
 *  @param start1 starting position of the first time series
 *  @param end1 ending position of the first time series
 *  @param name2 dataset name of the second time series
 *  @param idx2 index of the second time series
 *  @param start2 starting position of the second time series
 *  @param end2 ending position of the second time series
 *  @param distanceName name of the distance being used in the calculation
 *  @return distance between the 2 time series
 */
data_t distance(const string& name1, int idx1, int start1, int end1,
                const string& name2, int idx2, int start2, int end2,
                const string& distanceName)
{
  return genexAPI.distanceBetween(name1, idx1, start1, end1, 
                                  name2, idx2, start2, end2,
                                  distanceName);
}

/**
 *  @brief gets a single similar time series to the query
 * 
 *  If both 'start' and 'end' are smaller than 0. The whole time series is selected.
 *  @param target_name name of the target dataset
 *  @param query_name name of the query dataset
 *  @param index index of the query time series
 *  @param start start location in the query time series. Default: -1
 *  @param end end location in the query time series (exclusive). Default: -1
 *  @return a dict 
 *          { 
 *            "dist": <distance to result>, 
 *            "data": {
 *                      "index": <index of result>, 
 *                      "start": <starting position of result>,
 *                      "end": <ending position of result> 
 *                    }
 *          }
 */
py::dict sim(const string& target_name
            , const string& query_name
            , int index
            , int start
            , int end)
{
  auto res = genexAPI.getBestMatch(target_name, query_name, index, start, end);
  return candidateTimeSeriesToPythonDict(res);
}

/**
 *  @brief gets k similar time series to the query
 * 
 *  If both 'start' and 'end' are smaller than 0. The whole time series is selected.
 *  @param k number of similar time series to find
 *  @param ke number of time series to examine
 *  @param target_name name of the result dataset
 *  @param query_name name of the query dataset
 *  @param index index of the query time series.
 *  @param start start location in the query time series. Default: -1
 *  @param end end location in the query time series (exclusive). Default: -1
 *  @return a list of k or less dicts (less when k is larger than the total number of time series)
 *          [{ 
 *             "dist": <distance to result>, 
 *             "data": {
 *                       "index": <index of result>, 
 *                       "start": <starting position of result>,
 *                       "end": <ending position of result> 
 *                     }
 *            }, ...]
 */
py::list ksim(int k
              , int ke
              , const string& target_name
              , const string& query_name
              , int index
              , int start
              , int end)
{
  auto res = genexAPI.getKBestMatches(k, ke, target_name, query_name, index, start, end);
  py::list resList;
  for (auto r : res) {
    resList.append(candidateTimeSeriesToPythonDict(r));
  }
  return resList;
}

/**
 *  @brief gets k similar TimeSeries to the query, exhaustively.
 *  Provides the exact distance.
 * 
 *  @param k the number of similar time series to find
 *  @param target_name the index of the result dataset
 *  @param query_name the index of the query dataset
 *  @param index the index of the timeseries in the query dataset
 *  @param start the start of the index
 *  @param end the end of the index
 *  @param distance distance used in computing DTW
 *  @return k similar time series in the same format as ksim
 */
py::list ksimbf(int k
                , const string& target_name
                , const string& query_name
                , int index
                , int start
                , int end
                , const string& distance)
{
  auto res = genexAPI.getKBestMatchesBruteForce(
    k, target_name, query_name, index, start, end, distance);
  py::list resList;
  for (auto r : res) {
    resList.append(candidateTimeSeriesToPythonDict(r));
  }
  return resList;
}

/**
 * @brief prepare a dataset for matching with PAA
 * @param name name of a dataset
 * @blockSize block size used for PAA
 */
void preparePAA(const string& name, int blockSize) 
{
  genexAPI.preparePAA(name, blockSize);
}

/**
 *  @brief gets k similar TimeSeries to the query, using PAA.
 *  Provides approximated distance.
 * 
 *  @param k the number of similar time series to find
 *  @param target_name the index of the result dataset
 *  @param query_name the index of the query dataset
 *  @param index the index of the timeseries in the query dataset
 *  @param start the start of the index
 *  @param end the end of the index
 *  @param distance distance used in computing DTW.
 *  @return k similar time series in the same format as ksim.
 */
py::list ksimpaa(int k
                , const string& target_name
                , const string& query_name
                , int index
                , int start
                , int end
                , const string& distance)
{
  auto res = genexAPI.getKBestMatchesPAA(
    k, target_name, query_name, index, start, end, distance);
  py::list resList;
  for (auto r : res) {
    resList.append(candidateTimeSeriesToPythonDict(r));
  }
  return resList;
}

/**
 *  @brief gets a time series
 * 
 *  If both 'start' and 'end' are smaller than 0. The whole time series is selected.
 *  @param name the dataset that the time series comes from
 *  @param idx index of the time series
 *  @param start starting position of the time series. Default: -1
 *  @param end ending position of the time series. Default: -1
 * 
 *  @return a list containing all data points in the time series
 */
py::list getTimeSeries(const string& name, int idx, int start, int end)
{
  return timeSeriesToPythonList(genexAPI.getTimeSeries(name, idx, start, end));
}

py::list getAllDistances()
{
  vector<distance_metadata_t> distances = genexAPI.getAllDistanceInfo();
  py::list distList;
  for (auto d : distances)
  {
    distList.append(d.name);
  }
  return distList;
}

void setWarpignBandRatio(double ratio) {
  genexAPI.setWarpingBandRatio(ratio);
}

BOOST_PYTHON_MODULE(pygenex)
{
  py::def("loadDataset", loadDataset,
          (py::arg("separators")=" "
          , py::arg("maxNumRow")=0
          , py::arg("startCol")=0
          , py::arg("hasNameCol")=false));
  py::def("unloadDataset", unloadDataset);
  py::def("saveDataset", saveDataset);
  py::def("normalize", normalize);
  py::def("getTimeSeriesName", getTimeSeriesName);
  py::def("group", groupDataset,
          (py::arg("distanceName")="euclidean", py::arg("numThreads")=1));
  py::def("preparePAA", preparePAA);
  py::def("saveGroups", saveGroups);
  py::def("saveGroupsSize", saveGroupsSize);
  py::def("loadGroups", loadGroups);
  py::def("distance", distance);
  py::def("sim", sim, (py::arg("start")=-1, py::arg("end")=-1));
  py::def("ksim", ksim, (py::arg("start")=-1, py::arg("end")=-1));
  py::def("ksimbf", ksimbf, (py::arg("start")=-1, py::arg("end")=-1, py::arg("distance")="euclidean"));  
  py::def("ksimpaa", ksimpaa, (py::arg("start")=-1, py::arg("end")=-1, py::arg("distance")="euclidean")); 
  py::def("getTimeSeries", getTimeSeries, (py::arg("start")=-1, py::arg("end")=-1));
  py::def("getAllDistances", getAllDistances);
  py::def("setWarpingBandRatio", setWarpingBandRatio);
}
