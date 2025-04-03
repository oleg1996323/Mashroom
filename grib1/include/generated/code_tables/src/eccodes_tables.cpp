#include "generated/code_tables/include/eccodes_tables.h"
const ParmTable* parameter_table(unsigned char center, unsigned char subcenter, unsigned char param_num){
    switch(center){
    case 233:
    switch(subcenter){
        case 253:
          return &ParmTable_2_233_253[param_num];
           break;
        case 1:
          return &ParmTable_2_233_1[param_num];
           break;
        default:
        return NULL;
        break;
    }
    case 82:
    switch(subcenter){
        case 253:
          return &ParmTable_2_82_253[param_num];
           break;
        case 129:
          return &ParmTable_2_82_129[param_num];
           break;
        case 133:
          return &ParmTable_2_82_133[param_num];
           break;
        case 134:
          return &ParmTable_2_82_134[param_num];
           break;
        case 136:
          return &ParmTable_2_82_136[param_num];
           break;
        case 130:
          return &ParmTable_2_82_130[param_num];
           break;
        case 135:
          return &ParmTable_2_82_135[param_num];
           break;
        case 128:
          return &ParmTable_2_82_128[param_num];
           break;
        case 1:
          return &ParmTable_2_82_1[param_num];
           break;
        case 131:
          return &ParmTable_2_82_131[param_num];
           break;
        default:
        return NULL;
        break;
    }
    case 98:
    switch(subcenter){
        case 171:
          return &ParmTable_2_98_171[param_num];
           break;
        case 213:
          return &ParmTable_2_98_213[param_num];
           break;
        case 173:
          return &ParmTable_2_98_173[param_num];
           break;
        case 151:
          return &ParmTable_2_98_151[param_num];
           break;
        case 128:
          return &ParmTable_2_128[param_num];
           break;
        case 235:
          return &ParmTable_2_98_235[param_num];
           break;
        case 201:
          return &ParmTable_2_98_201[param_num];
           break;
        case 170:
          return &ParmTable_2_98_170[param_num];
           break;
        case 220:
          return &ParmTable_2_98_220[param_num];
           break;
        case 133:
          return &ParmTable_2_98_133[param_num];
           break;
        case 129:
          return &ParmTable_2_98_129[param_num];
           break;
        case 174:
          return &ParmTable_2_98_174[param_num];
           break;
        case 140:
          return &ParmTable_2_98_140[param_num];
           break;
        case 190:
          return &ParmTable_2_98_190[param_num];
           break;
        case 228:
          return &ParmTable_2_98_228[param_num];
           break;
        case 172:
          return &ParmTable_2_98_172[param_num];
           break;
        case 180:
          return &ParmTable_2_98_180[param_num];
           break;
        case 175:
          return &ParmTable_2_98_175[param_num];
           break;
        case 210:
          return &ParmTable_2_98_210[param_num];
           break;
        case 130:
          return &ParmTable_2_98_130[param_num];
           break;
        case 211:
          return &ParmTable_2_98_211[param_num];
           break;
        case 162:
          return &ParmTable_2_98_162[param_num];
           break;
        case 230:
          return &ParmTable_2_98_230[param_num];
           break;
        case 160:
          return &ParmTable_2_98_160[param_num];
           break;
        case 200:
          return &ParmTable_2_98_200[param_num];
           break;
        case 150:
          return &ParmTable_2_98_150[param_num];
           break;
        case 132:
          return &ParmTable_2_98_132[param_num];
           break;
        case 131:
          return &ParmTable_2_98_131[param_num];
           break;
        case 215:
          return &ParmTable_2_98_215[param_num];
           break;
        default:
        return NULL;
        break;
    }
    case 0:
    switch(subcenter){
        case 1:
          return &ParmTable_2_0_1[param_num];
           break;
        case 3:
          return &ParmTable_2_0_3[param_num];
           break;
        case 2:
          return &ParmTable_2_0_2[param_num];
           break;
        default:
        return NULL;
        break;
    }
    case 46:
    switch(subcenter){
        case 254:
          return &ParmTable_2_46_254[param_num];
           break;
        default:
        return NULL;
        break;
    }
    case 34:
    switch(subcenter){
        case 200:
          return &ParmTable_2_34_200[param_num];
           break;
        default:
        return NULL;
        break;
    }
    case 253:
    switch(subcenter){
        case 128:
          return &ParmTable_2_253_128[param_num];
           break;
        default:
        return NULL;
        break;
    }
    default:
    return NULL;
    break;
    }
}
