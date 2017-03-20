#ifndef GENEX_API_H
#define GENEX_API_H

namespace genex {

/**
 *  @brief loads data from a text file to the memory
 *
 *  Values in the text file have to be arranged in a table format. The number of
 *  values (a.k.a columns) is inferred from the first line. All lines must have
 *  the same number of columns. If the number of lines exceeds maxNumRow, only
 *  maxNumRow lines are read and the rest is discarded. On the other hand, if
 *  maxNumRow is larger than or equal to the actual number of lines, all lines
 *  are read.
 *
 *  @param filePath path to a text file
 *  @param maxNumRow maximum number of rows to be read
 *  @param separator a string containings possible separator characters for values
 *         in a line. Default: " "
 *  @param startCol columns before startCol are discarded. Default: 0
 *  @return an index used to refer to this dataset
 *
 *  @throw GenexException if cannot read from the given file
 */
int loadDataset(const std::string& filePath, int maxNumRow,
                const std::string& separators = " ", int startCol = 0);

/**
 *  @brief gets the number of loaded dataset
 *
 *  @return number of loaded dataset
 */
int getDatasetCount();

} // namespace genex

#endif