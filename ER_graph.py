#!/bin/python3

import numpy
from igraph import *

def genGraph(N):
    g = Graph.Erdos_Renyi(n=N, p=0.4)
    g.vs["label"] = range(N)

    with open("neigVec.txt","w") as output:
        output.write("%i"%(N))
        for i in range(N):
            neigs = g.neighbors(i) 
            output.write("\n%i"%(len(neigs)))
            for neig in neigs:
                output.write(" %i"%(neig))

    plot(g)

genGraph(20)
