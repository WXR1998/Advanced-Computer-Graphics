#ifndef KDTREE_H
#define KDTREE_H

#include <cstdio>
#include <algorithm>
#include "vecmath.h"

extern const double eps;

struct SPPMNode {
    Vector3f point, color, normal;
    int index;  // 对应到图像数组的数组下标
    double prob, radius;    // radius 是当前节点的影响距离 注意该距离不是球，而是边长为2radius的正方体
    SPPMNode() {
        index = -1; 
        prob = 1;
    }
    SPPMNode(Vector3f point, Vector3f color, Vector3f normal, double radius = 1, int index = -1, double prob = 1):
        point(point), color(color), normal(normal), radius(radius), index(index), prob(prob) {}
}

struct PixelColor {
    double strength;
    Vector3f sumColor;

    PixelColor(): strength(0), sumColor(0, 0, 0) {}
    PixelColor(double strength, Vector3f sumColor): strength(strength), sumColor(sumColor) {}
    void add(Vector3f color, double st = 1.0) {
        strength += st;
        sumColor = sumColor + color;
    }
    Vector3f getColor() {
        if (fabs(strength) < eps) return sumColor;
        return sumColor / strength;
    }
    void reset() {
        strength = 0.0;
        sumColor = Vector3f::ZERO;
    }

    PixelColor operator * (double r) const {
        return PixelColor(r * strength, r * sumColor);
    }
    PixelColor operator + (const PixelColor &p) const {
        return PixelColor(p.strength + strength, p.sumColor + sumColor);
    }
    PixelColor operator / (double r) const {
        return PixelColor(r / strength, r / sumColor);
    }
};

/*
    kd树用于保存所有遇到的漫反射点
*/
class KDTree {
private:
    static int Dim;
    int size, root;
    struct Node {
        SPPMNode sppm;
        Vector3f min, max;      // 保存这个kd树节点的包围盒的两个顶点
        int lc, rc;             // 两个孩子
        Node(): sppm() {
            lc = rc = 0;
        }
        bool operator < (const Node &a) const {
            switch (Dim) {
                case 0: return sppm.point.x() < a.sppm.point.x(); break;
                case 1: return sppm.point.y() < a.sppm.point.y(); break;
                case 2: return sppm.point.z() < a.sppm.point.z(); break;
            }
        }
    };
    Node *tree;

    void maintain(int obj, int sub){    // 维护kd树节点的包围盒，sub要把自己的包围盒更新到obj去
        tree[obj].min = tree[obj].min.minimum(tree[sub].min);
        tree[obj].max = tree[obj].max.maximum(tree[sub].max);
    }
    int build(int l, int r, int d){    // 递归构建kd树 [l,r]
        Dim = d;
        int mid = l + r >> 1;
        std::nth_element(tree + l, tree + mid, tree + r + 1);
        tree[mid].min = tree[mid].sppm.point - tree[mid].sppm.radius;
        tree[mid].max = tree[mid].sppm.point + tree[mid].sppm.radius;
        if (l < mid){
            tree[mid].lc = build(l, mid - 1, (d+1)%3);
            maintain(mid, tree[mid].lc);
        }
        if (r > mid){
            tree[mid].rc = build(mid + 1, r, (d+1)%3);
            maintain(mid, tree[mid].rc);
        }
        return mid;
    }

    double length(Vector3f p, Vector3f a, Vector3f b) {
        return (Vector3f().maximum(p - b).maximum(a - p)).squaredLength();
    }

    void _query(const SPPMNode &n, PixelColor *c, int p) {
        if ((tree[p].sppm.point - n.point).squaredLength() <= sqr(tree[p].sppm.radius) && 
            Vector3f::dot(tree[p].sppm.normal, n.normal) >= 0)
            c[tree[p].sppm.index].add(n.color * tree[p].sppm.color, n.prob);

        double disl, disr;
        if (tree[p].lc > 0)
            disl = length(n.point, tree[tree[p].lc].min, tree[tree[p].lc].max);
        else disl = 1e10;
        if (tree[p].rc > 0)
            disr = length(n.point, tree[tree[p].rc].min, tree[tree[p].rc].max);
        else disr = 1e10;

        if (disl >= disr){
            if (disr < eps) _query(n, c, tree[p].rc);
            if (disl < eps) _query(n, c, tree[p].lc);
        }else{
            if (disl < eps) _query(n, c, tree[p].lc);
            if (disr < eps) _query(n, c, tree[p].rc);
        }
    }

    void _update(int p) {
        tree[p].min = tree[p].sppm.point - tree[p].sppm.radius;
        tree[p].max = tree[p].sppm.point + tree[p].sppm.radius;
        if (tree[p].lc > 0){
            _update(tree[p].lc);
            maintain(p, tree[p].lc);
        }
        if (tree[p].rc > 0){
            _update(tree[p].rc);
            maintain(p, tree[p].rc);
        }
    }

public:

    KDTree(std::vector<SPPMNode> &nodes) {
        int s = nodes.size();
        if (tree != nullptr)
            delete[] tree;
        tree = new Node[s + 1];
        for (int i = 1; i <= s; ++i)
            tree[i].sppm = nodes[i-1];
        root = build(1, s, 0);
    }

    
    KDTree() {
        tree = nullptr;
    }
    ~KDTree() {
        if (tree != nullptr)
            delete[] tree;
    }

    // 光源发射光子到达了node处，计算这个光子给图像的最终贡献
    void query(SPPMNode node, PixelColor *c) {
        _query(node, c, root);
    }
    void update() {
        _update(root);
    }
};

#endif