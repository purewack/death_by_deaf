# L U A T I F

Re-imagined  M O T I F backend powered by Lua

Core audio stuff in C++ 🔊
Slower functions and UI in lua 🌖





### Electron implementation in consideration

REQUIRED IMMEDIATELY
- Fix relative paths for scripts and gfx if and when prog name changes
- Migrate to CMake
- Clean up files and separate out to app.cpp







Done so far ✅
- Lua is embedded into system
- Scripts act as screens, no more complex hard coded UIs
- VElements for UI, no direct drawing (although possible wrapping in the future)
- per-frame animation function
- VActions for event-based animation
- Message banner
- Navigation between scripts with possible animated transitions

Left to do 📝
- Button light functions
- Input event handlers 
- Midi mapped handlers
- Wrapper for clips tracks launcher and effects and all things audio
- Navigable traversal 
- (VWaveform)
- Help file

# Workings

Each script is a screen which has a stack of VElements and VActions for manipulating screen contents. You can Present() and Dismiss() between scripts to give illusion of a menu system.



# Visual Elements (V prefix)

### VElement
Base class for other elements, basic square otherwise

---
    x,y
        Type: int
        Range: --
        Desc: position of anchored point

---
    gx,gy
        Type: int
        Range: --
        Desc: position of anchored point on grid

---
    ax,ay
        Type: float
        Range: 0.0->1.0
        Desc: normalised position of center point within element

---
    w,h
        Type: int
        Range: --
        Desc: size of VElement

---
    r,g,b,a
        Type: int
        Range: 0->255
        Desc: colour of VElement

---
    hue
        Type: int
        Range: 0->255
        Desc: colour of VElement mapped to MOTIF colours

---
    tag
        Type: string
        Range: --
        Desc: identification for referencing

---
    focus
        Type: bool
        Range: --
        Desc: current selection status

---

### VLabel
Label with different fonts

---
    text
        Type: string
        Range: --
        Desc: actual message

---
    size
        Type: int
        Range: 8+
        Desc: height of text in pixels

---
    font
        Type: int
        Range: 0+
        Desc: font index from loaded fonts, see LoadFont 

---

### VImage
Textures for VImage, resuable by multiple elements especially for tiled images

---
    tex
        Type: string
        Range:
        Desc: path to image

---
    tiles_count_x, tiles_count_y
        Type: int
        Range: 1+ (1 default)
        Desc: number of tiles horizontally and vertically (x * y tiles)

---
    tile_x, tile_y
        Type: int
        Range: 1+ (1 default)
        Desc: currently displayed tile coordinate on tilemap

---


### VTimer
Progress bar / circle

---
    progress
        Type: float
        Range: 0.0->1.0
        Desc: path to image

---
    circular
        Type: bool
        Range: 
        Desc: if the style is circular or not

---
    invert
        Type: bool
        Range: 
        Desc: mirror the style

---

### VButton
Interactive element with lambda functions for different events

---
    action()
        Type: function
        Range: 
        Desc: lambda for when the button goes from off to on

---
    release()
        Type: function
        Range: 
        Desc: lambda for when the button goes from on to off

---
    state
        Type: bool
        Range: 
        Desc: current state of the button

---
    onPress()
        Type: function
        Range: 
        Desc: manual invocation of action()

---
    onRelease()
        Type: function
        Range: 
        Desc: manual invocation of release()

---

### VButton
Interactive element emulating the real hardware

---
    note
        Type: int
        Range: 0->127
        Desc: the MIDI note number to send on event

---
    type
        Type: int
        Range: 0x80,0x90
        Desc: the MIDI status number to send on event

---

# Functions

### UI

---
    CreateTexture(string path)
        Param (path): path to source image
        Return: VTex
        Desc: create an automatically managed texture for VImage

---
    RemoveVElement(string withTag)
        Param (withTag): the name of the element to remove
        Return: nil
        Desc: removes the named VElement from the scene

---
    AddV*type*()
        Return: VElement
        Desc: create and add a VElement of type *type* to the scene. 
        Different elements take different params

---
    PlaceVElement(VElement v, float x, float y)
        Param (x,y): the new position of VElement
        Return: VElement
        Desc: teleport element to x,y

---    
    OffsetVElement(VElement v, float x, float y)
        Param (x,y): the relative position of VElement
        Return: VElement
        Desc: nudge element by x,y

---
    MoveVElement(VElement v, float x, float y, int duration)
        Param (x,y): the new position of VElement
        Param (duration) : time to take moving the element
        Return: VElement
        Desc: smoothly move element to *x,y* in time *duration*

---
    AddVAction(action_table,?withName)
        Param (action_table:action(dt)) : function to execute on each fps frame
        Param (action_table:duration(ms)) : duration of overall action in ms
        Param (?withName) : string name for referencing after adding to the stack
        Return: nil
        Desc: Add a single action function to the stack with no delay.
            Example:
            AddVAction({
                duration = 1000, <- ms
                action = function(dt)
                    e.x = dt
                    print("test action")
                end
            },"action key")

---

    AddVSequence(action_array,?withName)
        Param (action_array) : array of VActions to exeute in sequence, each action has a different duration, see AddVAction(action_table,?withName)
        Param (?withName) : string name for referencing after adding to the stack
        Return: nil
        Desc: Add a single action function to the stack with no delay.
            Example:
            AddVSequnece(
            {
                {
                    duration = 1000, <- ms
                    action = function()
                        print("first action")
                    end
                },
                {
                    duration = 500 --delay action
                },
                {
                    duration = 2000, <- ms
                    action = function()  
                        print("next action")
                    end
                }
            },"actions key")

 ---       
    Message(string text)
        Param(text) : The message to display on a banner 
        Return: nil
        Desc: Display a message on a banner (persistent for 5 seconds and inbetween screens)

    Present(string path_to_script)
        Param(path_to_script) : switch to this new script
        Return: nil
        Desc: Mechanism for changing to a different screen (script) for menu navigation

    Dismiss()
        Return: nil
        Desc: Go back to the previous screen in current chain

---    
Look at source for debug functions 🗿
