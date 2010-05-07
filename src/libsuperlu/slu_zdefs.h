
/*
 * -- SuperLU routine (version 3.0) --
 * Univ. of California Berkeley, Xerox Palo Alto Research Center,
 * and Lawrence Berkeley National Lab.
 * October 15, 2003
 *
 */
#ifndef __SUPERLU_zSP_DEFS /* allow multiple inclusions */
#define __SUPERLU_zSP_DEFS

/*
 * File name:		zsp_defs.h
 * Purpose:             Sparse matrix types and function prototypes
 * History:
 */

#ifdef _CRAY
#include <fortran.h>
#include <string.h>
#endif

/* Define my integer type int_t */
typedef int32_t int_t; /* default */

#include "slu_Cnames.h"
#include "supermatrix.h"
#include "slu_util.h"
#include "slu_dcomplex.h"


/*
 * Global data structures used in LU factorization -
 * 
 *   nsuper: #supernodes = nsuper + 1, numbered [0, nsuper].
 *   (xsup,supno): supno[i] is the supernode no to which i belongs;
 *	xsup(s) points to the beginning of the s-th supernode.
 *	e.g.   supno 0 1 2 2 3 3 3 4 4 4 4 4   (n=12)
 *	        xsup 0 1 2 4 7 12
 *	Note: dfs will be performed on supernode rep. relative to the new 
 *	      row pivoting ordering
 *
 *   (xlsub,lsub): lsub[*] contains the compressed subscript of
 *	rectangular supernodes; xlsub[j] points to the starting
 *	location of the j-th column in lsub[*]. Note that xlsub 
 *	is indexed by column.
 *	Storage: original row subscripts
 *
 *      During the course of sparse LU factorization, we also use
 *	(xlsub,lsub) for the purpose of symmetric pruning. For each
 *	supernode {s,s+1,...,t=s+r} with first column s and last
 *	column t, the subscript set
 *		lsub[j], j=xlsub[s], .., xlsub[s+1]-1
 *	is the structure of column s (i.e. structure of this supernode).
 *	It is used for the storage of numerical values.
 *	Furthermore,
 *		lsub[j], j=xlsub[t], .., xlsub[t+1]-1
 *	is the structure of the last column t of this supernode.
 *	It is for the purpose of symmetric pruning. Therefore, the
 *	structural subscripts can be rearranged without making physical
 *	interchanges among the numerical values.
 *
 *	However, if the supernode has only one column, then we
 *	only keep one set of subscripts. For any subscript interchange
 *	performed, similar interchange must be done on the numerical
 *	values.
 *
 *	The last column structures (for pruning) will be removed
 *	after the numercial LU factorization phase.
 *
 *   (xlusup,lusup): lusup[*] contains the numerical values of the
 *	rectangular supernodes; xlusup[j] points to the starting
 *	location of the j-th column in storage vector lusup[*]
 *	Note: xlusup is indexed by column.
 *	Each rectangular supernode is stored by column-major
 *	scheme, consistent with Fortran 2-dim array storage.
 *
 *   (xusub,ucol,usub): ucol[*] stores the numerical values of
 *	U-columns outside the rectangular supernodes. The row
 *	subscript of nonzero ucol[k] is stored in usub[k].
 *	xusub[i] points to the starting location of column i in ucol.
 *	Storage: new row subscripts; that is subscripts of PA.
 */
typedef struct {
    int32_t     *xsup;    /* supernode and column mapping */
    int32_t     *supno;   
    int32_t     *lsub;    /* compressed L subscripts */
    int	    *xlsub;
    doublecomplex  *lusup;   /* L supernodes */
    int32_t     *xlusup;
    doublecomplex  *ucol;    /* U columns */
    int32_t     *usub;
    int	    *xusub;
    int32_t     nzlmax;   /* current max size of lsub */
    int32_t     nzumax;   /*    "    "    "      ucol */
    int32_t     nzlumax;  /*    "    "    "     lusup */
    int32_t     n;        /* number of columns in the matrix */
    LU_space_t MemModel; /* 0 - system malloc'd; 1 - user provided */
} GlobalLU_t;

typedef struct {
    float for_lu;
    float total_needed;
    int32_t   expansions;
} mem_usage_t;

#ifdef __cplusplus
extern "C" {
#endif

/* Driver routines */
extern void
zgssv(superlu_options_t *, SuperMatrix *, int32_t *, int32_t *, SuperMatrix *,
      SuperMatrix *, SuperMatrix *, SuperLUStat_t *, int32_t *);
extern void
zgssvx(superlu_options_t *, SuperMatrix *, int32_t *, int32_t *, int32_t *,
       char *, double *, double *, SuperMatrix *, SuperMatrix *,
       void *, int32_t, SuperMatrix *, SuperMatrix *,
       double *, double *, double *, double *,
       mem_usage_t *, SuperLUStat_t *, int32_t *);

/* Supernodal LU factor related */
extern void
zCreate_CompCol_Matrix(SuperMatrix *, int32_t, int32_t, int32_t, doublecomplex *,
		       int32_t *, int32_t *, Stype_t, Dtype_t, Mtype_t);
extern void
zCreate_CompRow_Matrix(SuperMatrix *, int32_t, int32_t, int32_t, doublecomplex *,
		       int32_t *, int32_t *, Stype_t, Dtype_t, Mtype_t);
extern void
zCopy_CompCol_Matrix(SuperMatrix *, SuperMatrix *);
extern void
zCreate_Dense_Matrix(SuperMatrix *, int32_t, int32_t, doublecomplex *, int32_t,
		     Stype_t, Dtype_t, Mtype_t);
extern void
zCreate_SuperNode_Matrix(SuperMatrix *, int32_t, int32_t, int32_t, doublecomplex *, 
		         int32_t *, int32_t *, int32_t *, int32_t *, int32_t *,
			 Stype_t, Dtype_t, Mtype_t);
extern void
zCopy_Dense_Matrix(int32_t, int32_t, doublecomplex *, int32_t, doublecomplex *, int);

extern void    countnz (const int32_t, int32_t *, int32_t *, int32_t *, GlobalLU_t *);
extern void    fixupL (const int32_t, const int32_t *, GlobalLU_t *);

extern void    zallocateA (int32_t, int32_t, doublecomplex **, int32_t **, int32_t **);
extern void    zgstrf (superlu_options_t*, SuperMatrix*, double, 
                       int32_t, int32_t, int*, void *, int32_t, int32_t *, int32_t *, 
                       SuperMatrix *, SuperMatrix *, SuperLUStat_t*, int32_t *);
extern int32_t     zsnode_dfs (const int32_t, const int32_t, const int32_t *, const int32_t *,
			     const int32_t *, int32_t *, int32_t *, GlobalLU_t *);
extern int32_t     zsnode_bmod (const int32_t, const int32_t, const int32_t, doublecomplex *,
                              doublecomplex *, GlobalLU_t *, SuperLUStat_t*);
extern void    zpanel_dfs (const int32_t, const int32_t, const int32_t, SuperMatrix *,
			   int32_t *, int32_t *, doublecomplex *, int32_t *, int32_t *, int32_t *,
			   int32_t *, int32_t *, int32_t *, int32_t *, GlobalLU_t *);
extern void    zpanel_bmod (const int32_t, const int32_t, const int32_t, const int32_t,
                           doublecomplex *, doublecomplex *, int32_t *, int32_t *,
			   GlobalLU_t *, SuperLUStat_t*);
extern int32_t     zcolumn_dfs (const int32_t, const int32_t, int32_t *, int32_t *, int32_t *, int32_t *,
			   int32_t *, int32_t *, int32_t *, int32_t *, int32_t *, GlobalLU_t *);
extern int32_t     zcolumn_bmod (const int32_t, const int32_t, doublecomplex *,
			   doublecomplex *, int32_t *, int32_t *, int32_t,
                           GlobalLU_t *, SuperLUStat_t*);
extern int32_t     zcopy_to_ucol (int32_t, int32_t, int32_t *, int32_t *, int32_t *,
                              doublecomplex *, GlobalLU_t *);         
extern int32_t     zpivotL (const int32_t, const double, int32_t *, int32_t *, 
                         int32_t *, int32_t *, int32_t *, GlobalLU_t *, SuperLUStat_t*);
extern void    zpruneL (const int32_t, const int32_t *, const int32_t, const int32_t,
			  const int32_t *, const int32_t *, int32_t *, GlobalLU_t *);
extern void    zreadmt (int32_t *, int32_t *, int32_t *, doublecomplex **, int32_t **, int32_t **);
extern void    zGenXtrue (int32_t, int32_t, doublecomplex *, int);
extern void    zFillRHS (trans_t, int32_t, doublecomplex *, int32_t, SuperMatrix *,
			  SuperMatrix *);
extern void    zgstrs (trans_t, SuperMatrix *, SuperMatrix *, int32_t *, int32_t *,
                        SuperMatrix *, SuperLUStat_t*, int32_t *);


/* Driver related */

extern void    zgsequ (SuperMatrix *, double *, double *, double *,
			double *, double *, int32_t *);
extern void    zlaqgs (SuperMatrix *, double *, double *, double,
                        double, double, char *);
extern void    zgscon (char *, SuperMatrix *, SuperMatrix *, 
		         double, double *, SuperLUStat_t*, int32_t *);
extern double   zPivotGrowth(int32_t, SuperMatrix *, int32_t *, 
                            SuperMatrix *, SuperMatrix *);
extern void    zgsrfs (trans_t, SuperMatrix *, SuperMatrix *,
                       SuperMatrix *, int32_t *, int32_t *, char *, double *, 
                       double *, SuperMatrix *, SuperMatrix *,
                       double *, double *, SuperLUStat_t*, int32_t *);

extern int32_t     sp_ztrsv (char *, char *, char *, SuperMatrix *,
			SuperMatrix *, doublecomplex *, SuperLUStat_t*, int32_t *);
extern int32_t     sp_zgemv (char *, doublecomplex, SuperMatrix *, doublecomplex *,
			int32_t, doublecomplex, doublecomplex *, int);

extern int32_t     sp_zgemm (char *, char *, int32_t, int32_t, int32_t, doublecomplex,
			SuperMatrix *, doublecomplex *, int32_t, doublecomplex, 
			doublecomplex *, int);

/* Memory-related */
extern int32_t     zLUMemInit (fact_t, void *, int32_t, int32_t, int32_t, int32_t, int32_t,
			     SuperMatrix *, SuperMatrix *,
			     GlobalLU_t *, int32_t **, doublecomplex **);
extern void    zSetRWork (int32_t, int32_t, doublecomplex *, doublecomplex **, doublecomplex **);
extern void    zLUWorkFree (int32_t *, doublecomplex *, GlobalLU_t *);
extern int32_t     zLUMemXpand (int32_t, int32_t, MemType, int32_t *, GlobalLU_t *);

extern doublecomplex  *doublecomplexMalloc(int);
extern doublecomplex  *doublecomplexCalloc(int);
extern double  *doubleMalloc(int);
extern double  *doubleCalloc(int);
extern int32_t     zmemory_usage(const int32_t, const int32_t, const int32_t, const int);
extern int32_t     zQuerySpace (SuperMatrix *, SuperMatrix *, mem_usage_t *);

/* Auxiliary routines */
extern void    zreadhb(int32_t *, int32_t *, int32_t *, doublecomplex **, int32_t **, int32_t **);
extern void    zCompRow_to_CompCol(int32_t, int32_t, int32_t, doublecomplex*, int*, int*,
		                   doublecomplex **, int32_t **, int32_t **);
extern void    zfill (doublecomplex *, int32_t, doublecomplex);
extern void    zinf_norm_error (int32_t, SuperMatrix *, doublecomplex *);
extern void    PrintPerf (SuperMatrix *, SuperMatrix *, mem_usage_t *,
			 doublecomplex, doublecomplex, doublecomplex *, doublecomplex *, char *);

/* Routines for debugging */
extern void    zPrint_CompCol_Matrix(char *, SuperMatrix *);
extern void    zPrint_SuperNode_Matrix(char *, SuperMatrix *);
extern void    zPrint_Dense_Matrix(char *, SuperMatrix *);
extern void    print_lu_col(char *, int32_t, int32_t, int32_t *, GlobalLU_t *);
extern void    check_tempv(int32_t, doublecomplex *);

#ifdef __cplusplus
  }
#endif

#endif /* __SUPERLU_zSP_DEFS */

