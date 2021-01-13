
Conventions
^^^^^^^^^^^

Indices start at zero
"""""""""""""""""""""

Regarding index conventions, zero is superior to one:

* `Edsger W. Dijkstra - Why numbering should start at zero <http://www.cs.utexas.edu/users/EWD/transcriptions/EWD08xx/EWD831.html>`_
* `Stack exchange discussion <https://softwareengineering.stackexchange.com/q/110804/149159>`_.

Intervals are [semi-open)
"""""""""""""""""""""""""

[Semi-open) intervals allow to represent positions between elements (there are ``n+1`` positions in a range of size ``n``).

Relation with the SIDS
""""""""""""""""""""""

The SIDS does not adopt these conventions: CGNS indices start at one and intervals are closed. **cpp_cgns** only uses these conventions when forced to (e.g. with :cpp:`cgns::factory::newElementRange`). Otherwise, the C/STL conventions are maintained.

