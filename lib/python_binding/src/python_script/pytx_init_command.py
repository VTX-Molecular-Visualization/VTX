import PyTX.Core
import PyTX.Command

def addCommand(p_funcName, p_func):
    setattr(PyTX.Command, p_funcName, p_func)

PyTX.Core.addCommand = addCommand