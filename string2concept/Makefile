CC     := clang++
CFLAGS := -std=c++11 -O2 -pedantic -Wall -I.
OBJ    := tests/TestHash.o \
          tests/TestString.o \
	      tests/TestVector.o \
	      tests/TestHashTable.o \
          tests/TestWords.o \
          tests/TestConceptExtractor.o \
	      ConceptExtractor.o \
		  string2concept.o
		  
TARGET := string2concept

.PHONY: all clean test

all: $(TARGET)

$(OBJ): %.o : %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(TARGET) $(OBJ)

test:
	./$(TARGET) --test
