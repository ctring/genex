#include <boost/python.hpp>
// #include <boost/foreach.hpp>
// #include <boost/range/combine.hpp>

#include <algorithm>

#include "GenexAPI.hpp"

using namespace genex;

GenexAPI genexAPI;

/**
 *  @brief loads data from a text file to the memory
 *
 *  Values in the text file have to be arranged in a table format. The number of
 *  values (a.k.a columns) is inferred from the first line. All lines must have
 *  the same number of columns. If the number of lines exceeds maxNumRow, only
 *  maxNumRow lines are read and the rest is discarded. On the other hand, if
 *  maxNumRow is larger than or equal to the actual number of lines, or maxNumRow is
 *  not positive all lines are read.
 *
 *  @param filePath path to a text file
 *  @param maxNumRow maximum number of rows to be read. If this value is not positive,
 *         all lines are read
 *  @param separator a string containings possible separator characters for values
 *         in a line
 *  @param startCol columns before startCol are discarded
 *  @return an index used to refer to the just loaded dataset
 *
 *  @throw GenexException if cannot read from the given file
 */
int loadDataset(const char* path, 
                int maxNumRow, 
                int startCol, 
                const char* separators)
{
  dataset_info_t info = genexAPI.loadDataset(path, maxNumRow, startCol, separators);
  return info.id;
}

BOOST_PYTHON_MODULE(pygenex)
{
  using namespace boost::python;
  def("loadDataset", loadDataset);
}
