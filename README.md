Linux only. Maybe one day non-linux. Not today, though.

### This is an unofficial port. Don't bug the Skycoin team for support.



(Currently this project isn't in an operable state)

### Building

1. Run `./bootstrap.sh` to install dependencies
2. 
```
mkdir build
cd build
cmake ..
make
```
3. This will emit `libskywire.a` which can be used for other projects, and various executables.