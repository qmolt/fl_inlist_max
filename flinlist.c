#include "flinlist.h"

void C74_EXPORT main() {

	t_class *c;

	c = class_new("flinlist",(method)fl_inlist_new, (method)NULL,sizeof(t_fl_inlist), 0, 0);
	class_addmethod(c, (method)fl_inlist_assist,"assist", A_CANT, 0);
	class_addmethod(c,(method)fl_inlist_int, "int", A_LONG, 0);
	class_addmethod(c,(method)fl_inlist_float, "float", A_LONG, 0);
	class_addmethod(c,(method)fl_inlist_list, "list", A_GIMME, 0);
	class_addmethod(c,(method)fl_inlist_mod_val, "mod_base", A_GIMME, 0);
	class_addmethod(c,(method)fl_inlist_filter_mode, "filter_mode", A_GIMME, 0);

	class_register(CLASS_BOX, c);
	fl_inlist_class = c; 
}

void *fl_inlist_new(t_symbol *s, short argc, t_atom *argv) 
{
	t_fl_inlist *x = (t_fl_inlist *)object_alloc(fl_inlist_class);

	inlet_new((t_object *)x, "list");
	x->m_outlet2 = intout((t_object *)x);
	x->m_outlet = intout((t_object *)x);

	x->filter_mode = FM_NONE;
	x->mod_base = DFLT_MODBASE;
	x->list_len = DFLT_LIST_LEN;
	
	for (long i = 0; i < MAX_LEN_LIST; i++) {
		x->map_list[i] = 0;
	}
	
	return x;
}

void fl_inlist_assist(t_fl_inlist *x, void *b, long msg, long arg, char *dst)
{
	if (msg == ASSIST_INLET) {										
		switch (arg) {
		case I_INPUT: sprintf(dst, "integer"); break;
		case I_LIST: sprintf(dst, "list"); break;
		}

	}
	else if (msg == ASSIST_OUTLET) {    
		switch (arg) {
		case O_OUTPUT: sprintf(dst, "filtered/approximated integer"); break;
		case O_OUTPUT2: sprintf(dst, "filtered integer"); break;
		}
	}
}

void fl_inlist_float(t_fl_inlist *x, double f) 
{
	fl_inlist_int(x, (long)f);
}

void fl_inlist_int(t_fl_inlist *x, long n) 
{
	if (n != n) { object_error((t_object *)x, "wrong number"); return; }
	if (n < 0) { object_error((t_object *)x, "input must be a positive integer"); return; }

	short filter_mode = x->filter_mode;
	long list_len = x->list_len;
	long *map_list = x->map_list;
	long mod_base = x->mod_base;
	long n_overmod = n / mod_base;
	long moded_n = z_mod(n, mod_base);
	long close_n;
	long filt_n;

	if (list_len < 1) { filter_mode = FM_NONE; }

	if(map_list[moded_n] == 1){ 
		outlet_int(x->m_outlet2, n);
		outlet_int(x->m_outlet, n);
	}
	else {
		close_n = moded_n;

		switch (filter_mode) {
		case FM_CHROM_DOWN:
			close_n--;
			if (close_n < 0) { 
				close_n = z_mod(close_n, mod_base);
				n_overmod--; 
			}
			break;
		
		case FM_CHROM_UP:
			close_n++;
			if (close_n >= mod_base) { 
				close_n = z_mod(close_n, mod_base);
				n_overmod++; 
			}
			break;
		
		case FM_DIAT_DOWN:
			do {
				if (--close_n < 0) { 
					close_n = z_mod(close_n, mod_base);
					n_overmod--; 
				}
			} while (map_list[close_n] == 0);
			break;
		
		case FM_DIAT_UP:
			do {
				if (++close_n >= mod_base) {
					close_n = z_mod(close_n, mod_base);
					n_overmod++;
				}
			} while (map_list[close_n] == 0);
			break;
		
		default:
			return;
		}

		filt_n = n_overmod * mod_base + close_n;
		outlet_int(x->m_outlet, filt_n);
	}
}

void fl_inlist_list(t_fl_inlist *x, t_symbol *s, long argc, t_atom *argv) 
{
	long ac = argc;
	t_atom *ap = argv;
	long mod_base = x->mod_base;
	long list_len;
	long n;

	for (long i = 0; i < MAX_LEN_LIST; i++) {
		x->map_list[i] = 0;
	}

	list_len = 0;
	for (long i = 0; i < ac; i++) {
		n = (long)atom_getlong(ap + i);
		n = z_mod(n, mod_base);
		
		if (x->map_list[n] == 0) { 
			x->map_list[n] = 1;
			list_len++; 
		}
	}

	x->list_len = list_len;
}

void fl_inlist_mod_val(t_fl_inlist *x, t_symbol *s, long argc, t_atom *argv)
{
	long ac = argc;
	t_atom *ap = argv;
	long mod_base;

	if (ac != 1) { object_error((t_object *)x, "mod: (1 arg) base"); return; }
	mod_base = (long)atom_getlong(ap);

	if (mod_base < 1) { object_error((t_object *)x, "mod: argument must be a positive number"); return; }
	if (mod_base >= MAX_LEN_LIST) { object_error((t_object *)x, "mod: argument must be less than %d", MAX_LEN_LIST); return; }

	x->mod_base = mod_base;
}

void fl_inlist_filter_mode(t_fl_inlist *x, t_symbol *s, long argc, t_atom *argv)
{
	long ac = argc;
	t_atom *ap = argv;
	short filter_mode;

	if (ac != 1) { object_error((t_object *)x, "filter_mode: (1 arg) 0 - 4"); return; }
	if (atom_gettype(ap) != A_LONG && atom_gettype(ap) != A_FLOAT) { object_error((t_object *)x, "filter_mode: argument must be a number"); return; }

	filter_mode = (short)atom_getlong(ap);
	if (filter_mode < FM_NONE && filter_mode >= FM_TOTAL) { object_error((t_object *)x, "filter_mode: not a valid mode"); return; }
	
	x->filter_mode = filter_mode;
}

long z_mod(long x, long base)
{
	long y = x;
	long b = labs(base);
	while (y < 0) { y += b; }
	y = y % b;
	return y;
}