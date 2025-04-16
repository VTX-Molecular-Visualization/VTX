import vtx_python_api.API as api

print(dir(api))
print("Creating a Vec3f but using it wrong")
c1 = api.Vec3f()
c1.x = 13.0
c1.y = 14.0
c1.zz = 15.0

