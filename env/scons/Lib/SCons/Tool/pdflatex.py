"""SCons.Tool.pdflatex

Tool-specific initialization for pdflatex.

There normally shouldn't be any need to import this module directly.
It will usually be imported through the generic SCons.Tool.Tool()
selection method.

"""

#
# Copyright (c) 2001, 2002, 2003, 2004 The SCons Foundation
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
# KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

__revision__ = "src\engine\SCons\Tool\pdflatex.py 0.96 2005/11/07 20:52:44 chenli"

import SCons.Action
import SCons.Defaults
import SCons.Util
import SCons.Tool.tex

PDFLaTeXAction = SCons.Action.Action('$PDFLATEXCOM', '$PDFLATEXCOMSTR')

def PDFLaTeXAuxFunction(target = None, source= None, env=None):
    SCons.Tool.tex.InternalLaTeXAuxAction( PDFLaTeXAction, target, source, env )

PDFLaTeXAuxAction = SCons.Action.Action(PDFLaTeXAuxFunction, strfunction=None)

def generate(env):
    """Add Builders and construction variables for pdflatex to an Environment."""
    try:
        bld = env['BUILDERS']['PDF']
    except KeyError:
        bld = SCons.Defaults.PDF()
        env['BUILDERS']['PDF'] = bld

    bld.add_action('.ltx', PDFLaTeXAuxAction)
    bld.add_action('.latex', PDFLaTeXAuxAction)

    env['PDFLATEX']      = 'pdflatex'
    env['PDFLATEXFLAGS'] = SCons.Util.CLVar('')
    env['PDFLATEXCOM']   = '$PDFLATEX $PDFLATEXFLAGS $SOURCE'
    env['LATEXRETRIES']  = 3

def exists(env):
    return env.Detect('pdflatex')