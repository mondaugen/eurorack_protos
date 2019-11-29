#include <stdint.h>
#include "m_pd.h"
#include "fixed_heap_f32.h"
#define FIXED_HEAP_F32_SIZE 100000

static t_class *pqueue_tilde_class;

typedef struct _pqueue_tilde
{
    t_object x_obj;
    t_float x_f;
    struct fixed_heap *heap;
    uint64_t cur_samp;

} t_pqueue_tilde;

static void *pqueue_tilde_new(t_floatarg f)
{
    t_pqueue_tilde *x = (t_pqueue_tilde *)pd_new(pqueue_tilde_class);
    signalinlet_new(&x->x_obj, f);
    outlet_new(&x->x_obj, &s_signal);
    x->x_f = 0;
    struct fixed_heap_f32_init fh_init = {
        .max_n_items = FIXED_HEAP_F32_SIZE,
        .max_heap = 0
    };
    x->heap = fixed_heap_f32_new(&fh_init);
    x->cur_samp = 0;
    /* TODO: What if allocating this heap fails? */
    return (x);
}

static void pqueue_tilde_free(t_pqueue_tilde *x)
{
    fixed_heap_free(x->heap);
}

t_int *pqueue_tilde_perform(t_int *w)
{
    t_sample *in1 = (t_sample *)(w[1]);
    t_sample *in2 = (t_sample *)(w[2]);
    t_sample *out = (t_sample *)(w[3]);
    int N = (int)(w[4]), n = 0;
    t_pqueue_tilde *x = (t_pqueue_tilde*)(w[5]);
    while (N--)
    {
        float f1 = *in1++, f2 = *in2++;
        if (f1 != 0) {
            if (f2 < 0) { f2 = 0; }
            struct fixed_heap_item_f32 item = {
                .index = x->cur_samp + n + f2,
                .value = f1
            };
            fixed_heap_insert(x->heap,&item);
        }
        n++;
    }
    n = (int)(w[4]);
    while (n--) {
        *out = 0;
        const struct fixed_heap_item_f32 *item = fixed_heap_access(x->heap,0);
        while (item && (item->index <= x->cur_samp)) {
            *out += item->value;
            fixed_heap_remove_top(x->heap);
            item = fixed_heap_access(x->heap,0);
        }
        out++;
        x->cur_samp++;
    }
    return (w+6);
}

static void pqueue_tilde_dsp(t_pqueue_tilde *x, t_signal **sp)
{
    dsp_add(pqueue_tilde_perform, 5,
        sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n, x);
}

void pqueue_tilde_setup(void)
{
    pqueue_tilde_class = class_new(gensym("pqueue~"), (t_newmethod)pqueue_tilde_new, 0,
        sizeof(t_pqueue_tilde), 0, A_DEFFLOAT, 0);
    CLASS_MAINSIGNALIN(pqueue_tilde_class, t_pqueue_tilde, x_f);
    class_addmethod(pqueue_tilde_class, (t_method)pqueue_tilde_dsp,
        gensym("dsp"), A_CANT, 0);
}


