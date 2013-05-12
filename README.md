jump-point-search
===================

jps+ path searching

This is the astar path finding algorithm accelerated by jump-point-search known as jps
based on the papers of its author Daniel Harabor and Alban Grastien from NICTA and 
the Australian National University.

This algorithm is implemented with jps+ where "corner-cutting" movement is not allowed and
which is faster both compared with jps where the first paper described. Jps+ was a newer 
derivative search strategy demonstrated by the second paper about jump-point-search where 
only straight direction has forced neighbours and the searching stops whenever the goal has
been found while recursive jumping.

Both papers can be downloaded from http://users.cecs.anu.edu.au/~dharabor/publications.html.

The testing result from my implementation is acceptable despite it that it's only 5 to 12
times faster than the normal astar algorithm with a little disappointation.


