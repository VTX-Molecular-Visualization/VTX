import vtx_python_bin_no_opengl.Command
import os

def openMultipleFile(p_folder, prefix):

    for filename in os.listdir(p_folder):
        fullpath = os.path.join(p_folder, filename)
        if os.path.isfile(fullpath) and filename.startswith(prefix) :
            print("Opening <%s> file." % str(fullpath) )
            vtx_python_bin_no_opengl.Command.openFile(fullpath)


print("Open all files in data starting with 7R7P")
openMultipleFile(os.path.dirname(__file__), "7R7P")
