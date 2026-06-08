#include "KDTree.hpp"

#include <algorithm>
#include <cmath>

KDNode::KDNode(const VectorItem& v)
    : item(v) {}

KDTree::KDTree(int d)
    : dims(d) {}

KDTree::~KDTree()
{
    destroy(root);
}

void KDTree::destroy(KDNode* n)
{
    if (!n)
        return;

    destroy(n->left);
    destroy(n->right);

    delete n;
}

KDNode* KDTree::ins(
    KDNode* n,
    const VectorItem& v,
    int depth)
{
    if (!n)
        return new KDNode(v);

    int axis = depth % dims;

    if (v.emb[axis] < n->item.emb[axis])
        n->left = ins(n->left, v, depth + 1);
    else
        n->right = ins(n->right, v, depth + 1);

    return n;
}

void KDTree::insert(const VectorItem& v)
{
    root = ins(root, v, 0);
}

void KDTree::knn(
    KDNode* n,
    const std::vector<float>& q,
    int k,
    int depth,
    DistFn dist,
    std::priority_queue<std::pair<float, int>>& heap)
{
    if (!n)
        return;

    float nodeDist = dist(q, n->item.emb);

    if ((int)heap.size() < k || nodeDist < heap.top().first)
    {
        heap.push({nodeDist, n->item.id});

        if ((int)heap.size() > k)
            heap.pop();
    }

    int axis = depth % dims;

    float diff = q[axis] - n->item.emb[axis];

    KDNode* closer =
        diff < 0 ? n->left : n->right;

    KDNode* farther =
        diff < 0 ? n->right : n->left;

    knn(
        closer,
        q,
        k,
        depth + 1,
        dist,
        heap
    );

    if ((int)heap.size() < k ||
        std::abs(diff) < heap.top().first)
    {
        knn(
            farther,
            q,
            k,
            depth + 1,
            dist,
            heap
        );
    }
}

std::vector<std::pair<float, int>>
KDTree::knn(
    const std::vector<float>& q,
    int k,
    DistFn dist)
{
    std::priority_queue<std::pair<float, int>> heap;

    knn(
        root,
        q,
        k,
        0,
        dist,
        heap
    );

    std::vector<std::pair<float, int>> result;

    while (!heap.empty())
    {
        result.push_back(heap.top());
        heap.pop();
    }

    std::sort(
        result.begin(),
        result.end()
    );

    return result;
}

void KDTree::rebuild(
    const std::vector<VectorItem>& items)
{
    destroy(root);

    root = nullptr;

    for (const auto& v : items)
        insert(v);
}