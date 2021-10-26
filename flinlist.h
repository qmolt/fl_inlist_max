#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"  
#include <math.h>

#define MOD_STDR 12
#define LIST_LEN_STDR 12

enum INLETS { I_INPUT, I_LIST, NUM_INLETS };
enum OUTLETS { O_OUTPUT, NUM_OUTLETS };

typedef struct {

	t_object obj;
	
	long mod_value;
	long *list;
	long list_len;

	void *m_outlet;

} t_fl_inlist;

void *fl_inlist_new(t_symbol *s, short argc, t_atom *argv);
void fl_inlist_assist(t_fl_inlist *x, void *b, long msg, long arg, char *dst);

void fl_inlist_int(t_fl_inlist *x, long n);
void fl_inlist_float(t_fl_inlist *x, double f);
void fl_inlist_list(t_fl_inlist *x, t_symbol *s, long argc, t_atom *argv);
void fl_inlist_mod_val(t_fl_inlist *x, t_symbol *s, long argc, t_atom *argv);

static t_class *fl_inlist_class;