Building the Documentation
--------------------------

HTML documentation for the Cuddl project may be generated from the
reStructuredText source files in the project's ``doc`` subdirectory using the
`Sphinx <sphinx>` documentation tool.

These instructions describe how to set up a working Sphinx installation:

  `<https://www.codecraftsmen.org/sphinx-notes.html>`_

Once you have activated the virtual Python environment and installed the
required Sphinx extensions and system packages, the Cuddl documentation can
be built by typing the following command in the ``doc`` directory::

  make html

You may then open the resulting top-level HTML file in a web browser::

  _build/html/index.html
