// @HEADER
// ***********************************************************************
// 
//                IFPACK
//                 Copyright (2004) Sandia Corporation
// 
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
// 
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the
// License, or (at your option) any later version.
//  
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//  
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
// Questions? Contact Michael A. Heroux (maherou@sandia.gov) 
// 
// ***********************************************************************
// @HEADER

#include "Ifpack_ConfigDefs.h"
#if defined(HAVE_IFPACK_AMESOS) && defined(HAVE_IFPACK_TEUCHOS)
#ifdef HAVE_MPI
#include "Epetra_MpiComm.h"
#endif
#include "Epetra_SerialComm.h"
#include "Epetra_CrsMatrix.h"
#include "Epetra_Vector.h"
#include "Epetra_LinearProblem.h"
#include "Trilinos_Util_CrsMatrixGallery.h"
#include "Teuchos_ParameterList.hpp"
#include "Ifpack_DenseContainer.h"
#include "Ifpack_SparseContainer.h"
#include "Ifpack_Amesos.h"

using namespace Trilinos_Util;
static bool verbose = false;

// ======================================================================
bool TestContainer(string Type, CrsMatrixGallery& Gallery)
{

  Epetra_LinearProblem* Problem = Gallery.GetLinearProblem();

  Epetra_MultiVector& RHS = *(Problem->GetRHS());
  Epetra_MultiVector& LHS = *(Problem->GetLHS());
  Epetra_RowMatrix* A = Problem->GetMatrix();

  int NumVectors = RHS.NumVectors();
  int NumMyRows = A->NumMyRows();

  if (verbose) {
    cout << "Container type = " << Type << endl;
    cout << "NumMyRows = " << NumMyRows << ", NumVectors = " << NumVectors << endl;
  }
  LHS.PutScalar(0.0);
  
  Ifpack_Container* Container;

  if (Type == "dense")
    Container = new Ifpack_DenseContainer(A->NumMyRows(), NumVectors);
  else
    Container = new Ifpack_SparseContainer<Ifpack_Amesos>(A->NumMyRows(), NumVectors);

  assert (Container != 0);

  IFPACK_CHK_ERR(Container->Initialize());
  // set as ID all the local rows of A
  for (int i = 0 ; i < A->NumMyRows() ; ++i)
    Container->ID(i) = i;

  // extract submatrix (in this case, the entire matrix)
  // and complete setup
  IFPACK_CHK_ERR(Container->Compute(*A));

  // set the RHS and LHS
  for (int i = 0 ; i < A->NumMyRows() ; ++i)
    for (int j = 0 ; j < NumVectors ; ++j) {
      Container->RHS(i,j) = RHS[j][i];
      Container->LHS(i,j) = LHS[j][i];
    }
  
  // set parameters (empty for dense containers)
  Teuchos::ParameterList List;
  List.set("amesos: solver type", Type);
  IFPACK_CHK_ERR(Container->SetParameters(List));

  // solve the linear system
  IFPACK_CHK_ERR(Container->ApplyInverse());

  // get the computed solution, store it in LHS
  for (int i = 0 ; i < A->NumMyRows() ; ++i)
    for (int j = 0 ; j < NumVectors ; ++j) {
       LHS[j][i] = Container->LHS(i,j);
    }

  // check residual
  double* residual = new double[NumVectors];
  double *diff = new double[NumVectors];

  Gallery.ComputeResidual(residual);
  Gallery.ComputeDiffBetweenStartingAndExactSolutions(diff);

  if (A->Comm().MyPID() == 0 && verbose) {
    for (int i = 0 ; i < NumVectors ; ++i) {
      cout << "eq " << i << ", ||b-Ax||_2 = " << residual[i] << endl;
      cout << "eq " << i << ", ||x_exact - x||_2 = " << diff[i] << endl;
    }
    cout << *Container;
  }

  bool passed = false;
  if ((residual[0] < 1e-5) && (diff[0] < 1e-5))
    passed = true;

  delete [] residual;
  delete [] diff;
  delete Container;

  return(passed);

}

// ======================================================================
int main(int argc, char *argv[])
{

#ifdef HAVE_MPI
  MPI_Init(&argc,&argv);
  Epetra_MpiComm Comm(MPI_COMM_WORLD);
#else
  Epetra_SerialComm Comm;
#endif
  Epetra_SerialComm SerialComm;

  verbose = (Comm.MyPID() == 0);

  const int NumPoints = 900;

  CrsMatrixGallery Gallery("laplace_2d", SerialComm);
  Gallery.Set("problem_size", NumPoints);
  Gallery.Set("map_type", "linear");
  Gallery.Set("num_vectors", 5);

  int TestPassed = true;

  if (!TestContainer("dense",Gallery))
    TestPassed = false;
  if (!TestContainer("sparse",Gallery))
    TestPassed = false;

  if (TestPassed)
    cout << "Test `TestContainer.exe' passed!" << endl;
  else {
    cout << "Test `TestContainer.exe' FAILED!" << endl;
    exit(EXIT_FAILURE);
  }

#ifdef HAVE_MPI
  MPI_Finalize(); 
#endif

  exit(EXIT_SUCCESS);
}

#else

#ifdef HAVE_MPI
#include "Epetra_MpiComm.h"
#else
#include "Epetra_SerialComm.h"
#endif

int main(int argc, char *argv[])
{

#ifdef HAVE_MPI
  MPI_Init(&argc,&argv);
  Epetra_MpiComm Comm( MPI_COMM_WORLD );
#else
  Epetra_SerialComm Comm;
#endif

  puts("please configure IFPACK with --enable-teuchos");
  puts("--enable-amesos to run this test");

#ifdef HAVE_MPI
  MPI_Finalize() ;
#endif
  return(EXIT_SUCCESS);
}

#endif
