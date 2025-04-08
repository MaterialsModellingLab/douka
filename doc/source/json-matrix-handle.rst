.. _json-matrix-handle:

####################
JSON array to Matrix
####################

*****************
Covariance Matrix
*****************

Based on its size, the JSON array for the covariance matrix is loaded from a JSON file, representing these three cases:

1. Zero Matrix
2. Full Matrix
3. Diagonal Matrix

Here, we will explain how to define the covariance matrix in JSON file.


Case 1: Zero matrix
===================

If the JSON parameter ``Q`` is defined as an empty array or absent in JSON file, then the zero matrix is automatically used as the covariance matrix.

.. code-block:: JSON

   "k": 3,
   "Q": []

The parameter ``Q`` defined as above will be converted to a matrix of size ``k`` x ``k`` as follows:

.. math::

   Q =
   \begin{pmatrix}
      0.0 & 0.0 & 0.0 \\
      0.0 & 0.0 & 0.0 \\
      0.0 & 0.0 & 0.0
   \end{pmatrix}


Case 2: Full matrix
===================

If the JSON parameter ``Q`` is a full matrix, it should be defined as a 1D array with ``k * k`` elements.

.. code-block:: JSON

   "k": 3,
   "Q": [
      1.0, 2.0, 3.0,
      4.0, 5.0, 6.0,
      7.0, 8.0, 9.0
   ]

Please note that newline characters do not alter the shape of the matrix here.
The parameter ``Q`` defined as above will be converted to a matrix of size ``k`` x ``k`` as follows:

.. math::

   Q =
   \begin{pmatrix}
      1.0 & 2.0 & 3.0 \\
      4.0 & 5.0 & 6.0 \\
      7.0 & 8.0 & 9.0
   \end{pmatrix}


Case 3: Diagonal matrix
=======================

If the JSON parameter ``Q`` is a diagonal matrix, it should be defined as a 1D array with ``k`` elements.

.. code-block:: JSON

   "k": 3,
   "Q": [1.0, 2.0, 3.0]

The parameter ``Q`` defined as above will be converted to a diagonal matrix of size ``k`` x ``k`` as follows:

.. math::

   Q =
   \begin{pmatrix}
      1.0 & 0.0 & 0.0 \\
      0.0 & 2.0 & 0.0 \\
      0.0 & 0.0 & 3.0
   \end{pmatrix}

.. note::
   The ``douka`` JSON loader automatically detects these two cases.
   An error is raised if the size of the given array is neither empty, ``k`` nor ``k`` * ``k``.


******************
Observation Matrix
******************

Observation matrix is used to converted the dimension of the state vector to the dimension of the observation vector.
That is,

.. math::

   y = H \cdot x

where

   - :math:`y` is the observation vector with dimension ``l``
   - :math:`x` is the state vector with dimension ``k``
   - :math:`H` is the observation matrix with dimension ``l`` x ``k``


So the JSON parameter ``H`` should be defined as a 1D array with ``l * k`` elements.

Case 1: Manual definition
=========================

.. code-block:: JSON

   "k": 3,
   "l": 2,
   "H": [
      1.0, 2.0, 3.0,
      4.0, 5.0, 6.0
   ]

The parameter ``H`` defined as above will be converted to a matrix of size ``l`` x ``k`` as follows:

.. math::
   H =
   \begin{pmatrix}
      1.0 & 2.0 & 3.0 \\
      4.0 & 5.0 & 6.0
   \end{pmatrix}


Case 2: Auto definition
=======================

.. code:: JSON

   "k": 3,
   "l": 2,
   "H": []

If the parameter ``H`` is defined as an empty array or absent in JSON file, then the identity matrix is automatically used as the observation matrix.
The parameter ``H`` defined as above will be converted to a matrix of size ``l`` x ``k`` as follows:

.. math::

   H =
   \begin{pmatrix}
      1.0 & 0.0 & 0.0 \\
      0.0 & 1.0 & 0.0
   \end{pmatrix}
