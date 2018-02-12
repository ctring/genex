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
 *  @return a dataset_metadata_t struct containing metadata of the dataset
 *
 *  @throw GenexException if cannot read from the given file or the dataset name 
 *                        has already been used.
 */
string loadDataset(const string& name
                   , const string& path
                   , const string& separators
                   , int maxNumRow
                   , int startCol)
{
  dataset_metadata_t info = genexAPI.loadDataset(name, path, separators, maxNumRow, startCol);
  return info.name;
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
 *  @brief loads all groups of a dataset from a file.
 *
 *  This function load all saved groups from a file to a dataset. The dimension of
 *  the dataset (e.g. size, lenth) must match the dimensions saved in the group file. 
 *
 *  @param name name of the dataset
 *  @param path path to the group file
 */
void loadGroups(const string& name, const string& path)
{
  genexAPI.loadGroups(name, path);
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
 *  @param target_name name of the target dataset
 *  @param query_name name of the query dataset
 *  @param index index of the query time series
 *  @param start start location in the query time series
 *  @param end end location in the query time series (exclusive)
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
 *  @param k number of similar time series to find
 *  @param ke number of time series to examine
 *  @param target_name name of the result dataset
 *  @param query_name name of the query dataset
 *  @param index index of the query time series
 *  @param start start location in the query time series
 *  @param end end location in the query time series (exclusive)
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

BOOST_PYTHON_MODULE(pygenex)
{
  py::def("loadDataset", loadDataset,
          (py::arg("separators")=" ", py::arg("maxNumRow")=0, py::arg("startCol")=0));
  py::def("saveDataset", saveDataset);
  py::def("normalize", normalize);
  py::def("group", groupDataset,
          (py::arg("distanceName")="euclidean", py::arg("numThreads")=1));
  py::def("saveGroups", saveGroups);
  py::def("loadGroups", loadGroups);
  py::def("distance", distance);
  py::def("sim", sim, (py::arg("start")=-1, py::arg("end")=-1));
  py::def("ksim", ksim, (py::arg("start")=-1, py::arg("end")=-1));
}
