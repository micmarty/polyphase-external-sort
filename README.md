# Polyphase external sort algorithm implementation
C++ project written during my studies. It's more like a proof of concept app.
Unfortunately output is in polish language...
### I don't plan any further updates
[![asciicast](https://asciinema.org/a/wHTKjANzDCmYajENwUMHWmrDa.png)](https://asciinema.org/a/wHTKjANzDCmYajENwUMHWmrDa)

## How to run
```bash
# Warning! 
# This program requires empty files named: A, B, C, INPUT in the same directory as binary.
# Those are prepared already in build folder
# Otherwise it doesn't work...

./build/PolyphaseSort.out
```

## How to compile it manually
```bash
g++ -c src/*.cpp && mv *.o build    
g++ build/*.o -o build/PolyphaseSort.out
```

## How it works
It just sorts but not in the memory (RAM) like quicksort or bubblesort. 
What it does instead is it uses disk files and assumes that these files may be HUGE (like gigabytes) so that the data can't be put into memory in single chunk.
The whole concept relies on distributing chunks into multiple tapes (a'la Fibonacci sequence), merging and repeating this process until sorted - dramatic simplification.

## Example video
14.475 External Merge Sort - **Jens Dittrich**
https://www.youtube.com/watch?v=ATK74YSzwxg
