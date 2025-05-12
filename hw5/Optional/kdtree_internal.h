#ifndef __KDTREE_INTERNAL_H__
#define __KDTREE_INTERNAL_H__

#include "location.h"

// Define kdtree_node here so it's accessible to both kdtree.c and kdtree_helpers.h
typedef struct kdtree_node {
    location loc;
    int cut_dim;
    struct kdtree_node *left;
    struct kdtree_node *right;
} kdtree_node;

#endif
