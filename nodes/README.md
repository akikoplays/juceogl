# Nodes 

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
                
        