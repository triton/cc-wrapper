# CC-Wrapper
Cleans up arguments to a stock gcc / clang / binutils toolchain
to add desired flags and make compiler output more deterministic. This is
written explicitly for use in the triton project.

## Building
```
./autogen.sh
./configure
make
```

## Testing
```
make check
```
