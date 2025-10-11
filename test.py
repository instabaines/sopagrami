import sopagrami as sp

G = sp.DataGraph()
G.load_from_lg("../datasets/citeseer.lg", directed=True)

p = sp.Params()
p.tau = 15
p.directed = False
p.sorted_seeds = True
p.num_threads = 8
p.compute_full_support = True

out = sp.run_sopagrami(G, p)

print("patterns:", len(out.frequent_patterns))
for f in out.frequent_patterns[:5]:
    print("k=", len(f.pat.vlab), "E=", len(f.pat.pedges),
          "full=", f.full_support, "key=", f.pat.key())


