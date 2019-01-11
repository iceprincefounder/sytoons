
![sytoons_network](https://user-images.githubusercontent.com/16664056/50583144-8fb4ea80-0ea2-11e9-8f0b-49f2c1be6a64.png)

# sytoons

Arnold sytoons shaders are a shader kit to render the celluloid printing style image with power and speed!

We contents a full kit to create custom design and styling images.

# Supported Platforms


----

## LINUX    [![Build Status](https://travis-ci.org/appleseedhq/appleseed.svg?branch=master)](https://travis-ci.org/appleseedhq/appleseed)   || WINDOWS    [![Build Status](https://travis-ci.org/appleseedhq/appleseed.svg?branch=master)](https://travis-ci.org/appleseedhq/appleseed) 

----

Windows 7 or newer;Centos 6.7 or newer;

Arnold 4.2.11.3 or newer(not support Arnold 5 yet)



# How to Install

## windows

For in-place install
- Libraries and MTD files found in the 'bin' directory go somewhere in your ARNOLD_PLUGIN_PATH or in 'shaders' under your MtoA installation directory
- *.py interface templates found in the 'ae' directory go somwhere in your MTOA_TEMPLATES_PATH or in 'scripts/mtoa/ui/ae' under your MtoA installation directory

For custom install - necessary for Maya 2016 and above
- Extract the package to an alShaders folder somewhere on your filesystem.
- Set the ARNOLD_PLUGIN_PATH variable to include /path/to/sytoons/bin
- Set the MTOA_TEMPLATES_PATH variable to include /path/to/sytoons/ae
- Set the MAYA_CUSTOM_TEMPLATE_PATH variable to include /path/to/sytoons/aexml

Or in Maya.env file

```

ARNOLD_PLUGIN_PATH=path\to\sytoons\bin
MTOA_TEMPLATES_PATH=path\to\sytoons\ae
MAYA_CUSTOM_TEMPLATE_PATH=path\to\sytoons\aexml

```

## linux

set those environment variable in shell script

```
export MTOA_TEMPLATES_PATH=/path/to/sytoons/ae:$MTOA_TEMPLATES_PATH

export MAYA_CUSTOM_TEMPLATE_PATH=/path/to/sytoons/aexml:$MAYA_CUSTOM_TEMPLATE_PATH

export ARNOLD_PLUGIN_PATH=/path/to/sytoons/bin:$ARNOLD_PLUGIN_PATH
```


# How to Compile

## windows

set ARNOLD_PATH in Compile.bat

set your Visual Studio 14.0 vcvarsall.bat file path

```
cd /path/to/sytoons
Compile.bat
```

## linux

set ARNOLD_PATH in Makefile file

![sytoons_network](https://user-images.githubusercontent.com/16664056/50583144-8fb4ea80-0ea2-11e9-8f0b-49f2c1be6a64.png)

# sytoons

Arnold sytoons shaders are a shader kit to render the celluloid printing style image with power and speed!

We contents a full kit to create custom design and styling images.

# Supported Platforms


----

## LINUX    [![Build Status](https://travis-ci.org/appleseedhq/appleseed.svg?branch=master)](https://travis-ci.org/appleseedhq/appleseed)   || WINDOWS    [![Build Status](https://travis-ci.org/appleseedhq/appleseed.svg?branch=master)](https://travis-ci.org/appleseedhq/appleseed) 

----

Windows 7 or newer;Centos 6.7 or newer;

Arnold 4.2.11.3 or newer(not support Arnold 5 yet)



# How to Install

## windows

For in-place install
- Libraries and MTD files found in the 'bin' directory go somewhere in your ARNOLD_PLUGIN_PATH or in 'shaders' under your MtoA installation directory
- *.py interface templates found in the 'ae' directory go somwhere in your MTOA_TEMPLATES_PATH or in 'scripts/mtoa/ui/ae' under your MtoA installation directory

For custom install - necessary for Maya 2016 and above
- Extract the package to an alShaders folder somewhere on your filesystem.
- Set the ARNOLD_PLUGIN_PATH variable to include /path/to/sytoons/bin
- Set the MTOA_TEMPLATES_PATH variable to include /path/to/sytoons/ae
- Set the MAYA_CUSTOM_TEMPLATE_PATH variable to include /path/to/sytoons/aexml

Or in Maya.env file

```

ARNOLD_PLUGIN_PATH=path\to\sytoons\bin
MTOA_TEMPLATES_PATH=path\to\sytoons\ae
MAYA_CUSTOM_TEMPLATE_PATH=path\to\sytoons\aexml

```

## linux

set those environment variable in shell script

```
export MTOA_TEMPLATES_PATH=/path/to/sytoons/ae:$MTOA_TEMPLATES_PATH

export MAYA_CUSTOM_TEMPLATE_PATH=/path/to/sytoons/aexml:$MAYA_CUSTOM_TEMPLATE_PATH

export ARNOLD_PLUGIN_PATH=/path/to/sytoons/bin:$ARNOLD_PLUGIN_PATH
```


# How to Compile

## windows

set ARNOLD_PATH in Compile.bat

set your Visual Studio 14.0 vcvarsall.bat file path

```
cd /path/to/sytoons
Compile.bat
```

## linux

set ARNOLD_PATH in Makefile file

```
cd /path/to/sytoons
make 
```

# For details,check out our wikis

https://github.com/iceprincefounder/sytoons/wiki

# Maya ID

```

Autodesk Maya Developer Node ID Block:

    Node ID Block is: 0x0012d340 - 0x0012d37f
    Registered for syToons [Kevin Tsui]

```

```
cd /path/to/sytoons
make 
```

# For details,check out our wikis

https://github.com/iceprincefounder/sytoons/wiki

# Maya ID

```

Autodesk Maya Developer Node ID Block:

    Node ID Block is: 0x0012d340 - 0x0012d37f
    Registered for syToons [Kevin Tsui]

```
