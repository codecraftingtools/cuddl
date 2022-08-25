# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
import os
import sys
sys.path.insert(0, os.path.abspath('.'))
sys.path.insert(0, os.path.abspath('./kerneldoc'))

# Run Doxygen to generate C++ API xml files
from subprocess import call
call('doxygen')

# -- Project information -----------------------------------------------------

project = 'Cuddl'
copyright = '2022 Jeff Webb'
author = 'Jeff Webb'

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'kerneldoc',
    'sphinx.ext.githubpages',
    'sphinx.ext.intersphinx',
    'sphinx.ext.extlinks',
    'breathe', # For C++ API
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

# The name of the Pygments (syntax highlighting) style to use.
pygments_style = None

default_role = 'any'

# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_rtd_theme'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

# -- Extension configuration -------------------------------------------------
intersphinx_mapping = {'cc': ('https://www.codecraftsmen.org', None)}

extlinks = {
    'github-repo': ('https://github.com/codecraftingtools/tydl/%s',''),
    'repo': ('https://github.com/codecraftingtools/cuddl/blob/master/%s','')
}

kerneldoc_bin = ['perl', './kerneldoc/kernel-doc']
kerneldoc_srctree = '..'

breathe_projects = {"Cuddl": "doxyxml/"}
breathe_default_project = "Cuddl"
