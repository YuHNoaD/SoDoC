#include "Tree.h"
#include <algorithm>
#include <set>

PersonNode* TreeAlgorithms::findLCA(PersonNode* u, PersonNode* v) {
    if (!u || !v) return nullptr;

    std::set<PersonNode*> anc;  // ancestors of u
    PersonNode* cur = u;
    while (cur) {
        anc.insert(cur);
        cur = cur->parent;
    }

    cur = v;
    while (cur) {
        if (anc.find(cur) != anc.end()) {
            return cur;
        }
        cur = cur->parent;
    }

    return nullptr;
}

bool TreeAlgorithms::canMarry(PersonNode* u, PersonNode* v) {
    PersonNode* lca = findLCA(u, v);
    if (!lca) return true;  // no blood relation
    
    int d1 = 0;
    PersonNode* cur1 = u;
    while (cur1 != lca && cur1) {
        cur1 = cur1->parent;
        d1++;
    }
    
    int d2 = 0;
    PersonNode* cur2 = v;
    while (cur2 != lca && cur2) {
        cur2 = cur2->parent;
        d2++;
    }
    
    // Cannot marry if total distance <= 6 (Vietnamese law)
    return (d1 + d2 >3);
}

int TreeAlgorithms::getGenerationDistance(PersonNode* ancestor, PersonNode* p1, PersonNode* p2) {
    // Neu khong co to tien chung, tra ve -1
    if (!ancestor) return -1;

    // Dem so the he tu p1 den to tien chung
    int d1 = 0;
    PersonNode* temp = p1;
    while (temp != ancestor) {
        d1++;
        temp = temp->parent;  // Di len cha me
    }

    // Dem so the he tu p2 den to tien chung
    int d2 = 0;
    temp = p2;
    while (temp != ancestor) {
        d2++;
        temp = temp->parent;  // Di len cha me
    }

    // Tra ve khoang cach lon nhat giua 2 nguoi den to tien chung
    return std::max(d1, d2);
}
