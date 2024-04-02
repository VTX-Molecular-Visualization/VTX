import argparse
import subprocess
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
GpuAvailable = False
args = None # meant to hold kv of user argument

class Job:
    preparationFailed = False
    runFailed = False

emJob = Job()
nvtJob = Job()
nptJob = Job()
prodJob = Job()


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
    parser.add_argument("-nt", dest="nt", default=None , help="Number of thread to start. Usually correlate with the number of CPU core used. Will be forwarded to mdrun.")
    return parser.parse_args()
    
def submitCmd(cmdStr: str, stdoutFilePath : Path=None):
    rc = subprocess.run(cmdStr, shell=True, stdout= subprocess.PIPE, stderr= subprocess.STDOUT, capture_output=False)
    if stdoutFilePath is not None:
        stdoutFilePath.write_bytes(rc.stdout)
    return rc

def doesPgmExists(command : str):
    try:
        submitCmd(command)
        return True
    except Exception:
        return False 

def getResourceString():
    outStr = ("-nt %s" % args.nt if args.nt is not None and args.nt != "" else "")
    if GpuAvailable:
        if outStr != "":
            outStr += " "
        outStr = "-nb gpu %s" % ("-gpu_id %s" % args.gpu_id if args.gpu_id is not None and args.gpu_id != "" else "") 
        
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
    if isGmxInPath() == False:
        print("Error : gmx binary not found in PATH. Ask help from the system manager.")
        return False
        
    if isGmxlibDefined() == False:
        print("Warning : Environment variable GMXLIB not defined. It is not mandatory to run gromacs. However if the MD fails to start, it would be worth investigating this first.")
        
    GpuAvailable = isGpuAvailable()
    if GpuAvailable:
        print("Graphics Processing Unit found. Running gromacs in GPU mode.")
        
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
    return True
    
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
    cmdStr = "gmx mdrun -v -deffnm em %s" % getResourceString()
    print("Starting Energy Minimization ... ", end="", flush=True)
    
    def jobFailedSignal():
        emJob.runFailed = True
        
    runJob(cmdStr, "em", jobFailedSignal)
        
def isMinimizationOk():
    return (
        (not emJob.runFailed) 
        and (getMdRootDir() / "em.gro").is_file()
    )
    
def runNvtEquil():
    print("Preparing NVT Equilibration run ...", flush=True, end="")
    cmdStr = "gmx grompp -f nvt.mdp -c em.gro -r em.gro -p topol.top -o nvt.tpr"
    
    def jobFailedSignal():
        nvtJob.preparationFailed = True
    
    runJob(cmdStr, "nvt_prep", jobFailedSignal)
    
    if (nvtJob.preparationFailed == True):
        return
    
    print("Starting NVT Equilibration run ...", flush=True, end="")
    cmdStr = "gmx mdrun -deffnm nvt %s" % getResourceString()
    
    def jobFailedSignal():
        nvtJob.runFailed = True
    
    runJob(cmdStr, "nvt", jobFailedSignal)
    
def isNvtEquilOk():
    return (
        (nvtJob.preparationFailed == False and nvtJob.runFailed == False)
        and (getMdRootDir() / "nvt.gro").is_file()
    )
    
def runNptEquil():
    return
    
def isNptEquilOk():
    return False
    
def runProd():
    return
    
def isProdOk():
    return False
    
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
    
    print("MD simulation for %s system finished successfully. To visualize the trajectory, load the [trajectory file] into VTX." % fileStem) # TODO [trajectory file]
    return
    
    
if __name__ == "__main__":
    args = parseArgs()
    runMD()