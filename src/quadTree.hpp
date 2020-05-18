#pragma once
#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include "Utility/vec.hpp"
#include <array>
#include <memory>
#include <tuple>
#include <vector>

///////////////////////////////////////////////////////////////////////////
/// Use the shared mini namespace
using namespace mini;

/////////////////////////////////////////////////////////////////////////
/// \class  QuadTree
/// \brief  A recursive structure, partitioning objects in 2D space.
/// \tparam T   The type of object to hold.
template <typename T> class QuadTree {
    public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Default Destructor.
    ~QuadTree() noexcept = default;
    //////////////////////////////////////////////////////////////////////
    /// \brief  Default copy constructor.
    QuadTree(const QuadTree& o) = default;
    //////////////////////////////////////////////////////////////////////
    /// \brief  Default move constructor.
    QuadTree(QuadTree&& o) noexcept = default;
    //////////////////////////////////////////////////////////////////////
    /// \brief  Default constructor.
    QuadTree() noexcept = default;
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Custom Constructor
    /// \param  pos     the position of this tree in 2D space.
    /// \param  scale   the scale of this tree in 2D space.
    /// \param  level   the level of this tree in its hierarchy.
    /// \param  parent  pointer to this tree's parent.
    QuadTree(
        const vec2& pos, const vec2& scale, const int level = 0,
        QuadTree<T>* parent = nullptr) noexcept
        : m_pos(pos), m_scale(scale), m_level(level), m_parent(parent){};

    //////////////////////////////////////////////////////////////////////
    /// \brief  Default copy-assignment operator.
    QuadTree& operator=(const QuadTree& p) = default;
    //////////////////////////////////////////////////////////////////////
    /// \brief  Default move-assignment operator.
    QuadTree& operator=(QuadTree&& p) noexcept = default;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Insert an object into this tree, directly or into its children.
    /// \param  obj     the object to insert.
    /// \param  pos     the position of this object in 2D space.
    /// \param  scale   the scale of this object in 2D space.
    void insert(const T& obj, const vec2& pos, const vec2& scale) {
        if (hasSubtree()) {
            for (const auto& index : getChildIndex(pos, scale))
                m_children[index]->insert(obj, pos, scale);
        } else {
            m_objects.emplace_back(obj, pos, scale);

            // Check if we should split
            if (m_objects.size() > 10 && m_level < 5 && !hasSubtree())
                split();
        }
    }
    void clear() {
        m_hasChildren = false;
        m_objects.clear();
        for (auto& childNode : m_children)
            if (childNode)
                childNode->clear();
    }
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Search this tree for all objects within a particular bounds.
    /// \param  pos     the position of the bounds.
    /// \param  scale   the scale of the bounds.
    /// \return vector of all objects overlapping with this bounds.
    [[nodiscard]] std::vector<T>
    search(const vec2& pos, const vec2& scale) const {
        // Copy all objects in this node
        std::vector<T> overlappingObjects;
        overlappingObjects.reserve(m_objects.size());
        std::transform(
            m_objects.cbegin(), m_objects.cend(),
            std::back_inserter(overlappingObjects),
            [](const auto& obj) { return std::get<0>(obj); });

        // Copy all objects in children nodes
        if (hasSubtree()) {
            for (const auto& index : getChildIndex(pos, scale)) {
                auto newObjs = m_children[index]->search(pos, scale);
                overlappingObjects.insert(
                    overlappingObjects.end(),
                    std::make_move_iterator(newObjs.begin()),
                    std::make_move_iterator(newObjs.end()));
            }
        }
        return overlappingObjects;
    }

    private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Check if this tree has children nodes.
    /// \return true if this tree has children nodes, false otherwise.
    [[nodiscard]] bool hasSubtree() const noexcept { return m_hasChildren; }
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Split this tree into subtrees, migrating its children.
    void split() {
        // Create 4 child nodes once
        if (!m_children[0])
            std::generate(m_children.begin(), m_children.end(), [] {
                return std::make_shared<QuadTree<T>>();
            });

        // Populate child nodes
        const vec2 childScale = m_scale / 2.0F;
        constexpr vec2 childDir[4] = { vec2(1), vec2(1, -1), vec2(-1),
                                       vec2(-1, 1) };
        for (auto x = 0; x < 4; ++x) {
            *m_children[x] = QuadTree(
                m_pos + (childDir[x] * childScale), childScale, m_level + 1,
                this);
        }

        // Migrate objects into child nodes
        for (const auto& [objT, objPos, objScale] : m_objects) {
            for (const auto& index : getChildIndex(objPos, objScale))
                m_children[index]->insert(objT, objPos, objScale);
        }
        m_objects.clear();
        m_hasChildren = true;
    }
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Find all children that could contain the input bounds.
    /// \param  pos     the position of the bounds.
    /// \param  scale   the scale of the bounds.
    /// \return a set of all applicable children containing the supplied bounds.
    [[nodiscard]] std::vector<int>
    getChildIndex(const vec2& pos, const vec2& scale) const {
        constexpr auto BoxVsBox = [](const vec2& posA, const vec2& sclA,
                                     const vec2& posB,
                                     const vec2& sclB) noexcept {
            const bool x = std::abs(posA.data()[0] - posB.data()[0]) <=
                           (sclA.data()[0] + sclB.data()[0]);
            const bool y = std::abs(posA.data()[1] - posB.data()[1]) <=
                           (sclA.data()[1] + sclB.data()[1]);
            return x && y;
        };

        std::vector<int> indices;
        indices.reserve(4);
        for (auto index = 0; index < 4; ++index)
            if (BoxVsBox(
                    m_children[index]->m_pos, m_children[index]->m_scale, pos,
                    scale))
                indices.emplace_back(index);
        return indices;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// Private Members
    std::array<std::shared_ptr<QuadTree<T>>, 4> m_children; ///< Child trees.
    std::vector<std::tuple<T, vec2, vec2>>
        m_objects;                   ///< Objects owned by this tree.
    vec2 m_pos;                      ///< Position of this tree.
    vec2 m_scale;                    ///< Scale of this tree.
    int m_level = 0;                 ///< Level of this tree.
    bool m_hasChildren = false;      ///< Indicates if this tree split.
    QuadTree<T>* m_parent = nullptr; ///< Parent tree in hierarchy.
};

#endif // QUADTREE_HPP