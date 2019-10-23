#include <nonstd/optional.hpp>
#include <nonstd/string_view.hpp>

#include <linker/args.hpp>

namespace cc_wrapper {
namespace linker {
namespace args {

// clang-format off
%%{
  machine libs;

  action rpath {
    libs.addRPath({p, static_cast<size_t>(pe-p)});
    fbreak;
  }
  action libpath {
    libs.addLibPath({p, static_cast<size_t>(pe-p)});
    fbreak;
  }
  action lib {
    if (!ignore_libs)
      libs.addLib({p, static_cast<size_t>(pe-p)});
    fbreak;
  }
  action file {
    libs.addFile({p, static_cast<size_t>(pe-p)});
    fbreak;
  }
  action rpath_next {
    if (++it != args.end())
      libs.addRPath(*it);
  }
  action libpath_next {
    if (++it != args.end())
      libs.addLibPath(*it);
  }
  action lib_next {
    if (++it != args.end() && !ignore_libs)
      libs.addLib(*it);
  }
  action skip_next { ++it; }

  ignored_ls = ( 'dynamic-linker' | ( 'plugin' '-opt'? ) ) %/skip_next;
  ignored_s = ( [hImozTe] | 'soname' | 'Map' ) %/skip_next;
  rpath_ls = 'rpath' %/rpath_next '=' any* >rpath;
  lib_s = 'l' %/lib_next any* @lib;
  lib_l = 'library' %/lib_next '=' any* >lib;
  libpath_s = 'L' %/libpath_next any* @libpath;
  libpath_l = 'library-path' %/libpath_next '=' any* >libpath;
  static_s = 'Bstatic' %/{ ignore_libs = true; };
  dynamic_s = 'Bdynamic' %/{ ignore_libs = false; };
  short = ( ignored_s | lib_s | libpath_s | static_s | dynamic_s );
  long_short = '-'? ( ignored_ls | rpath_ls );
  long = '-' ( lib_l | libpath_l );
  flag = '-' ( long | long_short | short );
  file = [^\-] @file;
  main := flag | file;

  write data;
}%%
// clang-format on

void parseLibs(state::Libs &libs, nonstd::span<const nonstd::string_view> args) {
  int cs;
  bool ignore_libs = false;
  for (auto it = args.begin(); it < args.end(); ++it) {
    const char *p = it->data();
    const char *pe = p + it->size();
    const char *eof = pe;
    nonstd::optional<nonstd::string_view> path;
    // clang-format off
    %% write init;
    %% write exec;
    // clang-format on
  }
}

}  // namespace args
}  // namespace linker
}  // namespace cc_wrapper
