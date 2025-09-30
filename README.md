# SoPaGrami (C++17)

This repo is a minimal, production-friendly implementation of the SoPaGrami algorithm for mining frequent subgraphs from a single large graph

## Layout
```
sopagrami/
├── cpp/
│   ├── CMakeLists.txt
│   ├── include/
│   │   ├── alg.hpp
│   │   └── common/timer.hpp
│   ├── src/
│   │   ├── alg.cpp
│   │   └── main.cpp


```

## C++ (CMake) quickstart
```bash
cd cpp
cmake -S . -B sopagrami -DCMAKE_BUILD_TYPE=Release
cmake --build sopagrami -j
```
# SoPaGraMi — Command-Line Usage
## CLI

```bash
./sopagrami/run <graph.lg> [tau] [directed] [sorted] [threads] [out_dir]
```

| Arg        | Meaning                           | Type | Allowed             | Default |
| ---------- | --------------------------------- | ---- | ------------------- | ------- |
| `tau`      | Support threshold (MNI)           | int  | ≥ 1                 | `2`     |
| `directed` | Treat graph as directed?          | int  | `1` = yes, `0` = no | `0`     |
| `sorted`   | SoGraMi seed ordering             | int  | `1` = on, `0` = off | `1`     |
| `threads`  | Number of worker threads (OpenMP) | int  | ≥ 0                 | `4`     |
| `out_dir1` | Directory to save outputs         | str  |                     |         |

Notes

* threads=0 ⇒ auto (uses omp_get_max_threads()).

* Keep sorted=1 to expand high-support seeds first (SoGraMi ordering).

## Examples

1. Minimal (undirected, all defaults)
```
./sopagrami/run data/graph.lg
```

2. Undirected with higher support (τ=15), auto threads
```
./sopagrami/run data/graph.lg 15 0 1 0
```

3. Directed, SoGraMi ordering on, 8 threads
```
./sopagrami/run data/graph.lg 10 1 1 8
```

4.  Directed, no sorting, single thread
```
./sopagrami/run data/graph.lg 5 1 0 1
```