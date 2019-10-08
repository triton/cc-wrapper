#pragma once
#include <functional>
#include <memory>
#include <parallel_hashmap/phmap.h>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace std {

template <typename T>
struct equal_to<std::reference_wrapper<T>> {
  inline bool operator()(std::reference_wrapper<T> a,
                         std::reference_wrapper<T> b) {
    return std::equal_to<typename std::remove_const<T>::type>{}(a, b);
  }
};

template <typename T>
struct hash<std::reference_wrapper<T>> {
  inline std::size_t operator()(std::reference_wrapper<T> s) {
    return std::hash<typename std::remove_const<T>::type>{}(s);
  }
};

}  // namespace std

namespace cc_wrapper {
namespace graph {

namespace detail {

template <typename T, typename Enable = void>
struct KeyRef {
  using type = std::reference_wrapper<const T>;
};

template <typename T>
struct KeyRef<
    T, typename std::enable_if<std::is_trivially_copyable<T>::value>::type> {
  using type = T;
};

}  // namespace detail

template <typename Id>
class Graph {
public:
  using Key = typename detail::KeyRef<Id>::type;

  template <typename T>
  void insert(T &&id) {
    insertInternal(std::forward<T>(id));
  }

  void connect(const Id &from, const Id &to) {
    nodes.at(from)->to.insert(nodes.at(to).get());
  }

  template <typename T, typename U>
  void connectAndInsert(T &&from, U &&to) {
    auto &nodeFrom = *insertInternal(std::forward<T>(from))->second;
    auto &nodeTo = *insertInternal(std::forward<U>(to))->second;
    nodeFrom.to.insert(&nodeTo);
  }

  std::vector<Key> toposort() const {
    std::vector<Key> ret;
    using NodeRef = std::reference_wrapper<const Node>;
    std::vector<std::pair<NodeRef, typename decltype(Node::to)::const_iterator>>
        dfs;
    auto appendNext = [&dfs](const Node &next) {
      if (next.visited_at_all)
        return;
      if (next.visited_dfs)
        throw std::runtime_error("Cycle detected in topology");
      next.visited_dfs = true;
      dfs.push_back({next, next.to.begin()});
    };
    for (const auto &kv : nodes) {
      kv.second->visited_dfs = false;
      kv.second->visited_at_all = false;
    }
    for (const auto &kv : nodes) {
      const Node &start = *kv.second;
      appendNext(start);
      while (!dfs.empty()) {
        const Node &curr = dfs.back().first;
        auto &it = dfs.back().second;
        if (it == curr.to.end()) {
          dfs.pop_back();
          curr.visited_dfs = false;
          curr.visited_at_all = true;
          ret.push_back(curr.id);
        } else {
          const Node &next = **it;
          it++;
          appendNext(next);
        }
      };
    }
    return ret;
  }

private:
  struct Node {
    template <typename T>
    Node(T &&id) : id(std::forward<T>(id)) {}
    Node(const Node &) = delete;
    Node(Node &&) = delete;
    Node &operator=(const Node &) = delete;
    Node &operator=(Node &&) = delete;

    const Id id;
    phmap::flat_hash_set<Node *> to;
    mutable bool visited_dfs, visited_at_all;
  };

  using NodeMap = phmap::flat_hash_map<Key, std::unique_ptr<Node>>;
  using value_type = typename NodeMap::value_type;
  NodeMap nodes;

  typename NodeMap::iterator insertInternal(Id &&id) {
    auto it = nodes.find(id);
    if (it != nodes.end())
      return it;
    std::unique_ptr<Node> node(new Node(std::move(id)));
    auto ref = std::cref(node->id);
    return nodes.insert({ref, std::move(node)}).first;
  }

  typename NodeMap::iterator insertInternal(const Id &id) {
    auto it = nodes.find(id);
    if (it != nodes.end())
      return it;
    std::unique_ptr<Node> node(new Node(id));
    auto ref = std::cref(node->id);
    return nodes.insert({ref, std::move(node)}).first;
  }
};

}  // namespace graph
}  // namespace cc_wrapper
