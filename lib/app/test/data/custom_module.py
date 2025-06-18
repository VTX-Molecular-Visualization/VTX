class PythonTestClass:
    """Test class for PyTX Wrapper"""

    def __init__(self):
        self.value = 10

    def print(self):
        print(self.value)

    def get(self):
        return self.value
    
    def set(self, newValue):
        self.value = newValue

    def add(self, newValue):
        self.value += newValue
        return self.value

def generatePythonTestClass():
    return PythonTestClass()


def testStr(str="default"):
    return "call testStr with " + str + " arg"
