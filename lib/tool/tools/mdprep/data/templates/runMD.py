import argparse
import subprocess
import time
import re
import io
import os
from pathlib import Path

###############################################################################################################
# ______  ___ _____ ___   ______ _____ _   _ _____ _____ _____ _____ _   _                                    #
# |  _  \/ _ \_   _/ _ \  |  _  \_   _| | | |_   _/  ___|_   _|  _  | \ | |                                   #
# | | | / /_\ \| |/ /_\ \ | | | | | | | | | | | | \ `--.  | | | | | |  \| |                                   #
# | | | |  _  || ||  _  | | | | | | | | | | | | |  `--. \ | | | | | | . ` |                                   #
# | |/ /| | | || || | | | | |/ / _| |_\ \_/ /_| |_/\__/ /_| |_\ \_/ / |\  |                                   #
# |___/ \_| |_/\_/\_| |_/ |___/  \___/ \___/ \___/\____/ \___/ \___/\_| \_/                                   #
#                                                                                                             #
###############################################################################################################


fileStem = "${fileStem}" # templated variable
minimTprInput = "em.tpr"
nvtMdpInput = "nvt.mdp"
nptMdpInput = "npt.mdp"
prodMdpInput = "prod.mdp"
prodTop = "topol.top"
posresTop = "posres.top"
GpuAvailable = False
args = None # meant to hold kv of user argument
gmxCmdAlias = "gmx"

class Job:
    preparationFailed = False
    runFailed = False

emJob = Job()
nvtJob = Job()
nptJob = Job()
prodJob = Job()
resultJob = Job()

taskDeclarationSize = 35 # Control the size of the task messages displayed (for the . filling feature)

###############################################################################################################
#  ____________ _____ _____  ___________ _   _______ _____  ______ _____ _   _ _____ _____ _____ _____ _   _  #
# | ___ \ ___ \  _  /  __ \|  ___|  _  \ | | | ___ \  ___| |  _  \_   _| | | |_   _/  ___|_   _|  _  | \ | |  #
# | |_/ / |_/ / | | | /  \/| |__ | | | | | | | |_/ / |__   | | | | | | | | | | | | \ `--.  | | | | | |  \| |  #
# |  __/|    /| | | | |    |  __|| | | | | | |    /|  __|  | | | | | | | | | | | |  `--. \ | | | | | | . ` |  #
# | |   | |\ \\ \_/ / \__/\| |___| |/ /| |_| | |\ \| |___  | |/ / _| |_\ \_/ /_| |_/\__/ /_| |_\ \_/ / |\  |  #
# \_|   \_| \_|\___/ \____/\____/|___/  \___/\_| \_\____/  |___/  \___/ \___/ \___/\____/ \___/ \___/\_| \_/  #
#                                                                                                             #
###############################################################################################################
                                                                                                          

def parseArgs():
    parser = argparse.ArgumentParser(
        prog="runMd.py",
        description= "VTX Molecular Dynamics Runner for Gromacs\nRun all MD simulations steps on system prepared by VTX",
    )
    parser.add_argument("--pass-gpu-check", action="store_true", default=False, dest="ignore_gpu_check", help="runMD.py will consider that a gpu is available.")
    parser.add_argument("-gpu_id", dest="gpu_id", default=None, help="Name of the GPU(s) to aim for. Will be forwarded to mdrun.")
    parser.add_argument("-gmx_path", dest="gmx_path", default=None, help="Path of the gmx executable. If not provided, will use the one in the path environment variable.")
    parser.add_argument("-gmx_lib", dest="gmx_lib", default=None, help="Path of the gmx top folder. If not provided, will use GMXLIB environment variable.")
    parser.add_argument("-nt", dest="nt", default=None , help="Number of thread to start. Usually correlate with the number of CPU core used. Will be forwarded to mdrun.")
    return parser.parse_args()
    
def submitCmd(cmdStr: str, stdoutFilePath : Path=None):
    rc = subprocess.run(cmdStr, shell=True, stdout= subprocess.PIPE, stderr= subprocess.STDOUT, capture_output=False)
    if stdoutFilePath is not None:
        stdoutFilePath.write_bytes(rc.stdout)
    return rc

def doesPgmExists(command : str):
    try:
        rc = submitCmd(command)
        if rc.returncode == 0:
            return True
    except Exception:
        return False 
    return False

def getResourceString():
    outStr = ("-nt %s" % args.nt if args.nt is not None and args.nt != "" else "")
    if GpuAvailable:
        if outStr != "":
            outStr += " "
        outStr = "-nb gpu %s" % ("-gpu_id %s" % args.gpu_id if args.gpu_id is not None else "") 
        
    return outStr

def isGmxInPath():
    return doesPgmExists("gmx")
  
# Coslty operation. Storing the result is a good idea  
def isGpuAvailable():
    return args.ignore_gpu_check or doesPgmExists("nvidia-smi")
    
def getMdRootDir():
    return Path(__file__).parent.resolve()

# Shouldn't prevent jobs to start but used to display a warning for the user
def isGmxlibDefined():
    return "GMXLIB" in os.environ #TODO

# Check current system to make sure if gromacs can run and how. Then return True if the system can run    
def checkSystem():
    if args.gmx_path is not None and Path(args.gmx_path).is_file() == False:
        print("Error : gmx binary not found in <%s>. Please check if the provided path leads to it." % args.gmx_path)
        return False
        
    if args.gmx_path is not None:
        global gmxCmdAlias 
        gmxCmdAlias = args.gmx_path
        
    if args.gmx_path is None and isGmxInPath() == False:
        print("Error : gmx binary not found in PATH. Ask help from the system manager.")
        return False
      
    if args.gmx_lib is not None and Path(args.gmx_lib).is_dir() == False:
        print("Error : Provided forcefield directory <%s> not found." % args.gmx_lib)
        return False
        
    if args.gmx_lib is not None :
        os.environ["GMXLIB"] = args.gmx_lib
        
    if args.gmx_lib is None and isGmxlibDefined() == False:
        print("Warning : Environment variable GMXLIB not defined. It is not mandatory to run gromacs. However if the MD fails to start, it would be worth investigating this first.")
        
    global GpuAvailable
    GpuAvailable = isGpuAvailable()
    if args.ignore_gpu_check:
        print("GPU check ignored. Trying to run gromacs in GPU mode.")  
    elif GpuAvailable:
        print("Graphics Processing Unit found. Running gromacs in GPU mode.")
    else:
        print("No Graphics Processing Unit found. Running gromacs in CPU mode.")
        
    if args.gpu_id is not None:
        print("Asking Gromacs to run on <%s>." % args.gpu_id)
    if args.nt is not None:
        print("Asking Gromacs to make at most %s threads." % args.nt)
    
    return True

def checkFolder():
    errMsg = "Error : Required file <%s> not found in folder. It means you're MD folder is incomplete. Doing preparation steps through VTX again may fix the problem." 
    if (getMdRootDir() / minimTprInput).is_file() == False:
        print(errMsg % minimTprInput)
        return False
    if (getMdRootDir() / nvtMdpInput).is_file() == False:
        print(errMsg % nvtMdpInput)
        return False
    if (getMdRootDir() / nptMdpInput).is_file() == False:
        print(errMsg % nptMdpInput)
        return False
    if (getMdRootDir() / prodMdpInput).is_file() == False:
        print(errMsg % prodMdpInput)
        return False
    if (getMdRootDir() / posresTop).is_file() == False:
        print(errMsg % prodMdpInput)
        return False
    if (getMdRootDir() / prodTop).is_file() == False:
        print(errMsg % prodMdpInput)
        return False
    return True
    
# print the formated task message
def printTaskDeclarationMessage(msg:str):
    msg += " "
    while len(msg) < taskDeclarationSize:
        msg += "."
    msg += " "
    
    print(msg, end="", flush=True)

def runJob(cmd: str, fileStem: str, jobFailedSignal: callable):
    logErrMsg = "Please refer to the %s and %s file to understand what went wrong."
    def printFailure(msg = None):
        print("failed with error : <%s>" % ("Too big to be printed" if len(msg)> 50 else msg ))
        print(logErrMsg % ("%s.log" % fileStem, "%s.txt" % fileStem))   
        
    try:
        rc = submitCmd(cmd, Path("%s.txt" % fileStem))
        if rc.returncode != 0:
            printFailure(rc.stdout)
            jobFailedSignal()
        else:
            print("done")
    except Exception as e:
        print()
        jobFailedSignal()
        if ("stdout" in dir(e)):
            printFailure(e.stdout if e.stdout != None else "")
        else:
            printFailure(repr(e))

def runMinimization():
    # Minimzation is the part that should be ready from the get-go. So we just need to start mdrun and wait
    cmdStr = "%s mdrun -v -deffnm em %s" % (gmxCmdAlias, getResourceString())
    printTaskDeclarationMessage("Starting Energy Minimization")
    
    def jobFailedSignal():
        emJob.runFailed = True
        
    runJob(cmdStr, "em", jobFailedSignal)
        
def isMinimizationOk():
    return (
        (not emJob.runFailed) 
        and (getMdRootDir() / "em.gro").is_file()
    )
    
def runNvtEquil():
    printTaskDeclarationMessage("Preparing NVT Equilibration")
    cmdStr = "%s grompp -f nvt.mdp -c em.gro -r em.gro -p %s -o nvt.tpr" % (gmxCmdAlias, posresTop)
    
    def jobFailedSignal():
        nvtJob.preparationFailed = True
    
    runJob(cmdStr, "nvt_prep", jobFailedSignal)
    
    if (nvtJob.preparationFailed == True):
        return
    
    printTaskDeclarationMessage("Starting NVT Equilibration")
    cmdStr = "%s mdrun -deffnm nvt %s" % (gmxCmdAlias, getResourceString())
    
    def jobFailedSignal():
        nvtJob.runFailed = True
    
    runJob(cmdStr, "nvt", jobFailedSignal)
    
def isNvtEquilOk():
    return (
        (nvtJob.preparationFailed == False and nvtJob.runFailed == False)
        and (getMdRootDir() / "nvt.gro").is_file()
    )
    
def runNptEquil():
    printTaskDeclarationMessage("Preparing NPT Equilibration")
    cmdStr = "%s grompp -f npt.mdp -c nvt.gro -t nvt.cpt -r nvt.gro -p %s -o npt.tpr" % (gmxCmdAlias, posresTop)
    
    def jobFailedSignal():
        nptJob.preparationFailed = True
    
    runJob(cmdStr, "npt_prep", jobFailedSignal)
    
    if (nvtJob.preparationFailed == True):
        return
    
    printTaskDeclarationMessage("Starting NPT Equilibration")
    cmdStr = "%s mdrun -deffnm npt %s" % (gmxCmdAlias, getResourceString())
    
    def jobFailedSignal():
        nptJob.runFailed = True
    
    runJob(cmdStr, "npt", jobFailedSignal)
    
def isNptEquilOk():
    return (
        (nptJob.preparationFailed == False and nptJob.runFailed == False)
        and (getMdRootDir() / "npt.gro").is_file()
    )
    
def runProd():
    printTaskDeclarationMessage("Preparing Production run")
    cmdStr = "%s grompp -f prod.mdp -c npt.gro -p %s -o prod.tpr" % (gmxCmdAlias, prodTop)
    
    def jobFailedSignal():
        prodJob.preparationFailed = True
    
    runJob(cmdStr, "prod_prep", jobFailedSignal)
    
    if prodJob.preparationFailed :
        return
    
    if (nvtJob.preparationFailed == True):
        return
    
    printTaskDeclarationMessage("Starting Production run")
    cmdStr = "%s mdrun -deffnm prod %s" % (gmxCmdAlias, getResourceString())
    
    def jobFailedSignal():
        prodJob.runFailed = True
    
    runJob(cmdStr, "prod", jobFailedSignal)
    
def isProdOk():
    return (
        (prodJob.preparationFailed == False and prodJob.runFailed == False)
        and (getMdRootDir() / "prod.gro").is_file()
    )

# Run trjconv on the prod run results
def runTrjConv():
    """
    Tjrconv is different from other functions as it asks questions to the caller.
    The goal of this function is to not only call trjconv, but to answer its questions as well.
    For more flexibility, we will analyse the questions with regex in order to find the right answer for most systems.
    The first question is about which group of atom we should use for centering.
    The second question is to determine atoms that will be written in the output file
    """
    
    # Analyse the stdout of process in argument and check if the trjconv gromacs process is waiting for user input
    def waitForInputAndReturnOutput(proc):
        proc.stdout.seek(0, io.SEEK_END)
        s = str(proc.stdout.peek().decode("utf-8"));
        i = 0
        while not s.endswith("Select a group: "):
            if (i > 100000): 
                raise Exception("Waited to much for trjconv")
            time.sleep(0.1)
            i+=1
            proc.stdout.seek(0, io.SEEK_END)
            s += str(proc.stdout.peek().decode("utf-8"));
            
        return s
    
    def jobFailedSignal():
        resultJob.runFailed = True
        
    logErrMsg = "Please refer to the %s and %s file to understand what went wrong."
    _fileStem = "trjconv"
    def printFailure(msg = None):
        print("failed with error : <%s>" % ("Too big to be printed" if len(msg)> 100 else msg ))
        print(logErrMsg % ("%s.log" % _fileStem, "%s.txt" % _fileStem))   
    
    # Execute command do all tedious interactions with trjconv (Protein then System)
    def executeTrjconvCommand(command:str):
        try:
            # We start the process
            proc = subprocess.Popen(command, shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            
            # We wait for the output to be readable
            while not proc.stdout.readable():
                continue
            
            # We wait for gromacs to phrase its question
            protein_query = waitForInputAndReturnOutput(proc)
            groupListPattern = "\r?\n(Group(.|\r|\n)+?)Select a group:"
            centeringGroupList = re.search(groupListPattern, protein_query)
            if (centeringGroupList is None):
                raise Exception("Gromacs was not understood : Centering Group didn't match")
                
            centeringGroupList = centeringGroupList.group(0)
            
            proteinPattern = "Group +?(\\d+) *?\\( *Protein *\\)" # Aims to extract the number to give trjconv for the Protein answer
            proteinOptionMatch = re.search(proteinPattern, centeringGroupList)
            
            if (proteinOptionMatch is None):
                raise Exception("Gromacs was not understood : Protein option not found.")
            
            proteinOption = proteinOptionMatch.group(1)
            proteinOption += '\n'
            
            proc.stdin.write(proteinOption.encode())
            proc.stdin.flush() # If we don't do that, trjconv doesn't take it sometimes
            time.sleep(1) 
            
            write_query = waitForInputAndReturnOutput(proc)
            
            system_pattern = "Group +?(\\d+) *?\\( *System *\\)" # Aims to extract the number to give trjconv for the System answer
            systemOptionMatch = re.search(system_pattern, write_query)
            
            if (systemOptionMatch is None):
                raise Exception("Gromacs was not understood : System option not found.")
            
            systemOption = systemOptionMatch.group(1)
            systemOption += '\n'
            
            proc.stdin.write(systemOption.encode())
            proc.stdin.flush()
            proc.wait(10000)
            
            if proc.returncode != 0:
                printFailure(proc.stdout.read().decode("utf-8"))
                jobFailedSignal()
            else:
                print("done")
            return True
        except Exception as e:
            print()
            jobFailedSignal()
            if ("stdout" in dir(e)):
                printFailure(e.stdout if e.stdout != None else "")
            else:
                printFailure(repr(e))
                
        return False
    
    struct_cmd = "%s trjconv -f npt.gro -s npt.tpr -o %s.pdb -center -pbc mol -ur compact -conect" % (gmxCmdAlias, fileStem)
    traj_cmd = "%s trjconv -f prod.xtc -s prod.tpr -o %s_traj.xtc -center -pbc mol -ur compact" % (gmxCmdAlias, fileStem)
    
    
    printTaskDeclarationMessage("Writing structure to read")
    if (executeTrjconvCommand(struct_cmd) == False):
        return
        
    printTaskDeclarationMessage("Writing trajectory to load")
    if (executeTrjconvCommand(traj_cmd) == False):
        return
    
def isTrjconvOk():
    return resultJob.runFailed == False

# Main function of the script
# Will run minimzation, nvt equil, npt equil then production
# Files will have a standard name (i.e. name after the gromacs introduction tutorial)
def runMD():
    os.chdir(getMdRootDir()) # Make sure we are on the right directory before doing anything
    if checkSystem() == False or checkFolder() == False:
        print("VTX runMD.py failed to start MD. Please check error above for more details.")
        return
        
    runMinimization()
    if isMinimizationOk() == False:
        print("Minimization (1st step) failed.")
        return
    
    runNvtEquil()
    if isNvtEquilOk() == False:
        print("Nvt Equilibration (2nd step) failed.")
        return
    
    runNptEquil()
    if isNptEquilOk() == False:
        print("Npt Equilibration (3rd step) failed.")
        return
    
    runProd()
    if isProdOk() == False:
        print("Production (last step) failed.")
        return
        
    runTrjConv()
    if isTrjconvOk() == False:
        print("Result production failed. Your data is not lost. Please seek advice on how to exploit your raw results.")
        
    print("MD simulation for %s system finished successfully.\nTo visualize the trajectory, load the <%s.pdb> into VTX then the <%s_traj.xtc> (associate it with the %s)." % tuple(fileStem for i in range(4)))
    
    return
    
    
if __name__ == "__main__":
    args = parseArgs()
    runMD()