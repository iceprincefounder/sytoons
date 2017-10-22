# sytoons
Arnold Cartoons Shaders At SY3T

# Supported Platforms

Windows 7 or newer;Centos 6.7 or newer;

Arnold 4.2.11.3 or newer(not support Arnold 5 yeti)


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
