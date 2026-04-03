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
           print( "Welcome to VTX python helper.")
           print("You can have a very detailed list of functions and classes and what they do by running help(vtx) or a minimalistic list of classes and functions by running dir(vtx).")
           print("Use help([symbol]) to print information on a vtx class of function.")
           print("E.g. : help(vtx.runScript)")
        else:
            self._original(*args, **kwargs)

    def __repr__(self):
        return repr(self._original)

# Replace globally
builtins.help = CustomHelper(builtins.help)

vtx.addCommand = addCommand
vtx.addSysPath = addSysPath