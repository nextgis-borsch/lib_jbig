/*
 *  Header file for the arithmetic encoder and decoder of
 *  the portable JBIG compression library
 *
 *  Markus Kuhn -- http://www.cl.cam.ac.uk/~mgk25/jbigkit/
 */

#ifndef JBG_AR_H
#define JBG_AR_H

#ifdef JBIG_STATIC
  #define EXTERN extern
#else
#   if defined (_WIN32) || defined (WINDOWS)
#    ifdef JBIG_DLL_EXPORTS
#      ifdef __GNUC__
#        define EXTERN extern __attribute__((dllexport))
#      else        
#        define EXTERN extern __declspec(dllexport)
#      endif 
#    else
#      ifdef __GNUC__
#        define EXTERN extern __attribute__((dllimport))
#      else        
#        define EXTERN extern __declspec(dllimport)
#      endif 
#    endif
#   else
#     if __GNUC__ >= 4
#       define EXTERN __attribute__((visibility("default")))
#     else
#       define EXTERN                extern
#     endif 
#   endif
#endif

/*
 * Status of arithmetic encoder
 */

struct jbg_arenc_state {
  unsigned char st[4096];    /* probability status for contexts, MSB = MPS */
  unsigned long c;                /* register C: base of coding intervall, *
                                   * layout as in Table 23                 */
  unsigned long a;       /* register A: normalized size of coding interval */
  long sc;     /* number of buffered 0xff values that might still overflow */
  int ct;  /* bit shift counter, determines when next byte will be written */
  int buffer;                /* buffer for most recent output byte != 0xff */
  void (*byte_out)(int, void *);  /* function that receives all PSCD bytes */
  void *file;                              /* parameter passed to byte_out */
};

/*
 * Status of arithmetic decoder
 */

struct jbg_ardec_state {
  unsigned char st[4096];    /* probability status for contexts, MSB = MPS */
  unsigned long c;                /* register C: base of coding intervall, *
                                   * layout as in Table 25                 */
  unsigned long a;       /* register A: normalized size of coding interval */
  unsigned char *pscd_ptr;               /* pointer to next PSCD data byte */
  unsigned char *pscd_end;                   /* pointer to byte after PSCD */
  int ct;    /* bit-shift counter, determines when next byte will be read;
              * special value -1 signals that zero-padding has started     */
  int startup;          /* boolean flag that controls initial fill of s->c */
  int nopadding;        /* boolean flag that triggers return -2 between
			 * reaching PSCD end and decoding the first symbol
			 * that might never have been encoded in the first
			 * place */
};

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
    // used by C++ source code
#endif

EXTERN void arith_encode_init(struct jbg_arenc_state *s, int reuse_st);
EXTERN void arith_encode_flush(struct jbg_arenc_state *s);
EXTERN void arith_encode(struct jbg_arenc_state *s, int cx, int pix);
EXTERN void arith_decode_init(struct jbg_ardec_state *s, int reuse_st);
EXTERN int  arith_decode(struct jbg_ardec_state *s, int cx);

#ifdef __cplusplus
}
#endif

#endif /* JBG_AR_H */
