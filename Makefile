# CMD_RM CMD_MKDIR CMD_CP LIBCACHEDIR CONFIG FLAGS
CORE_MAJ=1
CORE_MIN=0
LIBS=

LIBFLAGS::=$(foreach o,$(LIBS),-I$(LIBCACHEDIR)/$(o)/include) $(foreach o,$(LIBS),-L$(LIBCACHEDIR)/$(o)/lib-$(CONFIG)) $(foreach o,$(shell parselibs libnames $(LIBS)),-l$(o))
DEPENDENCIESFLAGS::=-DDEPENDENCIES="$(shell parselibs dependenciesdefn $(LIBS))"

core: core.exe


o:
	$(CMD_MKDIR) $@


#CORE_SUB_HDRS=libraries/core_sub.hpp libraries/core_sub.ipp libraries/core_sub.using
#
#o/core_sub.o: libraries/core_sub.cpp $(CORE_SUB_HDRS) | o
#	gcc $(FLAGS) $(LIBFLAGS) -x c++ -c $< -o $@

CORE_HDRS=libraries/core.hpp libraries/core.ipp libraries/core.using $(CORE_SUB_HDRS)

o/core.o: libraries/core.cpp $(CORE_HDRS) | o
	gcc $(FLAGS) $(LIBFLAGS) $(DEPENDENCIESFLAGS) -DLIB_MAJ=$(CORE_MAJ) -DLIB_MIN=$(CORE_MIN) -x c++ -c $< -o $@

CORE_OBJS=o/core.o $(patsubst libraries/core_%.cpp,o/core_%.o,$(wildcard libraries/core_*.cpp))


MAIN_HDRS=header.hpp $(CORE_HDRS)

o/%.o: %.cpp $(MAIN_HDRS) | o
	gcc $(FLAGS) $(LIBFLAGS) -x c++ -c $< -o $@

MAIN_OBJS=$(patsubst %.cpp,o/%.o,$(wildcard *.cpp))


LIBCACHEOUTDIR=$(LIBCACHEDIR)/core-$(CORE_MAJ).$(CORE_MIN)
core.exe: $(MAIN_OBJS) $(CORE_OBJS)
	gcc $(FLAGS) $^ -o $@ $(LIBFLAGS) -lstdc++
	$(CMD_RM) $(LIBCACHEOUTDIR)
	$(CMD_MKDIR) $(LIBCACHEOUTDIR)/lib-$(CONFIG)
	ar -rcsv $(LIBCACHEOUTDIR)/lib-$(CONFIG)/libcore.a $(CORE_OBJS)
	$(CMD_MKDIR) $(LIBCACHEOUTDIR)/include
	$(CMD_CP) --target $(LIBCACHEOUTDIR)/include $(CORE_HDRS)
