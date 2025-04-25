#pragma once

enum class Organization{
    UNDEF_ORG = -1,
    WMO = 0,
    Melbourne = 1,
    Melbourne_1 = 2,
    Melbourne_2 = 3,
    Moscow = 4,
    Moscow_1 = 5,
    Moscow_2 = 6,
    NCEP = 7,
    NWSTG = 8,
    US_other = 9,
    Cairo_RSMC = 10,
    Cairo_RSMC_1 = 11,
    Dakar_RSMC = 12,
    Dakar_RSMC_1 = 13,
    Nairobi_RSMC = 14,
    Nairobi_RSMC_1 = 15,
    Casablanca_RSMC = 16,
    Tunis_RSMC = 17,
    Tunis_Casablanca_RSMC = 18,
    Tunis_Casablanca_RSMC_1 = 19,
    Las_Palmas = 20,
    Algiers_RSMC = 21,
    ACMAD = 22,
    Mozambique_NMC = 23,
    Pretoria_RSMC = 24,
    Reunion_RSMC = 25,
    Khabarovsk_RSMC = 26,
    Khabarovsk_RSMC_1 = 27,
    New_Dehli_RSMC = 28,
    New_Dehli_RSMC_1 = 29,
    Novosibirsk_RSMC = 30,
    Novosibirsk_RSMC_1 = 31,
    Tashkent_RSMC = 32,
    Jeddah_RSMC = 33,
    JMA = 34,
    JMA_1 = 35,
    Bangkok = 36,
    Ulaanbaatar = 37,
    Beijing_RSMC = 38,
    Beijing_RSMC_1 = 39,
    Seoul = 40,
    Buenos_Aires_RSMC = 41,
    Buenos_Aires_RSMC_1 = 42,
    Brasilia_RSMC = 43,
    Brasilia_RSMC_1 = 44,
    Santiago = 45,
    BSA_INPE = 46,
    Colombia_NMC = 47,
    Ecuador_NMC = 48,
    Peru_NMC = 49,
    Venezuela_NMC = 50,
    Miami_RSMC = 51,
    Miami_RSMC_NHC = 52,
    Montreal_RSMC = 53,
    CMC = 54, //verify //Canadian Meteorological Centre
    San_Francisco = 55,
    ARINC_Center = 56,
    AFGWC = 57,
    FNMOC = 58,
    NOAA = 59,
    NCAR = 60,
    ARGOS_Landover = 61,
    NOO = 62, //US Naval Oceanographic Office
    IRI = 63,
    Honolulu_RSMC = 64,
    Darwin_RSMC = 65,
    Darwin_RSMC_1 = 66,
    Melbourne_RSMC = 67,
    Wellington_RSMC = 69,
    Wellington_RSMC_1 = 70,
    Nadi_RSMC = 71,
    Singapore = 72,
    Malaysia_NMC = 73,
    UK_Meteorological_Office_Exeter_RSMC = 74,
    UK_Meteorological_Office_Exeter_RSMC_1 = 75,
    Moscow_RSMC_2 = 76,
    DWD = 78,
    DWD_1 = 79,
    Rome_RSMC = 80,
    Rome_RSMC_1 = 81,
    Norrkoping = 82,
    Norrkoping_1 = 83,
    Toulouse_RSMC = 84,
    Toulouse_RSMC_1 = 85,
    Helsinki = 86,
    Belgrade = 87,
    Oslo = 88,
    Prague = 89,
    Episkopi = 90,
    Ankara = 91,
    Frankfurt_Main = 92,
    London_WAFC = 93,
    Copenhagen = 94,
    Rota = 95,
    Athens = 96,
    ESA = 97,
    ECMWF = 98,
    De_Bilt = 99,
    Brazzaville = 100,
    Abidjan = 101,
    Libya_NMC = 102,
    Madagascar_NMC = 103,
    Mauritius_NMC = 104,
    Niger_NMC = 105,
    Seychelles_NMC = 106,
    Uganda_NMC = 107,
    Tanzania_NMC = 108,
    Zimbabwe_NMC = 109,
    Hong_Kong = 110,
    Afghanistan_NMC = 111,
    Bahrain_NMC = 112,
    Bangladesh_NMC = 113,
    Bhutan_NMC = 114,
    Cambodia_NMC = 115,
    North_Korea_NMC = 116,
    Iran_NMC = 117,
    Iraq_NMC = 118,
    Kazakhstan_NMC = 119,
    Kuwait_NMC = 120,
    Kyrgyzstan_NMC = 121,
    Laos_NMC = 122,
    Macao = 123,
    Maldives_NMC = 124,
    Myanmar_NMC = 125,
    Nepal_NMC = 126,
    Oman_NMC = 127,
    Pakistan_NMC = 128,
    Qatar_NMC = 129,
    Yemen_NMC = 130,
    Sri_Lanka_NMC = 131,
    Tajikistan_NMC = 132,
    Turkmenistan_NMC = 133,
    United_Arab_Emirates_NMC = 134,
    Uzbekistan_NMC = 135,
    Vietnam_NMC = 136,
    Bolivia_NMC = 140,
    Guyana_NMC = 141,
    Paraguay_NMC = 142,
    Suriname_NMC = 143,
    Uruguay_NMC = 144,
    French_Guiana = 145,
    BNHC = 146,
    CONAE = 147,
    Antigua_Barbuda_NMC = 150,
    Bahamas_NMC = 151,
    Barbados_NMC = 152,
    Belize_NMC = 153,
    BCT = 154,
    San_Jose = 155,
    Cuba_NMC = 156,
    Dominica_NMC = 157,
    Dominican_Republic_NMC = 158,
    El_Salvador_NMC = 159,
    NOAA_NESDIS = 160,
    NOAA_OAR = 161,
    Guatemala_NMC = 162,
    Haiti_NMC = 163,
    Honduras_NMC = 164,
    Jamaica_NMC = 165,
    Mexico_City = 166,
    Curacao_and_Sint_Maarten_NMC = 167,
    Nicaragua_NMC = 168,
    Panama_NMC = 169,
    Saint_Lucia_NMC = 170,
    Trinidad_Tobago = 171,
    French_Departments_RA = 172,
    NASA = 173,
    ISDM_MEDS = 174,
    UCAR = 175,
    CIMSS = 176,
    NOAA_National_Ocean_Service = 177,
    Cook_Islands_NMC = 190,
    French_Polinesia_NMC = 191,
    Tonga_NMC = 192,
    Vanuatu_NMC = 193,
    Brunei_Darussalam_NMC = 194,
    Indonesia_NMC = 195,
    Kiribati_NMC = 196,
    Micronesia_NMC = 197,
    New_Caledonia_NMC = 198,
    Niue = 199,
    Papua_New_Guinea_NMC = 200,
    Philipines_NMC = 201,
    Samoa_NMC = 202,
    Solomon_Islands_NMC = 203,
    NIWA = 204,
    Frascati_ESA_ESRIN = 210,
    Lannion = 211,
    Lisbon = 212,
    Reykjavik = 213,
    Madrid = 214,
    Zurich = 215,
    ARGOS_Toulouse = 216,
    Bratislava = 217,
    Budapest = 218,
    Ljubljana = 219,
    Warsaw = 220,
    Zagreb = 221,
    Albania_NMC = 222,
    Armenia_NMC = 223,
    Austria_NMC = 224,
    Azerbaijan_NMC = 225,
    Belarus_NMC = 226,
    Belgium_NMC = 227,
    Bosnia_Herzegovina_NMC = 228,
    Bulgaria_NMC = 229,
    Cyprus_NMC = 230,
    Estonia_NMC = 231,
    Georgia_NMC = 232,
    Dublin = 233,
    Israel_NMC = 234,
    Jordan_NMC = 235,
    Latvia_NMC = 236,
    Lebanon_NMC = 237,
    Lithuania_NMC = 238,
    Luxembourg = 239,
    Malta_NMC = 240,
    Monaco = 241,
    Romania_NMC = 242,
    Syrian_Arab_Republic_NMC = 243,
    Macedonia_NMC = 244,
    Ukraine_NMC = 245,
    Moldova_NMC = 246,
    OPERA_EUMETNET = 247,
    Montenegro_NMC = 248,
    COSMO = 250,
    MetCoOp = 251,
    MPI_M = 252,
    EUMETSAT = 254,
    MISSING = 255
};

inline const char* center_to_text(Organization center){
    switch (center)
    {
    case Organization::WMO:
        return "WMO Secretariat";
    case Organization::Melbourne:
        return "Melbourne";
    case Organization::Melbourne_1:
        return "Melbourne";
    case Organization::Melbourne_2:
        return "Melbourne";
    case Organization::Moscow:
        return "Melbourne";
    case Organization::Moscow_1:
        return "Moscow";
    case Organization::Moscow_2:
        return "Moscow";
    case Organization::NCEP:
        return "US National Weather Service - National Centres for Environmental Prediction (NCEP)";
    case Organization::NWSTG:
        return "US National Weather Service Telecommunications Gateway (NWSTG)";
    case Organization::US_other:
        return "US National Weather Service - Other";
    case Organization::Cairo_RSMC:
        return "Cairo (RSMC)";
    case Organization::Cairo_RSMC_1:
        return "Cairo (RSMC)";
    case Organization::Dakar_RSMC:
        return "Dakar (RSMC)";
    case Organization::Dakar_RSMC_1:
        return "Dakar (RSMC)";
    case Organization::Nairobi_RSMC:
        return "Nairobi (RSMC)";
    case Organization::Nairobi_RSMC_1:
        return "Nairobi (RSMC)";
    case Organization::Casablanca_RSMC:
        return "Casablanca (RSMC)";
    case Organization::Tunis_RSMC:
        return "Tunis (RSMC)";
    case Organization::Tunis_Casablanca_RSMC:
        return "Tunis - Casablanca (RSMC)";
    case Organization::Tunis_Casablanca_RSMC_1:
        return "Tunis - Casablanca (RSMC)";
    case Organization::Las_Palmas:
        return "Las Palmas";
    case Organization::Algiers_RSMC:
        return "Algiers (RSMC)";
    case Organization::ACMAD:
        return "ACMAD";
    case Organization::Mozambique_NMC:
        return "Mozambique (NMC)";
    case Organization::Pretoria_RSMC:
        return "Pretoria (RSMC)";
    case Organization::Reunion_RSMC:
        return "La Réunion (RSMC)";
    case Organization::Khabarovsk_RSMC:
        return "Khabarovsk (RSMC)";
    case Organization::Khabarovsk_RSMC_1:
        return "Khabarovsk (RSMC)";
    case Organization::New_Dehli_RSMC:
        return "New Delhi (RSMC)";
    case Organization::New_Dehli_RSMC_1:
        return "New Delhi (RSMC)";
    case Organization::Novosibirsk_RSMC:
        return "Novosibirsk (RSMC)";
    case Organization::Novosibirsk_RSMC_1:
        return "Novosibirsk (RSMC)";
    case Organization::Tashkent_RSMC:
        return "Tashkent (RSMC)";
    case Organization::Jeddah_RSMC:
        return "Jeddah (RSMC)";
    case Organization::JMA:
        return "Tokyo (RSMC), Japan Meteorological Agency";
    case Organization::JMA_1:
        return "Tokyo (RSMC), Japan Meteorological Agency";
    case Organization::Bangkok:
        return "Bangkok";
    case Organization::Ulaanbaatar:
        return "Ulaanbaatar";
    case Organization::Beijing_RSMC:
        return "Beijing (RSMC)";
    case Organization::Beijing_RSMC_1:
        return "Beijing (RSMC)";
    case Organization::Seoul:
        return "Seoul";
    case Organization::Buenos_Aires_RSMC:
        return "Buenos Aires (RSMC)";
    case Organization::Buenos_Aires_RSMC_1:
        return "Buenos Aires (RSMC)";
    case Organization::Brasilia_RSMC:
        return "Brasilia (RSMC)";
    case Organization::Brasilia_RSMC_1:
        return "Brasilia (RSMC)";
    case Organization::Santiago:
        return "Santiago";
    case Organization::BSA_INPE:
        return "Brazilian Space Agency INPE";
    case Organization::Colombia_NMC:
        return "Colombia (NMC)";
    case Organization::Ecuador_NMC:
        return "Ecuador (NMC)";
    case Organization::Peru_NMC:
        return "Peru (NMC)";
    case Organization::Venezuela_NMC:
        return "Venezuela (Bolivarian Republic of) (NMC)";
    case Organization::Miami_RSMC:
        return "Miami (RSMC)";
    case Organization::Miami_RSMC_NHC:
        return "Miami (RSMC), National Hurricane Centre";
    case Organization::Montreal_RSMC:
        return "Montreal (RSMC)";
    case Organization::CMC://verify //Canadian Meteorological Centre
        return "Canadian Meteorological Centre";
    case Organization::San_Francisco:
        return "San Francisco";
    case Organization::ARINC_Center:
        return "ARINC Centre";
    case Organization::AFGWC:
        return "US Air Force - Air Force Global Weather Central";
    case Organization::FNMOC:
        return "Fleet Numerical Meteorology and Oceanography Center, Monterey, CA, United States";
    case Organization::NOAA:
        return "The NOAA Forecast Systems Laboratory, Boulder, CO, United States";
    case Organization::NCAR:
        return "United States National Center for Atmospheric Research (NCAR)";
    case Organization::ARGOS_Landover:
        return "Service ARGOS - Landover";
    case Organization::NOO://US Naval Oceanographic Office
        return "US Naval Oceanographic Office";
    case Organization::IRI:
        return "International Research Institute for Climate and Society (IRI)";
    case Organization::Honolulu_RSMC:
        return "Honolulu (RSMC)";
    case Organization::Darwin_RSMC:
        return "Darwin (RSMC)";
    case Organization::Darwin_RSMC_1:
        return "Darwin (RSMC)";
    case Organization::Melbourne_RSMC:
        return "Melbourne (RSMC)";
    case Organization::Wellington_RSMC:
        return "Wellington (RSMC)";
    case Organization::Wellington_RSMC_1:
        return "Wellington (RSMC)";
    case Organization::Nadi_RSMC:
        return "Nadi (RSMC)";
    case Organization::Singapore:
        return "Singapore";
    case Organization::Malaysia_NMC:
        return "Malaysia (NMC)";
    case Organization::UK_Meteorological_Office_Exeter_RSMC:
        return "UK Meteorological Office  Exeter (RSMC)";
    case Organization::UK_Meteorological_Office_Exeter_RSMC_1:
        return "UK Meteorological Office  Exeter (RSMC)";
    case Organization::Moscow_RSMC_2:
        return "Moscow (RSMC)";
    case Organization::DWD:
        return "Offenbach (RSMC)";
    case Organization::DWD_1:
        return "Offenbach (RSMC)";
    case Organization::Rome_RSMC:
        return "Rome (RSMC)";
    case Organization::Rome_RSMC_1:
        return "Rome (RSMC)";
    case Organization::Norrkoping:
        return "Norrköping";
    case Organization::Norrkoping_1:
        return "Norrköping";
    case Organization::Toulouse_RSMC:
        return "Toulouse (RSMC)";
    case Organization::Toulouse_RSMC_1:
        return "Toulouse (RSMC)";
    case Organization::Helsinki:
        return "Helsinki";
    case Organization::Belgrade:
        return "Belgrade";
    case Organization::Oslo:
        return "Oslo";
    case Organization::Prague:
        return "Prague";
    case Organization::Episkopi:
        return "Episkopi";
    case Organization::Ankara:
        return "Ankara";
    case Organization::Frankfurt_Main:
        return "Frankfurt/Main";
    case Organization::London_WAFC:
        return "London (WAFC)";
    case Organization::Copenhagen:
        return "Copenhagen";
    case Organization::Rota:
        return "Rota";
    case Organization::Athens:
        return "Athens";
    case Organization::ESA:
        return "European Space Agency (ESA)";
    case Organization::ECMWF:
        return "European Centre for Medium-Range Weather Forecasts (ECMWF) (RSMC)";
    case Organization::De_Bilt:
        return "De Bilt";
    case Organization::Brazzaville:
        return "Brazzaville";
    case Organization::Abidjan:
        return "Abidjan";
    case Organization::Libya_NMC:
        return "Libya (NMC)";
    case Organization::Madagascar_NMC:
        return "Madagascar (NMC)";
    case Organization::Mauritius_NMC:
        return "Mauritius (NMC)";
    case Organization::Niger_NMC:
        return "Niger (NMC)";
    case Organization::Seychelles_NMC:
        return "Seychelles (NMC)";
    case Organization::Uganda_NMC:
        return "Uganda (NMC)";
    case Organization::Tanzania_NMC:
        return "United Republic of Tanzania (NMC)";
    case Organization::Zimbabwe_NMC:
        return "Zimbabwe (NMC)";
    case Organization::Hong_Kong:
        return "Hong-Kong, China";
    case Organization::Afghanistan_NMC:
        return "Afghanistan (NMC)";
    case Organization::Bahrain_NMC:
        return "Bahrain (NMC)";
    case Organization::Bangladesh_NMC:
        return "Bangladesh (NMC)";
    case Organization::Bhutan_NMC:
        return "Bhutan (NMC)";
    case Organization::Cambodia_NMC:
        return "Cambodia (NMC)";
    case Organization::North_Korea_NMC:
        return "Democratic People's Republic of Korea (NMC)";
    case Organization::Iran_NMC:
        return "Islamic Republic of Iran (NMC)";
    case Organization::Iraq_NMC:
        return "Iraq (NMC)";
    case Organization::Kazakhstan_NMC:
        return "Kazakhstan (NMC)";
    case Organization::Kuwait_NMC:
        return "Kuwait (NMC)";
    case Organization::Kyrgyzstan_NMC:
        return "Kyrgyzstan (NMC)";
    case Organization::Laos_NMC:
        return "Lao People's Democratic Republic (NMC)";
    case Organization::Macao:
        return "Macao, China";
    case Organization::Maldives_NMC:
        return "Maldives (NMC)";
    case Organization::Myanmar_NMC:
        return "Myanmar (NMC)";
    case Organization::Nepal_NMC:
        return "Nepal (NMC)";
    case Organization::Oman_NMC:
        return "Oman (NMC)";
    case Organization::Pakistan_NMC:
        return "Pakistan (NMC)";
    case Organization::Qatar_NMC:
        return "Qatar (NMC)";
    case Organization::Yemen_NMC:
        return "Yemen (NMC)";
    case Organization::Sri_Lanka_NMC:
        return "Sri Lanka (NMC)";
    case Organization::Tajikistan_NMC:
        return "Tajikistan (NMC)";
    case Organization::Turkmenistan_NMC:
        return "Turkmenistan (NMC)";
    case Organization::United_Arab_Emirates_NMC:
        return "United Arab Emirates (NMC)";
    case Organization::Uzbekistan_NMC:
        return "Uzbekistan (NMC)";
    case Organization::Vietnam_NMC:
        return "Viet Nam (NMC)";
    case Organization::Bolivia_NMC:
        return "Bolivia (Plurinational State of) (NMC)";
    case Organization::Guyana_NMC:
        return "Guyana (NMC)";
    case Organization::Paraguay_NMC:
        return "Paraguay (NMC)";
    case Organization::Suriname_NMC:
        return "Suriname (NMC)";
    case Organization::Uruguay_NMC:
        return "Uruguay (NMC)";
    case Organization::French_Guiana:
        return "French Guiana";
    case Organization::BNHC:
        return "Brazilian Navy Hydrographic Centre";
    case Organization::CONAE:
        return "National Commission on Space Activities (CONAE) - Argentina";
    case Organization::Antigua_Barbuda_NMC:
        return "Antigua and Barbuda (NMC)";
    case Organization::Bahamas_NMC:
        return "Bahamas (NMC)";
    case Organization::Barbados_NMC:
        return "Barbados (NMC)";
    case Organization::Belize_NMC:
        return "Belize (NMC)";
    case Organization::BCT:
        return "British Caribbean Territories Centre";
    case Organization::San_Jose:
        return "San José";
    case Organization::Cuba_NMC:
        return "Cuba (NMC)";
    case Organization::Dominica_NMC:
        return "Dominica (NMC)";
    case Organization::Dominican_Republic_NMC:
        return "Dominican Republic (NMC)";
    case Organization::El_Salvador_NMC:
        return "El Salvador (NMC)";
    case Organization::NOAA_NESDIS:
        return "US NOAA/NESDIS";
    case Organization::NOAA_OAR:
        return "US NOAA Office of Oceanic and Atmospheric Research";
    case Organization::Guatemala_NMC:
        return "Guatemala (NMC)";
    case Organization::Haiti_NMC:
        return "Haiti (NMC)";
    case Organization::Honduras_NMC:
        return "Honduras (NMC)";
    case Organization::Jamaica_NMC:
        return "Jamaica (NMC)";
    case Organization::Mexico_City:
        return "Mexico City";
    case Organization::Curacao_and_Sint_Maarten_NMC:
        return "Curaçao and Sint Maarten (NMC)";
    case Organization::Nicaragua_NMC:
        return "Nicaragua (NMC)";
    case Organization::Panama_NMC:
        return "Panama (NMC)";
    case Organization::Saint_Lucia_NMC:
        return "Saint Lucia (NMC)";
    case Organization::Trinidad_Tobago:
        return "Trinidad and Tobago (NMC)";
    case Organization::French_Departments_RA:
        return "French Departments in RA IV";
    case Organization::NASA:
        return "US National Aeronautics and Space Administration (NASA)";
    case Organization::ISDM_MEDS:
        return "Integrated Science Data Management/Marine Environmental Data Service (ISDM/MEDS) - Canada";
    case Organization::UCAR:
        return "University Corporation for Atmospheric Research (UCAR) - United States";
    case Organization::CIMSS:
        return "Cooperative Institute for Meteorological Satellite Studies (CIMSS) - United States";
    case Organization::NOAA_National_Ocean_Service:
        return "NOAA National Ocean Service - United States";
    case Organization::Cook_Islands_NMC:
        return "Cook Islands (NMC)";
    case Organization::French_Polinesia_NMC:
        return "French Polynesia (NMC)";
    case Organization::Tonga_NMC:
        return "Tonga (NMC)";
    case Organization::Vanuatu_NMC:
        return "Vanuatu (NMC)";
    case Organization::Brunei_Darussalam_NMC:
        return "Brunei Darussalam (NMC)";
    case Organization::Indonesia_NMC:
        return "Indonesia (NMC)";
    case Organization::Kiribati_NMC:
        return "Kiribati (NMC)";
    case Organization::Micronesia_NMC:
        return "Federated States of Micronesia (NMC)";
    case Organization::New_Caledonia_NMC:
        return "New Caledonia (NMC)";
    case Organization::Niue:
        return "Niue";
    case Organization::Papua_New_Guinea_NMC:
        return "Papua New Guinea (NMC)";
    case Organization::Philipines_NMC:
        return "Philippines (NMC)";
    case Organization::Samoa_NMC:
        return "Samoa (NMC)";
    case Organization::Solomon_Islands_NMC:
        return "Solomon Islands (NMC)";
    case Organization::NIWA:
        return "National Institute of Water and Atmospheric Research (NIWA - New Zealand)";
    case Organization::Frascati_ESA_ESRIN:
        return "Frascati (ESA/ESRIN)";
    case Organization::Lannion:
        return "Lannion";
    case Organization::Lisbon:
        return "Lisbon";
    case Organization::Reykjavik:
        return "Reykjavik";
    case Organization::Madrid:
        return "Madrid";
    case Organization::Zurich:
        return "Zurich";
    case Organization::ARGOS_Toulouse:
        return "Service ARGOS - Toulouse";
    case Organization::Bratislava:
        return "Bratislava";
    case Organization::Budapest:
        return "Budapest";
    case Organization::Ljubljana:
        return "Ljubljana";
    case Organization::Warsaw:
        return "Warsaw";
    case Organization::Zagreb:
        return "Zagreb";
    case Organization::Albania_NMC:
        return "Albania (NMC)";
    case Organization::Armenia_NMC:
        return "Armenia (NMC)";
    case Organization::Austria_NMC:
        return "Austria (NMC)";
    case Organization::Azerbaijan_NMC:
        return "Azerbaijan (NMC)";
    case Organization::Belarus_NMC:
        return "Belarus (NMC)";
    case Organization::Belgium_NMC:
        return "Belgium (NMC)";
    case Organization::Bosnia_Herzegovina_NMC:
        return "Bosnia and Herzegovina (NMC)";
    case Organization::Bulgaria_NMC:
        return "Bulgaria (NMC)";
    case Organization::Cyprus_NMC:
        return "Cyprus (NMC)";
    case Organization::Estonia_NMC:
        return "Estonia (NMC)";
    case Organization::Georgia_NMC:
        return "Georgia (NMC)";
    case Organization::Dublin:
        return "Dublin";
    case Organization::Israel_NMC:
        return "Israel (NMC)";
    case Organization::Jordan_NMC:
        return "Jordan (NMC)";
    case Organization::Latvia_NMC:
        return "Latvia (NMC)";
    case Organization::Lebanon_NMC:
        return "Lebanon (NMC)";
    case Organization::Lithuania_NMC:
        return "Lithuania (NMC)";
    case Organization::Luxembourg:
        return "Luxembourg";
    case Organization::Malta_NMC:
        return "Malta (NMC)";
    case Organization::Monaco:
        return "Monaco";
    case Organization::Romania_NMC:
        return "Romania (NMC)";
    case Organization::Syrian_Arab_Republic_NMC:
        return "Syrian Arab Republic (NMC)";
    case Organization::Macedonia_NMC:
        return "The former Yugoslav Republic of Macedonia (NMC)";
    case Organization::Ukraine_NMC:
        return "Ukraine (NMC)";
    case Organization::Moldova_NMC:
        return "Republic of Moldova (NMC)";
    case Organization::OPERA_EUMETNET:
        return "Operational Programme for the Exchange of weather RAdar information (OPERA) - EUMETNET";
    case Organization::Montenegro_NMC:
        return "Montenegro (NMC)";
    case Organization::COSMO:
        return "COnsortium for Small scale MOdelling (COSMO)";
    case Organization::MetCoOp:
        return "Meteorological Cooperation on Operational NWP (MetCoOp)";
    case Organization::MPI_M:
        return "Max Planck Institute for Meteorology (MPI-M)";
    case Organization::EUMETSAT:
        return "EUMETSAT Operation Centre";
    case Organization::MISSING:
        return "Missing";
    default:
        return "Reserved";
        break;
    }
}

inline const char* center_to_abbr(Organization center){
    switch (center)
    {
    case Organization::WMO:
        return "WMO";
    case Organization::Melbourne:
        return "Melbourne";
    case Organization::Melbourne_1:
        return "Melbourne";
    case Organization::Melbourne_2:
        return "Melbourne";
    case Organization::Moscow:
        return "Melbourne";
    case Organization::Moscow_1:
        return "Moscow";
    case Organization::Moscow_2:
        return "Moscow";
    case Organization::NCEP:
        return "NCEP";
    case Organization::NWSTG:
        return "NWSTG";
    case Organization::US_other:
        return "USNWS";
    case Organization::Cairo_RSMC:
        return "Cairo";
    case Organization::Cairo_RSMC_1:
        return "Cairo";
    case Organization::Dakar_RSMC:
        return "Dakar";
    case Organization::Dakar_RSMC_1:
        return "Dakar";
    case Organization::Nairobi_RSMC:
        return "Nairobi";
    case Organization::Nairobi_RSMC_1:
        return "Nairobi";
    case Organization::Casablanca_RSMC:
        return "Casablanca";
    case Organization::Tunis_RSMC:
        return "Tunis";
    case Organization::Tunis_Casablanca_RSMC:
        return "TunisCasabl";
    case Organization::Tunis_Casablanca_RSMC_1:
        return "TunisCasabl";
    case Organization::Las_Palmas:
        return "LasPalmas";
    case Organization::Algiers_RSMC:
        return "Algiers";
    case Organization::ACMAD:
        return "ACMAD";
    case Organization::Mozambique_NMC:
        return "Mozambique";
    case Organization::Pretoria_RSMC:
        return "Pretoria";
    case Organization::Reunion_RSMC:
        return "LaRéunion";
    case Organization::Khabarovsk_RSMC:
        return "Khabarovsk";
    case Organization::Khabarovsk_RSMC_1:
        return "Khabarovsk";
    case Organization::New_Dehli_RSMC:
        return "NewDelhi";
    case Organization::New_Dehli_RSMC_1:
        return "NewDelhi";
    case Organization::Novosibirsk_RSMC:
        return "Novosibirsk";
    case Organization::Novosibirsk_RSMC_1:
        return "Novosibirsk";
    case Organization::Tashkent_RSMC:
        return "Tashkent";
    case Organization::Jeddah_RSMC:
        return "Jeddah";
    case Organization::JMA:
        return "JMA";
    case Organization::JMA_1:
        return "JMA";
    case Organization::Bangkok:
        return "Bangkok";
    case Organization::Ulaanbaatar:
        return "Ulaanbaatar";
    case Organization::Beijing_RSMC:
        return "Beijing";
    case Organization::Beijing_RSMC_1:
        return "Beijing";
    case Organization::Seoul:
        return "Seoul";
    case Organization::Buenos_Aires_RSMC:
        return "BuenosAires";
    case Organization::Buenos_Aires_RSMC_1:
        return "BuenosAires";
    case Organization::Brasilia_RSMC:
        return "Brasilia";
    case Organization::Brasilia_RSMC_1:
        return "Brasilia";
    case Organization::Santiago:
        return "Santiago";
    case Organization::BSA_INPE:
        return "BSA_INPE";
    case Organization::Colombia_NMC:
        return "Colombia";
    case Organization::Ecuador_NMC:
        return "Ecuador";
    case Organization::Peru_NMC:
        return "Peru";
    case Organization::Venezuela_NMC:
        return "Venezuela";
    case Organization::Miami_RSMC:
        return "Miami";
    case Organization::Miami_RSMC_NHC:
        return "Miami_NHC";
    case Organization::Montreal_RSMC:
        return "Montreal";
    case Organization::CMC://verify //Canadian Meteorological Centre
        return "CMC";
    case Organization::San_Francisco:
        return "SanFranc";
    case Organization::ARINC_Center:
        return "ARINC";
    case Organization::AFGWC:
        return "AFGWC";
    case Organization::FNMOC:
        return "FNMOC";
    case Organization::NOAA:
        return "NOAA";
    case Organization::NCAR:
        return "NCAR";
    case Organization::ARGOS_Landover:
        return "ARGOS";
    case Organization::NOO://US Naval Oceanographic Office
        return "NOO";
    case Organization::IRI:
        return "IRI";
    case Organization::Honolulu_RSMC:
        return "Honolulu";
    case Organization::Darwin_RSMC:
        return "Darwin";
    case Organization::Darwin_RSMC_1:
        return "Darwin";
    case Organization::Melbourne_RSMC:
        return "Melbourne";
    case Organization::Wellington_RSMC:
        return "Wellington";
    case Organization::Wellington_RSMC_1:
        return "Wellington";
    case Organization::Nadi_RSMC:
        return "Nadi";
    case Organization::Singapore:
        return "Singapore";
    case Organization::Malaysia_NMC:
        return "Malaysia";
    case Organization::UK_Meteorological_Office_Exeter_RSMC:
        return "UK_MOE";
    case Organization::UK_Meteorological_Office_Exeter_RSMC_1:
        return "UK_MOE";
    case Organization::Moscow_RSMC_2:
        return "Moscow";
    case Organization::DWD:
        return "Offenbach";
    case Organization::DWD_1:
        return "Offenbach";
    case Organization::Rome_RSMC:
        return "Rome";
    case Organization::Rome_RSMC_1:
        return "Rome";
    case Organization::Norrkoping:
        return "Norrköping";
    case Organization::Norrkoping_1:
        return "Norrköping";
    case Organization::Toulouse_RSMC:
        return "Toulouse";
    case Organization::Toulouse_RSMC_1:
        return "Toulouse";
    case Organization::Helsinki:
        return "Helsinki";
    case Organization::Belgrade:
        return "Belgrade";
    case Organization::Oslo:
        return "Oslo";
    case Organization::Prague:
        return "Prague";
    case Organization::Episkopi:
        return "Episkopi";
    case Organization::Ankara:
        return "Ankara";
    case Organization::Frankfurt_Main:
        return "Frankfurt_Main";
    case Organization::London_WAFC:
        return "WAFC";
    case Organization::Copenhagen:
        return "Copenhagen";
    case Organization::Rota:
        return "Rota";
    case Organization::Athens:
        return "Athens";
    case Organization::ESA:
        return "ESA";
    case Organization::ECMWF:
        return "ECMWF";
    case Organization::De_Bilt:
        return "DeBilt";
    case Organization::Brazzaville:
        return "Brazzaville";
    case Organization::Abidjan:
        return "Abidjan";
    case Organization::Libya_NMC:
        return "Libya";
    case Organization::Madagascar_NMC:
        return "Madagascar";
    case Organization::Mauritius_NMC:
        return "Mauritius";
    case Organization::Niger_NMC:
        return "Niger";
    case Organization::Seychelles_NMC:
        return "Seychelles";
    case Organization::Uganda_NMC:
        return "Uganda";
    case Organization::Tanzania_NMC:
        return "Tanzania";
    case Organization::Zimbabwe_NMC:
        return "Zimbabwe";
    case Organization::Hong_Kong:
        return "Hong_Kong";
    case Organization::Afghanistan_NMC:
        return "Afghanistan";
    case Organization::Bahrain_NMC:
        return "Bahrain";
    case Organization::Bangladesh_NMC:
        return "Bangladesh";
    case Organization::Bhutan_NMC:
        return "Bhutan";
    case Organization::Cambodia_NMC:
        return "Cambodia";
    case Organization::North_Korea_NMC:
        return "N_Korea";
    case Organization::Iran_NMC:
        return "Iran";
    case Organization::Iraq_NMC:
        return "Iraq";
    case Organization::Kazakhstan_NMC:
        return "Kazakhstan";
    case Organization::Kuwait_NMC:
        return "Kuwait";
    case Organization::Kyrgyzstan_NMC:
        return "Kyrgyzstan";
    case Organization::Laos_NMC:
        return "Laos";
    case Organization::Macao:
        return "Macao";
    case Organization::Maldives_NMC:
        return "Maldives";
    case Organization::Myanmar_NMC:
        return "Myanmar";
    case Organization::Nepal_NMC:
        return "Nepal";
    case Organization::Oman_NMC:
        return "Oman";
    case Organization::Pakistan_NMC:
        return "Pakistan";
    case Organization::Qatar_NMC:
        return "Qatar";
    case Organization::Yemen_NMC:
        return "Yemen";
    case Organization::Sri_Lanka_NMC:
        return "SriLanka";
    case Organization::Tajikistan_NMC:
        return "Tajikistan";
    case Organization::Turkmenistan_NMC:
        return "Turkmenistan";
    case Organization::United_Arab_Emirates_NMC:
        return "UAE";
    case Organization::Uzbekistan_NMC:
        return "Uzbekistan";
    case Organization::Vietnam_NMC:
        return "VietNam";
    case Organization::Bolivia_NMC:
        return "Bolivia";
    case Organization::Guyana_NMC:
        return "Guyana";
    case Organization::Paraguay_NMC:
        return "Paraguay";
    case Organization::Suriname_NMC:
        return "Suriname";
    case Organization::Uruguay_NMC:
        return "Uruguay";
    case Organization::French_Guiana:
        return "FrenchGuiana";
    case Organization::BNHC:
        return "BNHC";
    case Organization::CONAE:
        return "CONAE";
    case Organization::Antigua_Barbuda_NMC:
        return "Antigua_Barbuda";
    case Organization::Bahamas_NMC:
        return "Bahamas";
    case Organization::Barbados_NMC:
        return "Barbados";
    case Organization::Belize_NMC:
        return "Belize";
    case Organization::BCT:
        return "BCT";
    case Organization::San_Jose:
        return "San José";
    case Organization::Cuba_NMC:
        return "Cuba";
    case Organization::Dominica_NMC:
        return "Dominica";
    case Organization::Dominican_Republic_NMC:
        return "Dominicana";
    case Organization::El_Salvador_NMC:
        return "Salvador";
    case Organization::NOAA_NESDIS:
        return "NOAA_NESDIS";
    case Organization::NOAA_OAR:
        return "NOAA_OAR";
    case Organization::Guatemala_NMC:
        return "Guatemala";
    case Organization::Haiti_NMC:
        return "Haiti";
    case Organization::Honduras_NMC:
        return "Honduras";
    case Organization::Jamaica_NMC:
        return "Jamaica";
    case Organization::Mexico_City:
        return "Mexico";
    case Organization::Curacao_and_Sint_Maarten_NMC:
        return "Curaçao_SintMaarten";
    case Organization::Nicaragua_NMC:
        return "Nicaragua";
    case Organization::Panama_NMC:
        return "Panama";
    case Organization::Saint_Lucia_NMC:
        return "SaintLucia";
    case Organization::Trinidad_Tobago:
        return "Trinidad_Tobago";
    case Organization::French_Departments_RA:
        return "French_Departments_RA_IV";
    case Organization::NASA:
        return "NASA";
    case Organization::ISDM_MEDS:
        return "ISDM_MEDS";
    case Organization::UCAR:
        return "UCAR";
    case Organization::CIMSS:
        return "CIMSS";
    case Organization::NOAA_National_Ocean_Service:
        return "NOAA_NOS";
    case Organization::Cook_Islands_NMC:
        return "CookIslands";
    case Organization::French_Polinesia_NMC:
        return "FrenchPolynesia";
    case Organization::Tonga_NMC:
        return "Tonga";
    case Organization::Vanuatu_NMC:
        return "Vanuatu";
    case Organization::Brunei_Darussalam_NMC:
        return "BruneiDarussalam";
    case Organization::Indonesia_NMC:
        return "Indonesia";
    case Organization::Kiribati_NMC:
        return "Kiribati";
    case Organization::Micronesia_NMC:
        return "Micronesia";
    case Organization::New_Caledonia_NMC:
        return "NewCaledonia";
    case Organization::Niue:
        return "Niue";
    case Organization::Papua_New_Guinea_NMC:
        return "PapuaNewGuinea";
    case Organization::Philipines_NMC:
        return "Philippines";
    case Organization::Samoa_NMC:
        return "Samoa";
    case Organization::Solomon_Islands_NMC:
        return "SolomonIslands";
    case Organization::NIWA:
        return "NIWA";
    case Organization::Frascati_ESA_ESRIN:
        return "ESA_ESRIN";
    case Organization::Lannion:
        return "Lannion";
    case Organization::Lisbon:
        return "Lisbon";
    case Organization::Reykjavik:
        return "Reykjavik";
    case Organization::Madrid:
        return "Madrid";
    case Organization::Zurich:
        return "Zurich";
    case Organization::ARGOS_Toulouse:
        return "ARGOS_Toulouse";
    case Organization::Bratislava:
        return "Bratislava";
    case Organization::Budapest:
        return "Budapest";
    case Organization::Ljubljana:
        return "Ljubljana";
    case Organization::Warsaw:
        return "Warsaw";
    case Organization::Zagreb:
        return "Zagreb";
    case Organization::Albania_NMC:
        return "Albania";
    case Organization::Armenia_NMC:
        return "Armenia";
    case Organization::Austria_NMC:
        return "Austria";
    case Organization::Azerbaijan_NMC:
        return "Azerbaijan";
    case Organization::Belarus_NMC:
        return "Belarus";
    case Organization::Belgium_NMC:
        return "Belgium";
    case Organization::Bosnia_Herzegovina_NMC:
        return "Bosnia_Herzegovina";
    case Organization::Bulgaria_NMC:
        return "Bulgaria";
    case Organization::Cyprus_NMC:
        return "Cyprus";
    case Organization::Estonia_NMC:
        return "Estonia";
    case Organization::Georgia_NMC:
        return "Georgia";
    case Organization::Dublin:
        return "Dublin";
    case Organization::Israel_NMC:
        return "Israel";
    case Organization::Jordan_NMC:
        return "Jordan";
    case Organization::Latvia_NMC:
        return "Latvia";
    case Organization::Lebanon_NMC:
        return "Lebanon";
    case Organization::Lithuania_NMC:
        return "Lithuania";
    case Organization::Luxembourg:
        return "Luxembourg";
    case Organization::Malta_NMC:
        return "Malta";
    case Organization::Monaco:
        return "Monaco";
    case Organization::Romania_NMC:
        return "Romania";
    case Organization::Syrian_Arab_Republic_NMC:
        return "SAR";
    case Organization::Macedonia_NMC:
        return "Macedonia";
    case Organization::Ukraine_NMC:
        return "Ukraine";
    case Organization::Moldova_NMC:
        return "Moldova";
    case Organization::OPERA_EUMETNET:
        return "OPERA_EUMETNET";
    case Organization::Montenegro_NMC:
        return "Montenegro";
    case Organization::COSMO:
        return "COSMO";
    case Organization::MetCoOp:
        return "MetCoOp";
    case Organization::MPI_M:
        return "MPI_M";
    case Organization::EUMETSAT:
        return "EUMETSAT";
    case Organization::MISSING:
        return "Missing";
    default:
        return "Reserved";
        break;
    }
}

typedef enum{
    ERA_INTERIM = 128,
    ERA5 = 228
}ECMWFtable_version;