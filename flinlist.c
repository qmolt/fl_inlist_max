#include "flinlist.h"

void C74_EXPORT main() {

	t_class *c;

	c = class_new("flinlist",(method)fl_inlist_new, (method)NULL,sizeof(t_fl_inlist), 0, 0);
	class_addmethod(c, (method)fl_inlist_assist,"assist", A_CANT, 0);
	class_addmethod(c,(method)fl_inlist_int, "int", A_LONG, 0);
	class_addmethod(c,(method)fl_inlist_float, "float", A_LONG, 0);
	class_addmethod(c,(method)fl_inlist_list, "list", A_GIMME, 0);
	class_addmethod(c,(method)fl_inlist_mod_val, "mod_val", A_GIMME, 0);

	class_register(CLASS_BOX, c);
	fl_inlist_class = c; 
}

void *fl_inlist_new(t_symbol *s, short argc, t_atom *argv) 
{
	t_fl_inlist *x = (t_fl_inlist *)object_alloc(fl_inlist_class);

	inlet_new((t_object *)x, "list");
	x->m_outlet = intout((t_object *)x);

	x->mod_value = MOD_STDR;
	x->list_len = LIST_LEN_STDR;
	x->list = (long *)sysmem_newptr(LIST_LEN_STDR * sizeof(long));
	
	if (!x->list) { object_error((t_object *)x, "no memory for a 12-long pointer... really?"); }
	else {
		for (long i = 0; i < x->list_len; i++) {
			x->list[i] = i;
		}
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
		case O_OUTPUT: sprintf(dst, "filtered integer"); break;
		}
	}
}

void fl_inlist_float(t_fl_inlist *x, double f) 
{
	fl_inlist_int(x, (long)f);
}

void fl_inlist_int(t_fl_inlist *x, long n) 
{
	if (n != n) { object_warn((t_object *)x, "wrong number"); return; }
	if (n < 0) { object_warn((t_object *)x, "input must be a positive integer"); return; }

	long list_len = x->list_len;
	long *list = x->list;
	long mod_val = x->mod_value;

	for (long i = 0; i < list_len; i++) {
		if (list[i] == n % mod_val) {
			outlet_int(x->m_outlet, n);
			return;
		}
	}
}

void fl_inlist_list(t_fl_inlist *x, t_symbol *s, long argc, t_atom *argv) 
{
	long ac = argc;
	t_atom *ap = argv;
	long mod_val = x->mod_value;

	x->list = (long *)sysmem_resizeptr(x->list, ac * sizeof(long));
	if (!x->list) { object_error((t_object *)x, "no memory for list"); return; }

	x->list_len = ac;

	for (long i = 0; i < ac; i++) {
		x->list[i] = ((long)atom_getlong(ap + i)) % mod_val;
	}
}

void fl_inlist_mod_val(t_fl_inlist *x, t_symbol *s, long argc, t_atom *argv)
{
	long ac = argc;
	t_atom *ap = argv;
	long mod_val;

	if (ac != 1) { object_error((t_object *)x, "1 integer"); return; }
	mod_val = (long)atom_getlong(ap);

	if (mod_val != mod_val) { object_error((t_object *)x, "wrong number"); return; }

	x->mod_value = mod_val;
}