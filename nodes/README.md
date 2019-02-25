# Nodes 

Node based UI developed with JUCE and C++.


I have been using JUCE for quite some time, building tools for cross platforms, but those were mostly normal UI's, a couple of buttons, updateable render area / image display, with common interactions (clicks, dbl clicks, etc). It performed really well, so I decided to build something more delicate, more advanced with it - a node based system, where nodes can be user defined, can have multiple terminals, the whole canvas can be larger than actual window, user can drag it, scale it, multiselect and multidrag / delete / duplicate nodes etc. All in all - JUCE has proven to be a very handy framework for building dynamic, non trivial UI's, most of all I am impressed by its speed, there seems to be very little overhead, it's easy to debug, you can notice by analizing the call stack how compact and well written it is. 



## Dev notes

Elements are:

* Main window
    * Node List part
        * Drag and Drop onto Renderer 
    * Renderer part
        * Nodes
        * Connections
        * Move nodes
        * Create / delete connections
        
        
* Node
    * Part number (as listed in Node List)
    * Outlets  
        * defined in a component definition file
            * one per component
            * may contain icon or preview image
        * Direction SOURCE or SINK
            * SOURCE can accept only one SINK as its input
            * SINK can accept multiple sources
        * Type 
            * POWER BUS
            * BINARY
            * COMM
                * eth, can, serial ...
                
        
