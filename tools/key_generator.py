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

with open(filePath, "w") as file:
    file.write("#pragma once\n\n")
    file.write("namespace ctrader::data::field_id{\n\n")
    file.write("\tconst char* const keys[] = {\n")

    for i in range(0, keyNum-1):
        key = newKey(keySize)
        keyFmt = "\t\t\"" + key + "\",\n"
        file.write(keyFmt)
    
    key = newKey(keySize)
    keyFmt = "\t\t\"" + key + "\"\n"
    file.write(keyFmt)

    file.write("\t};\n\n")
    file.write("} // namespace ctrader::data::field_id \n")