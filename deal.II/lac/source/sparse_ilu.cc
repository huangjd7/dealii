//----------------------------  sparse_ilu.cc  ---------------------------
//    Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003 by the deal.II authors
//
//    This file is subject to QPL and may not be  distributed
//    without copyright and license information. Please refer
//    to the file deal.II/doc/license.html for the  text  and
//    further information on this license.
//
//----------------------------  sparse_ilu.cc  ---------------------------


#include <lac/sparse_ilu.h>
#include <lac/sparse_ilu.templates.h>


// explicit instantiations
template class SparseILU<double>;
template void SparseILU<double>::decompose<double> (const SparseMatrix<double> &,
						    const double);
template void SparseILU<double>::vmult <double> (Vector<double> &,
                                                 const Vector<double> &) const;
template void SparseILU<double>::decompose<float> (const SparseMatrix<float> &,
						   const double);
template void SparseILU<double>::vmult<float> (Vector<float> &,
                                               const Vector<float> &) const;


template class SparseILU<float>;
template void SparseILU<float>::decompose<double> (const SparseMatrix<double> &,
						   const double);
template void SparseILU<float>::vmult<double> (Vector<double> &,
                                               const Vector<double> &) const;
template void SparseILU<float>::decompose<float> (const SparseMatrix<float> &,
						  const double);
template void SparseILU<float>::vmult<float> (Vector<float> &,
                                              const Vector<float> &) const;
