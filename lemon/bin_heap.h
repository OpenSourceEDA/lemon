/* -*- mode: C++; indent-tabs-mode: nil; -*-
 *
 * This file is a part of LEMON, a generic C++ optimization library.
 *
 * Copyright (C) 2003-2011
 * Egervary Jeno Kombinatorikus Optimalizalasi Kutatocsoport
 * (Egervary Research Group on Combinatorial Optimization, EGRES).
 *
 * Permission to use, modify and distribute this software is granted
 * provided that this copyright notice appears in all copies. For
 * precise terms see the accompanying LICENSE file.
 *
 * This software is provided "AS IS" with no warranty of any kind,
 * express or implied, and with no claim as to its suitability for any
 * purpose.
 *
 */

#ifndef LEMON_BIN_HEAP_H
#define LEMON_BIN_HEAP_H

///\ingroup auxdat
///\file
///\brief Binary Heap implementation.

#include <vector>
#include <utility>
#include <functional>

namespace lemon {

  ///\ingroup auxdat
  ///
  ///\brief A Binary Heap implementation.
  ///
  ///This class implements the \e binary \e heap data structure.
  ///
  ///A \e heap is a data structure for storing items with specified values
  ///called \e priorities in such a way that finding the item with minimum
  ///priority is efficient. \c CMP specifies the ordering of the priorities.
  ///In a heap one can change the priority of an item, add or erase an
  ///item, etc.
  ///
  ///\tparam PR Type of the priority of the items.
  ///\tparam IM A read and writable item map with int values, used internally
  ///to handle the cross references.
  ///\tparam CMP A functor class for the ordering of the priorities.
  ///The default is \c std::less<PR>.
  ///
  ///\sa FibHeap
  ///\sa Dijkstra
  template <typename PR, typename IM, typename CMP = std::less<PR> >
  class BinHeap {

  public:
    ///\e
    typedef IM ItemIntMap;
    ///\e
    typedef PR Prio;
    ///\e
    typedef typename ItemIntMap::Key Item;
    ///\e
    typedef std::pair<Item,Prio> Pair;
    ///\e
    typedef CMP Compare;

    /// \brief Type to represent the items states.
    ///
    /// Each Item element have a state associated to it. It may be "in heap",
    /// "pre heap" or "post heap". The latter two are indifferent from the
    /// heap's point of view, but may be useful to the user.
    ///
    /// The item-int map must be initialized in such way that it assigns
    /// \c PRE_HEAP (<tt>-1</tt>) to any element to be put in the heap.
    enum State {
      IN_HEAP = 0,    ///< = 0.
      PRE_HEAP = -1,  ///< = -1.
      POST_HEAP = -2  ///< = -2.
    };

  private:
    std::vector<Pair> _data;
    Compare _comp;
    ItemIntMap &_iim;

  public:
    /// \brief The constructor.
    ///
    /// The constructor.
    /// \param map should be given to the constructor, since it is used
    /// internally to handle the cross references. The value of the map
    /// must be \c PRE_HEAP (<tt>-1</tt>) for every item.
    explicit BinHeap(ItemIntMap &map) : _iim(map) {}

    /// \brief The constructor.
    ///
    /// The constructor.
    /// \param map should be given to the constructor, since it is used
    /// internally to handle the cross references. The value of the map
    /// should be PRE_HEAP (-1) for each element.
    ///
    /// \param comp The comparator function object.
    BinHeap(ItemIntMap &map, const Compare &comp)
      : _iim(map), _comp(comp) {}


    /// The number of items stored in the heap.
    ///
    /// \brief Returns the number of items stored in the heap.
    int size() const { return _data.size(); }

    /// \brief Checks if the heap stores no items.
    ///
    /// Returns \c true if and only if the heap stores no items.
    bool empty() const { return _data.empty(); }

    /// \brief Make empty this heap.
    ///
    /// Make empty this heap. It does not change the cross reference map.
    /// If you want to reuse what is not surely empty you should first clear
    /// the heap and after that you should set the cross reference map for
    /// each item to \c PRE_HEAP.
    void clear() {
      _data.clear();
    }

  private:
    static int parent(int i) { return (i-1)/2; }

    static int second_child(int i) { return 2*i+2; }
    bool less(const Pair &p1, const Pair &p2) const {
      return _comp(p1.second, p2.second);
    }

    int bubble_up(int hole, Pair p) {
      int par = parent(hole);
      while( hole>0 && less(p,_data[par]) ) {
        move(_data[par],hole);
        hole = par;
        par = parent(hole);
      }
      move(p, hole);
      return hole;
    }

    int bubble_down(int hole, Pair p, int length) {
      int child = second_child(hole);
      while(child < length) {
        if( less(_data[child-1], _data[child]) ) {
          --child;
        }
        if( !less(_data[child], p) )
          goto ok;
        move(_data[child], hole);
        hole = child;
        child = second_child(hole);
      }
      child--;
      if( child<length && less(_data[child], p) ) {
        move(_data[child], hole);
        hole=child;
      }
    ok:
      move(p, hole);
      return hole;
    }

    void move(const Pair &p, int i) {
      _data[i] = p;
      _iim.set(p.first, i);
    }

  public:
    /// \brief Insert a pair of item and priority into the heap.
    ///
    /// Adds \c p.first to the heap with priority \c p.second.
    /// \param p The pair to insert.
    void push(const Pair &p) {
      int n = _data.size();
      _data.resize(n+1);
      bubble_up(n, p);
    }

    /// \brief Insert an item into the heap with the given heap.
    ///
    /// Adds \c i to the heap with priority \c p.
    /// \param i The item to insert.
    /// \param p The priority of the item.
    void push(const Item &i, const Prio &p) { push(Pair(i,p)); }

    /// \brief Returns the item with minimum priority relative to \c Compare.
    ///
    /// This method returns the item with minimum priority relative to \c
    /// Compare.
    /// \pre The heap must be nonempty.
    Item top() const {
      return _data[0].first;
    }

    /// \brief Returns the minimum priority relative to \c Compare.
    ///
    /// It returns the minimum priority relative to \c Compare.
    /// \pre The heap must be nonempty.
    Prio prio() const {
      return _data[0].second;
    }

    /// \brief Deletes the item with minimum priority relative to \c Compare.
    ///
    /// This method deletes the item with minimum priority relative to \c
    /// Compare from the heap.
    /// \pre The heap must be non-empty.
    void pop() {
      int n = _data.size()-1;
      _iim.set(_data[0].first, POST_HEAP);
      if (n > 0) {
        bubble_down(0, _data[n], n);
      }
      _data.pop_back();
    }

    /// \brief Deletes \c i from the heap.
    ///
    /// This method deletes item \c i from the heap.
    /// \param i The item to erase.
    /// \pre The item should be in the heap.
    void erase(const Item &i) {
      int h = _iim[i];
      int n = _data.size()-1;
      _iim.set(_data[h].first, POST_HEAP);
      if( h < n ) {
        if ( bubble_up(h, _data[n]) == h) {
          bubble_down(h, _data[n], n);
        }
      }
      _data.pop_back();
    }


    /// \brief Returns the priority of \c i.
    ///
    /// This function returns the priority of item \c i.
    /// \param i The item.
    /// \pre \c i must be in the heap.
    Prio operator[](const Item &i) const {
      int idx = _iim[i];
      return _data[idx].second;
    }

    /// \brief \c i gets to the heap with priority \c p independently
    /// if \c i was already there.
    ///
    /// This method calls \ref push(\c i, \c p) if \c i is not stored
    /// in the heap and sets the priority of \c i to \c p otherwise.
    /// \param i The item.
    /// \param p The priority.
    void set(const Item &i, const Prio &p) {
      int idx = _iim[i];
      if( idx < 0 ) {
        push(i,p);
      }
      else if( _comp(p, _data[idx].second) ) {
        bubble_up(idx, Pair(i,p));
      }
      else {
        bubble_down(idx, Pair(i,p), _data.size());
      }
    }

    /// \brief Decreases the priority of \c i to \c p.
    ///
    /// This method decreases the priority of item \c i to \c p.
    /// \param i The item.
    /// \param p The priority.
    /// \pre \c i must be stored in the heap with priority at least \c
    /// p relative to \c Compare.
    void decrease(const Item &i, const Prio &p) {
      int idx = _iim[i];
      bubble_up(idx, Pair(i,p));
    }

    /// \brief Increases the priority of \c i to \c p.
    ///
    /// This method sets the priority of item \c i to \c p.
    /// \param i The item.
    /// \param p The priority.
    /// \pre \c i must be stored in the heap with priority at most \c
    /// p relative to \c Compare.
    void increase(const Item &i, const Prio &p) {
      int idx = _iim[i];
      bubble_down(idx, Pair(i,p), _data.size());
    }

    /// \brief Returns if \c item is in, has already been in, or has
    /// never been in the heap.
    ///
    /// This method returns PRE_HEAP if \c item has never been in the
    /// heap, IN_HEAP if it is in the heap at the moment, and POST_HEAP
    /// otherwise. In the latter case it is possible that \c item will
    /// get back to the heap again.
    /// \param i The item.
    State state(const Item &i) const {
      int s = _iim[i];
      if( s>=0 )
        s=0;
      return State(s);
    }

    /// \brief Sets the state of the \c item in the heap.
    ///
    /// Sets the state of the \c item in the heap. It can be used to
    /// manually clear the heap when it is important to achive the
    /// better time complexity.
    /// \param i The item.
    /// \param st The state. It should not be \c IN_HEAP.
    void state(const Item& i, State st) {
      switch (st) {
      case POST_HEAP:
      case PRE_HEAP:
        if (state(i) == IN_HEAP) {
          erase(i);
        }
        _iim[i] = st;
        break;
      case IN_HEAP:
        break;
      }
    }

    /// \brief Replaces an item in the heap.
    ///
    /// The \c i item is replaced with \c j item. The \c i item should
    /// be in the heap, while the \c j should be out of the heap. The
    /// \c i item will out of the heap and \c j will be in the heap
    /// with the same prioriority as prevoiusly the \c i item.
    void replace(const Item& i, const Item& j) {
      int idx = _iim[i];
      _iim.set(i, _iim[j]);
      _iim.set(j, idx);
      _data[idx].first = j;
    }

  }; // class BinHeap

} // namespace lemon

#endif // LEMON_BIN_HEAP_H
