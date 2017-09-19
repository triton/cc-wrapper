#include <fmt/core.h>
#include <fmt/format.h>
#include <parallel_hashmap/phmap.h>
#include <re2/re2.h>

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	const phmap::flat_hash_map<std::string, std::string> m = {
		{ "a", "Hello" },
		{ "b", "World" },
	};
	fmt::print(stderr, "{} {}\n", m.at("a"), m.at("b"));
	return RE2::FullMatch("hello", "h.*o") ? 0 : 1;
}
