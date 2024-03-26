import subprocess
import os
from pathlib import Path

fileStem = "${fileStem}" # templated variable
minimTprInput = "em.tpr"
nvtMdpInput = "nvt.mdp"
nptMdpInput = "npt.mdp"
prodMdpInput = "prod.mdp"

GpuAvailable = False

def doesPgmExists(command : str):
    try:
        subprocess.check_output(command)
        return True
    except Exception:
        return False 
    

def isGmxInPath():
    return doesPgmExists("gmx")
    
def isGpuAvailable():
    return doesPgmExists("nvidia-smi")
    
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
        
    if isGmxInPath() == False:
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
    

def runMinimization():


    return

def isMinimizationOk():
    return False
    
def runNvtEquil():
    return
    
def isNvtEquilOk():
    return False
    
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
        
        
    runMinimization()
    if isMinimizationOk() == False:
        print("Minimization (1st step) failed. Please refer to minim.log for more details.")
        return
    
    runNvtEquil()
    if isNvtEquilOk() == False:
        print("Nvt Equilibration (2nd step) failed. Please refer to nvt.log for more details.")
        return
    
    runNptEquil()
    if isNptEquilOk() == False:
        print("Npt Equilibration (3rd step) failed. Please refer to npt.log for more details.")
        return
    
    runProd()
    if isProdOk() == False:
        print("Production (last step) failed. Please refer to prod.log for more details.")
        return
    
    print("MD simulation for %s system finished successfully. To visualize the trajectory, load the [trajectory file] into VTX." % fileStem) # TODO
    return
    
    
    
    
if __name__ == "__main__":
    runMD()