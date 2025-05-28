import vtx_python_api.Command
import os

def openMultipleFile(p_folder, prefix):

    for filename in os.listdir(p_folder):
        fullpath = os.path.join(p_folder, filename)
        if os.path.isfile(fullpath) and filename.startswith(prefix) :
            vtx_python_api.Command.openFile(fullpath)


print("Open all files in data starting with 7R7P")
openMultipleFile("./data", "7R7P", gnangnangipgez$)