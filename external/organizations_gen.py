import os
import argparse
import re
from pathlib import Path

def define_table_0(output_dir):
    head:str = "#pragma once\n\
#include <string_view>\n\
#include <optional>\n\
#include <cstdint> \n\n"
    enumeration = "enum class Organization:uint8_t{\n"
    switcher_abbr:str = "inline const char* center_to_abbr(Organization center){\n\
    switch (center)\n\
    {\n"
    switcher_text:str = "inline const char* center_to_text(Organization center){ \n\
    switch (center)\n\
    {\n"
    parse_abbr:str = "inline std::optional<Organization> abbr_to_center(std::string_view abbr){\n"

    set_of_abbr:dict = {}
    result:str=""
    postfix:str=""
    path = os.path.dirname(os.path.abspath(__file__))
    with open(f"{path}/organizations.dat") as f:
        for line in f:
            line=line[:-1]
            [number,abbr,text] = line.split(" ",2)
            if abbr in set_of_abbr:
                if abbr=="Reserved" or abbr=="Missing":
                    postfix="_"+str(number)
                else:
                    set_of_abbr[abbr]+=1
                    postfix="_"+str(set_of_abbr[abbr])
            else:
                set_of_abbr[abbr]=0
            switcher_text+=f"case Organization::{abbr}{postfix}:\n \
    return \"{text}\";\n \
    break;\n"
            switcher_abbr+=f"case Organization::{abbr}{postfix}:\n \
    return \"{abbr}\";\n \
    break;\n"
            parse_abbr+=f"if(abbr==\"{abbr}{postfix}\")\n \
    return Organization::{abbr}{postfix};\n"
            enumeration+=f"  {abbr}{postfix}={number},\n"
            postfix=""
    enumeration.removesuffix(",")
    enumeration+="};\n"
    switcher_text+=f"default:\n \
return \"Missing\";\n \
break; \
}}\n \
}}\n"
    switcher_abbr+=f"default:\n \
return \"Missing\";\n \
break;\n \
}}\n \
}}\n"
    parse_abbr+=f"return std::nullopt;\n\
}}\n"
    
    result+=head+"\n\n"+enumeration+"\n\n"+switcher_abbr+"\n\n"+switcher_text+"\n\n"+parse_abbr
    os.makedirs(output_dir, exist_ok=True)
    header = f"{output_dir}/table_0.h"
    with open(header, 'w') as f:
        f.write(result)


def main(argv):
    return define_table_0(argv.out)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--src', required=True)
    parser.add_argument('--out', required=True)
    args = parser.parse_args()
    main(args)
