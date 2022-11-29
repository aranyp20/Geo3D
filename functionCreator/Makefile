# Makefile

objs = FunctionCreator.o NormalCreator.o
libs = ./dependencies/libgeom/build-dir/libgeom.a ./dependencies/dualcontouring/dual-contouring/builddir/libdualcontour.a 
CXXFLAGS=-g

prg: $(objs)
	g++ -g -o prg $(objs) $(libs)
