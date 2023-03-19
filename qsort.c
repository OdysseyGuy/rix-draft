/* SPDX-License-Identifier: MIT */

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <compiler.h>

#define min(a, b) (((a) < (b)) ? (a) : (b))

#define SWAPINIT(a, es)                                                 \
    swaptype = (((uint8_t *)a - (uint8_t *)0 | es) % sizeof(size_t)) ?  \
        2 : es > sizeof(size_t) ? 1 : 0


#define PVINIT(pv, pm)                                                  \
    if (swaptype != 0) pv = arr, swap(pv, pm);                          \
    else pv = (uint8_t *)&v, v = *(size_t *)pm


#define exch(a, b, t) (t = a, a = b, b = t)
#define swap(a, b)                                                      \
    (swaptype != 0) ?                                                   \
        swapfunc(a, b, es, swaptype) :                                  \
        (void *)exch(*(size_t *)(a), *(size_t *)(b), t)

#define vecswap(a, b, n) if (n > 0) swapfunc(a, b, n, swaptype)


static void swapfunc(_in_ uint8_t *a, _in_ uint8_t *b, _in_ size_t n, _in_ int swaptype)
{
    if (swaptype <= 1) {
        size_t t;
        for (; n > 0; a += sizeof(size_t), b += sizeof(size_t), n -= sizeof(size_t)) {
            exch(*(size_t *)(a), *(size_t *)(b), t);
        }
    } else {
        uint8_t t;
        for (; n > 0; a++, b++, n--) {
            exch(*a, *b, t);
        }
    }
}

static uint8_t * med3(_in_ uint8_t *a, _in_ uint8_t *b, _in_ uint8_t *c,
                      _in_ int (*cmp)(const void *, const void *))
{
    return cmp(a, b) < 0 ?
            (cmp(b, c) < 0 ? b : cmp(a, c) < 0 ? c : a)
        :   (cmp(b, c) > 0 ? b : cmp(a, c) > 0 ? c : a);
}

/**
 * @brief
 * Quick sort.
 * 
 * @param[in] arr
 * Pointer to the array to be sorted.
 * 
 * @param[in] n
 * Number of elements.
 * 
 * @param[in] es
 * Size of each element in the array.
 * 
 * @param[in] cmp
 * Compare function for sorting.
 */
void qsort(_in_ void *arr, _in_ size_t n, _in_ size_t es,
           _in_ int (*cmp)(const void *, const void *))
{
    uint8_t *pa, *pb, *pc, *pd, *pl, *pm, *pn, *pv;
    int r, swaptype;
    size_t t, v;
    size_t s;

    SWAPINIT(arr, es);

    if (n < 7) {
        for (pm = arr + es; pm < (uint8_t *)arr + (n * es); pm += es) {
            for (pl = pm; pl > (uint8_t *)arr && cmp(pl - es, pl) > 0; pl -= es) {
                swap(pl, pl - es);
            }
        }
        return;
    }

    pm = arr + (n / 2) * es;

    if (n > 7) {
        pl = arr;
        pn = arr + (n - 1) * es;
        if (n > 40) {
            s = (n / 8) * es;
            pl = med3(pl, pl + s, pl + 2 * s, cmp);
            pm = med3(pm - s, pm, pm + s, cmp);
            pn = med3(pn - 2 * s, pn - s, pn, cmp);
        }
        pm = med3(pl, pm, pn, cmp);
    }

    PVINIT(pv, pm);
    pa = pb = arr;
    pc = pd = arr + (n - 1) * es;
    for (;;) {
        while (pb <= pc && (r = cmp(pb, pv)) <= 0) {
            if (r == 0) {
                swap(pa, pb);
                pa += es;
            }
            pb += es;
        }
        while (pc >= pb && (r = cmp(pc, pv)) >= 0) {
            if (r == 0) {
                swap(pc, pd);
                pd -= es;
            }
            pc -= es;
        }
        
        if (pb > pc) {
            break;
        }
        
        swap(pb, pc);
        pb += es;
        pc -= es;
    }

    pn = arr + n * es;
    s = min(pa - (uint8_t *)arr, pb - pa);
    vecswap(arr, pb-s, s);
    
    s = min(pd - pc, pn - pd - es);
    vecswap(pb, pn - s, s);

    /* TODO: replace recursion with iteration */
    if ((s = pb-pa) > es) {
        qsort(arr, s / es, es, cmp);
    }

    if ((s = pd-pc) > es) {
        qsort(pn - s, s / es, es, cmp);
    }
}