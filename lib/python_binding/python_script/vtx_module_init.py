import vtx_python_api.Core
import vtx_python_api.Command

def addCommand(p_funcName, p_func):
    setattr(vtx_python_api.Command, p_funcName, p_func)

def addSysPath(p_path):
    import sys 
    sys.path.append( p_path )
    

vtx_python_api.Core.addCommand = addCommand
vtx_python_api.Core.addSysPath = addSysPath