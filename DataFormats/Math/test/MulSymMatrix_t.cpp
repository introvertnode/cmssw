// #include "DataFormats/Math/interface/MulSymMatrix.h"
#include "Math/SMatrix.h"

typedef unsigned int IndexType;
// typedef unsigned long long IndexType;
template<typename T, IndexType N>
inline void
mult(ROOT::Math::SMatrix<T,N,N,ROOT::Math::MatRepSym<T,N> > & a, 
     ROOT::Math::SMatrix<T,N,N,ROOT::Math::MatRepSym<T,N> > const & rh,
     ROOT::Math::SMatrix<T,N,N,ROOT::Math::MatRepSym<T,N> > const & lh) {
  // a(i,j) = r(i,k)*l(k,j)
    for (IndexType i=0; i!=N; ++i) {
      IndexType off_i = a.fRep.Offsets()(i,0);
  for (IndexType k=0; k!=N; ++k) {
    IndexType off_k = a.fRep.Offsets()(k,0);
      if (k<i) {
	for (IndexType j=0; j!=(k+1); ++j) 
	  a.Array()[off_i+j] += rh(i,k)*lh.Array()[off_k+j];
	for (IndexType j=k+1; j!=(i+1); ++j) 
	  a.Array()[off_i+j] += rh(i,k)*lh(k,j);
      }
      else
	for (IndexType j=0; j!=(i+1); ++j) 
	  a.Array()[off_i+j] += rh(i,k)*lh.Array()[off_k+j];
    }
  }
}

#include<iostream>
#include "FWCore/Utilities/interface/HRRealTime.h"

#include<random>

namespace {
  std::mt19937 eng;
  std::uniform_real_distribution<double> rgen(-5.,5.);
  template<typename T, IndexType N>
  inline void
  fillRandom(ROOT::Math::SMatrix<T,N,N,ROOT::Math::MatRepSym<T,N> > & a) {
    for (IndexType k=0; k!=N; ++k)
      for (IndexType j=0; j!=(k+1); ++j)
	a(k,j) =  rgen(eng);
  }
}

template<typename T, IndexType N>
void go() {
  typedef ROOT::Math::SMatrix<T,N,N,ROOT::Math::MatRepStd<T,N> > Matrix;
  typedef ROOT::Math::SMatrix<T,N,N,ROOT::Math::MatRepSym<T,N> > SymMatrix;


  SymMatrix rh;  fillRandom(rh);
  SymMatrix lh;  fillRandom(lh);

  Matrix mrh = rh;
  Matrix mlh = lh;

  SymMatrix a; 
  edm::HRTimeType s1 = edm::hrRealTime();
  mult(a,rh,lh);
  s1 =  edm::hrRealTime() -s1;

  SymMatrix b;
  edm::HRTimeType s2 = edm::hrRealTime();
  ROOT::Math::AssignSym::Evaluate(b, rh*lh);
  s2 =  edm::hrRealTime() -s2;


  Matrix m;
  edm::HRTimeType s3 = edm::hrRealTime();
  m = mrh*mlh;
  s3 =  edm::hrRealTime() -s3;


  SymMatrix sm; ROOT::Math::AssignSym::Evaluate(sm,m);

  if (sm!=b) std::cout << "problem with SMatrix *" << std::endl;
  if (a!=b) std::cout << "problem with MulSymMatrix" << std::endl;
  if (a!=sm) std::cout << "problem with MulSymMatrix twice" << std::endl;
  
  std::cout << "sym mult " << s1 << std::endl;
  std::cout << "sym    * " << s2 << std::endl;
  std::cout << "mat    * " << s3 << std::endl;


}
 
int main() {

  go<double,15>();
  go<double,15>();

  return 0;
}
