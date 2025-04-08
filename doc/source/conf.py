# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

import pathlib
import sys
from datetime import datetime

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, ROOT)

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = "douka"
copyright = "2025, Materials Modelling Lab, The University of Tokyo"
copyright = f"{datetime.now().date().year}, douka developers"
author = "WATANABE Aoi"
version = "latest"

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    "breathe",
    "sphinx_design",
    "sphinx-jsonschema",
    "sphinx.ext.autodoc",
    "sphinx.ext.imgconverter",
    "sphinx.ext.intersphinx",
]

templates_path = ["_templates"]
exclude_patterns = [
    "Thumbs.db",
    ".DS_Store",
]


# -- Breathe configuration ---------------------------------------------------
# https://breathe.readthedocs.io/en/latest/quickstart.html#configuration

breathe_projects = {
    "douka": ROOT / "doc" / "build" / "xml",
}
breathe_default_project = "douka"


# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = "sphinx_rtd_theme"
html_title = "douka documentation"
html_static_path = ["_static"]
html_logo = "_static/logo-light.png"
html_favicon = '_static/favicon.ico'

latex_documents = [
    ("index", "douka.tex", "User Guide", author, "howto", True),
]
latex_logo = "_static/logo.png"

intersphinx_mapping = {
}
