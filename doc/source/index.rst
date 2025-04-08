###################
douka documentation
###################

``douka`` is a universal platform of data assimilation for materials research.

- **Flexible**: ``douka`` is not designed for any specific material model. You can easily integrate it into your own material model into data assimilation.
- **High Performance**: ``douka`` is implemented in C++ to achieve high performance. To accelerate dta assimilation computation, it leverages both process and thread parallelization.
- **Portable**: ``douka`` relies solely on few header-only libraries, ensuring easy portability across various environments, including diverse HPC setups.

.. toctree::
   :maxdepth: 1
   :hidden:

   douka <self>

.. toctree::
   :maxdepth: 2
   :caption: Build & Install

   installation
   create-plugin


.. toctree::
   :caption: Command usage
   :maxdepth: 1

   usage

   usage-obsgen
   usage-init

   usage-predict
   usage-filter


.. toctree::
   :caption: Parameter file format
   :maxdepth: 1

   json-matrix-handle
   json-schema-type
