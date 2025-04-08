.. _installation:

############
Installation
############

*******************
System requirements
*******************

The current target platforms for ``douka`` are:

- macOS
- Ubuntu 20.04 / 22.04
- Fedora
- A64FX Platform

If you are using Windows, please use WSL2 or Docker.

******************************************
Install development tools and dependencies
******************************************

.. tab-set::

    .. tab-item:: macOS
        :sync: macOS

        .. code-block:: bash

          brew install \
            cmake \
            ninja \
            eigen \
            nlohmann-json

    .. tab-item:: Ubuntu22.04 / 24.04
        :sync: ubuntu

        .. code-block:: bash

          sudo apt install \
            cmake \
            ninja-build \
            libeigen3-dev \
            nlohmann-json3-dev

    .. tab-item:: Fedora
        :sync: fedora

        .. code-block:: bash

          sudo dnf install \
            cmake \
            eigen3-devel \
            nlohmann-json-devel

    .. tab-item:: A64FX Platform
        :sync: a64fx

        You need to make sure that the following operations are performed on the login node.
        If the ``spack`` is available, the following command can be used to setup the dependencies.
        If not, please install the dependencies manually.

        .. code-block:: bash

          spack load cmake
          spack load ninja
          spack load eigen
          spack load nlohmann-json
          spack load fujitsu-ssl2

        .. note::
          ``spack`` need to be loaded and also the user need to specify the version of the packages.


*****************************
Build the project from source
*****************************

Clone this repository and change the directory to the cloned repository.

.. code-block:: bash

  git clone https://github.com/MaterialsModellingLab/douka.git

  cd douka


And type the following command to build the project form source.

.. tab-set::

    .. tab-item:: macOS
        :sync: macOS

        .. code-block:: bash

          cmake --preset release
          cmake --build build/release

    .. tab-item:: Ubuntu22.04 / 24.04
        :sync: ubuntu

        .. code-block:: bash

          cmake --preset release
          cmake --build build/release

    .. tab-item:: Fedora
        :sync: fedora

        .. code-block:: bash

          cmake --preset release
          cmake --build build/release

    .. tab-item:: A64FX Platform
        :sync: a64fx

        Please note that the following commands should be executed on the login node for cross-compilation.

        .. code-block:: bash

          cmake --preset a64fx-release
          cmake --build build/a64fx-release

.. tip::
  If you want to build the project with the Intel MKL library, please build with the following command.
  Then the performance will be improved with multi-threaded computation.

  .. code-block:: bash

    cmake --preset release -DDOUKA_USE_MKL=ON
    cmake --build build/release

**********************
Install built binaries
**********************

After building the project, you can install the binaries by typing the following command.

.. tab-set::
  .. tab-item:: macOS
      :sync: macOS

      It will be installed under ``~/.local`` by default.

      .. code-block:: bash

        cmake --install build

        # You will see the version of the installed binaries.
        douka --version

  .. tab-item:: Ubuntu22.04 / 24.04
      :sync: ubuntu

      It will be installed under ``~/.local`` by default.

      .. code-block:: bash

        cmake --install build

        # You will see the version of the installed binaries.
        douka --version

  .. tab-item:: Fedora
      :sync: fedora

      It will be installed under ``~/.local`` by default.

      .. code-block:: bash

        cmake --install build

        # You will see the version of the installed binaries.
        douka --version

  .. tab-item:: A64FX Platform
      :sync: a64fx

      It will be installed under ``~/.local/opt/douka-a64fx-release`` by default.

      .. code-block:: bash

        cmake --install build

      .. note::
        You will need to login to the compute node to use the installed binaries.

        .. code-block:: bash

          # Add the environment variables to the shell
          source ~/.local/opt/douka-a64fx-release/env/douka/setup-env.sh

          # You will see the version of the installed binaries.
          douka --version
