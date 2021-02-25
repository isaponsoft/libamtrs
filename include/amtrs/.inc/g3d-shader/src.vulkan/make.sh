#!/bin/sh


#./glslangValidator

compile () {
    glslangValidator $1 -V -l -o $1.spv
    xxd -i $1.spv > ../$1.hpp
    rm $1.spv
}

for shader in `ls vulkan.*.frag vulkan.*.vert`
do
    compile $shader
done
