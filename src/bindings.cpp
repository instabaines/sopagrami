// src/bindings.cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "alg.hpp"             


namespace py = pybind11;
using namespace algo;

PYBIND11_MODULE(_core, m) {
    m.doc() = "SoPaGraMi core";

    // ---- Pattern ----
    py::class_<Pattern::PEdge>(m, "PEdge")
        .def_readwrite("a", &Pattern::PEdge::a)
        .def_readwrite("b", &Pattern::PEdge::b)
        .def_readwrite("el", &Pattern::PEdge::el)
        .def_readwrite("dir", &Pattern::PEdge::dir);

    py::class_<Pattern>(m, "Pattern")
        .def(py::init<>())
        .def_readwrite("vlab", &Pattern::vlab)
        .def_readwrite("pedges", &Pattern::pedges)
        .def("key", &Pattern::key);

    // ---- Graph / params / output ----
    py::class_<DataGraph>(m, "DataGraph")
        .def(py::init<>())
        .def_readwrite("directed", &DataGraph::directed)
        .def_readonly("vlabels", &DataGraph::vlabels)
        .def("load_from_lg",
             [](DataGraph &g, const std::string& path, bool as_dir){
                 py::gil_scoped_release nogil;
                 g.load_from_lg(path, as_dir);
             },
             py::arg("path"), py::arg("directed") = false)
        .def("has_edge", &DataGraph::has_edge, py::arg("u"), py::arg("v"), py::arg("label"));

    py::class_<Params>(m, "Params")
        .def(py::init<>())
        .def_readwrite("tau", &Params::tau)
        .def_readwrite("directed", &Params::directed)
        .def_readwrite("sorted_seeds", &Params::sorted_seeds)
        .def_readwrite("num_threads", &Params::num_threads)
        .def_readwrite("compute_full_support", &Params::compute_full_support);

    py::class_<Found>(m, "Found")
        .def_readonly("pat", &Found::pat)
        .def_readonly("full_support", &Found::full_support);

    py::class_<Output>(m, "Output")
        .def_readonly("frequent_patterns", &Output::frequent_patterns);

    // ---- entry points ----
    m.def("run_sopagrami",
          [](const DataGraph& G, const Params& p) {
              py::gil_scoped_release nogil;
              return run_sopagrami(G, p);
          },
          py::arg("graph"), py::arg("params"),
          "Mine frequent subgraphs (static).");

        }