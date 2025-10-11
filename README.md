# SoPaGrami (Unofficial)


A minimal, production-friendly unofficial implementation of **SoPaGrami** for mining frequent (MNI) subgraphs in a single large labeled graph.

- Fast seed ordering (SoGraMi)
- Directed & undirected graphs
- OpenMP parallelism
- Optional export of per-pattern *image sets* (graph node IDs that realize each pattern vertex) and *sample embeddings*
- Optional Python bindings via `pybind11`/`scikit-build`

---

## Repo Layout
sopagrami/
root/
├─ include/
├─ src/
│  ├─ sopagrami/          ← Python package lives here
│  │  ├─ __init__.py
│  │  └─ _version.py
│  ├─ alg.cpp             ← C++ core sources
│  ├─ bindings.cpp        ← pybind11 module
│  └─ main.cpp            ← (CLI, optional)
├─ CMakeLists.txt
├─ pyproject.toml
└─ README.md


---

## C++ quickstart (CMake)

> Requires: CMake ≥ 3.15, a C++17 compiler. OpenMP is optional but recommended.

```bash
cd cpp
cmake -S . -B sopagrami \
  -DCMAKE_BUILD_TYPE=Release \
  -DSOPAGRAMI_BUILD_PYTHON=OFF
cmake --build sopagrami -j

```
If OpenMP isn’t auto-detected, add flags manually:

```bash
cmake -S . -B sopagrami \
  -DCMAKE_BUILD_TYPE=Release \
  -DSOPAGRAMI_BUILD_PYTHON=OFF \
  -DCMAKE_CXX_FLAGS="-fopenmp" \
  -DCMAKE_EXE_LINKER_FLAGS="-fopenmp"

```
The CLI binary will be at cpp/sopagrami/run.
# SoPaGraMi — Command-Line Usage
## CLI

```bash
./sopagrami/run <graph.lg> [tau] [directed] [sorted] [threads] [out_dir] [dump_images] [images_cap] [dump_emb] [emb_limit]

```
| Arg           | Meaning                          | Type | Allowed            | Default |
| ------------- | -------------------------------- | ---- | ------------------ | ------- |
| `graph.lg`    | Input graph in `.lg` format      | str  | —                  | —       |
| `tau`         | MNI support threshold            | int  | ≥ 1                | `2`     |
| `directed`    | Treat graph as directed?         | int  | `1` yes / `0` no   | `0`     |
| `sorted`      | SoGraMi seed ordering            | int  | `1` on / `0` off   | `1`     |
| `threads`     | Worker threads                   | int  | `0` auto, else ≥ 1 | `4`     |
| `out_dir`     | Directory to save outputs        | str  | empty = no files   | `""`    |
| `dump_images` | Write per-vertex image CSVs      | int  | `1` yes / `0` no   | `0`     |
| `images_cap`  | Cap per-vertex image count (CSV) | int  | ≥ 1 (if enabled)   | `100`   |
| `dump_emb`    | Write sample embeddings CSV      | int  | `1` yes / `0` no   | `0`     |
| `emb_limit`   | Max embedding rows (CSV)         | int  | ≥ 1 (if enabled)   | `50`    |


Notes

* threads=0 ⇒ auto (uses omp_get_max_threads()).

* Keep sorted=1 to expand high-support seeds first (SoGraMi ordering).
* If ```out_dir``` is provided the miner writes:
    * index.tsv (catalog)
    * one .lg + .dot per pattern
    * optional *.images.csv and *.emb.csv if enabled

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
5. Undirected, sorted, 8 thread, save output in results
```
./sopagrami/run data/graph.lg 10 0 1 8 results/

```
6. Undirected, sorted, 8 thread, save output in results Dump image CSVs with up to 100 IDs per pattern vertex:
```
./sopagrami/run data/graph.lg 10 0 1 8 results/ 1 100

```
7. Undirected, sorted, 8 thread, save output in results, dump embeddings with a limit of 25 rows:
```
./sopagrami/run data/graph.lg 10 0 1 8 results/ 1 100 1 25

```

## Python bindings (optional)
Build a wheel for sopagrami (module sopagrami._core) using scikit-build + pybind11.
You can build the C++ CLI and the Python wheel independently.
The Python build does not install the CLI.

```bash
# Enable Python module build:
export CMAKE_ARGS="-DSOPAGRAMI_BUILD_PYTHON=ON"
python -m pip install -e . -v

```