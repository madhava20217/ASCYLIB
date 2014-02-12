/*
 * File:
 *   skiplist-lock.c
 * Author(s):
 *   Vincent Gramoli <vincent.gramoli@epfl.ch>
 * Description:
 *   Skip list implementation of an integer set
 *
 * Copyright (c) 2009-2010.
 *
 * skiplist-lock.c is part of Synchrobench
 * 
 * Synchrobench is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "skiplist-lock.h"
#include "utils.h"

unsigned int levelmax;
__thread ssmem_allocator_t* alloc;

inline int
get_rand_level()
{
  int i, level = 1;
  for (i = 0; i < levelmax - 1; i++) 
    {
      if ((rand_range(100)-1) < 50)
	level++;
      else
	break;
    }
  /* 1 <= level <= levelmax */
  return level;
}

int
floor_log_2(unsigned int n) 
{
  int pos = 0;
  if (n >= 1<<16) { n >>= 16; pos += 16; }
  if (n >= 1<< 8) { n >>=  8; pos +=  8; }
  if (n >= 1<< 4) { n >>=  4; pos +=  4; }
  if (n >= 1<< 2) { n >>=  2; pos +=  2; }
  if (n >= 1<< 1) {           pos +=  1; }
  return ((n == 0) ? (-1) : pos);
}

/* 
 * Create a new node without setting its next fields. 
 */
sl_node_t*
sl_new_simple_node(val_t val, int toplevel, int transactional)
{
  sl_node_t* node;
	
#if GC == 1
  if (unlikely(transactional))
    {
      /* use levelmax instead of toplevel in order to be able to use the ssalloc allocator*/
      size_t ns = sizeof(sl_node_t) + levelmax * sizeof(sl_node_t *);
      size_t ns_rm = ns % 64;
      if (ns_rm)
	{
	  ns += 64 - ns_rm;
	}
      node = (sl_node_t*)ssalloc(ns);
    }
  else 
    {
      size_t ns = sizeof(sl_node_t) + levelmax * sizeof(sl_node_t *);
#  if defined(DO_PAD)
      size_t ns_rm = ns & 63;
      if (ns_rm)
	{
	  ns += 64 - ns_rm;
	}
#  endif
      node = (sl_node_t*) ssmem_alloc(alloc, ns);
    }
#else
  size_t ns = sizeof(sl_node_t) + levelmax * sizeof(sl_node_t *);
  if (transactional)
    {
      size_t ns_rm = ns % 64;
      if (ns_rm)
	{
	  ns += 64 - ns_rm;
	}
    }
  node = (sl_node_t*)ssalloc(ns);
#endif

  node->val = val;
  node->toplevel = toplevel;
  node->marked = 0;
  node->fullylinked = 0;
  INIT_LOCK(ND_GET_LOCK(node));

#if defined(__tile__)
  MEM_BARRIER;
#endif

  return node;
}

/* 
 * Create a new node with its next field. 
 * If next=NULL, then this create a tail node. 
 */
sl_node_t*
sl_new_node(val_t val, sl_node_t *next, int toplevel, int transactional)
{
  sl_node_t *node;
  int i;
	
  node = sl_new_simple_node(val, toplevel, transactional);
	
  for (i = 0; i < toplevel; i++)
    node->next[i] = next;
	
  MEM_BARRIER;

  return node;
}

void
sl_delete_node(sl_node_t *n)
{
  DESTROY_LOCK(ND_GET_LOCK(n));
  ssfree_alloc(1, n);
}

sl_intset_t*
sl_set_new()
{
  sl_intset_t *set;
  sl_node_t *min, *max;
	
  if ((set = (sl_intset_t *)ssalloc(sizeof(sl_intset_t))) == NULL)
    {
      perror("malloc");
      exit(1);
    }

  ssalloc_align_alloc(0);
  /* set = (sl_intset_t *)xmalloc(sizeof(sl_intset_t)); */
  max = sl_new_node(VAL_MAX, NULL, levelmax, 1);
  min = sl_new_node(VAL_MIN, max, levelmax, 1);
  max->fullylinked = 1;
  min->fullylinked = 1;
  set->head = min;

#if defined(LL_GLOBAL_LOCK)
  ssalloc_align_alloc(0);
  set->lock = (volatile ptlock_t*) ssalloc(sizeof(ptlock_t));
  if (set->lock == NULL)
    {
      perror("malloc");
      exit(1);
    }
  GL_INIT_LOCK(set->lock);
  ssalloc_align_alloc(0);
#endif

  return set;
}

void
sl_set_delete(sl_intset_t *set)
{
  sl_node_t *node, *next;
	
  node = set->head;
  while (node != NULL)
    {
      next = node->next[0];
      sl_delete_node(node);
      node = next;
    }
#if defined(LL_GLOBAL_LOCK)
  ssfree((void*) set->lock);
#endif
  ssfree((void*) set);
}

int sl_set_size(sl_intset_t *set)
{
  int size = 0;
  sl_node_t *node;
	
  /* We have at least 2 elements */
  node = set->head->next[0];
  while (node->next[0] != NULL) 
    {
      if (node->fullylinked && !node->marked)
	{
	  size++;
	}
      node = node->next[0];
    }
  return size;
}
