#pragma once
#include <cstdint>
#include "def.h"
#include "code_tables/table_0.h"
#include "aux_code/def.h"

#ifdef __cplusplus
#include <span>
#include <string>
template<Organization CENTER>
struct PDSLocal;
template<>
struct PDSLocal<ECMWF>{
    unsigned char* buf_;

    #ifdef __cplusplus
    PDSLocal(unsigned char* buffer):buf_(buffer){}
    std::string get_version() const noexcept{
        return  std::to_string(PDS_Ec16Version(buf_)[0])+'.'+std::to_string(PDS_Ec16Version(buf_)[1])+'.'+
                std::to_string(PDS_Ec16Version(buf_)[2])+'.'+std::to_string(PDS_Ec16Version(buf_)[3]);
    }

    unsigned char local_def_number() const noexcept{
        return PDS_LocalId(buf_);
    }
    unsigned char get_class() const noexcept{
        return PDS_EcClass(buf_);
    }
    unsigned char get_type() const noexcept{
        return PDS_EcClass(buf_);
    }
    unsigned short get_stream() const noexcept{
        return PDS_EcStream(buf_);
    }
    template<unsigned char ID>
    struct Definition;
    #endif
};
//TODO
struct PDSLocal<NCEP>{
    unsigned char* buf_;

    #ifdef __cplusplus
    PDSLocal(unsigned char* buffer):buf_(buffer){}
    //Identifies application 
    //1 = Ensemble 
    unsigned char local_def_number() const noexcept{
        return PDS_LocalId(buf_);
    }
    // Identification number 
    // If Byte 42 = 1, byte 43 will = 1 or 2: 
    // 1 = High resolution control forecast 
    // 2 = Low resolution control forecast
    // If Byte 42 = 2 or 3, byte 43 will = 1,2,3,4 or 5: 
    // This number indicates the identification number 
    // for this member of a pair. That is, there is a 
    // first negative perturbation and a first positive 
    // perturbation (n1 and p1). 
    unsigned char get_ID_number() const noexcept{
        return PDS_NcepFcstNo(buf_);
    }
    //Type 
    //1 = Unperturbed control forecast 
    //2 = Individual negatively perturbed forecast 
    //3 = Individual positively perturbed forecast 
    //4 = Cluster 
    //5 = Whole ensemble 
    unsigned char get_type() const noexcept{
        return PDS_NcepFcstType(buf_);
    }
    // Product Identifier
    // 1 = Full field (individual forecast) / Unweighted mean (cluster/ens) 
    // 2 = Weighted mean (of raw forecasts)
    // 3 = Full field (individual forecast of  bias correction) : the values are modified by considered bias
    // 4 = Weighted mean ( of bias corrected forecasts)
    // 5 = Weights: statistically decided weights for each ensemble member
    // 6 = Climate percentile: percentile values from climate distribution
    // 7 = daily climate mean: from 40-year NCEP/NCAR reanalysis
    // 8 = daily climate standard deviation from daily mean: from 40-year NCEP/NCAR reanalysis
    // 11 = Standard deviation with respect to ensemble mean 
    // 12 = Standard deviation with respect to ensemble mean, normalized
    // 21 = maximum value of all members
    // 22 = minimum value of all members
    // 23 = The ensemble forecast value for X% probability
    // (X is given in byte 45 - integer value 0 - 100)
    // 24 = The ensemble mode forecast (mode = 3*medium - 2*mean)
    unsigned short get_ID_product() const noexcept{
        return PDS_NcepFcstProd(buf_);
    }
    template<unsigned char ID>
    struct Definition;
    #endif
};
#endif