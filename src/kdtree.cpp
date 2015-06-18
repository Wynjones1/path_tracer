#include "kdtree.h"

KDNode::KDNode(KDNode *parent, const std::vector<glm::vec3>  &vertices,
        const std::vector<idx_vec3> &faces,
        const AABB &bounds, uint32_t depth, uint32_t min_elems, uint8_t axis)
: leaf(false)
, split(0.0f)
, axis(axis)
#if KDTREE_DEBUG
, depth(depth)
, parent(parent)
#endif
{
    memset(children, 0x00, sizeof(children));
    if(depth == 0 || faces.size() < min_elems)
    {
        leaf = true;
        this->faces = faces;
    }
    else
    {
        //Calculate the split
        uint32_t count = 0;
        for(auto v : vertices)
        {
            if(bounds.min[axis] <= v[axis] && v[axis] < bounds.max[axis])
            {
                count += 1;
                split += v[axis];
            }
        }
        split /= count;

        //Seperate the triangles
        std::vector<idx_vec3> child_faces[2];
        AABB                  child_bounds[2] = {bounds, bounds};

        child_bounds[0].max[axis] = split;
        child_bounds[1].min[axis] = split;
#if KDTREE_DEBUG
        uint32_t in_count = 0;
#endif
        for(auto &f : faces)
        {
#if KDTREE_DEBUG
            bool  temp = false;
#endif
			float min_point = std::min(std::min(vertices[f[0]][axis], vertices[f[1]][axis]), vertices[f[2]][axis]);
			float max_point = std::max(std::max(vertices[f[0]][axis], vertices[f[1]][axis]), vertices[f[2]][axis]);
			if(min_point < split && bounds.min[axis] <= max_point)
            {
                child_faces[0].push_back(f);
#if KDTREE_DEBUG
                temp = true;
#endif
            }
			if(min_point < bounds.max[axis] && split <= max_point)
            {
                child_faces[1].push_back(f);
#if KDTREE_DEBUG
                temp = true;
#endif
            }
#if KDTREE_DEBUG
            if(temp)
			{
				in_count += 1;
			}
#endif
        }
#if KDTREE_DEBUG
        if(in_count != faces.size())
        {
            std::cerr << "Geometry lost" << std::endl;
            exit(0);
        }
#endif
        if(child_faces[0].size() && child_faces[1].size())
        {
            for(auto i = 0; i < 2; i++)
            {
                children[i] = new KDNode(this, vertices, child_faces[i], child_bounds[i], depth - 1, min_elems, (axis + 1) % 3);
            }
        }
        else
        {
            leaf = true;
            this->faces = faces;
        }
    }
}

/* Find a given triangle in the tree, point must be in the bounds
   (because a given triangle can be in more than one node.)       */
KDNode *KDNode::find(const idx_vec3 &face, const AABB &bounds, glm::vec3 point)
{
    if(leaf)
    {
        for(auto f : faces)
        {
            if(face == f)
            {
				std::cout << bounds << std::endl;
				std::cout << point.x << "," << point.y << "," << point.z << std::endl;
				if(bounds.contains(point))
				{
					return this;
				}
            }
        }
        return NULL;
    }
    else
    {
		AABB temp_bounds = bounds;
		temp_bounds.max[axis] = split;
        KDNode *t = children[0]->find(face, temp_bounds, point);
        if(t) return t;
		temp_bounds = bounds;
		temp_bounds.min[axis] = split;
        t = children[1]->find(face, temp_bounds, point);
        return t;
    }
}


KDTree::KDTree(const std::vector<idx_vec3> &faces,const std::vector<glm::vec3> &vertices, uint32_t max_depth, uint32_t min_elems)
: vertices(vertices)
, faces(faces)
, bounds(vertices)
, node(NULL, vertices, faces, bounds, max_depth, min_elems, 0)
{}

#if KDTREE_DEBUG
bool KDNode::is_ancestor(const KDNode *input) const
{
	const KDNode *test = this->parent;
	while(test)
	{
		if(input == test)
		{
			return true;
		}
		test = test->parent;
	}
	return false;
}
#endif