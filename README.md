# vulkanProj
well see where this goes

Alrighty going to break this up into a few sections to start.

1. What i'm using
    a. shell = git bash (https://git-scm.com/downloads)
    b. visual studio (for now)
    c. visual studio code
    d. glfw - git clone https://github.com/charles-lunarg/vk-bootstrap.git
    e. vk-bootstrap - git clone https://github.com/glfw/glfw.git

2. Objectives
    a. Boil down in some way (TBD), further boil down glfw and the setup 
        portion of vk-bootstrap into perhapse seperate helper classes that 
        handle all of the setup via constructors/ cleanup via destructors

3. Current issues *feel free to work ahead with whatever workaround you can find*
    a. trying to use cmake to build and install glfw and vk-bootstrap correctly,
        at the moment if you open each via open directory in visual studio it will
        compile them down into .lib's, but im looking to do it via script or maybe
        a top level cmake CMakeList.txt file we would have to put together,
        the latter being the preferable option.

4. PS
    a. both glfw and vk-bootstrap have example src code within and .exe's
