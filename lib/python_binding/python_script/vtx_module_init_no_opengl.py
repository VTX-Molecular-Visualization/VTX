import vtx_python_bin_no_opengl.Core
import vtx_python_bin_no_opengl.Command

def addCommand(p_funcName, p_func):
    setattr(vtx_python_bin_no_opengl.Command, p_funcName, p_func)

def addSysPath(p_path):
    import sys 
    sys.path.append( p_path )
    

vtx_python_bin_no_opengl.Core.addCommand = addCommand
vtx_python_bin_no_opengl.Core.addSysPath = addSysPath