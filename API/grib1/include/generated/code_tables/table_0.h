#pragma once
#include <string_view>
#include <optional>



enum class Organization:int{
    Undefined=-1,
  WMO=0,
  Melbourne=1,
  Melbourne_1=2,
  Melbourne_2=3,
  Melbourne_3=4,
  Moscow=5,
  Moscow_1=6,
  NCEP=7,
  NWSTG=8,
  USNWS=9,
  Cairo=10,
  Cairo_1=11,
  Dakar=12,
  Dakar_1=13,
  Nairobi=14,
  Nairobi_1=15,
  Casablanca=16,
  Tunis=17,
  TunisCasabl=18,
  TunisCasabl_1=19,
  LasPalmas=20,
  Algiers=21,
  ACMAD=22,
  Mozambique=23,
  Pretoria=24,
  LaRéunion=25,
  Khabarovsk=26,
  Khabarovsk_1=27,
  NewDelhi=28,
  NewDelhi_1=29,
  Novosibirsk=30,
  Novosibirsk_1=31,
  Tashkent=32,
  Jeddah=33,
  JMA=34,
  JMA_1=35,
  Bangkok=36,
  Ulaanbaatar=37,
  Beijing=38,
  Beijing_1=39,
  Seoul=40,
  BuenosAires=41,
  BuenosAires_1=42,
  Brasilia=43,
  Brasilia_1=44,
  Santiago=45,
  BSA_INPE=46,
  Colombia=47,
  Ecuador=48,
  Peru=49,
  Venezuela=50,
  Miami=51,
  Miami_NHC=52,
  Montreal=53,
  CMC=54,
  SanFranc=55,
  ARINC=56,
  AFGWC=57,
  FNMOC=58,
  NOAA=59,
  NCAR=60,
  ARGOS=61,
  NOO=62,
  IRI=63,
  Honolulu=64,
  Darwin=65,
  Darwin_1=66,
  Melbourne_4=67,
  Reserved=68,
  Wellington=69,
  Wellington_1=70,
  Nadi=71,
  Singapore=72,
  Malaysia=73,
  UK_MOE=74,
  UK_MOE_1=75,
  Moscow_2=76,
  Reserved_77=77,
  Offenbach=78,
  Offenbach_1=79,
  Rome=80,
  Rome_1=81,
  Norrköping=82,
  Norrköping_1=83,
  Toulouse=84,
  Toulouse_1=85,
  Helsinki=86,
  Belgrade=87,
  Oslo=88,
  Prague=89,
  Episkopi=90,
  Ankara=91,
  Frankfurt_Main=92,
  WAFC=93,
  Copenhagen=94,
  Rota=95,
  Athens=96,
  ESA=97,
  ECMWF=98,
  DeBilt=99,
  Brazzaville=100,
  Abidjan=101,
  Libya=102,
  Madagascar=103,
  Mauritius=104,
  Niger=105,
  Seychelles=106,
  Uganda=107,
  Tanzania=108,
  Zimbabwe=109,
  Hong_Kong=110,
  Afghanistan=111,
  Bahrain=112,
  Bangladesh=113,
  Bhutan=114,
  Cambodia=115,
  N_Korea=116,
  Iran=117,
  Iraq=118,
  Kazakhstan=119,
  Kuwait=120,
  Kyrgyzstan=121,
  Laos=122,
  Macao=123,
  Maldives=124,
  Myanmar=125,
  Nepal=126,
  Oman=127,
  Pakistan=128,
  Qatar=129,
  Yemen=130,
  SriLanka=131,
  Tajikistan=132,
  Turkmenistan=133,
  UAE=134,
  Uzbekistan=135,
  VietNam=136,
  Reserved_137=137,
  Reserved_138=138,
  Reserved_139=139,
  Bolivia=140,
  Guyana=141,
  Paraguay=142,
  Suriname=143,
  Uruguay=144,
  FrenchGuiana=145,
  BNHC=146,
  CONAE=147,
  Reserved_148=148,
  Reserved_149=149,
  Antigua_Barbuda=150,
  Bahamas=151,
  Barbados=152,
  Belize=153,
  BCT=154,
  San=155,
  Cuba=156,
  Dominica=157,
  Dominicana=158,
  Salvador=159,
  NOAA_NESDIS=160,
  NOAA_OAR=161,
  Guatemala=162,
  Haiti=163,
  Honduras=164,
  Jamaica=165,
  Mexico=166,
  Curaçao_SintMaarten=167,
  Nicaragua=168,
  Panama=169,
  SaintLucia=170,
  Trinidad_Tobago=171,
  French_Departments_RA_IV=172,
  NASA=173,
  ISDM_MEDS=174,
  UCAR=175,
  CIMSS=176,
  NOAA_NOS=177,
  Reserved_178=178,
  Reserved_179=179,
  Reserved_180=180,
  Reserved_181=181,
  Reserved_182=182,
  Reserved_183=183,
  Reserved_184=184,
  Reserved_185=185,
  Reserved_186=186,
  Reserved_187=187,
  Reserved_188=188,
  Reserved_189=189,
  CookIslands=190,
  FrenchPolynesia=191,
  Tonga=192,
  Vanuatu=193,
  BruneiDarussalam=194,
  Indonesia=195,
  Kiribati=196,
  Micronesia=197,
  NewCaledonia=198,
  Niue=199,
  PapuaNewGuinea=200,
  Philippines=201,
  Samoa=202,
  SolomonIslands=203,
  NIWA=204,
  Reserved_205=205,
  Reserved_206=206,
  Reserved_207=207,
  Reserved_208=208,
  Reserved_209=209,
  ESA_ESRIN=210,
  Lannion=211,
  Lisbon=212,
  Reykjavik=213,
  Madrid=214,
  Zurich=215,
  ARGOS_Toulouse=216,
  Bratislava=217,
  Budapest=218,
  Ljubljana=219,
  Warsaw=220,
  Zagreb=221,
  Albania=222,
  Armenia=223,
  Austria=224,
  Azerbaijan=225,
  Belarus=226,
  Belgium=227,
  Bosnia_Herzegovina=228,
  Bulgaria=229,
  Cyprus=230,
  Estonia=231,
  Georgia=232,
  Dublin=233,
  Israel=234,
  Jordan=235,
  Latvia=236,
  Lebanon=237,
  Lithuania=238,
  Luxembourg=239,
  Malta=240,
  Monaco=241,
  Romania=242,
  SAR=243,
  Macedonia=244,
  Ukraine=245,
  Moldova=246,
  OPERA_EUMETNET=247,
  Montenegro=248,
  Reserved_249=249,
  COSMO=250,
  MetCoOp=251,
  MPI_M=252,
  Reserved_253=253,
  EUMETSAT=254,
  Missing=255,
};


inline const char* center_to_abbr(Organization center){
    switch (center)
    {
case Organization::WMO:
     return "WMO";
     break;
case Organization::Melbourne:
     return "Melbourne";
     break;
case Organization::Melbourne_1:
     return "Melbourne";
     break;
case Organization::Melbourne_2:
     return "Melbourne";
     break;
case Organization::Melbourne_3:
     return "Melbourne";
     break;
case Organization::Moscow:
     return "Moscow";
     break;
case Organization::Moscow_1:
     return "Moscow";
     break;
case Organization::NCEP:
     return "NCEP";
     break;
case Organization::NWSTG:
     return "NWSTG";
     break;
case Organization::USNWS:
     return "USNWS";
     break;
case Organization::Cairo:
     return "Cairo";
     break;
case Organization::Cairo_1:
     return "Cairo";
     break;
case Organization::Dakar:
     return "Dakar";
     break;
case Organization::Dakar_1:
     return "Dakar";
     break;
case Organization::Nairobi:
     return "Nairobi";
     break;
case Organization::Nairobi_1:
     return "Nairobi";
     break;
case Organization::Casablanca:
     return "Casablanca";
     break;
case Organization::Tunis:
     return "Tunis";
     break;
case Organization::TunisCasabl:
     return "TunisCasabl";
     break;
case Organization::TunisCasabl_1:
     return "TunisCasabl";
     break;
case Organization::LasPalmas:
     return "LasPalmas";
     break;
case Organization::Algiers:
     return "Algiers";
     break;
case Organization::ACMAD:
     return "ACMAD";
     break;
case Organization::Mozambique:
     return "Mozambique";
     break;
case Organization::Pretoria:
     return "Pretoria";
     break;
case Organization::LaRéunion:
     return "LaRéunion";
     break;
case Organization::Khabarovsk:
     return "Khabarovsk";
     break;
case Organization::Khabarovsk_1:
     return "Khabarovsk";
     break;
case Organization::NewDelhi:
     return "NewDelhi";
     break;
case Organization::NewDelhi_1:
     return "NewDelhi";
     break;
case Organization::Novosibirsk:
     return "Novosibirsk";
     break;
case Organization::Novosibirsk_1:
     return "Novosibirsk";
     break;
case Organization::Tashkent:
     return "Tashkent";
     break;
case Organization::Jeddah:
     return "Jeddah";
     break;
case Organization::JMA:
     return "JMA";
     break;
case Organization::JMA_1:
     return "JMA";
     break;
case Organization::Bangkok:
     return "Bangkok";
     break;
case Organization::Ulaanbaatar:
     return "Ulaanbaatar";
     break;
case Organization::Beijing:
     return "Beijing";
     break;
case Organization::Beijing_1:
     return "Beijing";
     break;
case Organization::Seoul:
     return "Seoul";
     break;
case Organization::BuenosAires:
     return "BuenosAires";
     break;
case Organization::BuenosAires_1:
     return "BuenosAires";
     break;
case Organization::Brasilia:
     return "Brasilia";
     break;
case Organization::Brasilia_1:
     return "Brasilia";
     break;
case Organization::Santiago:
     return "Santiago";
     break;
case Organization::BSA_INPE:
     return "BSA_INPE";
     break;
case Organization::Colombia:
     return "Colombia";
     break;
case Organization::Ecuador:
     return "Ecuador";
     break;
case Organization::Peru:
     return "Peru";
     break;
case Organization::Venezuela:
     return "Venezuela";
     break;
case Organization::Miami:
     return "Miami";
     break;
case Organization::Miami_NHC:
     return "Miami_NHC";
     break;
case Organization::Montreal:
     return "Montreal";
     break;
case Organization::CMC:
     return "CMC";
     break;
case Organization::SanFranc:
     return "SanFranc";
     break;
case Organization::ARINC:
     return "ARINC";
     break;
case Organization::AFGWC:
     return "AFGWC";
     break;
case Organization::FNMOC:
     return "FNMOC";
     break;
case Organization::NOAA:
     return "NOAA";
     break;
case Organization::NCAR:
     return "NCAR";
     break;
case Organization::ARGOS:
     return "ARGOS";
     break;
case Organization::NOO:
     return "NOO";
     break;
case Organization::IRI:
     return "IRI";
     break;
case Organization::Honolulu:
     return "Honolulu";
     break;
case Organization::Darwin:
     return "Darwin";
     break;
case Organization::Darwin_1:
     return "Darwin";
     break;
case Organization::Melbourne_4:
     return "Melbourne";
     break;
case Organization::Reserved:
     return "Reserved";
     break;
case Organization::Wellington:
     return "Wellington";
     break;
case Organization::Wellington_1:
     return "Wellington";
     break;
case Organization::Nadi:
     return "Nadi";
     break;
case Organization::Singapore:
     return "Singapore";
     break;
case Organization::Malaysia:
     return "Malaysia";
     break;
case Organization::UK_MOE:
     return "UK_MOE";
     break;
case Organization::UK_MOE_1:
     return "UK_MOE";
     break;
case Organization::Moscow_2:
     return "Moscow";
     break;
case Organization::Reserved_77:
     return "Reserved";
     break;
case Organization::Offenbach:
     return "Offenbach";
     break;
case Organization::Offenbach_1:
     return "Offenbach";
     break;
case Organization::Rome:
     return "Rome";
     break;
case Organization::Rome_1:
     return "Rome";
     break;
case Organization::Norrköping:
     return "Norrköping";
     break;
case Organization::Norrköping_1:
     return "Norrköping";
     break;
case Organization::Toulouse:
     return "Toulouse";
     break;
case Organization::Toulouse_1:
     return "Toulouse";
     break;
case Organization::Helsinki:
     return "Helsinki";
     break;
case Organization::Belgrade:
     return "Belgrade";
     break;
case Organization::Oslo:
     return "Oslo";
     break;
case Organization::Prague:
     return "Prague";
     break;
case Organization::Episkopi:
     return "Episkopi";
     break;
case Organization::Ankara:
     return "Ankara";
     break;
case Organization::Frankfurt_Main:
     return "Frankfurt_Main";
     break;
case Organization::WAFC:
     return "WAFC";
     break;
case Organization::Copenhagen:
     return "Copenhagen";
     break;
case Organization::Rota:
     return "Rota";
     break;
case Organization::Athens:
     return "Athens";
     break;
case Organization::ESA:
     return "ESA";
     break;
case Organization::ECMWF:
     return "ECMWF";
     break;
case Organization::DeBilt:
     return "DeBilt";
     break;
case Organization::Brazzaville:
     return "Brazzaville";
     break;
case Organization::Abidjan:
     return "Abidjan";
     break;
case Organization::Libya:
     return "Libya";
     break;
case Organization::Madagascar:
     return "Madagascar";
     break;
case Organization::Mauritius:
     return "Mauritius";
     break;
case Organization::Niger:
     return "Niger";
     break;
case Organization::Seychelles:
     return "Seychelles";
     break;
case Organization::Uganda:
     return "Uganda";
     break;
case Organization::Tanzania:
     return "Tanzania";
     break;
case Organization::Zimbabwe:
     return "Zimbabwe";
     break;
case Organization::Hong_Kong:
     return "Hong_Kong";
     break;
case Organization::Afghanistan:
     return "Afghanistan";
     break;
case Organization::Bahrain:
     return "Bahrain";
     break;
case Organization::Bangladesh:
     return "Bangladesh";
     break;
case Organization::Bhutan:
     return "Bhutan";
     break;
case Organization::Cambodia:
     return "Cambodia";
     break;
case Organization::N_Korea:
     return "N_Korea";
     break;
case Organization::Iran:
     return "Iran";
     break;
case Organization::Iraq:
     return "Iraq";
     break;
case Organization::Kazakhstan:
     return "Kazakhstan";
     break;
case Organization::Kuwait:
     return "Kuwait";
     break;
case Organization::Kyrgyzstan:
     return "Kyrgyzstan";
     break;
case Organization::Laos:
     return "Laos";
     break;
case Organization::Macao:
     return "Macao";
     break;
case Organization::Maldives:
     return "Maldives";
     break;
case Organization::Myanmar:
     return "Myanmar";
     break;
case Organization::Nepal:
     return "Nepal";
     break;
case Organization::Oman:
     return "Oman";
     break;
case Organization::Pakistan:
     return "Pakistan";
     break;
case Organization::Qatar:
     return "Qatar";
     break;
case Organization::Yemen:
     return "Yemen";
     break;
case Organization::SriLanka:
     return "SriLanka";
     break;
case Organization::Tajikistan:
     return "Tajikistan";
     break;
case Organization::Turkmenistan:
     return "Turkmenistan";
     break;
case Organization::UAE:
     return "UAE";
     break;
case Organization::Uzbekistan:
     return "Uzbekistan";
     break;
case Organization::VietNam:
     return "VietNam";
     break;
case Organization::Reserved_137:
     return "Reserved";
     break;
case Organization::Reserved_138:
     return "Reserved";
     break;
case Organization::Reserved_139:
     return "Reserved";
     break;
case Organization::Bolivia:
     return "Bolivia";
     break;
case Organization::Guyana:
     return "Guyana";
     break;
case Organization::Paraguay:
     return "Paraguay";
     break;
case Organization::Suriname:
     return "Suriname";
     break;
case Organization::Uruguay:
     return "Uruguay";
     break;
case Organization::FrenchGuiana:
     return "FrenchGuiana";
     break;
case Organization::BNHC:
     return "BNHC";
     break;
case Organization::CONAE:
     return "CONAE";
     break;
case Organization::Reserved_148:
     return "Reserved";
     break;
case Organization::Reserved_149:
     return "Reserved";
     break;
case Organization::Antigua_Barbuda:
     return "Antigua_Barbuda";
     break;
case Organization::Bahamas:
     return "Bahamas";
     break;
case Organization::Barbados:
     return "Barbados";
     break;
case Organization::Belize:
     return "Belize";
     break;
case Organization::BCT:
     return "BCT";
     break;
case Organization::San:
     return "San";
     break;
case Organization::Cuba:
     return "Cuba";
     break;
case Organization::Dominica:
     return "Dominica";
     break;
case Organization::Dominicana:
     return "Dominicana";
     break;
case Organization::Salvador:
     return "Salvador";
     break;
case Organization::NOAA_NESDIS:
     return "NOAA_NESDIS";
     break;
case Organization::NOAA_OAR:
     return "NOAA_OAR";
     break;
case Organization::Guatemala:
     return "Guatemala";
     break;
case Organization::Haiti:
     return "Haiti";
     break;
case Organization::Honduras:
     return "Honduras";
     break;
case Organization::Jamaica:
     return "Jamaica";
     break;
case Organization::Mexico:
     return "Mexico";
     break;
case Organization::Curaçao_SintMaarten:
     return "Curaçao_SintMaarten";
     break;
case Organization::Nicaragua:
     return "Nicaragua";
     break;
case Organization::Panama:
     return "Panama";
     break;
case Organization::SaintLucia:
     return "SaintLucia";
     break;
case Organization::Trinidad_Tobago:
     return "Trinidad_Tobago";
     break;
case Organization::French_Departments_RA_IV:
     return "French_Departments_RA_IV";
     break;
case Organization::NASA:
     return "NASA";
     break;
case Organization::ISDM_MEDS:
     return "ISDM_MEDS";
     break;
case Organization::UCAR:
     return "UCAR";
     break;
case Organization::CIMSS:
     return "CIMSS";
     break;
case Organization::NOAA_NOS:
     return "NOAA_NOS";
     break;
case Organization::Reserved_178:
     return "Reserved";
     break;
case Organization::Reserved_179:
     return "Reserved";
     break;
case Organization::Reserved_180:
     return "Reserved";
     break;
case Organization::Reserved_181:
     return "Reserved";
     break;
case Organization::Reserved_182:
     return "Reserved";
     break;
case Organization::Reserved_183:
     return "Reserved";
     break;
case Organization::Reserved_184:
     return "Reserved";
     break;
case Organization::Reserved_185:
     return "Reserved";
     break;
case Organization::Reserved_186:
     return "Reserved";
     break;
case Organization::Reserved_187:
     return "Reserved";
     break;
case Organization::Reserved_188:
     return "Reserved";
     break;
case Organization::Reserved_189:
     return "Reserved";
     break;
case Organization::CookIslands:
     return "CookIslands";
     break;
case Organization::FrenchPolynesia:
     return "FrenchPolynesia";
     break;
case Organization::Tonga:
     return "Tonga";
     break;
case Organization::Vanuatu:
     return "Vanuatu";
     break;
case Organization::BruneiDarussalam:
     return "BruneiDarussalam";
     break;
case Organization::Indonesia:
     return "Indonesia";
     break;
case Organization::Kiribati:
     return "Kiribati";
     break;
case Organization::Micronesia:
     return "Micronesia";
     break;
case Organization::NewCaledonia:
     return "NewCaledonia";
     break;
case Organization::Niue:
     return "Niue";
     break;
case Organization::PapuaNewGuinea:
     return "PapuaNewGuinea";
     break;
case Organization::Philippines:
     return "Philippines";
     break;
case Organization::Samoa:
     return "Samoa";
     break;
case Organization::SolomonIslands:
     return "SolomonIslands";
     break;
case Organization::NIWA:
     return "NIWA";
     break;
case Organization::Reserved_205:
     return "Reserved";
     break;
case Organization::Reserved_206:
     return "Reserved";
     break;
case Organization::Reserved_207:
     return "Reserved";
     break;
case Organization::Reserved_208:
     return "Reserved";
     break;
case Organization::Reserved_209:
     return "Reserved";
     break;
case Organization::ESA_ESRIN:
     return "ESA_ESRIN";
     break;
case Organization::Lannion:
     return "Lannion";
     break;
case Organization::Lisbon:
     return "Lisbon";
     break;
case Organization::Reykjavik:
     return "Reykjavik";
     break;
case Organization::Madrid:
     return "Madrid";
     break;
case Organization::Zurich:
     return "Zurich";
     break;
case Organization::ARGOS_Toulouse:
     return "ARGOS_Toulouse";
     break;
case Organization::Bratislava:
     return "Bratislava";
     break;
case Organization::Budapest:
     return "Budapest";
     break;
case Organization::Ljubljana:
     return "Ljubljana";
     break;
case Organization::Warsaw:
     return "Warsaw";
     break;
case Organization::Zagreb:
     return "Zagreb";
     break;
case Organization::Albania:
     return "Albania";
     break;
case Organization::Armenia:
     return "Armenia";
     break;
case Organization::Austria:
     return "Austria";
     break;
case Organization::Azerbaijan:
     return "Azerbaijan";
     break;
case Organization::Belarus:
     return "Belarus";
     break;
case Organization::Belgium:
     return "Belgium";
     break;
case Organization::Bosnia_Herzegovina:
     return "Bosnia_Herzegovina";
     break;
case Organization::Bulgaria:
     return "Bulgaria";
     break;
case Organization::Cyprus:
     return "Cyprus";
     break;
case Organization::Estonia:
     return "Estonia";
     break;
case Organization::Georgia:
     return "Georgia";
     break;
case Organization::Dublin:
     return "Dublin";
     break;
case Organization::Israel:
     return "Israel";
     break;
case Organization::Jordan:
     return "Jordan";
     break;
case Organization::Latvia:
     return "Latvia";
     break;
case Organization::Lebanon:
     return "Lebanon";
     break;
case Organization::Lithuania:
     return "Lithuania";
     break;
case Organization::Luxembourg:
     return "Luxembourg";
     break;
case Organization::Malta:
     return "Malta";
     break;
case Organization::Monaco:
     return "Monaco";
     break;
case Organization::Romania:
     return "Romania";
     break;
case Organization::SAR:
     return "SAR";
     break;
case Organization::Macedonia:
     return "Macedonia";
     break;
case Organization::Ukraine:
     return "Ukraine";
     break;
case Organization::Moldova:
     return "Moldova";
     break;
case Organization::OPERA_EUMETNET:
     return "OPERA_EUMETNET";
     break;
case Organization::Montenegro:
     return "Montenegro";
     break;
case Organization::Reserved_249:
     return "Reserved";
     break;
case Organization::COSMO:
     return "COSMO";
     break;
case Organization::MetCoOp:
     return "MetCoOp";
     break;
case Organization::MPI_M:
     return "MPI_M";
     break;
case Organization::Reserved_253:
     return "Reserved";
     break;
case Organization::EUMETSAT:
     return "EUMETSAT";
     break;
case Organization::Missing:
     return "Missing";
     break;
default:
 return "Undefined";
 break;
 }
 }


inline const char* center_to_text(Organization center){ 
    switch (center)
    {
case Organization::WMO:
     return "WMO Secretariat";
     break;
case Organization::Melbourne:
     return "Melbourne";
     break;
case Organization::Melbourne_1:
     return "Melbourne";
     break;
case Organization::Melbourne_2:
     return "Melbourne";
     break;
case Organization::Melbourne_3:
     return "Melbourne";
     break;
case Organization::Moscow:
     return "Moscow";
     break;
case Organization::Moscow_1:
     return "Moscow";
     break;
case Organization::NCEP:
     return "US National Weather Service - National Centres for Environmental Prediction (NCEP)";
     break;
case Organization::NWSTG:
     return "US National Weather Service Telecommunications Gateway (NWSTG)";
     break;
case Organization::USNWS:
     return "US National Weather Service - Other";
     break;
case Organization::Cairo:
     return "Cairo (RSMC)";
     break;
case Organization::Cairo_1:
     return "Cairo (RSMC)";
     break;
case Organization::Dakar:
     return "Dakar (RSMC)";
     break;
case Organization::Dakar_1:
     return "Dakar (RSMC)";
     break;
case Organization::Nairobi:
     return "Nairobi (RSMC)";
     break;
case Organization::Nairobi_1:
     return "Nairobi (RSMC)";
     break;
case Organization::Casablanca:
     return "Casablanca (RSMC)";
     break;
case Organization::Tunis:
     return "Tunis (RSMC)";
     break;
case Organization::TunisCasabl:
     return "Tunis - Casablanca (RSMC)";
     break;
case Organization::TunisCasabl_1:
     return "Tunis - Casablanca (RSMC)";
     break;
case Organization::LasPalmas:
     return "Las Palmas";
     break;
case Organization::Algiers:
     return "Algiers (RSMC)";
     break;
case Organization::ACMAD:
     return "ACMAD";
     break;
case Organization::Mozambique:
     return "Mozambique (NMC)";
     break;
case Organization::Pretoria:
     return "Pretoria (RSMC)";
     break;
case Organization::LaRéunion:
     return "La Réunion (RSMC)";
     break;
case Organization::Khabarovsk:
     return "Khabarovsk (RSMC)";
     break;
case Organization::Khabarovsk_1:
     return "Khabarovsk (RSMC)";
     break;
case Organization::NewDelhi:
     return "New Delhi (RSMC)";
     break;
case Organization::NewDelhi_1:
     return "New Delhi (RSMC)";
     break;
case Organization::Novosibirsk:
     return "Novosibirsk (RSMC)";
     break;
case Organization::Novosibirsk_1:
     return "Novosibirsk (RSMC)";
     break;
case Organization::Tashkent:
     return "Tashkent (RSMC)";
     break;
case Organization::Jeddah:
     return "Jeddah (RSMC)";
     break;
case Organization::JMA:
     return "Tokyo (RSMC), Japan Meteorological Agency";
     break;
case Organization::JMA_1:
     return "Tokyo (RSMC), Japan Meteorological Agency";
     break;
case Organization::Bangkok:
     return "Bangkok";
     break;
case Organization::Ulaanbaatar:
     return "Ulaanbaatar";
     break;
case Organization::Beijing:
     return "Beijing (RSMC)";
     break;
case Organization::Beijing_1:
     return "Beijing (RSMC)";
     break;
case Organization::Seoul:
     return "Seoul";
     break;
case Organization::BuenosAires:
     return "Buenos Aires (RSMC)";
     break;
case Organization::BuenosAires_1:
     return "Buenos Aires (RSMC)";
     break;
case Organization::Brasilia:
     return "Brasilia (RSMC)";
     break;
case Organization::Brasilia_1:
     return "Brasilia (RSMC)";
     break;
case Organization::Santiago:
     return "Santiago";
     break;
case Organization::BSA_INPE:
     return "Brazilian Space Agency INPE";
     break;
case Organization::Colombia:
     return "Colombia (NMC)";
     break;
case Organization::Ecuador:
     return "Ecuador (NMC)";
     break;
case Organization::Peru:
     return "Peru (NMC)";
     break;
case Organization::Venezuela:
     return "Venezuela (Bolivarian Republic of) (NMC)";
     break;
case Organization::Miami:
     return "Miami (RSMC)";
     break;
case Organization::Miami_NHC:
     return "Miami (RSMC), National Hurricane Centre";
     break;
case Organization::Montreal:
     return "Montreal (RSMC)";
     break;
case Organization::CMC:
     return "Canadian Meteorological Centre";
     break;
case Organization::SanFranc:
     return "San Francisco";
     break;
case Organization::ARINC:
     return "ARINC Centre";
     break;
case Organization::AFGWC:
     return "US Air Force - Air Force Global Weather Central";
     break;
case Organization::FNMOC:
     return "Fleet Numerical Meteorology and Oceanography Center, Monterey, CA, United States";
     break;
case Organization::NOAA:
     return "The NOAA Forecast Systems Laboratory, Boulder, CO, United States";
     break;
case Organization::NCAR:
     return "United States National Center for Atmospheric Research (NCAR)";
     break;
case Organization::ARGOS:
     return "Service ARGOS - Landover";
     break;
case Organization::NOO:
     return "US Naval Oceanographic Office";
     break;
case Organization::IRI:
     return "International Research Institute for Climate and Society (IRI)";
     break;
case Organization::Honolulu:
     return "Honolulu (RSMC)";
     break;
case Organization::Darwin:
     return "Darwin (RSMC)";
     break;
case Organization::Darwin_1:
     return "Darwin (RSMC)";
     break;
case Organization::Melbourne_4:
     return "Melbourne (RSMC)";
     break;
case Organization::Reserved:
     return "Reserved";
     break;
case Organization::Wellington:
     return "Wellington (RSMC)";
     break;
case Organization::Wellington_1:
     return "Wellington (RSMC)";
     break;
case Organization::Nadi:
     return "Nadi (RSMC)";
     break;
case Organization::Singapore:
     return "Singapore";
     break;
case Organization::Malaysia:
     return "Malaysia (NMC)";
     break;
case Organization::UK_MOE:
     return "UK Meteorological Office  Exeter (RSMC)";
     break;
case Organization::UK_MOE_1:
     return "UK Meteorological Office  Exeter (RSMC)";
     break;
case Organization::Moscow_2:
     return "Moscow (RSMC)";
     break;
case Organization::Reserved_77:
     return "Reserved";
     break;
case Organization::Offenbach:
     return "Offenbach (RSMC)";
     break;
case Organization::Offenbach_1:
     return "Offenbach (RSMC)";
     break;
case Organization::Rome:
     return "Rome (RSMC)";
     break;
case Organization::Rome_1:
     return "Rome (RSMC)";
     break;
case Organization::Norrköping:
     return "Norrköping";
     break;
case Organization::Norrköping_1:
     return "Norrköping";
     break;
case Organization::Toulouse:
     return "Toulouse (RSMC)";
     break;
case Organization::Toulouse_1:
     return "Toulouse (RSMC)";
     break;
case Organization::Helsinki:
     return "Helsinki";
     break;
case Organization::Belgrade:
     return "Belgrade";
     break;
case Organization::Oslo:
     return "Oslo";
     break;
case Organization::Prague:
     return "Prague";
     break;
case Organization::Episkopi:
     return "Episkopi";
     break;
case Organization::Ankara:
     return "Ankara";
     break;
case Organization::Frankfurt_Main:
     return "Frankfurt/Main";
     break;
case Organization::WAFC:
     return "London (WAFC)";
     break;
case Organization::Copenhagen:
     return "Copenhagen";
     break;
case Organization::Rota:
     return "Rota";
     break;
case Organization::Athens:
     return "Athens";
     break;
case Organization::ESA:
     return "European Space Agency (ESA)";
     break;
case Organization::ECMWF:
     return "European Centre for Medium-Range Weather Forecasts (ECMWF) (RSMC)";
     break;
case Organization::DeBilt:
     return "De Bilt";
     break;
case Organization::Brazzaville:
     return "Brazzaville";
     break;
case Organization::Abidjan:
     return "Abidjan";
     break;
case Organization::Libya:
     return "Libya (NMC)";
     break;
case Organization::Madagascar:
     return "Madagascar (NMC)";
     break;
case Organization::Mauritius:
     return "Mauritius (NMC)";
     break;
case Organization::Niger:
     return "Niger (NMC)";
     break;
case Organization::Seychelles:
     return "Seychelles (NMC)";
     break;
case Organization::Uganda:
     return "Uganda (NMC)";
     break;
case Organization::Tanzania:
     return "United Republic of Tanzania (NMC)";
     break;
case Organization::Zimbabwe:
     return "Zimbabwe (NMC)";
     break;
case Organization::Hong_Kong:
     return "Hong-Kong, China";
     break;
case Organization::Afghanistan:
     return "Afghanistan (NMC)";
     break;
case Organization::Bahrain:
     return "Bahrain (NMC)";
     break;
case Organization::Bangladesh:
     return "Bangladesh (NMC)";
     break;
case Organization::Bhutan:
     return "Bhutan (NMC)";
     break;
case Organization::Cambodia:
     return "Cambodia (NMC)";
     break;
case Organization::N_Korea:
     return "Democratic People's Republic of Korea (NMC)";
     break;
case Organization::Iran:
     return "Islamic Republic of Iran (NMC)";
     break;
case Organization::Iraq:
     return "Iraq (NMC)";
     break;
case Organization::Kazakhstan:
     return "Kazakhstan (NMC)";
     break;
case Organization::Kuwait:
     return "Kuwait (NMC)";
     break;
case Organization::Kyrgyzstan:
     return "Kyrgyzstan (NMC)";
     break;
case Organization::Laos:
     return "Lao People's Democratic Republic (NMC)";
     break;
case Organization::Macao:
     return "Macao, China";
     break;
case Organization::Maldives:
     return "Maldives (NMC)";
     break;
case Organization::Myanmar:
     return "Myanmar (NMC)";
     break;
case Organization::Nepal:
     return "Nepal (NMC)";
     break;
case Organization::Oman:
     return "Oman (NMC)";
     break;
case Organization::Pakistan:
     return "Pakistan (NMC)";
     break;
case Organization::Qatar:
     return "Qatar (NMC)";
     break;
case Organization::Yemen:
     return "Yemen (NMC)";
     break;
case Organization::SriLanka:
     return "Sri Lanka (NMC)";
     break;
case Organization::Tajikistan:
     return "Tajikistan (NMC)";
     break;
case Organization::Turkmenistan:
     return "Turkmenistan (NMC)";
     break;
case Organization::UAE:
     return "United Arab Emirates (NMC)";
     break;
case Organization::Uzbekistan:
     return "Uzbekistan (NMC)";
     break;
case Organization::VietNam:
     return "Viet Nam (NMC)";
     break;
case Organization::Reserved_137:
     return "Reserved";
     break;
case Organization::Reserved_138:
     return "Reserved";
     break;
case Organization::Reserved_139:
     return "Reserved";
     break;
case Organization::Bolivia:
     return "Bolivia (Plurinational State of) (NMC)";
     break;
case Organization::Guyana:
     return "Guyana (NMC)";
     break;
case Organization::Paraguay:
     return "Paraguay (NMC)";
     break;
case Organization::Suriname:
     return "Suriname (NMC)";
     break;
case Organization::Uruguay:
     return "Uruguay (NMC)";
     break;
case Organization::FrenchGuiana:
     return "French Guiana";
     break;
case Organization::BNHC:
     return "Brazilian Navy Hydrographic Centre";
     break;
case Organization::CONAE:
     return "National Commission on Space Activities (CONAE) - Argentina";
     break;
case Organization::Reserved_148:
     return "Reserved";
     break;
case Organization::Reserved_149:
     return "Reserved";
     break;
case Organization::Antigua_Barbuda:
     return "Antigua and Barbuda (NMC)";
     break;
case Organization::Bahamas:
     return "Bahamas (NMC)";
     break;
case Organization::Barbados:
     return "Barbados (NMC)";
     break;
case Organization::Belize:
     return "Belize (NMC)";
     break;
case Organization::BCT:
     return "British Caribbean Territories Centre";
     break;
case Organization::San:
     return "José San José";
     break;
case Organization::Cuba:
     return "Cuba (NMC)";
     break;
case Organization::Dominica:
     return "Dominica (NMC)";
     break;
case Organization::Dominicana:
     return "Dominican Republic (NMC)";
     break;
case Organization::Salvador:
     return "El Salvador (NMC)";
     break;
case Organization::NOAA_NESDIS:
     return "US NOAA/NESDIS";
     break;
case Organization::NOAA_OAR:
     return "US NOAA Office of Oceanic and Atmospheric Research";
     break;
case Organization::Guatemala:
     return "Guatemala (NMC)";
     break;
case Organization::Haiti:
     return "Haiti (NMC)";
     break;
case Organization::Honduras:
     return "Honduras (NMC)";
     break;
case Organization::Jamaica:
     return "Jamaica (NMC)";
     break;
case Organization::Mexico:
     return "Mexico City";
     break;
case Organization::Curaçao_SintMaarten:
     return "Curaçao and Sint Maarten (NMC)";
     break;
case Organization::Nicaragua:
     return "Nicaragua (NMC)";
     break;
case Organization::Panama:
     return "Panama (NMC)";
     break;
case Organization::SaintLucia:
     return "Saint Lucia (NMC)";
     break;
case Organization::Trinidad_Tobago:
     return "Trinidad and Tobago (NMC)";
     break;
case Organization::French_Departments_RA_IV:
     return "French Departments in RA IV";
     break;
case Organization::NASA:
     return "US National Aeronautics and Space Administration (NASA)";
     break;
case Organization::ISDM_MEDS:
     return "Integrated Science Data Management/Marine Environmental Data Service (ISDM/MEDS) - Canada";
     break;
case Organization::UCAR:
     return "University Corporation for Atmospheric Research (UCAR) - United States";
     break;
case Organization::CIMSS:
     return "Cooperative Institute for Meteorological Satellite Studies (CIMSS) - United States";
     break;
case Organization::NOAA_NOS:
     return "NOAA National Ocean Service - United States";
     break;
case Organization::Reserved_178:
     return "Reserved";
     break;
case Organization::Reserved_179:
     return "Reserved";
     break;
case Organization::Reserved_180:
     return "Reserved";
     break;
case Organization::Reserved_181:
     return "Reserved";
     break;
case Organization::Reserved_182:
     return "Reserved";
     break;
case Organization::Reserved_183:
     return "Reserved";
     break;
case Organization::Reserved_184:
     return "Reserved";
     break;
case Organization::Reserved_185:
     return "Reserved";
     break;
case Organization::Reserved_186:
     return "Reserved";
     break;
case Organization::Reserved_187:
     return "Reserved";
     break;
case Organization::Reserved_188:
     return "Reserved";
     break;
case Organization::Reserved_189:
     return "Reserved";
     break;
case Organization::CookIslands:
     return "Cook Islands (NMC)";
     break;
case Organization::FrenchPolynesia:
     return "French Polynesia (NMC)";
     break;
case Organization::Tonga:
     return "Tonga (NMC)";
     break;
case Organization::Vanuatu:
     return "Vanuatu (NMC)";
     break;
case Organization::BruneiDarussalam:
     return "Brunei Darussalam (NMC)";
     break;
case Organization::Indonesia:
     return "Indonesia (NMC)";
     break;
case Organization::Kiribati:
     return "Kiribati (NMC)";
     break;
case Organization::Micronesia:
     return "Federated States of Micronesia (NMC)";
     break;
case Organization::NewCaledonia:
     return "New Caledonia (NMC)";
     break;
case Organization::Niue:
     return "Niue";
     break;
case Organization::PapuaNewGuinea:
     return "Papua New Guinea (NMC)";
     break;
case Organization::Philippines:
     return "Philippines (NMC)";
     break;
case Organization::Samoa:
     return "Samoa (NMC)";
     break;
case Organization::SolomonIslands:
     return "Solomon Islands (NMC)";
     break;
case Organization::NIWA:
     return "National Institute of Water and Atmospheric Research (NIWA - New Zealand)";
     break;
case Organization::Reserved_205:
     return "Reserved";
     break;
case Organization::Reserved_206:
     return "Reserved";
     break;
case Organization::Reserved_207:
     return "Reserved";
     break;
case Organization::Reserved_208:
     return "Reserved";
     break;
case Organization::Reserved_209:
     return "Reserved";
     break;
case Organization::ESA_ESRIN:
     return "Frascati (ESA/ESRIN)";
     break;
case Organization::Lannion:
     return "Lannion";
     break;
case Organization::Lisbon:
     return "Lisbon";
     break;
case Organization::Reykjavik:
     return "Reykjavik";
     break;
case Organization::Madrid:
     return "Madrid";
     break;
case Organization::Zurich:
     return "Zurich";
     break;
case Organization::ARGOS_Toulouse:
     return "Service ARGOS - Toulouse";
     break;
case Organization::Bratislava:
     return "Bratislava";
     break;
case Organization::Budapest:
     return "Budapest";
     break;
case Organization::Ljubljana:
     return "Ljubljana";
     break;
case Organization::Warsaw:
     return "Warsaw";
     break;
case Organization::Zagreb:
     return "Zagreb";
     break;
case Organization::Albania:
     return "Albania (NMC)";
     break;
case Organization::Armenia:
     return "Armenia (NMC)";
     break;
case Organization::Austria:
     return "Austria (NMC)";
     break;
case Organization::Azerbaijan:
     return "Azerbaijan (NMC)";
     break;
case Organization::Belarus:
     return "Belarus (NMC)";
     break;
case Organization::Belgium:
     return "Belgium (NMC)";
     break;
case Organization::Bosnia_Herzegovina:
     return "Bosnia and Herzegovina (NMC)";
     break;
case Organization::Bulgaria:
     return "Bulgaria (NMC)";
     break;
case Organization::Cyprus:
     return "Cyprus (NMC)";
     break;
case Organization::Estonia:
     return "Estonia (NMC)";
     break;
case Organization::Georgia:
     return "Georgia (NMC)";
     break;
case Organization::Dublin:
     return "Dublin";
     break;
case Organization::Israel:
     return "Israel (NMC)";
     break;
case Organization::Jordan:
     return "Jordan (NMC)";
     break;
case Organization::Latvia:
     return "Latvia (NMC)";
     break;
case Organization::Lebanon:
     return "Lebanon (NMC)";
     break;
case Organization::Lithuania:
     return "Lithuania (NMC)";
     break;
case Organization::Luxembourg:
     return "Luxembourg";
     break;
case Organization::Malta:
     return "Malta (NMC)";
     break;
case Organization::Monaco:
     return "Monaco";
     break;
case Organization::Romania:
     return "Romania (NMC)";
     break;
case Organization::SAR:
     return "Syrian Arab Republic (NMC)";
     break;
case Organization::Macedonia:
     return "The former Yugoslav Republic of Macedonia (NMC)";
     break;
case Organization::Ukraine:
     return "Ukraine (NMC)";
     break;
case Organization::Moldova:
     return "Republic of Moldova (NMC)";
     break;
case Organization::OPERA_EUMETNET:
     return "Operational Programme for the Exchange of weather RAdar information (OPERA) - EUMETNET";
     break;
case Organization::Montenegro:
     return "Montenegro (NMC)";
     break;
case Organization::Reserved_249:
     return "Reserved";
     break;
case Organization::COSMO:
     return "COnsortium for Small scale MOdelling (COSMO)";
     break;
case Organization::MetCoOp:
     return "Meteorological Cooperation on Operational NWP (MetCoOp)";
     break;
case Organization::MPI_M:
     return "Max Planck Institute for Meteorology (MPI-M)";
     break;
case Organization::Reserved_253:
     return "Reserved";
     break;
case Organization::EUMETSAT:
     return "EUMETSAT Operation Centre";
     break;
case Organization::Missing:
     return "Missing";
     break;
default:
 return "Undefined";
 break; }
 }


inline std::optional<Organization> abbr_to_center(std::string_view abbr){
if(abbr=="WMO")
     return Organization::WMO;
if(abbr=="Melbourne")
     return Organization::Melbourne;
if(abbr=="Melbourne_1")
     return Organization::Melbourne_1;
if(abbr=="Melbourne_2")
     return Organization::Melbourne_2;
if(abbr=="Melbourne_3")
     return Organization::Melbourne_3;
if(abbr=="Moscow")
     return Organization::Moscow;
if(abbr=="Moscow_1")
     return Organization::Moscow_1;
if(abbr=="NCEP")
     return Organization::NCEP;
if(abbr=="NWSTG")
     return Organization::NWSTG;
if(abbr=="USNWS")
     return Organization::USNWS;
if(abbr=="Cairo")
     return Organization::Cairo;
if(abbr=="Cairo_1")
     return Organization::Cairo_1;
if(abbr=="Dakar")
     return Organization::Dakar;
if(abbr=="Dakar_1")
     return Organization::Dakar_1;
if(abbr=="Nairobi")
     return Organization::Nairobi;
if(abbr=="Nairobi_1")
     return Organization::Nairobi_1;
if(abbr=="Casablanca")
     return Organization::Casablanca;
if(abbr=="Tunis")
     return Organization::Tunis;
if(abbr=="TunisCasabl")
     return Organization::TunisCasabl;
if(abbr=="TunisCasabl_1")
     return Organization::TunisCasabl_1;
if(abbr=="LasPalmas")
     return Organization::LasPalmas;
if(abbr=="Algiers")
     return Organization::Algiers;
if(abbr=="ACMAD")
     return Organization::ACMAD;
if(abbr=="Mozambique")
     return Organization::Mozambique;
if(abbr=="Pretoria")
     return Organization::Pretoria;
if(abbr=="LaRéunion")
     return Organization::LaRéunion;
if(abbr=="Khabarovsk")
     return Organization::Khabarovsk;
if(abbr=="Khabarovsk_1")
     return Organization::Khabarovsk_1;
if(abbr=="NewDelhi")
     return Organization::NewDelhi;
if(abbr=="NewDelhi_1")
     return Organization::NewDelhi_1;
if(abbr=="Novosibirsk")
     return Organization::Novosibirsk;
if(abbr=="Novosibirsk_1")
     return Organization::Novosibirsk_1;
if(abbr=="Tashkent")
     return Organization::Tashkent;
if(abbr=="Jeddah")
     return Organization::Jeddah;
if(abbr=="JMA")
     return Organization::JMA;
if(abbr=="JMA_1")
     return Organization::JMA_1;
if(abbr=="Bangkok")
     return Organization::Bangkok;
if(abbr=="Ulaanbaatar")
     return Organization::Ulaanbaatar;
if(abbr=="Beijing")
     return Organization::Beijing;
if(abbr=="Beijing_1")
     return Organization::Beijing_1;
if(abbr=="Seoul")
     return Organization::Seoul;
if(abbr=="BuenosAires")
     return Organization::BuenosAires;
if(abbr=="BuenosAires_1")
     return Organization::BuenosAires_1;
if(abbr=="Brasilia")
     return Organization::Brasilia;
if(abbr=="Brasilia_1")
     return Organization::Brasilia_1;
if(abbr=="Santiago")
     return Organization::Santiago;
if(abbr=="BSA_INPE")
     return Organization::BSA_INPE;
if(abbr=="Colombia")
     return Organization::Colombia;
if(abbr=="Ecuador")
     return Organization::Ecuador;
if(abbr=="Peru")
     return Organization::Peru;
if(abbr=="Venezuela")
     return Organization::Venezuela;
if(abbr=="Miami")
     return Organization::Miami;
if(abbr=="Miami_NHC")
     return Organization::Miami_NHC;
if(abbr=="Montreal")
     return Organization::Montreal;
if(abbr=="CMC")
     return Organization::CMC;
if(abbr=="SanFranc")
     return Organization::SanFranc;
if(abbr=="ARINC")
     return Organization::ARINC;
if(abbr=="AFGWC")
     return Organization::AFGWC;
if(abbr=="FNMOC")
     return Organization::FNMOC;
if(abbr=="NOAA")
     return Organization::NOAA;
if(abbr=="NCAR")
     return Organization::NCAR;
if(abbr=="ARGOS")
     return Organization::ARGOS;
if(abbr=="NOO")
     return Organization::NOO;
if(abbr=="IRI")
     return Organization::IRI;
if(abbr=="Honolulu")
     return Organization::Honolulu;
if(abbr=="Darwin")
     return Organization::Darwin;
if(abbr=="Darwin_1")
     return Organization::Darwin_1;
if(abbr=="Melbourne_4")
     return Organization::Melbourne_4;
if(abbr=="Reserved")
     return Organization::Reserved;
if(abbr=="Wellington")
     return Organization::Wellington;
if(abbr=="Wellington_1")
     return Organization::Wellington_1;
if(abbr=="Nadi")
     return Organization::Nadi;
if(abbr=="Singapore")
     return Organization::Singapore;
if(abbr=="Malaysia")
     return Organization::Malaysia;
if(abbr=="UK_MOE")
     return Organization::UK_MOE;
if(abbr=="UK_MOE_1")
     return Organization::UK_MOE_1;
if(abbr=="Moscow_2")
     return Organization::Moscow_2;
if(abbr=="Reserved_77")
     return Organization::Reserved_77;
if(abbr=="Offenbach")
     return Organization::Offenbach;
if(abbr=="Offenbach_1")
     return Organization::Offenbach_1;
if(abbr=="Rome")
     return Organization::Rome;
if(abbr=="Rome_1")
     return Organization::Rome_1;
if(abbr=="Norrköping")
     return Organization::Norrköping;
if(abbr=="Norrköping_1")
     return Organization::Norrköping_1;
if(abbr=="Toulouse")
     return Organization::Toulouse;
if(abbr=="Toulouse_1")
     return Organization::Toulouse_1;
if(abbr=="Helsinki")
     return Organization::Helsinki;
if(abbr=="Belgrade")
     return Organization::Belgrade;
if(abbr=="Oslo")
     return Organization::Oslo;
if(abbr=="Prague")
     return Organization::Prague;
if(abbr=="Episkopi")
     return Organization::Episkopi;
if(abbr=="Ankara")
     return Organization::Ankara;
if(abbr=="Frankfurt_Main")
     return Organization::Frankfurt_Main;
if(abbr=="WAFC")
     return Organization::WAFC;
if(abbr=="Copenhagen")
     return Organization::Copenhagen;
if(abbr=="Rota")
     return Organization::Rota;
if(abbr=="Athens")
     return Organization::Athens;
if(abbr=="ESA")
     return Organization::ESA;
if(abbr=="ECMWF")
     return Organization::ECMWF;
if(abbr=="DeBilt")
     return Organization::DeBilt;
if(abbr=="Brazzaville")
     return Organization::Brazzaville;
if(abbr=="Abidjan")
     return Organization::Abidjan;
if(abbr=="Libya")
     return Organization::Libya;
if(abbr=="Madagascar")
     return Organization::Madagascar;
if(abbr=="Mauritius")
     return Organization::Mauritius;
if(abbr=="Niger")
     return Organization::Niger;
if(abbr=="Seychelles")
     return Organization::Seychelles;
if(abbr=="Uganda")
     return Organization::Uganda;
if(abbr=="Tanzania")
     return Organization::Tanzania;
if(abbr=="Zimbabwe")
     return Organization::Zimbabwe;
if(abbr=="Hong_Kong")
     return Organization::Hong_Kong;
if(abbr=="Afghanistan")
     return Organization::Afghanistan;
if(abbr=="Bahrain")
     return Organization::Bahrain;
if(abbr=="Bangladesh")
     return Organization::Bangladesh;
if(abbr=="Bhutan")
     return Organization::Bhutan;
if(abbr=="Cambodia")
     return Organization::Cambodia;
if(abbr=="N_Korea")
     return Organization::N_Korea;
if(abbr=="Iran")
     return Organization::Iran;
if(abbr=="Iraq")
     return Organization::Iraq;
if(abbr=="Kazakhstan")
     return Organization::Kazakhstan;
if(abbr=="Kuwait")
     return Organization::Kuwait;
if(abbr=="Kyrgyzstan")
     return Organization::Kyrgyzstan;
if(abbr=="Laos")
     return Organization::Laos;
if(abbr=="Macao")
     return Organization::Macao;
if(abbr=="Maldives")
     return Organization::Maldives;
if(abbr=="Myanmar")
     return Organization::Myanmar;
if(abbr=="Nepal")
     return Organization::Nepal;
if(abbr=="Oman")
     return Organization::Oman;
if(abbr=="Pakistan")
     return Organization::Pakistan;
if(abbr=="Qatar")
     return Organization::Qatar;
if(abbr=="Yemen")
     return Organization::Yemen;
if(abbr=="SriLanka")
     return Organization::SriLanka;
if(abbr=="Tajikistan")
     return Organization::Tajikistan;
if(abbr=="Turkmenistan")
     return Organization::Turkmenistan;
if(abbr=="UAE")
     return Organization::UAE;
if(abbr=="Uzbekistan")
     return Organization::Uzbekistan;
if(abbr=="VietNam")
     return Organization::VietNam;
if(abbr=="Reserved_137")
     return Organization::Reserved_137;
if(abbr=="Reserved_138")
     return Organization::Reserved_138;
if(abbr=="Reserved_139")
     return Organization::Reserved_139;
if(abbr=="Bolivia")
     return Organization::Bolivia;
if(abbr=="Guyana")
     return Organization::Guyana;
if(abbr=="Paraguay")
     return Organization::Paraguay;
if(abbr=="Suriname")
     return Organization::Suriname;
if(abbr=="Uruguay")
     return Organization::Uruguay;
if(abbr=="FrenchGuiana")
     return Organization::FrenchGuiana;
if(abbr=="BNHC")
     return Organization::BNHC;
if(abbr=="CONAE")
     return Organization::CONAE;
if(abbr=="Reserved_148")
     return Organization::Reserved_148;
if(abbr=="Reserved_149")
     return Organization::Reserved_149;
if(abbr=="Antigua_Barbuda")
     return Organization::Antigua_Barbuda;
if(abbr=="Bahamas")
     return Organization::Bahamas;
if(abbr=="Barbados")
     return Organization::Barbados;
if(abbr=="Belize")
     return Organization::Belize;
if(abbr=="BCT")
     return Organization::BCT;
if(abbr=="San")
     return Organization::San;
if(abbr=="Cuba")
     return Organization::Cuba;
if(abbr=="Dominica")
     return Organization::Dominica;
if(abbr=="Dominicana")
     return Organization::Dominicana;
if(abbr=="Salvador")
     return Organization::Salvador;
if(abbr=="NOAA_NESDIS")
     return Organization::NOAA_NESDIS;
if(abbr=="NOAA_OAR")
     return Organization::NOAA_OAR;
if(abbr=="Guatemala")
     return Organization::Guatemala;
if(abbr=="Haiti")
     return Organization::Haiti;
if(abbr=="Honduras")
     return Organization::Honduras;
if(abbr=="Jamaica")
     return Organization::Jamaica;
if(abbr=="Mexico")
     return Organization::Mexico;
if(abbr=="Curaçao_SintMaarten")
     return Organization::Curaçao_SintMaarten;
if(abbr=="Nicaragua")
     return Organization::Nicaragua;
if(abbr=="Panama")
     return Organization::Panama;
if(abbr=="SaintLucia")
     return Organization::SaintLucia;
if(abbr=="Trinidad_Tobago")
     return Organization::Trinidad_Tobago;
if(abbr=="French_Departments_RA_IV")
     return Organization::French_Departments_RA_IV;
if(abbr=="NASA")
     return Organization::NASA;
if(abbr=="ISDM_MEDS")
     return Organization::ISDM_MEDS;
if(abbr=="UCAR")
     return Organization::UCAR;
if(abbr=="CIMSS")
     return Organization::CIMSS;
if(abbr=="NOAA_NOS")
     return Organization::NOAA_NOS;
if(abbr=="Reserved_178")
     return Organization::Reserved_178;
if(abbr=="Reserved_179")
     return Organization::Reserved_179;
if(abbr=="Reserved_180")
     return Organization::Reserved_180;
if(abbr=="Reserved_181")
     return Organization::Reserved_181;
if(abbr=="Reserved_182")
     return Organization::Reserved_182;
if(abbr=="Reserved_183")
     return Organization::Reserved_183;
if(abbr=="Reserved_184")
     return Organization::Reserved_184;
if(abbr=="Reserved_185")
     return Organization::Reserved_185;
if(abbr=="Reserved_186")
     return Organization::Reserved_186;
if(abbr=="Reserved_187")
     return Organization::Reserved_187;
if(abbr=="Reserved_188")
     return Organization::Reserved_188;
if(abbr=="Reserved_189")
     return Organization::Reserved_189;
if(abbr=="CookIslands")
     return Organization::CookIslands;
if(abbr=="FrenchPolynesia")
     return Organization::FrenchPolynesia;
if(abbr=="Tonga")
     return Organization::Tonga;
if(abbr=="Vanuatu")
     return Organization::Vanuatu;
if(abbr=="BruneiDarussalam")
     return Organization::BruneiDarussalam;
if(abbr=="Indonesia")
     return Organization::Indonesia;
if(abbr=="Kiribati")
     return Organization::Kiribati;
if(abbr=="Micronesia")
     return Organization::Micronesia;
if(abbr=="NewCaledonia")
     return Organization::NewCaledonia;
if(abbr=="Niue")
     return Organization::Niue;
if(abbr=="PapuaNewGuinea")
     return Organization::PapuaNewGuinea;
if(abbr=="Philippines")
     return Organization::Philippines;
if(abbr=="Samoa")
     return Organization::Samoa;
if(abbr=="SolomonIslands")
     return Organization::SolomonIslands;
if(abbr=="NIWA")
     return Organization::NIWA;
if(abbr=="Reserved_205")
     return Organization::Reserved_205;
if(abbr=="Reserved_206")
     return Organization::Reserved_206;
if(abbr=="Reserved_207")
     return Organization::Reserved_207;
if(abbr=="Reserved_208")
     return Organization::Reserved_208;
if(abbr=="Reserved_209")
     return Organization::Reserved_209;
if(abbr=="ESA_ESRIN")
     return Organization::ESA_ESRIN;
if(abbr=="Lannion")
     return Organization::Lannion;
if(abbr=="Lisbon")
     return Organization::Lisbon;
if(abbr=="Reykjavik")
     return Organization::Reykjavik;
if(abbr=="Madrid")
     return Organization::Madrid;
if(abbr=="Zurich")
     return Organization::Zurich;
if(abbr=="ARGOS_Toulouse")
     return Organization::ARGOS_Toulouse;
if(abbr=="Bratislava")
     return Organization::Bratislava;
if(abbr=="Budapest")
     return Organization::Budapest;
if(abbr=="Ljubljana")
     return Organization::Ljubljana;
if(abbr=="Warsaw")
     return Organization::Warsaw;
if(abbr=="Zagreb")
     return Organization::Zagreb;
if(abbr=="Albania")
     return Organization::Albania;
if(abbr=="Armenia")
     return Organization::Armenia;
if(abbr=="Austria")
     return Organization::Austria;
if(abbr=="Azerbaijan")
     return Organization::Azerbaijan;
if(abbr=="Belarus")
     return Organization::Belarus;
if(abbr=="Belgium")
     return Organization::Belgium;
if(abbr=="Bosnia_Herzegovina")
     return Organization::Bosnia_Herzegovina;
if(abbr=="Bulgaria")
     return Organization::Bulgaria;
if(abbr=="Cyprus")
     return Organization::Cyprus;
if(abbr=="Estonia")
     return Organization::Estonia;
if(abbr=="Georgia")
     return Organization::Georgia;
if(abbr=="Dublin")
     return Organization::Dublin;
if(abbr=="Israel")
     return Organization::Israel;
if(abbr=="Jordan")
     return Organization::Jordan;
if(abbr=="Latvia")
     return Organization::Latvia;
if(abbr=="Lebanon")
     return Organization::Lebanon;
if(abbr=="Lithuania")
     return Organization::Lithuania;
if(abbr=="Luxembourg")
     return Organization::Luxembourg;
if(abbr=="Malta")
     return Organization::Malta;
if(abbr=="Monaco")
     return Organization::Monaco;
if(abbr=="Romania")
     return Organization::Romania;
if(abbr=="SAR")
     return Organization::SAR;
if(abbr=="Macedonia")
     return Organization::Macedonia;
if(abbr=="Ukraine")
     return Organization::Ukraine;
if(abbr=="Moldova")
     return Organization::Moldova;
if(abbr=="OPERA_EUMETNET")
     return Organization::OPERA_EUMETNET;
if(abbr=="Montenegro")
     return Organization::Montenegro;
if(abbr=="Reserved_249")
     return Organization::Reserved_249;
if(abbr=="COSMO")
     return Organization::COSMO;
if(abbr=="MetCoOp")
     return Organization::MetCoOp;
if(abbr=="MPI_M")
     return Organization::MPI_M;
if(abbr=="Reserved_253")
     return Organization::Reserved_253;
if(abbr=="EUMETSAT")
     return Organization::EUMETSAT;
if(abbr=="Missing")
     return Organization::Missing;
return std::nullopt;
}
