import PyTX.Core
import PyTX.Command

def addCommand(p_funcName, p_func):
    setattr(PyTX.Command, p_funcName, p_func)

def addSysPath(p_path):
    import sys 
    sys.path.append( p_path )
    

PyTX.Core.addCommand = addCommand
PyTX.Core.addSysPath = addSysPath