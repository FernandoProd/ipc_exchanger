# IPC exchanger

## Part I

### Build and debug
```
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build 
```

### General protocol module
We have three main structures for packing messages: [ MAGIC (4 bytes) ][ SIZE (4 bytes) ][ PAYLOAD (SIZE bytes) ]
 - `MAGIC`
 - `SIZE`
 - `PAYLOAD`