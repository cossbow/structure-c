//
// Created by j30036461 on 2022/11/1.
//

#ifndef AVL_H
#define AVL_H


typedef struct avl_node_t AvlNode;

MixType NodeKey(AvlNode *);

MixType NodeValue(AvlNode *);

typedef struct avl_tree_t AvlTree;

typedef struct {
    MixTypeFreer freeKey;
    MixTypeFreer freeValue;
} KeyValueFreer;

AvlTree *CreateTree(MixTypeComparator comparator, KeyValueFreer freer);

AvlTree *CreatePrimaryTree(MixTypeComparator comparator);

void DestroyTree(AvlTree *tree);

Optional TreeSet(AvlTree *tree, MixType key, MixType value);

Optional TreeGet(AvlTree *tree, MixType key);

Optional TreeDel(AvlTree *tree, MixType key);

int TreeSize(AvlTree *tree);

void TreeClear(AvlTree *tree);

AvlNode *TreeBegin(AvlTree *tree);

AvlNode *TreeEnd(AvlTree *tree);

AvlNode *NodeSuccessor(AvlNode *node);

AvlNode *NodePredecessor(AvlNode *node);


#endif //AVL_H
