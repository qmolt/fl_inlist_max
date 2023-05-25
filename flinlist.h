#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"  
#include <math.h>

#define DFLT_MODBASE 12
#define DFLT_LIST_LEN 0
#define MAX_LEN_LIST 256

enum FILTER_MODE { FM_NONE, FM_CHROM_DOWN, FM_CHROM_UP, FM_DIAT_DOWN, FM_DIAT_UP, FM_TOTAL };

enum INLETS { I_INPUT, I_LIST, NUM_INLETS };
enum OUTLETS { O_OUTPUT, O_OUTPUT2, NUM_OUTLETS };

typedef struct {

	t_object obj;
	
	short filter_mode;
	long mod_base;
	long map_list[MAX_LEN_LIST];
	long list_len;

	void *m_outlet;
	void *m_outlet2;

} t_fl_inlist;

void *fl_inlist_new(t_symbol *s, short argc, t_atom *argv);
void fl_inlist_assist(t_fl_inlist *x, void *b, long msg, long arg, char *dst);

void fl_inlist_int(t_fl_inlist *x, long n);
void fl_inlist_float(t_fl_inlist *x, double f);
void fl_inlist_list(t_fl_inlist *x, t_symbol *s, long argc, t_atom *argv);
void fl_inlist_mod_val(t_fl_inlist *x, t_symbol *s, long argc, t_atom *argv);
void fl_inlist_filter_mode(t_fl_inlist *x, t_symbol *s, long argc, t_atom *argv);

long z_mod(long x, long base);

static t_class *fl_inlist_class;