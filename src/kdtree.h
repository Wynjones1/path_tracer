#ifndef KDTREE_H
#define KDTREE_H
#include <stdint.h>
#include <string.h>
#include "aabb.h"
#include "types.h"
#define KDTREE_DEBUG 0

class KDNode
{
public:
    KDNode(KDNode *parent, const std::vector<glm::vec3>  &vertices,
           const std::vector<idx_vec3> &faces,
           const AABB &bounds, uint32_t depth, uint32_t min_elems, uint8_t axis);

	KDNode *KDNode::find(const idx_vec3 &face, const AABB &bounds, glm::vec3 point);
    bool               leaf;
    float              split;
    uint8_t            axis;
    std::vector<idx_vec3> faces;
    KDNode *children[2];

#if KDTREE_DEBUG
	bool    is_ancestor(const KDNode *input) const;
	uint32_t depth;
	KDNode *parent;
#endif
};

class KDTree
{
public:
    KDTree(const std::vector<idx_vec3> &faces,const std::vector<glm::vec3> &vertices, uint32_t max_depth = 5, uint32_t min_elems = 3);

    std::vector<glm::vec3> vertices;
    std::vector<idx_vec3>  faces;
    AABB    bounds;
    KDNode  node;
};


#endif
