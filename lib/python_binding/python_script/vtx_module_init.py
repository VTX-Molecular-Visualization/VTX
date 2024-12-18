import vtx_python_bin.Core
import vtx_python_bin.Command

def addCommand(p_funcName, p_func):
    setattr(vtx_python_bin.Command, p_funcName, p_func)

def addSysPath(p_path):
    import sys 
    sys.path.append( p_path )
    

vtx_python_bin.Core.addCommand = addCommand
vtx_python_bin.Core.addSysPath = addSysPath