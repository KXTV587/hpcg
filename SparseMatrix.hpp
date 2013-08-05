
//@HEADER
// ************************************************************************
// 
//               HPCG: Simple Conjugate Gradient Benchmark Code
// Questions? Contact Michael A. Heroux (maherou@sandia.gov) 
// 
// ************************************************************************
//@HEADER

#ifndef SPARSEMATRIX_HPP
#define SPARSEMATRIX_HPP

#include <map>
#include <vector>
#include "Geometry.hpp"
#include <iostream>

struct SparseMatrix_STRUCT {
  char   *title;
  int totalNumberOfRows;
  int totalNumberOfNonzeros;
  int localNumberOfRows;
  int localNumberOfColumns;  // Must be defined in make_local_matrix
  int localNumberOfNonzeros;
  int  * nonzerosInRow;
  global_int_t ** matrixIndices;
  double ** matrixValues;
  double ** matrixDiagonal;
  std::map< int, int > globalToLocalMap;
  std::vector< int > localToGlobalMap;

#ifdef USING_MPI
  int numberOfExternalValues;
  int numberOfSendNeighbors;
  int totalToBeSent;
  int *elementsToSend;
  int *neighbors;
  int *receiveLength;
  int *sendLength;
  double *sendBuffer;
#endif
};
typedef struct SparseMatrix_STRUCT SparseMatrix;

inline void initializeMatrix(SparseMatrix & A) {
	A.title = 0;
	A.totalNumberOfRows = 0;
	A.totalNumberOfNonzeros = 0;
	A.localNumberOfRows = 0;
	A.localNumberOfColumns = 0;
	A.localNumberOfNonzeros = 0;
	A.nonzerosInRow = 0;
	A.matrixIndices = 0;
	A.matrixValues = 0;
	A.matrixDiagonal = 0;

	#ifdef USING_MPI
	A.numberOfExternalValues = 0;
	A.numberOfSendNeighbors = 0;
	A.totalToBeSent = 0;
	A.elementsToSend = 0;
	A.neighbors = 0;
	A.receiveLength = 0;
	A.sendLength = 0;
	A.sendBuffer = 0;
	#endif
}

inline void destroyMatrix(SparseMatrix & A) {

  for (int i = 0; i< A.localNumberOfRows; ++i) {
     delete [] A.matrixValues[i];
     delete [] A.matrixIndices[i];
  }
  if(A.title)                  delete [] A.title;
  if(A.nonzerosInRow)             delete [] A.nonzerosInRow;
  if(A.matrixIndices) delete [] A.matrixIndices;
  if(A.matrixValues) delete [] A.matrixValues;
  if(A.matrixDiagonal)           delete [] A.matrixDiagonal;

#ifdef USING_MPI
  if(A.elementsToSend)       delete [] A.elementsToSend;
  if(A.neighbors)              delete [] A.neighbors;
  if(A.receiveLength)            delete [] A.receiveLength;
  if(A.sendLength)            delete [] A.sendLength;
  if(A.sendBuffer)            delete [] A.sendBuffer;
#endif
 initializeMatrix(A);
}

inline int getRankOfMatrixRow(const Geometry & geom, const SparseMatrix & A, int index) {
	// For the global row id given in the argument index, return the MPI process rank that is assigned that row
	int gnx = geom.nx*geom.npx;
	int gny = geom.ny*geom.npy;

	int iz = index/(gny*gnx);
	int iy = (index-iz*gny*gnx)/gnx;
	int ix = index%gnx;
	int ipz = iz/geom.nz;
	int ipy = iy/geom.ny;
	int ipx = ix/geom.nx;
	int rank = ipx+ipy*geom.npx+ipz*geom.npy*geom.npx;
	//std::cout << "In getRank, rank = " << geom.rank << " index, gnx, gny, ipz, ipy, ipx, rank = " << index << " " << gnx << " "  << " " << gny << " " << ipz << " " << ipy << " " << ipx << " " << rank << std::endl;
	return(rank);

}

#endif // SPARSEMATRIX_HPP

