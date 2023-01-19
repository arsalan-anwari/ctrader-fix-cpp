import random, string
import os

'''
#pragma once

namespace ctrader::data::field_id{

    const char* const keys[] = {
        "Hello",
        "World"
    };
    
}
'''

def newKey(size):
    key = ''.join(random.choices(string.ascii_letters + string.digits, k=size))
    return key


absPath = os.path.dirname(__file__)
filePath = absPath + "/../data/field_id.hpp"
keyNum = 1024
keySize = 6

if keyNum > 65536:
        print("WARNING: Cannot generate more than 65536 keys, maximum of 65536 is used!")
        keyNum = 65536

with open(filePath, "w") as file:

    file.write("#pragma once\n\n")
    file.write("#include <stdint.h>\n\n")
    file.write("namespace ctrader::data::field_id{\n\n")
    file.write("\tconstexpr uint16_t KeyNum = " + str(keyNum) + ";\n\n" )
    file.write("\tconst char* const Keys[] = {\n")

    for i in range(0, keyNum-1):
        key = newKey(keySize)
        keyFmt = "\t\t\"" + key + "\",\n"
        file.write(keyFmt)
    
    key = newKey(keySize)
    keyFmt = "\t\t\"" + key + "\"\n"
    file.write(keyFmt)

    file.write("\t};\n\n")
    file.write("} // namespace ctrader::data::field_id \n")