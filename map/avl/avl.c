//
// Created by j30036461 on 2022/11/1.
//

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "common.h"
#include "avl.h"


void *alloc_init(size_t size)
{
    void *p = malloc(size);
    assert(p != NULL);
    memset(p, 0, size);
    return p;
}


typedef struct avl_node_t {
    MixType key;
    MixType value;

    struct avl_node_t *left;
    struct avl_node_t *right;
    struct avl_node_t *parent;
    int height;
} Node;

Node *createNode(MixType key, MixType value)
{
    Node *node = alloc_init(sizeof(Node));
    node->key = key;
    node->value = value;
    return node;
}

MixType NodeKey(AvlNode *node)
{
    return node->key;
}

MixType NodeValue(AvlNode *node)
{
    return node->value;
}

typedef struct avl_tree_t {
    Node *root;
    int size;

    MixTypeComparator comparator;

    struct {
        MixTypeFreer freeKey;
        MixTypeFreer freeValue;
    };

} Tree;

Tree *CreateTree(MixTypeComparator comparator, KeyValueFreer freer)
{
    MixType a = {}, b = {};
    comparator(a, b);

    Tree *tree = alloc_init(sizeof(Tree));
    tree->comparator = comparator;
    tree->freeKey = freer.freeKey;
    tree->freeValue = freer.freeValue;

    return tree;
}

Tree *CreatePrimaryTree(MixTypeComparator comparator)
{
    KeyValueFreer freer = {NULL};
    return CreateTree(comparator, freer);
}

void DestroyTree(Tree *tree)
{
    TreeClear(tree);
    free(tree);
}

void freeNode(Tree *tree, Node *node, bool needFreeValue)
{
    if (tree->freeKey != NULL) {
        tree->freeKey(node->key);
    }
    if (needFreeValue && tree->freeValue != NULL) {
        tree->freeValue(node->value);
    }
    free(node);
}

void recursiveFreeNode(Tree *tree, Node *node)
{
    if (node == NULL) {
        return;
    }
    recursiveFreeNode(tree, node->left);
    recursiveFreeNode(tree, node->right);
    freeNode(tree, node, true);
}

typedef struct {
    Node *current;
    Node *parent;
    int cmp;
} FindResult;

FindResult g_not_found = {NULL, NULL, 0};

Optional empty = {false};

FindResult findNode(Tree *tree, MixType key)
{
    FindResult fr = g_not_found;
    fr.current = tree->root;
    while (fr.current != NULL) {
        fr.cmp = tree->comparator(key, fr.current->key);
        if (fr.cmp < 0) {
            fr.parent = fr.current;
            fr.current = fr.current->left;
        } else if (fr.cmp > 0) {
            fr.parent = fr.current;
            fr.current = fr.current->right;
        } else {
            break;
        }
    }
    return fr;
}

Node *leftmost(Node *node)
{
    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}

Node *rightmost(Node *node)
{
    while (node->right != NULL) {
        node = node->right;
    }
    return node;
}

Node *seekSuccessor(Node *node)
{
    if (node == NULL) { return NULL; }
    if (node->right != NULL) {
        return leftmost(node->right);
    }
    Node *pre = node;
    Node *cur = node->parent;
    while (cur != NULL && cur->right == pre) {
        pre = cur;
        cur = cur->parent;
    }
    return cur;
}

Node *seekPredecessor(Node *node)
{
    if (node == NULL) { return NULL; }
    if (node->left != NULL) {
        return rightmost(node->left);
    }
    Node *pre = node;
    Node *cur = node->parent;
    while (cur != NULL && cur->left == pre) {
        pre = cur;
        cur = cur->parent;
    }
    return cur;
}

int heightOf(Node *node)
{
    return node != NULL ? node->height : 0;
}

/**
 * @param node 要更新的节点
 * @return true-有更新
 */
bool updateHeight(Node *node)
{
    int old = node->height;
    int lh = heightOf(node->left);
    int rh = heightOf(node->right);
    node->height = (lh > rh ? lh : rh) + 1;
    return old != node->height;
}

Node *rotateLeft(Tree *tree, Node *x)
{
    Node *y = x->right;
    if (y == NULL) { return x; }

    x->right = y->left;
    if (x->right != NULL) {
        x->right->parent = x;
    }

    Node *p = x->parent;
    y->parent = p;
    if (p == NULL) {
        tree->root = y;
    } else if (p->left == x) {
        p->left = y;
    } else {
        p->right = y;
    }

    y->left = x;
    x->parent = y;

    updateHeight(x);
    updateHeight(y);

    return y;
}

Node *rotateRight(Tree *tree, Node *x)
{
    Node *y = x->left;
    if (y == NULL) { return x; }

    x->left = y->right;
    if (x->left != NULL) {
        x->left->parent = x;
    }

    Node *p = x->parent;
    y->parent = p;
    if (p == NULL) {
        tree->root = y;
    } else if (p->left == x) {
        p->left = y;
    } else {
        p->right = y;
    }

    y->right = x;
    x->parent = y;

    updateHeight(x);
    updateHeight(y);

    return y;
}


void reBalance(Tree *tree, Node *node, bool isInsert)
{
    while (node != NULL) {
        int dh = heightOf(node->left) - heightOf(node->right);
        if (dh < -1) {
            if (heightOf(node->right->left) > heightOf(node->right->right)) {
                rotateRight(tree, node->right);
            }
            node = rotateLeft(tree, node);
            if (isInsert) { break; }
        } else if (dh > 1) {
            if (heightOf(node->left->left) < heightOf(node->left->right)) {
                rotateLeft(tree, node->left);
            }
            node = rotateRight(tree, node);
            if (isInsert) { break; }
        } else {
            if (!updateHeight(node)) {
                break;
            }
        }
        node = node->parent;
    }
}

void afterInsertion(Tree *tree, Node *node)
{
    updateHeight(node);
    reBalance(tree, node->parent, true);
}

void afterDeletion(Tree *tree, Node *node)
{
    reBalance(tree, node, false);
}

void insertNode(Tree *tree, Node *node, Node *parent, int cmp)
{
    tree->size++;

    node->parent = parent;
    if (parent == NULL) {
        tree->root = node;
    } else if (cmp < 0) {
        parent->left = node;
    } else {
        parent->right = node;
    }

    afterInsertion(tree, node);
}

Optional TreeSet(Tree *tree, MixType key, MixType value)
{
    Optional r = empty;
    FindResult fr = findNode(tree, key);
    if (fr.current != NULL) {
        r.exists = true;
        r.data = fr.current->value;
        fr.current->value = value;
    } else {
        insertNode(tree, createNode(key, value), fr.parent, fr.cmp);
    }
    return r;
}

Optional TreeGet(Tree *tree, MixType key)
{
    Optional r = empty;
    FindResult fr = findNode(tree, key);
    if (fr.current != NULL) {
        r.exists = true;
        r.data = fr.current->value;
    }
    return r;
}

Node *chooseReplacer(Node *node)
{
    if (heightOf(node->left) < heightOf(node->right)) {
        return seekSuccessor(node);
    } else {
        return seekPredecessor(node);
    }
}

void removeNode(Tree *tree, Node *node)
{
    tree->size--;

    if (node->left != NULL && node->right != NULL) {
        Node *replacer = chooseReplacer(node);
        node->key = replacer->key;
        node->value = replacer->value;
        node = replacer;
    }

    Node *replacer = node->left != NULL ? node->left : node->right;
    Node *parent = node->parent;
    if (parent == NULL) {
        tree->root = replacer;
    } else if (parent->left == node) {
        parent->left = replacer;
    } else {
        parent->right = replacer;
    }
    if (replacer != NULL) {
        replacer->parent = parent;
    }

    freeNode(tree, node, false);

    if (parent != NULL) {
        afterDeletion(tree, parent);
    }
}

Optional TreeDel(Tree *tree, MixType key)
{
    Optional r = empty;
    FindResult fr = findNode(tree, key);
    if (fr.current != NULL) {
        r.exists = true;
        r.data = fr.current->value;
        removeNode(tree, fr.current);
    }
    return r;
}

int TreeSize(Tree *tree)
{
    return tree->size;
}

void TreeClear(Tree *tree)
{
    recursiveFreeNode(tree, tree->root);
    tree->root = NULL;
    tree->size = 0;
}

Node *TreeBegin(Tree *tree)
{
    if (tree->root == NULL) { return NULL; }
    return leftmost(tree->root);
}

Node *TreeEnd(Tree *tree)
{
    if (tree->root == NULL) { return NULL; }
    return rightmost(tree->root);
}

Node *NodeSuccessor(Node *node)
{
    return seekSuccessor(node);
}

Node *NodePredecessor(Node *node)
{
    return seekPredecessor(node);
}
