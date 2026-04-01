import vtx

def addCommand(p_funcName, p_func):
    setattr(vtx, p_funcName, p_func)

def addSysPath(p_path):
    import sys 
    sys.path.append( p_path )
    

vtx.addCommand = addCommand
vtx.addSysPath = addSysPath