# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information
import sys
import os

project = 'FSMLang'
version = '1.46'
copyright = '2025, The Maintainers'
author = 'The Maintainers'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
	'sphinxcontrib.plantuml',
	'sphinxcontrib.svgbob',
	'rst2pdf.pdfbuilder',
	'breathe'
]

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

# -- Options for breathe output ----------------------------------------------
breathe_projects = {
	"pod_os": "./_build/doxygen_out/xml/"
}
breath_default_project = "FSMLang"

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_theme = 'agogo'
html_theme_options = {
	'pagewidth': '85%'
	, 'documentwidth': '75%'
	, 'sidebarwidth': '15%'
}
html_static_path = ['_static']
html_css_files = [
	 'fsmlangweb.css'
	, 'fsmlang.css'
]

# -- Options for PDF output --------------------------------------------------

# Grouping the document tree into PDF files. List of tuples
# (source start file, target name, title, author, options).
#
# If there is more than one author, separate them with \\.
# For example: r'Guido van Rossum\\Fred L. Drake, Jr., editor'
#
# The options element is a dictionary that lets you override
# this config per-document. For example:
#
# ('index', 'MyProject', 'My Project', 'Author Name', {'pdf_compressed': True})
#
# would mean that specific document would be compressed
# regardless of the global 'pdf_compressed' setting.

#pdf_documents = [
#    ('index', 'MyProject', 'My Project', 'Author Name'),
#]

# A comma-separated list of custom stylesheets. Example:
pdf_stylesheets = ['sphinx', 'a4', 'fsmlang']

# A list of folders to search for stylesheets. Example:
pdf_style_path = ['.', '_styles']

# Create a compressed PDF
# Use True/False or 1/0
# Example: compressed=True
# pdf_compressed = False

# A colon-separated list of folders to search for fonts. Example:
# pdf_font_path = ['/usr/share/fonts', '/usr/share/texmf-dist/fonts/']

# Language to be used for hyphenation support
pdf_language = "en_US"

# Mode for literal blocks wider than the frame. Can be
# overflow, shrink or truncate
pdf_fit_mode = "shrink"

# Section level that forces a break page.
# For example: 1 means top-level sections start in a new page
# 0 means disabled
pdf_break_level = 0

# When a section starts in a new page, force it to be 'even', 'odd',
# or just use 'any'
pdf_breakside = 'any'

# Insert footnotes where they are defined instead of
# at the end.
# pdf_inline_footnotes = True

# verbosity level. 0 1 or 2
# pdf_verbosity = 0

# If false, no index is generated.
pdf_use_index = True

# If false, no modindex is generated.
# pdf_use_modindex = True

# If false, no coverpage is generated.
pdf_use_coverpage = True

# Name of the cover page template to use
pdf_cover_template = 'sphinxcover.tmpl'

# Label to use as a prefix for the subtitle on the cover page
# subtitle_prefix = 'version'

# Documents to append as an appendix to all manuals.
# pdf_appendices = []

# Enable experimental feature to split table cells. Use it
# if you get "DelayedTable too big" errors
# pdf_splittables = False

# Set the default DPI for images
# pdf_default_dpi = 72

# Enable rst2pdf extension modules
# pdf_extensions = []

# Page template name for "regular" pages
# pdf_page_template = 'cutePage'

# Show Table Of Contents at the beginning?
# pdf_use_toc = True

# How many levels deep should the table of contents be?
pdf_toc_depth = 9999

# Add section number to section references
pdf_use_numbered_links = False

# Background images fitting mode
pdf_fit_background_mode = 'scale'

# Repeat table header on tables that cross a page boundary?
pdf_repeat_table_rows = True

# Enable smart quotes (1, 2 or 3) or disable by setting to 0
pdf_smartquotes = 0

def setup(sphinx):
	sys.path.insert(0, os.path.abspath('./_pygments'))
	from fsmlang import FSMLangLexer
	sphinx.add_lexer("fsmlang", FSMLangLexer)

