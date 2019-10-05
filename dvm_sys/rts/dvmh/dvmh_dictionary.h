#pragma once

#include <cassert>
#include <cstdarg>

#include "dvmh_types.h"

namespace libdvmh {

template <typename Tk, typename Tv>
struct DictionaryRecord {
    Tk key;
    Tv value;
    DictionaryRecord<Tk, Tv> *left, *right; //Left - with lesser keys, right - with greater keys
    int balance; // Height of left subtree minus height of right subtree (left-right)
};

template <typename Tk, typename Tv>
class Dictionary {
public:
    Dictionary() {
        root = 0;
        nodeCount = 0;
    }
public:
    Tv *find(const Tv &key) const {
        DictionaryRecord<Tk, Tv> *found = findInternal(key);
        if (found)
            return &(found->value);
        else
            return 0;
    }
    int erase(const Tk &key) {
        if (root) {
            int heightChanged;
            int found;
            root = eraseInternal(root, key, &heightChanged, &found);
            if (found)
                nodeCount--;
            return found;
        } else
            return 0;
    }
    int add(const Tk &key, const Tv &value) {
        int heightChanged;
        int inserted;
        root = addInternal(root, key, value, &heightChanged, &inserted);
        if (inserted)
            nodeCount++;
        return inserted;
    }
    int replace(const Tk &key, const Tv &value) {
        DictionaryRecord<Tk, Tv> *found = findInternal(key);
        if (found) {
            found->value = value;
            return 1;
        } else
            return 0;
    }
    void forEach(DvmHandlerFunc f, int numArgs, ...) const {
        if (numArgs < 0)
            numArgs = 0;
#ifdef NON_CONST_AUTOS
        void *params[numArgs + 2];
#else
        void *params[MAX_PARAM_COUNT];
#endif
        if (numArgs > 0) {
            va_list ap;
            va_start(ap, numArgs);
            int i;
            for (i = 0; i < numArgs; i++)
                params[i] = va_arg(ap, void *);
            va_end(ap);
        }
        if (root)
            forEachInternal(root, f, params, numArgs);
    }
public:
    ~Dictionary() {
        if (root)
            deleteInternal(root);
    }
protected:
    DictionaryRecord<Tk, Tv> *findInternal(const Tk &key) const {
        DictionaryRecord<Tk, Tv> *cur = root;
        while (cur) {
            if (key == cur->key)
                return cur;
            else if (key < cur->key)
                cur = cur->left;
            else
                cur = cur->right;
        }
        return 0;
    }
    static DictionaryRecord<Tk, Tv> *fixBalanceInternal(DictionaryRecord<Tk, Tv> *root, int *pHeightChanged);
    static DictionaryRecord<Tk, Tv> *pickBiggestInternal(DictionaryRecord<Tk, Tv> *root, int *pHeightChanged, DictionaryRecord<Tk, Tv> **res) {
        *pHeightChanged = 0;
        if (root->right == 0) {
            *pHeightChanged = 1;
            *res = root;
            root = root->left;
        } else {
            int heightDelta = 0;
            root->right = pickBiggestInternal(root->right, &heightDelta, res);
            if (root->balance < 0)
                *pHeightChanged = heightDelta;
            root->balance += heightDelta;
        }
        if (root) {
            int heightDelta = 0;
            root = fixBalanceInternal(root, &heightDelta);
            *pHeightChanged += heightDelta;
        }
        return root;
    }
    static DictionaryRecord<Tk, Tv> *eraseInternal(DictionaryRecord<Tk, Tv> *root, const Tk &key, int *pHeightChanged, int *pFound);
    static DictionaryRecord<Tk, Tv> *addInternal(DictionaryRecord<Tk, Tv> *root, const Tk &key, const Tv &value, int *pHeightChanged, int *pInserted);
    static void forEachInternal(const DictionaryRecord<Tk, Tv> *root, DvmHandlerFunc f, void *params[], int numArgs) {
        if (root->left)
            forEachInternal(root->left, f, params, numArgs);
        params[numArgs] = root->key;
        params[numArgs + 1] = root->value;
        executeFunction(f, params, numArgs + 2);
        if (root->right)
            forEachInternal(root->right, f, params, numArgs);
    }
    static void deleteInternal(DictionaryRecord<Tk, Tv> *root) {
        if (root->left)
            deleteInternal(root->left);
        if (root->right)
            deleteInternal(root->right);
        delete root;
    }
protected:
    DictionaryRecord<Tk, Tv> *root;
    int nodeCount;
};

template <typename Tk, typename Tv>
DictionaryRecord<Tk, Tv> *Dictionary<Tk, Tv>::fixBalanceInternal(DictionaryRecord<Tk, Tv> *root, int *pHeightChanged) {
    DictionaryRecord<Tk, Tv> *R, *L;
    R = root->right;
    L = root->left;
    *pHeightChanged = root->balance == 2 || root->balance == -2;
    if (root->balance == -2) {
        // Right is too big
        DictionaryRecord<Tk, Tv> *RL = R->left;
        if (R->balance == -1) {
            root->balance = 0; // h(RL)=H-3 h(L)=H-3 => h(N)=H-2
            R->balance = 0;    // h(RR)=H-2 => h(R)=H-1

            root->right = RL;
            R->left = root;
            root = R;
        } else if (R->balance == 0) {
            root->balance = -1; // h(RL)=H-2 h(L)=H-3 => h(N)=H-1
            R->balance = 1;     // h(RR)=H-2 => h(L)=H

            root->right = RL;
            R->left = root;
            root = R;
            *pHeightChanged = 0;
        } else {
            // R->balance == 1
            DictionaryRecord<Tk, Tv> *RLR, *RLL;
            RLR = RL->right;
            RLL = RL->left;

            R->left = RLR;
            R->balance = std::min(-RL->balance, 0); //1 => -1, 0 => 0, -1 => 0

            root->right = RLL;
            root->balance = std::max(-RL->balance, 0); //1 => 0, 0 => 0, -1 => 1

            RL->right = R;
            RL->left = root;
            RL->balance = 0;

            root = RL;
        }
    } else if (root->balance == 2) {
        DictionaryRecord<Tk, Tv> *LR = L->right;
        if (L->balance == 1) {
            root->balance = 0; // h(LR)=H-3 h(R)=H-3 => h(N)=H-2
            L->balance = 0;    // h(LL)=H-2 => h(L)=H-1

            root->left = LR;
            L->right = root;
            root = L;
        } else if (L->balance == 0) {
            root->balance = 1; // h(LR)=H-2 h(R)=H-3 => h(N)=H-1
            L->balance = -1;   // h(LL)=H-2 => h(L)=H

            root->left = LR;
            L->right = root;
            root = L;
            *pHeightChanged = 0;
        } else {
            // L->balance == -1
            DictionaryRecord<Tk, Tv> *LRL, *LRR;
            LRL = LR->left;
            LRR = LR->right;

            L->right = LRL;
            L->balance = std::max(-LR->balance, 0); //1 => 0, 0 => 0, -1 => 1

            root->left = LRR;
            root->balance = std::min(-LR->balance, 0); //1 => -1, 0 => 0, -1 => 0

            LR->left = L;
            LR->right = root;
            LR->balance = 0;
            root = LR;
        }
    }
    return root;
}

template <typename Tk, typename Tv>
DictionaryRecord<Tk, Tv> *Dictionary<Tk, Tv>::eraseInternal(DictionaryRecord<Tk, Tv> *root, const Tk &key, int *pHeightChanged, int *pFound) {
    if (pFound)
        *pFound = 0;
    *pHeightChanged = 0;
    if (key == root->key) {
        if (pFound)
            *pFound = 1;
        DictionaryRecord<Tk, Tv> *save = root;
        if (root->left == 0 || root->right == 0) {
            *pHeightChanged = 1;
            if (root->left == 0)
                root = root->right;
            else
                root = root->left;
        } else {
            int heightDelta = 0;
            DictionaryRecord<Tk, Tv> *newRoot = 0;
            root->left = pickBiggestInternal(root->left, &heightDelta, &newRoot);
            assert(newRoot);
            newRoot->left = root->left;
            newRoot->right = root->right;
            newRoot->balance = root->balance;
            //newRoot->key = root->key;
            root = newRoot;
            if (root->balance > 0)
                *pHeightChanged = heightDelta;
            root->balance -= heightDelta;
        }
        delete save;
    } else if (key < root->key && root->left != 0) {
        int heightDelta = 0;
        root->left = eraseInternal(root->left, key, &heightDelta, pFound);
        if (root->balance > 0)
            *pHeightChanged = heightDelta;
        root->balance -= heightDelta;
    } else if (key > root->key && root->right != 0) {
        int heightDelta = 0;
        root->right = eraseInternal(root->right, key, &heightDelta, pFound);
        if (root->balance < 0)
            *pHeightChanged = heightDelta;
        root->balance += heightDelta;
    }
    if (root) {
        int heightDelta = 0;
        root = fixBalanceInternal(root, &heightDelta);
        *pHeightChanged += heightDelta;
    }
    return root;
}

template <typename Tk, typename Tv>
DictionaryRecord<Tk, Tv> *Dictionary<Tk, Tv>::addInternal(DictionaryRecord<Tk, Tv> *root, const Tk &key, const Tv &value, int *pHeightChanged, int *pInserted) {
    if (pInserted)
        *pInserted = 0;
    *pHeightChanged = 0;
    if (root == 0) {
        if (pInserted)
            *pInserted = 1;
        root = new DictionaryRecord<Tk, Tv>();
        root->balance = 0;
        root->key = key;
        root->left = 0;
        root->right = 0;
        root->value = value;
        *pHeightChanged = 1;
    } else {
        if (key == root->key) {
            root->value = value;
        } else if (key < root->key) {
            int heightDelta = 0;
            root->left = addInternal(root->left, key, value, &heightDelta, pInserted);
            if (root->balance >= 0)
                *pHeightChanged = heightDelta;
            root->balance += heightDelta;
        } else {
            // key > root->key
            int heightDelta = 0;
            root->right = addInternal(root->right, key, value, &heightDelta, pInserted);
            if (root->balance <= 0)
                *pHeightChanged = heightDelta;
            root->balance -= heightDelta;
        }
    }
    if (root) {
        int heightDelta = 0;
        root = fixBalanceInternal(root, &heightDelta);
        *pHeightChanged -= heightDelta;
    }
    return root;
}

}
