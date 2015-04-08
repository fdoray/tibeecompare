/* Copyright (c) 2014 Francois Doray <francois.pierre-doray@polymtl.ca>
 *
 * This file is part of tibeecompare.
 *
 * tibeecompare is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tibeecompare is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tibeecompare.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _TIBEE_CONTAINERS_REDBLACKINTERVALTREE_HPP
#define _TIBEE_CONTAINERS_REDBLACKINTERVALTREE_HPP

#include <algorithm>
#include <assert.h>
#include <boost/utility.hpp>
#include <functional>
#include <stddef.h>
#include <vector>

#include "containers/Interval.hpp"

namespace tibee {
namespace containers {

// Implementation of a red-black interval tree.
// Intervals can be added to the tree with a value associated to each of them.
// @tparam T the type of the value associated with each interval.
template <typename T>
class RedBlackIntervalTree : boost::noncopyable {
 public:
  typedef std::pair<Interval, T> ElementPair;
  typedef std::function<void (const ElementPair&)> EnumerateCallback;

  // Constructor.
  RedBlackIntervalTree();

  // Destructor.
  ~RedBlackIntervalTree();

  // Move  assignment.
  RedBlackIntervalTree<T>& operator=(RedBlackIntervalTree<T>&& other);

  // Insert an element in the tree.
  // @param interval Interval of the element to insert.
  // @param value Value of the element to insert.
  void Insert(const Interval& interval, const T& value);

  // Enumerate all the intervals that intersect with
  // [interval.low(), interval.high()]. The elements are ordered by their
  // lower bound. When many elements have the same lower bound, they are
  // ordered from the longest to the shortest.
  // @param interval Interval from which the intersection is searched.
  // @param callback The callback that receives the found elements.
  void EnumerateIntersection(const Interval& interval,
                             const EnumerateCallback& callback) const;

  // @returns the number of elements in the tree.
  size_t size() const {
    return size_;
  }

 private:
  typedef bool Color;
  static const Color kRed = true;
  static const Color kBlack = false;

  // A node of the interval tree. All elements that start at the same lower
  // bound are contained in the same node.
  class Node {
   public:
    typedef std::vector<ElementPair> ElementVector;
    typedef typename ElementVector::reverse_iterator ElementIterator;

    // Constructor.
    // @param interval Interval of the element to insert in the node.
    // @param value Value of the element to insert in the node.
    Node(const Interval& interval, const T& value);

    // Destructor.
    ~Node();

    // Adds an element in the node.
    // @param interval Interval of the element to insert in the node.
    // @param value Value of the element to insert in the node.
    void AddElement(const Interval& interval, const T& value);

    // Begin iterator for a traversal of the elements of this node,
    // from the maximum to the minimum higer bound.
    // @returns begin iterator.
    ElementIterator ElementBegin() {
      return elements_.rbegin();
    }

    // End iterator for a traversal of the elements of this node,
    // from the maximum to the minimum higer bound.
    // @returns end iterator.
    ElementIterator ElementEnd() {
      return elements_.rend();
    }

    // Returns the key of the node, which is the lower bound common to all
    // the elements it contains.
    // @returns the key of the node.
    uint64_t Key() const;

    // @returns the lower bound common to all elements in this node.
    uint64_t NodeLow() const;

    // @returns the maximum higher bound of the elements in this node.
    uint64_t NodeMaxHigh() const;

    // @returns the color of the node.
    Color color() const {
      return color_;
    }

    // Set the color of the node.
    // @param color the new color of the node.
    void set_color(Color color) {
      color_ = color;
    }

    // @returns the left child.
    Node* left() const {
      return left_;
    }

    // Set the left child.
    // @param left the new left child.
    void set_left(Node* left) {
      left_ = left;
      UpdateSubtreeMaxHigh();
    }

    // @returns the right child.
    Node* right() const {
      return right_;
    }

    // Set the right child.
    // @param left the new right child.
    void set_right(Node* right) {
      right_ = right;
      UpdateSubtreeMaxHigh();
    }

    // @returns the maximum higher bound of the elements in this node and
    //    its children.
    uint64_t subtree_max_high() const {
      return subtree_max_high_;
    }

   private:
    // Predicate to sort elements by the higher bound of their intervals.
    struct ElementsSortPredicate {
      bool operator()(const ElementPair& left,
                      const ElementPair& right) const {
        return left.first.high() < right.first.high();
      }
    };

    void UpdateSubtreeMaxHigh();

    // Elements contained in the node. They all have the same lower bound.
    ElementVector elements_;

    // Color of the node.
    Color color_;
    
    // Left child.
    Node* left_;
    
    // Right child.
    Node* right_;

    // Maximum higher bound of the elements in this node and its children.
    uint64_t subtree_max_high_;
  };

  Node* InsertInternal(Node* h, const Interval& interval, const T& value);
  void EnumerateIntersectionInternal(Node* h,
                                     const Interval& interval,
                                     const EnumerateCallback& callback) const;

  Node* RotateLeft(Node* h);
  Node* RotateRight(Node* h);
  void FlipColors(Node* h);
  bool IsRed(Node* node) const;

  // Root of the interval tree.
  Node* root_;

  // Number of elements in the tree.
  size_t size_;
};

template<typename T>
RedBlackIntervalTree<T>::RedBlackIntervalTree() : root_(NULL), size_(0) {
}

template<typename T>
RedBlackIntervalTree<T>::~RedBlackIntervalTree() {
  delete root_;
}

template <typename T>
RedBlackIntervalTree<T>& RedBlackIntervalTree<T>::operator=(RedBlackIntervalTree<T>&& other) {
  delete root_;
  root_ = other.root_;
  size_ = other.size_;
  other.root_ = nullptr;
  other.size_ = 0;
  return *this;
}

template<typename T>
void RedBlackIntervalTree<T>::Insert(const Interval& interval, const T& value) {
  root_ = InsertInternal(root_, interval, value);
  root_->set_color(kBlack);
  ++size_;
}

template<typename T>
void RedBlackIntervalTree<T>::EnumerateIntersection(
    const Interval& interval,
    const EnumerateCallback& callback) const {
  EnumerateIntersectionInternal(root_, interval, callback);
}

template<typename T>
typename RedBlackIntervalTree<T>::Node*
    RedBlackIntervalTree<T>::InsertInternal(
        typename RedBlackIntervalTree<T>::Node* h,
        const Interval& interval,
        const T& value) {
  if (h == NULL)
    return new Node(interval, value);

  if (IsRed(h->left()) && IsRed(h->right()))
    FlipColors(h);

  uint64_t key = interval.low();

  if (key < h->Key())
    h->set_left(InsertInternal(h->left(), interval, value));
  else if (key == h->Key())
    h->AddElement(interval, value);
  else /* if (key > h->Key()) */
    h->set_right(InsertInternal(h->right(), interval, value));

  if (!IsRed(h->left()) && IsRed(h->right()))
    h = RotateLeft(h);

  if (IsRed(h->left()) && IsRed(h->left()->left()))
    h = RotateRight(h);

  return h;
}

template<typename T>
void RedBlackIntervalTree<T>::EnumerateIntersectionInternal(
    Node* h,
    const Interval& interval,
    const EnumerateCallback& callback) const {
  if (h == NULL)
    return;

  // Search the left tree if its higher bound is after the lower bound of the
  // search interval.
  if (h->left() != NULL &&
      h->left()->subtree_max_high() > interval.low()) {
    EnumerateIntersectionInternal(h->left(), interval, callback);
  }

  // If the lower bound of this node is after the higher bound of the search
  // interval, no matching element can be found in this node or its right
  // child.
  if (h->NodeLow() > interval.high())
    return;

  // Search the elements of this node if their higher bound is after the
  // lower bound of the search interval. Because of the previous check, it
  // is guaranteed that all elements in this node start before the end of
  // the search interval.
  if (h->NodeMaxHigh() > interval.low()) {
    for (typename Node::ElementIterator it = h->ElementBegin();
         it != h->ElementEnd(); ++it) {
      if (it->first.high() < interval.low())
        break;
      callback(*it);
    }
  }

  // Search the right subtree.
  EnumerateIntersectionInternal(h->right(), interval, callback);
}

template<typename T>
typename RedBlackIntervalTree<T>::Node* RedBlackIntervalTree<T>::RotateLeft(
    typename RedBlackIntervalTree<T>::Node* h) {
  assert(h != NULL);
  assert(h->right() != NULL);

  Node* child = h->right();
  h->set_right(child->left());
  child->set_left(h);

  child->set_color(h->color());
  h->set_color(kRed);

  return child;
}

template<typename T>
typename RedBlackIntervalTree<T>::Node*
    RedBlackIntervalTree<T>::RotateRight(
        typename RedBlackIntervalTree<T>::Node* h) {
  assert(h != NULL);
  assert(h->left() != NULL);

  Node* child = h->left();
  h->set_left(child->right());
  child->set_right(h);

  child->set_color(h->color());
  h->set_color(kRed);

  return child;
}

template<typename T>
void RedBlackIntervalTree<T>::FlipColors(
    typename RedBlackIntervalTree<T>::Node* h) {
  assert(h != NULL);

  h->set_color(!h->color());

  if (h->left() != NULL)
    h->left()->set_color(!h->left()->color());

  if (h->right() != NULL)
    h->right()->set_color(!h->right()->color());
}

template<typename T>
bool RedBlackIntervalTree<T>::IsRed(
    typename RedBlackIntervalTree<T>::Node* node) const {
  return node != NULL && node->color() == kRed;
}

template<typename T>
RedBlackIntervalTree<T>::Node::Node(const Interval& interval, const T& value)
    : color_(kRed),
      left_(NULL),
      right_(NULL),
      subtree_max_high_(0) {
  AddElement(interval, value);
}

template<typename T>
RedBlackIntervalTree<T>::Node::~Node() {
  delete left_;
  delete right_;
}

template<typename T>
void RedBlackIntervalTree<T>::Node::AddElement(const Interval& interval,
                                          const T& value) {
  elements_.push_back(std::make_pair(interval, value));
  std::sort(elements_.begin(), elements_.end(), ElementsSortPredicate());
  UpdateSubtreeMaxHigh();
}

template<typename T>
uint64_t RedBlackIntervalTree<T>::Node::Key() const {
  assert(!elements_.empty());
  return elements_.at(0).first.low();
}

template<typename T>
uint64_t RedBlackIntervalTree<T>::Node::NodeLow() const {
  assert(!elements_.empty());
  return elements_.at(0).first.low();
}

template<typename T>
uint64_t RedBlackIntervalTree<T>::Node::NodeMaxHigh() const {
  assert(!elements_.empty());
  return elements_.at(elements_.size() - 1).first.high();
}

template<typename T>
void RedBlackIntervalTree<T>::Node::UpdateSubtreeMaxHigh() {
  subtree_max_high_ = std::max(
      std::max(left() == NULL ? 0 : left()->subtree_max_high(),
               right() == NULL ? 0 : right()->subtree_max_high()),
      NodeMaxHigh());
}

}  // namespace containers
}  // namespace tibee

#endif  // _TIBEE_CONTAINERS_REDBLACKINTERVALTREE_HPP
