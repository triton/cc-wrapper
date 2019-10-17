#pragma once
#include <nonstd/string_view.hpp>
#include <string>
#include <utility>

namespace cc_wrapper {
namespace strings {

namespace detail {

inline size_t sizeViews() { return 0; }
template <typename View, typename... Views>
inline size_t sizeViews(View view, Views... views) {
  return view.size() + sizeViews(views...);
}

inline void appendViews(std::string &) {}
template <typename View, typename... Views>
inline void appendViews(std::string &dst, View view, Views... views) {
  dst.append(view.data(), view.size());
  appendViews(dst, views...);
}

template <typename... Views>
inline void appendViewsReserve(std::string &dst, Views... views) {
  dst.reserve(sizeViews(views...));
  appendViews(dst, views...);
}

}  // namespace detail

template <typename... Strs>
inline void append(std::string &dst, const Strs &... strs) {
  detail::appendViewsReserve(dst, nonstd::string_view(strs)...);
}

template <typename... Strs>
inline std::string cat(const Strs &... strs) {
  std::string ret;
  append(ret, strs...);
  return ret;
}
template <typename... Strs>
inline std::string cat(std::string &&in, const Strs &... strs) {
  std::string ret = std::move(in);
  append(ret, strs...);
  return ret;
}

inline bool startsWith(nonstd::string_view str, nonstd::string_view prefix) {
  return str.substr(0, prefix.size()) == prefix;
}

nonstd::string_view split(nonstd::string_view &str, char sep);
bool endsWith(nonstd::string_view str, nonstd::string_view suffix);

}  // namespace strings
}  // namespace cc_wrapper
