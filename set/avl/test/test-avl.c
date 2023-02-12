//
// Created by j30036461 on 2022/11/1.
//

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>

#include "common.h"
#include "avl.h"


typedef struct MyTime {
    double start, end;
} MyTime;

void MyTime_start(MyTime *tm)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    tm->start = ((double) tv.tv_sec) + ((double) tv.tv_usec) * 1e-6;
}

double MyTime_stop(MyTime *tm)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    tm->end = ((double) tv.tv_sec) + ((double) tv.tv_usec) * 1e-6;
    return tm->end - tm->start;
}



int primaryCompare(MixType a, MixType b)
{
    return a.u64 < b.u64 ? -1 : (a.u64 > b.u64 ? 1 : 0);
}

typedef struct test_node_t {
    MixType key;
    MixType value;

    struct test_node_t *left;
    struct test_node_t *right;
    struct test_node_t *parent;
    int height;
} testNode;
typedef struct {
    testNode *root;
    int size;

    MixTypeComparator comparator;
    struct {
        MixTypeFreer freeKey;
        MixTypeFreer freeValue;
    };
} testTree;

int validHeight(testTree *tt, testNode *node)
{
    if (node == NULL) { return 0; }
    int lh = validHeight(tt, node->left);
    int rh = validHeight(tt, node->right);
    assert(abs(lh - rh) < 2);
    int h = (lh > rh ? lh : rh) + 1;
    assert(h == node->height);
    return h;
}

void valid(AvlTree *tree)
{
#ifndef NDEBUG
    AvlNode *last = NULL;
    for (AvlNode *cursor = TreeBegin(tree); cursor != NULL; cursor = NodeSuccessor(cursor)) {
        if (last != NULL) {
            MixType lk = NodeKey(last);
            MixType ck = NodeKey(cursor);
            int cmp = primaryCompare(lk, ck);
            if (cmp >= 0) {
                fprintf(stderr, "bad rank: %d → %d\n", lk.u64, ck.u64);
            }
            assert(cmp < 0);
        }
        last = cursor;
    }
    testTree *tt = (testTree *) tree;
    validHeight(tt, tt->root);
#endif
}

void testValid(const int batch, bool useRandKey)
{
    if (useRandKey) {
        printf("========= use random key\n");
        srand(time(0));
    } else {
        printf("========= use serialized key\n");
    }

    MixType *keys = malloc(sizeof(MixType) * batch);
    for (unsigned i = 0; i < batch; ++i) {
        if (useRandKey) {
            keys[i].u64 = (((uint64_t) rand()) << 32) + ((uint64_t) rand());
        } else {
            keys[i].u64 = (uint64_t) i + (((uint64_t) i) << 32);
        }
    }

    AvlTree *tree = CreatePrimaryTree(primaryCompare);

    MyTime mt;
    MyTime_start(&mt);
    int size = 0;
    for (int i = 0; i < batch; ++i) {
        Optional r = TreeSet(tree, keys[i], keys[i]);
        if (!r.exists) {
            size++;
            valid(tree);
        }
    }
    valid(tree);
    printf("Set: %f\n", MyTime_stop(&mt));

    MyTime_start(&mt);
    for (int i = 0; i < batch; ++i) {
        Optional r = TreeGet(tree, keys[i]);
        assert(r.exists);
        assert(primaryCompare(r.data, keys[i]) == 0);
    }
    printf("Get: %f\n", MyTime_stop(&mt));

    MyTime_start(&mt);
    for (int i = 0; i < batch; ++i) {
        Optional r = TreeDel(tree, keys[i]);
        if (r.exists) {
            size--;
            assert(TreeSize(tree) == size);
            assert(primaryCompare(r.data, keys[i]) == 0);
            valid(tree);
        }
    }
    printf("Del: %f\n", MyTime_stop(&mt));

    DestroyTree(tree);
}

int main(int argc, char **argv)
{
#ifdef NDEBUG
    const int batch = 2000000;
    testValid(batch, false);
    testValid(batch, true);
#else
    for (int i = 1; i <= 64; ++i) {
        testValid(i, false);
        testValid(i, true);
    }
    const int batch = 100;
    testValid(batch, false);
    testValid(batch, true);
#endif


    return 0;
}
