CXXFLAGS	 =  -O2 -Wall -fmessage-length=0 -I.
# -g -fprofile-arcs -ftest-coverage
SOURCES		 := $(wildcard msg/*.cpp)
TESTS        := $(wildcard test/*.cpp)
OBJECTS		 := $(SOURCES:%.cpp=%.o)
TEST_OBJECTS := $(TESTS:.cpp=.o)
DEPS         := $(OBJECTS:.o=.d)
TEST_DEPS    := $(TEST_OBJECTS:.o=.d)
GTEST_I      := -I$(GOOGLE_TEST_PATH)/include -I. -I../stdcore
GTEST_L      := -L$(GOOGLE_TEST_PATH)/make -L. -L../stdcore
INC			 := -I../stdcore
TARGET		 = libmsgcore.a
TEST_TARGET  = test_msgcore

-include $(DEPS)
-include $(TEST_DEPS)

all: lib

lib: $(TARGET)

test: lib $(TEST_TARGET)

check: test
	./$(TEST_TARGET)

$(TARGET): $(OBJECTS)
	ar rvs $(TARGET) $(OBJECTS)

msg/%.o: msg/%.cpp 
	$(CXX) $(CXXFLAGS) $(INC) -MM -MF $(patsubst %.o,%.d,$@) -MT $@ -c $<
	$(CXX) $(CXXFLAGS) $(INC) -c -o $@ $<

$(TEST_TARGET): $(TEST_OBJECTS) $(GOOGLE_TEST_PATH)/make/gtest_main.o
	$(CXX) $(CXXFLAGS) $(GTEST_L) $^ -pthread -lstdcore -l:gtest.a -o $(TEST_TARGET)

test/%.o: test/%.cpp
	$(CXX) $(CXXFLAGS) $(GTEST_I) -MM -MF $(patsubst %.o,%.d,$@) -MT $@ -c $<
	$(CXX) $(CXXFLAGS) $(GTEST_I) $< -c -o $@
	
clean:
	rm -f msg/*.o test/*.o
	rm -f msg/*.d test/*.d
	rm -f msg/*.gcda test/*.gcda
	rm -f msg/*.gcno test/*.gcno
	rm -f $(TARGET) $(TEST_TARGET)
