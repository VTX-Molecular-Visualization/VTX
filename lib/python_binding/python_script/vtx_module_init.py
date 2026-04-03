import vtx

def addCommand(p_funcName, p_func):
    setattr(vtx, p_funcName, p_func)

def addSysPath(p_path):
    import sys 
    sys.path.append( p_path )

import functools
import builtins

class CustomHelper:
    def __init__(self, original):
        self._original = original
        # copy over metadata
        functools.update_wrapper(self, original)

    def __call__(self, *args, **kwargs):
        if not args and not kwargs:
            return """Welcome to VTX python helper.\nYou can have a very detailed list of functions and classes and what they do by running help(vtx).\n"""
        else:
            self._original(*args, **kwargs)

    def __repr__(self):
        return repr(self._original)

# Replace globally
builtins.help = CustomHelper(builtins.help)

vtx.addCommand = addCommand
vtx.addSysPath = addSysPath