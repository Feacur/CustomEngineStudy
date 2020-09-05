### __Assets usage tracking__
Probably, shouldn't dispose of them immediately, but delay the fact for a few frames/seconds  

### __Memory management__
`placement news` might be wrapped into a type-safe template like:  
```
// which looks a bit ugly, though
new (data) std::remove_pointer<decltype(data)>::type;
```

There are instances of temporary data holders; might use a preallocated transient memory for this purpose, which we will be disposed of at the end of a frame or later. All in all, there are lots of best practicies; mentioned in, like, the Game Engine Architecture book. Just need to offload some thoughts for the later and actually approach the design iteratively.  

A relocation table for the references data might find some use, so that data is actually stored contiguously. Currently I store gaps array anyways.  
