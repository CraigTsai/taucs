/*********************************************************/
/* TAUCS                                                 */
/* Author: Sivan Toledo                                  */
/*********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "taucs.h"

#ifdef TAUCS_CONFIG_PFUNC
#include "pfunc.h"
#endif

#ifndef TAUCS_CORE
#error "You must define TAUCS_CORE to compile this file"
#endif

/*********************************************************/
/* split into left, right columns                        */
/*********************************************************/

#ifdef TAUCS_CORE_GENERAL
void 
taucs_ccs_split(taucs_ccs_matrix*  A, 
		taucs_ccs_matrix** L, 
		taucs_ccs_matrix** R, 
		int p)
{

#ifdef TAUCS_DOUBLE_IN_BUILD
  if (A->flags & TAUCS_DOUBLE)
    taucs_dccs_split(A,L,R,p);
#endif

#ifdef TAUCS_SINGLE_IN_BUILD
  if (A->flags & TAUCS_SINGLE)
    taucs_sccs_split(A,L,R,p);
#endif

#ifdef TAUCS_DCOMPLEX_IN_BUILD
  if (A->flags & TAUCS_DCOMPLEX)
    taucs_zccs_split(A,L,R,p);
#endif

#ifdef TAUCS_SCOMPLEX_IN_BUILD
  if (A->flags & TAUCS_SCOMPLEX)
    taucs_cccs_split(A,L,R,p);
#endif
  
  
}
#endif /*TAUCS_CORE_GENERAL*/

#ifndef TAUCS_CORE_GENERAL

/* split into left p columns, right p columns */
void 
taucs_dtl(ccs_split)(taucs_ccs_matrix*  A, 
		     taucs_ccs_matrix** L, 
		     taucs_ccs_matrix** R, 
		     int p)
{
  int i,n;
  int Lnnz, Rnnz;

  assert((A->flags & TAUCS_SYMMETRIC) || (A->flags & TAUCS_TRIANGULAR));
  assert(A->flags & TAUCS_LOWER);

  n = A->n;

  *L = (taucs_ccs_matrix*) taucs_malloc(sizeof(taucs_ccs_matrix));
  *R = (taucs_ccs_matrix*) taucs_malloc(sizeof(taucs_ccs_matrix));
  if (!(*L) || !(*R)) { 
    taucs_printf("taucs_ccs_split: out of memory\n");
    taucs_free(*L);
    taucs_free(*R);
    *L = *R = NULL;
    return; 
  }

  Lnnz = 0;
  for (i=0; i<p; i++)
    Lnnz += ( (A->colptr)[i+1] - (A->colptr)[i] );
    
  (*L)->flags |= TAUCS_SYMMETRIC | TAUCS_LOWER;
  (*L)->n = n;
  (*L)->m = n;
  (*L)->colptr = (int*)    taucs_malloc((n+1) * sizeof(int));
  (*L)->rowind = (int*)    taucs_malloc(Lnnz   * sizeof(int));
  (*L)->taucs_values = (void*)   taucs_malloc(Lnnz   * sizeof(taucs_datatype));
  if (!((*L)->colptr) || !((*L)->rowind) || !((*L)->rowind)) {
    	taucs_printf("taucs_ccs_split: out of memory: n=%d nnz=%d\n",n,Lnnz);
	taucs_free((*L)->colptr); taucs_free((*L)->rowind); taucs_free((*L)->taucs_values);
	taucs_free ((*L));
	return; 
  }

  for (i=0; i<=p; i++)
    ((*L)->colptr)[i] = (A->colptr)[i];   
  for (i=p+1; i<n+1; i++)
    ((*L)->colptr)[i] = ((*L)->colptr)[p]; /* other columns are empty */

  for (i=0; i<Lnnz; i++) {
    ((*L)->rowind)[i] = (A->rowind)[i];
    ((*L)->taucs_values)[i] = (A->taucs_values)[i];
  }

  /* now copy right part of matrix into a p-by-p matrix */

  Rnnz = 0;
  for (i=p; i<n; i++)
    Rnnz += ( (A->colptr)[i+1] - (A->colptr)[i] );
    
  (*R)->flags = TAUCS_SYMMETRIC | TAUCS_LOWER;
  (*R)->n = n-p;
  (*R)->m = n-p;
  (*R)->colptr = (int*)    taucs_malloc((n-p+1) * sizeof(int));
  (*R)->rowind = (int*)    taucs_malloc(Rnnz   * sizeof(int));
  (*R)->taucs_values = (void*)   taucs_malloc(Rnnz   * sizeof(taucs_datatype));
  if (!((*R)->colptr) || !((*R)->rowind) || !((*R)->rowind)) {
    	taucs_printf("taucs_ccs_split: out of memory (3): p=%d nnz=%d\n",p,Rnnz);
	taucs_free((*R)->colptr); taucs_free((*R)->rowind); taucs_free((*R)->taucs_values);
	taucs_free((*L)->colptr); taucs_free((*L)->rowind); taucs_free((*L)->taucs_values);
	taucs_free ((*R));
	taucs_free ((*L));
	return; 
  }
    
  for (i=0; i<=(n-p); i++)
    ((*R)->colptr)[i] = (A->colptr)[i+p] - Lnnz;   

  for (i=0; i<Rnnz; i++) {
    ((*R)->rowind)[i] = (A->rowind)[i + Lnnz] - p;
    ((*R)->taucs_values)[i] = (A->taucs_values)[i + Lnnz];
  }
} 

#endif /*#ifndef TAUCS_CORE_GENERAL*/

/*********************************************************/
/* permute symmetrically                                 */
/*********************************************************/

#ifdef TAUCS_CORE_GENERAL
taucs_ccs_matrix* 
taucs_ccs_permute_symmetrically(taucs_ccs_matrix* A, int* perm, int* invperm)
{

#ifdef TAUCS_DOUBLE_IN_BUILD
  if (A->flags & TAUCS_DOUBLE)
    return taucs_dccs_permute_symmetrically(A,perm,invperm);
#endif

#ifdef TAUCS_SINGLE_IN_BUILD
  if (A->flags & TAUCS_SINGLE)
    return taucs_sccs_permute_symmetrically(A,perm,invperm);
#endif

#ifdef TAUCS_DCOMPLEX_IN_BUILD
  if (A->flags & TAUCS_DCOMPLEX)
    return taucs_zccs_permute_symmetrically(A,perm,invperm);
#endif

#ifdef TAUCS_SCOMPLEX_IN_BUILD
  if (A->flags & TAUCS_SCOMPLEX)
    return taucs_cccs_permute_symmetrically(A,perm,invperm);
#endif
  
  assert(0);
  return NULL;
}
#endif /*TAUCS_CORE_GENERAL*/

#ifndef TAUCS_CORE_GENERAL

taucs_ccs_matrix* 
taucs_dtl(ccs_permute_symmetrically)(taucs_ccs_matrix* A, int* perm, int* invperm)
{
  taucs_ccs_matrix* PAPT;
  int n;
  int nnz;
  /*int* colptr;*/
  int* len;
  int i,j,ip,I,J;
  taucs_datatype AIJ;

  assert(A->flags & TAUCS_SYMMETRIC || A->flags & TAUCS_HERMITIAN);
  assert(A->flags & TAUCS_LOWER);

  n   = A->n;
  nnz = (A->colptr)[n];

  PAPT = taucs_dtl(ccs_create)(n,n,nnz);
  if (!PAPT) return NULL;

  /*PAPT->flags = TAUCS_SYMMETRIC | TAUCS_LOWER;*/
  PAPT->flags = A->flags;

  len    = (int*) taucs_malloc(n * sizeof(int));
  /*colptr = (int*) taucs_malloc(n * sizeof(int));*/
  if (!len) {
    taucs_printf("taucs_ccs_permute_symmetrically: out of memory\n");
    taucs_ccs_free(PAPT);
    return NULL;
  }

  for (j=0; j<n; j++) len[j] = 0;

  for (j=0; j<n; j++) {
    for (ip = (A->colptr)[j]; ip < (A->colptr)[j+1]; ip++) {
      /*i = (A->rowind)[ip] - (A->indshift);*/
      i = (A->rowind)[ip];

      I = invperm[i];
      J = invperm[j];

      if (I < J) {
	int T = I; 
	I = J;
	J = T;
      }

      len[J] ++;
      
    }
  }

  (PAPT->colptr)[0] = 0;
  for (j=1; j<=n; j++) (PAPT->colptr)[j] = (PAPT->colptr)[j-1] + len[j-1];

  for (j=0; j<n; j++) len[j] = (PAPT->colptr)[j];
  
  for (j=0; j<n; j++) {
    for (ip = (A->colptr)[j]; ip < (A->colptr)[j+1]; ip++) {
      /*i   = (A->rowind)[ip] - (A->indshift);*/
      i   = (A->rowind)[ip];
      AIJ = (A->taucs_values)[ip];

      I = invperm[i];
      J = invperm[j];

      if (I < J) {
	int T = I; 
	I = J;
	J = T;
	if (A->flags & TAUCS_HERMITIAN) AIJ = taucs_conj(AIJ);
      }

      /*(PAPT->rowind)[ len[J] ] = I + (PAPT->indshift);*/
      (PAPT->rowind)[ len[J] ] = I;
      (PAPT->taucs_values)[ len[J] ] = AIJ;

      len[J] ++;
    }
  }

  taucs_free(len);
  return PAPT;
}

#endif /*#ifndef TAUCS_CORE_GENERAL*/

/*********************************************************/
/* compute B = A*X                                       */
/* current restrictions: A must be square, real          */
/*********************************************************/

#ifdef TAUCS_CORE_GENERAL
void 
taucs_ccs_times_vec(taucs_ccs_matrix* m, 
		    void* X,
		    void* B)
{

#ifdef TAUCS_DOUBLE_IN_BUILD
  if (m->flags & TAUCS_DOUBLE)
    taucs_dccs_times_vec(m, (taucs_double*) X, (taucs_double*) B);
#endif

#ifdef TAUCS_SINGLE_IN_BUILD
  if (m->flags & TAUCS_SINGLE)
    taucs_sccs_times_vec(m, (taucs_single*) X, (taucs_single*) B);
#endif

#ifdef TAUCS_DCOMPLEX_IN_BUILD
  if (m->flags & TAUCS_DCOMPLEX)
    taucs_zccs_times_vec(m, (taucs_dcomplex*) X, (taucs_dcomplex*) B);
#endif

#ifdef TAUCS_SCOMPLEX_IN_BUILD
  if (m->flags & TAUCS_SCOMPLEX)
    taucs_cccs_times_vec(m, (taucs_scomplex*) X, (taucs_scomplex*) B);
#endif
  
  
}

void 
taucs_ccs_times_vec_many(taucs_ccs_matrix* m, 
		    void* X,
		    void* B,
				int nrhs)
{

#ifdef TAUCS_DOUBLE_IN_BUILD
  if (m->flags & TAUCS_DOUBLE)
    taucs_dccs_times_vec_many(m, (taucs_double*) X, (taucs_double*) B,nrhs);
#endif

#ifdef TAUCS_SINGLE_IN_BUILD
  if (m->flags & TAUCS_SINGLE)
    taucs_sccs_times_vec_many(m, (taucs_single*) X, (taucs_single*) B,nrhs);
#endif

#ifdef TAUCS_DCOMPLEX_IN_BUILD
  if (m->flags & TAUCS_DCOMPLEX)
    taucs_zccs_times_vec_many(m, (taucs_dcomplex*) X, (taucs_dcomplex*) B,nrhs);
#endif

#ifdef TAUCS_SCOMPLEX_IN_BUILD
  if (m->flags & TAUCS_SCOMPLEX)
    taucs_cccs_times_vec_many(m, (taucs_scomplex*) X, (taucs_scomplex*) B,nrhs);
#endif
  
  
}
#endif /*TAUCS_CORE_GENERAL*/

#ifndef TAUCS_CORE_GENERAL

void 
taucs_dtl(ccs_times_vec)(taucs_ccs_matrix* m, 
			 taucs_datatype* X,
			 taucs_datatype* B)
{
  int i,ip,j,n,m_;
  taucs_datatype Aij;

  n = m->n;
  m_ = m->m;

  for (i=0; i < m_; i++) B[i] = taucs_zero;

  if (m->flags & TAUCS_SYMMETRIC) {
    for (j=0; j<n; j++) {
      for (ip = (m->colptr)[j]; ip < (m->colptr[j+1]); ip++) {
	i   = (m->rowind)[ip];
	Aij = (m->taucs_values)[ip];
	
	B[i] = taucs_add(B[i],taucs_mul(X[j],Aij));
	if (i != j) 
	  B[j] = taucs_add(B[j],taucs_mul(X[i],Aij));
      }
    }
  } else if (m->flags & TAUCS_HERMITIAN) {
    for (j=0; j<n; j++) {
      for (ip = (m->colptr)[j]; ip < (m->colptr[j+1]); ip++) {
	i   = (m->rowind)[ip];
	Aij = (m->taucs_values)[ip];
	
	B[i] = taucs_add(B[i],taucs_mul(X[j],Aij));
	if (i != j) 
	  B[j] = taucs_add(B[j],taucs_mul(X[i],
					  taucs_conj(Aij)));
      }
    }
  } else {
    for (j=0; j<n; j++) {
      for (ip = (m->colptr)[j]; ip < (m->colptr[j+1]); ip++) {
	i   = (m->rowind)[ip];
	Aij = (m->taucs_values)[ip];
	
	B[i] = taucs_add(B[i],taucs_mul(X[j],Aij));
      }
    }
  }
} 

void 
taucs_dtl(ccs_times_vec_many)(taucs_ccs_matrix* m, 
			 taucs_datatype* X,
			 taucs_datatype* B,
			 int nrhs)
{
  int i,ip,j,n,k,m_;
  taucs_datatype Aij;

  n = m->n;
  m_ = m->m;
	for (k=0; k<nrhs; k++) 
		for (i=0; i < m_; i++) B[i+k*m_] = taucs_zero;

	if (m->flags & TAUCS_SYMMETRIC) {
		for (j=0; j<n; j++) {
			for (ip = (m->colptr)[j]; ip < (m->colptr[j+1]); ip++) {
				i   = (m->rowind)[ip];
				Aij = (m->taucs_values)[ip];
	
				for (k=0; k<nrhs; k++) {
					B[i+k*m_] = taucs_add(B[i+k*m_],taucs_mul(X[j+k*m_],Aij));
					if (i != j) 
						B[j+k*m_] = taucs_add(B[j+k*m_],taucs_mul(X[i+k*m_],Aij));
				}
			}
		}
	} else if (m->flags & TAUCS_HERMITIAN) {
		for (j=0; j<n; j++) {
			for (ip = (m->colptr)[j]; ip < (m->colptr[j+1]); ip++) {
				i   = (m->rowind)[ip];
				Aij = (m->taucs_values)[ip];
		
				for (k=0; k<nrhs; k++) {
					B[i+k*m_] = taucs_add(B[i+k*m_],taucs_mul(X[j+k*m_],Aij));
					if (i != j) 
						B[j+k*m_] = taucs_add(B[j+k*m_],taucs_mul(X[i+k*m_],
							taucs_conj(Aij)));
				}
			}
		}
	} else {
			for (j=0; j<n; j++) {
				for (ip = (m->colptr)[j]; ip < (m->colptr[j+1]); ip++) {
			
				i   = (m->rowind)[ip];
				Aij = (m->taucs_values)[ip];
		
				for (k=0; k<nrhs; k++) {
					B[i+k*m_] = taucs_add(B[i+k*m_],taucs_mul(X[j+k*m_],Aij));
				}
			}
		}
	}
} 

#endif /*#ifndef TAUCS_CORE_GENERAL*/

/*********************************************************/
/* parallel compute B = A*X                              */
/* current restrictions: A must be square, real          */
/*********************************************************/

#ifdef TAUCS_CONFIG_PFUNC

#ifdef TAUCS_CORE_GENERAL
void 
taucs_ccs_parallel_times_vec(taucs_ccs_matrix* m, void* X, void* B, 
			     int sv, int mv, int ev, int tid, void *S, int nproc, pfunc_handle_t handle)
{

#ifdef TAUCS_DOUBLE_IN_BUILD
  if (m->flags & TAUCS_DOUBLE)
    taucs_dccs_parallel_times_vec(m, (taucs_double*) X, (taucs_double*) B, sv, mv, ev, tid, S, nproc, handle);
#endif

#ifdef TAUCS_SINGLE_IN_BUILD
  if (m->flags & TAUCS_SINGLE)
    taucs_sccs_parallel_times_vec(m, (taucs_single*) X, (taucs_single*) B, sv, mv, ev, tid, S, nproc, handle);
#endif

#ifdef TAUCS_DCOMPLEX_IN_BUILD
  if (m->flags & TAUCS_DCOMPLEX)
    taucs_zccs_parallel_times_vec(m, (taucs_dcomplex*) X, (taucs_dcomplex*) B, sv, mv, ev, tid, S, nproc, handle);
#endif

#ifdef TAUCS_SCOMPLEX_IN_BUILD
  if (m->flags & TAUCS_SCOMPLEX)
    taucs_cccs_parallel_times_vec(m, (taucs_scomplex*) X, (taucs_scomplex*) B, sv, mv, ev, tid, S, nproc, handle);
#endif
  
  
}

#endif /*TAUCS_CORE_GENERAL*/

#ifndef TAUCS_CORE_GENERAL

void 
taucs_dtl(ccs_parallel_times_vec)(taucs_ccs_matrix* m, taucs_datatype* X, taucs_datatype* B, 
				  int sv, int mv, int ev, int tid, taucs_datatype *S, int nproc, pfunc_handle_t handle)
{
  int i,ip,j,n,m_,k;
  taucs_datatype Aij;

  n = m->n;
  m_ = m->m;

  for (i = sv; i < ev; i++) 
    B[i] = taucs_zero;

  if (m->flags & TAUCS_SYMMETRIC) {
    
    //double wt = -taucs_wtime();
    //double wt1 = 0.0;

    taucs_datatype *o = B;


    for (j = sv; j < ev; j++) {
      if (j == mv) {
	o = S + tid * n;
	//wt1 -= taucs_wtime();
	pfunc_barrier();
	//wt1 += taucs_wtime();
      }

      for (ip = (m->colptr)[j]; ip < (m->colptr[j+1]); ip++) {
	i   = (m->rowind)[ip];
	Aij = (m->taucs_values)[ip];

	o[i] = taucs_add(o[i],taucs_mul(X[j],Aij));
	if (i != j) 
	  o[j] = taucs_add(o[j],taucs_mul(X[i],Aij));
      }
    }

    if (mv < ev) {
      //wt1 -= taucs_wtime();
      pfunc_barrier();
      //wt1 += taucs_wtime();
      for(ip = 0; ip < nproc; ip++)
	for (j = sv; j < ev; j++) {
	  B[j] = taucs_add(B[j], S[ip * n + j]);
	  S[ip * n + j] = taucs_zero;
	}
    } 

    //    wt += taucs_wtime();
    //    taucs_printf("(tid: %d) Time at barrier %.1f\n", tid, (wt1/wt) * 100);

    /*  
      // Before we can continue to boundary nodes we need to sync 
      // At this stage each thread must continue on its turn.
      // This is done using a sequence of barriers
      for (j=sv; j<ev; j++) {
      if (j == mv) {
	pfunc_barrier();
	for(k = 0; k < tid; k++)
	  pfunc_barrier();
      }
      
      for (ip = (m->colptr)[j]; ip < (m->colptr[j+1]); ip++) {
	i   = (m->rowind)[ip];
	Aij = (m->taucs_values)[ip];
	

	B[i] = taucs_add(B[i],taucs_mul(X[j],Aij));
	if (i != j) 
	  B[j] = taucs_add(B[j],taucs_mul(X[i], Aij));
      }
    }
    
    // Complete barriers
    if (mv < ev)
      for(k = tid; k < nproc; k++)
	pfunc_barrier();
    */
  } else if (m->flags & TAUCS_HERMITIAN) {
    for (j=sv; j<ev; j++) {
      // Before we can continue to boundary nodes we need to sync 
      // At this stage each thread must continue on its turn.
      // This is done using a sequence of barriers
      if (j == mv) {
	pfunc_barrier();
	for(k = 0; k < tid; k++)
	  pfunc_barrier();
      }

      for (ip = (m->colptr)[j]; ip < (m->colptr[j+1]); ip++) {
	i   = (m->rowind)[ip];
	Aij = (m->taucs_values)[ip];


	B[i] = taucs_add(B[i],taucs_mul(X[j],Aij));
	if (i != j) 
	  B[j] = taucs_add(B[j],taucs_mul(X[i],
					  taucs_conj(Aij)));
      }
      
      // Complete barriers
      if (mv < ev)
	for(k = tid; k < nproc; k++)
	  pfunc_barrier();
    }
  } else {
    /* NOT SUPPORTED */
  }
} 

#endif /*#ifndef TAUCS_CORE_GENERAL*/

#endif 

#ifdef TAUCS_CORE_SINGLE
void 
taucs_sccs_times_vec_dacc(taucs_ccs_matrix* m, 
			 taucs_single* X,
			 taucs_single* B)
{
  int i,ip,j,n;
  taucs_single Aij;
  taucs_double* Bd;

  assert(m->flags & TAUCS_SYMMETRIC);
  assert(m->flags & TAUCS_LOWER);
  assert(m->flags & TAUCS_SINGLE);

  n = m->n;

  Bd = (taucs_double*) taucs_malloc(n * sizeof(taucs_double));
  if (Bd == NULL) {
    taucs_sccs_times_vec(m,X,B);
    return;
  }
  
  for (i=0; i < n; i++) Bd[i] = 0.0;

  for (j=0; j<n; j++) {
    for (ip = (m->colptr)[j]; ip < (m->colptr[j+1]); ip++) {
      i   = (m->rowind)[ip];
      Aij = (m->taucs_values)[ip];

      Bd[i] += X[j] * Aij;
      if (i != j) 
	Bd[j] += X[i] *Aij;
    }
  }

  for (i=0; i < n; i++) B[i] = (taucs_single) Bd[i];
  taucs_free(Bd);
} 
#endif

/*********************************************************/
/* augment diagonals to diagonal dominance               */
/* current restrictions: A must be square, real          */
/*********************************************************/

#ifdef TAUCS_CORE_GENERAL
taucs_ccs_matrix* 
taucs_ccs_augment_nonpositive_offdiagonals(taucs_ccs_matrix* A)
{

#ifdef TAUCS_DOUBLE_IN_BUILD
  if (A->flags & TAUCS_DOUBLE)
    taucs_dccs_augment_nonpositive_offdiagonals(A);
#endif

#ifdef TAUCS_SINGLE_IN_BUILD
  if (A->flags & TAUCS_SINGLE)
    taucs_sccs_augment_nonpositive_offdiagonals(A);
#endif

#ifdef TAUCS_DCOMPLEX_IN_BUILD
  if (A->flags & TAUCS_DCOMPLEX)
    taucs_zccs_augment_nonpositive_offdiagonals(A);
#endif

#ifdef TAUCS_SCOMPLEX_IN_BUILD
  if (A->flags & TAUCS_SCOMPLEX)
    taucs_cccs_augment_nonpositive_offdiagonals(A);
#endif
  
      
  assert(0);
  return NULL;
}
#endif /*TAUCS_CORE_GENERAL*/

#ifndef TAUCS_CORE_GENERAL

taucs_ccs_matrix* 
taucs_dtl(ccs_augment_nonpositive_offdiagonals)(taucs_ccs_matrix* A)
{
#ifdef TAUCS_CORE_COMPLEX
  assert(0);
#else
  int n,i,j;
  int *tmp;
  taucs_ccs_matrix* A_tmp;
  
  if (!(A->flags & TAUCS_SYMMETRIC) || !(A->flags & TAUCS_LOWER)) {
    taucs_printf("taucs_ccs_augment_nonpositive_offdiagonal: matrix not symmetric or not lower\n");
    return NULL;
  }

  n=A->n;

  tmp = (int *)taucs_calloc((2*n+1),sizeof(int));
  if (!tmp) {
    taucs_printf("taucs_ccs_augment_nonpositive_offdiagonal: out of memory\n");
    return NULL;
  }

  A_tmp = taucs_dtl(ccs_create)(2*n,2*n,2*(A->colptr[n]));
  if (A_tmp == NULL) {
    taucs_free(tmp);
    return NULL;
  }
  A_tmp->flags |= TAUCS_SYMMETRIC | TAUCS_LOWER;
  
  
  for(i=0;i<n;i++) {
    for(j=A->colptr[i];j<A->colptr[i+1];j++) {
      if ((i == A->rowind[j])||(A->taucs_values[j] < 0)) {
	tmp[i]++;
	tmp[i+n]++;
      } else {
	tmp[i]++;
	tmp[A->rowind[j]]++;
      }
    }
  }

  A_tmp->colptr[0]=0;
  for(i=0;i<2*n;i++) A_tmp->colptr[i+1] = A_tmp->colptr[i] + tmp[i];
  for(i=0;i<2*n;i++) tmp[i] = A_tmp->colptr[i];
  
  for(i=0;i<n;i++) {
    for(j=A->colptr[i];j<A->colptr[i+1];j++) {
      if ((i == A->rowind[j])||(A->taucs_values[j] < 0)) {
	A_tmp->rowind[tmp[i]]=A->rowind[j];
	A_tmp->taucs_values[tmp[i]++]=A->taucs_values[j];
	A_tmp->rowind[tmp[i+n]]=A->rowind[j]+n;
	A_tmp->taucs_values[tmp[i+n]++]=A->taucs_values[j];
      } else {
	A_tmp->rowind[tmp[i]]=A->rowind[j]+n;
	A_tmp->taucs_values[tmp[i]++]=-A->taucs_values[j];
	A_tmp->rowind[tmp[A->rowind[j]]]=i+n;
	A_tmp->taucs_values[tmp[A->rowind[j]]++]=-A->taucs_values[j];
      }
    }
  }
  taucs_free(tmp);

  return A_tmp;
#endif
	/* added omer*/
	return NULL;
}

#endif /*#ifndef TAUCS_CORE_GENERAL*/

/*************************************************************************************
 * Function: multilu_transpose_sparse
 *
 * Description: Transpose a given sparse matrix A. If A->values is NULL or pattern_only is true
 *              the matrix is treated as "structure-only" matrix and the tranpose is too.
 *
 *************************************************************************************/

#ifdef TAUCS_CORE_GENERAL
taucs_ccs_matrix *taucs_ccs_transpose(taucs_ccs_matrix *A, int pattern_only)
{

#ifdef TAUCS_DOUBLE_IN_BUILD
  if (A->flags & TAUCS_DOUBLE)
    return taucs_dccs_transpose(A, pattern_only);
#endif

#ifdef TAUCS_SINGLE_IN_BUILD
  if (A->flags & TAUCS_SINGLE)
    return taucs_sccs_transpose(A, pattern_only);
#endif

#ifdef TAUCS_DCOMPLEX_IN_BUILD
  if (A->flags & TAUCS_DCOMPLEX)
    return taucs_zccs_transpose(A, pattern_only);
#endif

#ifdef TAUCS_SCOMPLEX_IN_BUILD
  if (A->flags & TAUCS_SCOMPLEX)
    return taucs_cccs_transpose(A, pattern_only);
#endif
      
  assert(0);
  return NULL;
}
#endif /*TAUCS_CORE_GENERAL*/

#ifndef TAUCS_CORE_GENERAL

taucs_ccs_matrix *taucs_dtl(ccs_transpose)(taucs_ccs_matrix *A, int pattern_only)
{
  taucs_ccs_matrix *At;
  int *row_size, *row_index;
  int i, j;
  int nnz;
  
  int do_values;

  if ( A->flags & TAUCS_SYMMETRIC ) return NULL;
  if ( A->flags & TAUCS_HERMITIAN ) return NULL;

  do_values = ( (A->taucs_values) != NULL && !pattern_only);
  
  /* Allocate space for At */
  At = taucs_malloc(sizeof(taucs_ccs_matrix));

  At->flags = A->flags;
  if ( A->flags & TAUCS_TRIANGULAR ) {
    if ( A->flags & TAUCS_LOWER ) {
      At->flags |=  TAUCS_UPPER;
      At->flags &= ~TAUCS_LOWER;
    } else {
      At->flags |=  TAUCS_LOWER;
      At->flags &= ~TAUCS_UPPER;
    }
  }

  At->m = A->n;
  At->n = A->m;
  nnz = A->colptr[ A->n ];
  At->colptr = taucs_calloc(At->n + 1, sizeof(int));
  At->rowind = taucs_calloc( nnz /*At->nnz*/, sizeof(int));
  if (do_values)
    (At->taucs_values) = taucs_calloc(nnz, sizeof(taucs_datatype));
  else
    (At->taucs_values) = NULL;
  
  /* Find row sizes */
  row_size = taucs_calloc(A->m, sizeof(int));
  for(i = 0; i < nnz; i++)
    row_size[A->rowind[i]]++;
  
  /* Initiate the colptr of At based on rowsizes */
  At->colptr[0] = 0;
  for(i = 1; i <= At->n; i++)
    At->colptr[i] = At->colptr[i - 1] + row_size[i - 1];
  
  /* Now put values and indexs into the new matrix. For growing row_index use the */
  /* same array as row_size (trick to make the names look good) */
  row_index = row_size;
  memset(row_index, 0, A->m * sizeof(int));
  for(i = 0; i < A->n; i++)
    for(j = A->colptr[i]; j < A->colptr[i + 1]; j++)
      {
	int row = A->rowind[j];
	int index = At->colptr[row] + row_index[row];
	
	At->rowind[index] = i;
	if (do_values)
	  (At->taucs_values)[index] = (A->taucs_values)[j];
	
	row_index[row]++;
      }
  
  
  taucs_free(row_size);
  
  return At;
}

#endif /* not TAUCS_CORE_GENERAL*/

/*************************************************************************************
 * Function: taucs_ccs_permute_rows_inplace
 *
 * Description: Reorders the rows of the given matrix, in place
 *
 *************************************************************************************/
#ifdef TAUCS_CORE_GENERAL
void taucs_ccs_permute_rows_inplace(taucs_ccs_matrix *A, int *row_order)
{
  int *inverse_r = taucs_malloc(A->m * sizeof(int));
  int i;
  
  /* Find inverse of row order */
  for (i = 0; i < A->m; i++)
    inverse_r[row_order[i]] = i;
  
  /* Change row numbers */
  for (i = 0; i < A->colptr[ A->n ]/*A->nnz*/; i++)
    A->rowind[i] = inverse_r[A->rowind[i]];
  
  taucs_free(inverse_r);
}
#endif

/*************************************************************************************
 * Function: taucs_ccs_find_ata_pattern
 *
 * Description: Finds the pattern of A'A (LOWER matrix returned).
 *
 *************************************************************************************/
#ifdef TAUCS_CORE_GENERAL
taucs_ccs_matrix *taucs_ccs_find_ata_pattern(taucs_ccs_matrix *A)
{
  int ata_nnz, mark, i, j, k, row, row1;
  taucs_ccs_matrix *ATA;
  taucs_ccs_matrix *AT = taucs_ccs_transpose(A, 1);

  int *flag = taucs_malloc(A->m * sizeof(int));
  memset(flag, 0, A->m * sizeof(int));

  /* Find size of A'A */
  ata_nnz = 0;
  mark = 0;
  for (i = 0; i < A->n; i++) {
    mark++;

    for(j = A->colptr[i]; j < A->colptr[i + 1]; j++) {
      int row = A->rowind[j];
      for(k = AT->colptr[row]; k < AT->colptr[row + 1]; k++) {
		row1 = AT->rowind[k];
		if (row1 >= i && flag[row1] != mark) {
			ata_nnz++;
			flag[row1] = mark;
		}
      }
    }
  }

  /* Now we can form AA' */
  ATA = taucs_ccs_create(A->n, A->n, ata_nnz, TAUCS_SYMMETRIC | TAUCS_LOWER | TAUCS_PATTERN);
  ata_nnz = 0;
  for (i = 0; i < A->n; i++) {
    ATA->colptr[i] = ata_nnz;
    mark++;

    for(j = A->colptr[i]; j < A->colptr[i + 1]; j++) {
      row = A->rowind[j];
      for(k = AT->colptr[row]; k < AT->colptr[row + 1]; k++) {
		row1 = AT->rowind[k];
		if (row1 >= i && flag[row1] != mark) {
			ATA->rowind[ata_nnz] = row1;
			ata_nnz++;
			flag[row1] = mark;
		}
      }
    }
  }
  ATA->colptr[A->n] = ata_nnz;

  taucs_ccs_free(AT);
  taucs_free(flag);

  return ATA;
}

#endif

/*********************************************************/
/*                                                       */
/*********************************************************/

