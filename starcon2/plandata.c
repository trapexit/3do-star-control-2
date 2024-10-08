#include "starcon.h"
#include "planets/paul/elemdata.h"

char	*cluster_array[] =
{
    "Vega",
    "Antliae",
    "Apodis",
    "Aquarii",
    "Aquilae",
    "Arae",
    "Arietis",
    "Aurigae",
    "Trianguli",
    "Caeli",
    "Camelopardalis",
    "Cancri",
    "Brahe",
    "Kepler",
    "Copernicus",
    "Capricorni",
    "Carinae",
    "Cassiopeiae",
    "Tucanae",
    "Cephei",
    "Ceti",
    "Crateris",
    "Circini",
    "Columbae",
    "Chandrasekhar",
    "Sagittae",
    "Cygnus",
    "Corvi",
    "Chamaeleonis",
    "Equulei",
    "Delphini",
    "Doradus",
    "Monocerotis",
    "Crucis",
    "Eridani",
    "Fornacis",
    "Geminorum",
    "Altair",
    "Antares",
    "Horologii",
    "Hydrae",
    "Andromedae",
    "Groombridge",
    "Lacertae",
    "Leonis",
    "Hyades",
    "Leporis",
    "Librae",
    "Lipi",
    "Lyncis",
    "Fomalhaut",
    "Menkar",
    "Microscopii",
    "Draconis",
    "Orionis",
    "Normae",
    "Octantis",
    "Ophiuchi",
    "Muscae",
    "Pavonis",
    "Pegasi",
    "Persei",
    "Phoenicis",
    "Pictoris",
    "Piscium",
    "Hyginus",
    "Puppis",
    "Pyxidis",
    "Reticuli",
    "Arianni",
    "Sagittarii",
    "Scorpii",
    "Sculptoris",
    "Scuti",
    "Serpentis",
    "Sextantis",
    "Tauri",
    "Telescopii",
    "Bootis",
    "Olber",
    "Centauri",
    "Ptolemae",
    "Gorno",
    "Velorum",
    "Virginis",
    "Volantis",
    "Vulpeculae",
    "Lalande",
    "Luyten",
    "Indi",
    "Lacaille",
    "Giclas",
    "Krueger",
    "Lyrae",
    "Wolf",
    "Saurus",
    "Raynet",
    "Zeeman",
    "Vela",
    "Mira",
    "Cerenkov",
    "Mersenne",
    "Maksutov",
    "Klystron",
    "Metis",
    "Mensae",
    "Illuminati",
    "Vitalis",
    "Herculis",
    "Gruis",
    "Squidi",
    "Almagest",
    "Alcor",
    "Algol",
    "Betelgeuse",
    "Aldebaran",
    "Achernar",
    "Procyon",
    "Rigel",
    "Bellatrix",
    "Mizar",
    "Hyperion",
    "Regulus",
    "Organon",
    "Pollux",
    "Capella",
    "Deneb",
    "Canopus",
    "Sirius",
    "Sol",
    "Arcturus",
    "Lentilis",
    "UNKNOWN",

    "Quasi Portal",
    "Talking Pet",
    "Utwig Bomb",
    "Sun Device",
    "Rosy Sphere",
    "Aqua Helix",
    "Clear Spindle",
    "Broken Ultron",
    "Broken Ultron",
    "Broken Ultron",
    "Perfect Ultron",
    "Shofixti Maidens",
    "Umgah Caster",
    "Burvix Caster",
    "1 DataPlate",
    "2 DataPlate",
    "3 DataPlate",
    "Taalo Shield",
    "Egg Case",
    "Egg Case",
    "Egg Case",
    "Syreen Shuttle",
    "VUX Beast",
    "Destruct Code",
    "Warp Pod",
    "Wimbli's Trident",
    "Glowing Rod",
    "Moon Base",

    "Hydrogen",
    "Helium",
    "Lithium",
    "Beryllium",
    "Boron",
    "Carbon",
    "Nitrogen",
    "Oxygen",
    "Fluorine",
    "Neon",
    "Sodium",
    "Magnesium",
    "Aluminum",
    "Silicon",
    "Phosphorus",
    "Sulfur",
    "Chlorine",
    "Argon",
    "Potassium",
    "Calcium",
    "Scandium",
    "Titanium",
    "Vanadium",
    "Chromium",
    "Manganese",
    "Iron",
    "Cobalt",
    "Nickel",
    "Copper",
    "Zinc",
    "Gallium",
    "Germanium",
    "Arsenic",
    "Selenium",
    "Bromine",
    "Krypton",
    "Rubidium",
    "Strontium",
    "Yttrium",
    "Zirconium",
    "Niobium",
    "Molybdenum",
    "Technetium",
    "Ruthenium",
    "Rhodium",
    "Palladium",
    "Silver",
    "Cadmium",
    "Indium",
    "Tin",
    "Antimony",
    "Tellurium",
    "Iodine",
    "Xenon",
    "Cesium",
    "Barium",
    "Lanthanum",
    "Cerium",
    "Praseo- dymium",
    "Neodymium",
    "Promethium",
    "Samarium",
    "Europium",
    "Gadolinium",
    "Terbium",
    "Dyprosium",
    "Holmium",
    "Erbium",
    "Thulium",
    "Ytterbium",
    "Lutetium",
    "Hafnium",
    "Tantalum",
    "Tungsten",
    "Rhenium",
    "Osmium",
    "Iridium",
    "Platinum",
    "Gold",
    "Mercury",
    "Thallium",
    "Lead",
    "Bismuth",
    "Polonium",
    "Astatine",
    "Radon",
    "Francium",
    "Radium",
    "Actinium",
    "Thorium",
    "Protactinium",
    "Uranium",
    "Neptunium",
    "Plutonium",

    "Ozone",
    "Free Radicals",
    "Carbon Dioxide",
    "Carbon Monoxide",
    "Ammonia",
    "Methane",
    "Sulfuric Acid",
    "Hydrochloric Acid",
    "Hydrocyanic Acid",
    "Formic Acid",
    "Phosphoric Acid",
    "Formaldehyde",
    "Cyano- acetylene",
    "Methanol",
    "Ethanol",
    "Silicon Monoxide",
    "Titanium Oxide",
    "Zirconium Oxide",
    "Water",
    "Silicon Compounds",
    "Metal Oxides",
    "Quantum Black Holes",
    "Neutronium",
    "Magnetic Monopoles",
    "Degenerate Matter",
    "Super Fluids",
    "Aguuti Nodules",
    "Iron Compounds",
    "Aluminum Compounds",
    "Nitrous Oxide",
    "Radioactives",
    "Hydro- carbons",
    "Carbon Compounds",
    "Antimatter",
    "Charon Dust",
    "Reisburg Helices",
    "Tzo Crystals",
    "Calcium Compounds",
    "Nitric Acid",

    "",
};

#define NUM_NAMES	((sizeof (ClusterNames) / sizeof (ClusterNames[0])) - 1)

STAR_DESC	starmap_array[] =
{
    {{5007,   35}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 6, 74},
    {{ 708,   41}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 7, 91},
    {{4714,   78}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 7, 74},
    {{2187,   83}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 0, 126},
    {{2814,   89}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 2, 82},
    {{4244,   91}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 0, 125},
    {{5652,   98}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 0, 124},
    {{2939,  116}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 3, 82},
    {{2771,  146}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 1, 82},
    {{5313,  150}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 6, 73},
    {{ 265,  156}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 5, 92},
    {{4529,  169}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 8, 74},
    {{4911,  180}, MAKE_STAR (GIANT_STAR, ORANGE_BODY, -1), 0, 1, 74},
    {{4747,  221}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 4, 74},
    {{9708,  250}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 0, 112},
    {{4861,  262}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 2, 74},
    {{2908,  269}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), SHOFIXTI_DEFINED, 4, 82},
    {{1855,  270}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 5, 81},
    {{7958,  270}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 2, 8},
    {{5160,  280}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 4, 73},
    {{ 570,  289}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 4, 92},
    {{4923,  294}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), YEHAT_DEFINED, 3, 74},
    {{2820,  301}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 5, 82},
    {{7934,  318}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 1, 8},
    {{8062,  318}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 3, 8},
    {{1116,  334}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 6, 91},
    {{ 803,  337}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 3, 91},
    {{1787,  338}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 6, 81},
    {{ 877,  340}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 4, 91},
    {{5338,  355}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 5, 73},
    {{5039,  373}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 1, 73},
    {{ 843,  380}, MAKE_STAR (GIANT_STAR, ORANGE_BODY, -1), 0, 1, 91},
    {{4872,  408}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 5, 74},
    {{1740,  423}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 7, 81},
    {{4596,  429}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 9, 74},
    {{ 843,  431}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 2, 91},
    {{2156,  440}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 4, 81},
    {{2004,  441}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 1, 81},
    {{ 530,  442}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 2, 92},
    {{ 958,  468}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 5, 91},
    {{2058,  475}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 2, 81},
    {{ 304,  477}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 1, 92},
    {{ 522,  525}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), PKUNK_DEFINED, 3, 92},
    {{2100,  554}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 3, 81},
    {{ 134,  565}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 6, 92},
    {{6858,  577}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), MYCON_TRAP_DEFINED, 0, 123},
    {{5014,  584}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 2, 73},
    {{5256,  608}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 3, 73},
    {{2411,  718}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 1, 9},
    {{2589,  741}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 2, 9},
    {{ 675,  742}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 8, 91},
    {{9292,  750}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 4, 5},
    {{1463,  779}, MAKE_STAR (GIANT_STAR, RED_BODY, -1), 0, 6, 80},
    {{3089,  782}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 4, 9},
    {{2854,  787}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 3, 9},
    {{3333,  801}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 5, 9},
    {{9237,  821}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 5, 5},
    {{9339,  843}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 1, 5},
    {{ 242,  857}, MAKE_STAR (GIANT_STAR, ORANGE_BODY, -1), 0, 3, 90},
    {{1515,  866}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 5, 80},
    {{4770,  895}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 5, 75},
    {{1412,  905}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 2, 80},
    {{4681,  916}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), RAINBOW_DEFINED, 6, 75},
    {{9333,  937}, MAKE_STAR (SUPER_GIANT_STAR, YELLOW_BODY, -1), MELNORME0_DEFINED, 2, 5},
    {{9419,  942}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 3, 5},
    {{ 230,  952}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 2, 90},
    {{ 146,  955}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 1, 90},
    {{4873,  968}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 4, 75},
    {{1559,  993}, MAKE_STAR (SUPER_GIANT_STAR, RED_BODY, -1), MELNORME1_DEFINED, 1, 80},
    {{1895, 1041}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 2, 93},
    {{4337, 1066}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 1, 75},
    {{3732, 1067}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 0, 122},
    {{1579, 1115}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 3, 80},
    {{4875, 1145}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 3, 75},
    {{4604, 1187}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 2, 75},
    {{5812, 1208}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 4, 72},
    {{1312, 1260}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 4, 80},
    {{1916, 1270}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 1, 93},
    {{6562, 1270}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 0, 121},
    {{ 416, 1301}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 0, 120},
    {{3958, 1354}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 2, 89},
    {{4000, 1363}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 1, 89},
    {{1752, 1450}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), SOL_DEFINED, 0, 129},
    {{2187, 1500}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 0, 127},
    {{1806, 1507}, MAKE_STAR (GIANT_STAR, WHITE_BODY, -1), 0, 0, 128},
    {{5708, 1520}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 5, 72},
    {{9469, 1548}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 2, 6},
    {{4333, 1562}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 3, 88},
    {{6041, 1562}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 1, 72},
    {{9375, 1583}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 1, 6},
    {{2881, 1614}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 2, 96},
    {{6083, 1625}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 3, 72},
    {{4250, 1645}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 1, 88},
    {{ 650, 1646}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 7, 85},
    {{9477, 1670}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 3, 6},
    {{2840, 1676}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 1, 96},
    {{9541, 1687}, MAKE_STAR (GIANT_STAR, RED_BODY, -1), 0, 4, 6},
    {{7395, 1687}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 3, 69},
    {{4333, 1687}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), VUX_DEFINED, 2, 88},
    {{9559, 1735}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 5, 6},
    {{ 736, 1737}, MAKE_STAR (GIANT_STAR, BLUE_BODY, -1), 0, 6, 85},
    {{1601, 1746}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 1, 94},
    {{7395, 1750}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 1, 69},
    {{ 951, 1770}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 1, 85},
    {{1666, 1812}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 2, 94},
    {{7187, 1833}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 2, 69},
    {{ 705, 1838}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 5, 85},
    {{1140, 1847}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 2, 85},
    {{6467, 1878}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 1, 71},
    {{2791, 1895}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 3, 96},
    {{6500, 1916}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 2, 71},
    {{5458, 1916}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 0, 119},
    {{1048, 1919}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 3, 85},
    {{3678, 1926}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 2, 99},
    {{3345, 1931}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), START_COLONY_DEFINED, 0, 98},
    {{8187, 1937}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 4, 7},
    {{3352, 1940}, MAKE_STAR (SUPER_GIANT_STAR, WHITE_BODY, -1), MELNORME2_DEFINED, 0, 97},
    {{ 977, 1953}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 4, 85},
    {{4221, 1986}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), MAIDENS_DEFINED, 1, 100},
    {{4500, 2000}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 2, 100},
    {{6833, 2000}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 1, 70},
    {{8163, 2009}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 3, 7},
    {{8080, 2011}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 2, 7},
    {{6036, 2035}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 4, 71},
    {{6479, 2062}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), EGG_CASE1_DEFINED, 3, 71},
    {{2104, 2083}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), ZOQ_SCOUT_DEFINED, 0, 118},
    {{8062, 2083}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 1, 7},
    {{ 270, 2187}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 1, 79},
    {{6500, 2208}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 6, 71},
    {{6291, 2208}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), MYCON_DEFINED, 5, 71},
    {{ 125, 2229}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 2, 79},
    {{ 312, 2250}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 3, 79},
    {{3884, 2262}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 3, 99},
    {{ 742, 2268}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), CHMMR_DEFINED, 0, 117},
    {{2306, 2285}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 1, 95},
    {{2402, 2309}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 2, 95},
    {{6395, 2312}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), SUN_DEVICE_DEFINED, 2, 12},
    {{8875, 2312}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 9, 61},
    {{3551, 2320}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 1, 99},
    {{6208, 2333}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 1, 12},
    {{3354, 2354}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 4, 99},
    {{9909, 2359}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 0, 111},
    {{2298, 2385}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 3, 95},
    {{7020, 2395}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 2, 70},
    {{9038, 2407}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 5, 61},
    {{9375, 2416}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 8, 61},
    {{6500, 2458}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 6, 12},
    {{ 217, 2509}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 4, 78},
    {{3641, 2512}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 2, 86},
    {{5625, 2520}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 3, 14},
    {{3713, 2537}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), ORZ_DEFINED, 3, 86},
    {{3587, 2566}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), ANDROSYNTH_DEFINED, 7, 86},
    {{9291, 2583}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 1, 61},
    {{3654, 2587}, MAKE_STAR (SUPER_GIANT_STAR, GREEN_BODY, -1), MELNORME3_DEFINED, 1, 86},
    {{3721, 2619}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), TAALO_PROTECTOR_DEFINED, 4, 86},
    {{5791, 2625}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 1, 14},
    {{6416, 2625}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 5, 12},
    {{6008, 2631}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), EGG_CASE0_DEFINED, 2, 14},
    {{3608, 2637}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 6, 86},
    {{3499, 2648}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 1, 87},
    {{9479, 2666}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 2, 61},
    {{3668, 2666}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 5, 86},
    {{ 229, 2666}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 1, 78},
    {{8895, 2687}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 7, 61},
    {{ 138, 2696}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 5, 78},
    {{5375, 2729}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 0, 116},
    {{6354, 2729}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), EGG_CASE2_DEFINED, 3, 12},
    {{6458, 2750}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 4, 12},
    {{2458, 2750}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 6, 106},
    {{ 351, 2758}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 2, 78},
    {{7083, 2770}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 3, 70},
    {{3759, 2778}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 4, 87},
    {{9333, 2791}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 3, 61},
    {{3400, 2804}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 2, 87},
    {{9469, 2806}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), DRUUGE_DEFINED, 6, 61},
    {{3619, 2830}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 3, 87},
    {{2208, 2854}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 7, 106},
    {{9250, 2854}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 4, 61},
    {{ 672, 2863}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 7, 78},
    {{ 167, 2875}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 6, 78},
    {{4030, 2887}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 2, 105},
    {{ 384, 2900}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 3, 78},
    {{2727, 2951}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 5, 106},
    {{4645, 2958}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 3, 105},
    {{5625, 2958}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 2, 13},
    {{8270, 2958}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 3, 66},
    {{8291, 2979}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 2, 66},
    {{6020, 2979}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), RAINBOW_DEFINED, 3, 13},
    {{6562, 3020}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 4, 70},
    {{2011, 3043}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 8, 106},
    {{8125, 3083}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 1, 66},
    {{2354, 3166}, MAKE_STAR (GIANT_STAR, YELLOW_BODY, -1), 0, 4, 106},
    {{3833, 3187}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 1, 105},
    {{5812, 3208}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 1, 13},
    {{9000, 3250}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 0, 113},
    {{ 291, 3250}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 1, 84},
    {{ 501, 3259}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 2, 84},
    {{ 791, 3270}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 3, 84},
    {{2354, 3291}, MAKE_STAR (SUPER_GIANT_STAR, RED_BODY, -1), MELNORME4_DEFINED, 1, 106},
    {{1104, 3333}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 4, 84},
    {{2687, 3333}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 3, 106},
    {{3187, 3375}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 2, 107},
    {{1758, 3418}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 2, 108},
    {{2520, 3437}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 2, 106},
    {{8437, 3458}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 1, 64},
    {{8770, 3458}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 4, 64},
    {{3000, 3500}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 1, 107},
    {{ 149, 3519}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 5, 76},
    {{8791, 3541}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 2, 64},
    {{2148, 3551}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 2, 109},
    {{7375, 3562}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 0, 115},
    {{9312, 3562}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 2, 63},
    {{9599, 3583}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 4, 63},
    {{9375, 3604}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 3, 63},
    {{  90, 3614}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 6, 76},
    {{2770, 3625}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 4, 107},
    {{8708, 3625}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 3, 64},
    {{ 267, 3645}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 3, 76},
    {{1604, 3645}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 1, 108},
    {{2274, 3663}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 1, 109},
    {{ 229, 3666}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), ILWRATH_DEFINED, 1, 76},
    {{3083, 3674}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 3, 107},
    {{2416, 3687}, MAKE_STAR (GIANT_STAR, ORANGE_BODY, -1), SPATHI_DEFINED, 5, 109},
    {{9333, 3708}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 1, 63},
    {{2250, 3708}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 3, 109},
    {{ 288, 3735}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 2, 76},
    {{2354, 3741}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 4, 109},
    {{2583, 3750}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 6, 109},
    {{4125, 3770}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), SYREEN_DEFINED, 0, 114},
    {{ 166, 3770}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 4, 76},
    {{6270, 3833}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 4, 10},
    {{2145, 3916}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 1, 110},
    {{6125, 3937}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 3, 10},
    {{6291, 3937}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 9, 10},
    {{5937, 3937}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), SHIP_VAULT_DEFINED, 5, 10},
    {{2479, 3958}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 7, 109},
    {{ 926, 3972}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 2, 83},
    {{2062, 3991}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 2, 110},
    {{5895, 4020}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 8, 10},
    {{ 285, 4020}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 1, 77},
    {{6062, 4041}, MAKE_STAR (GIANT_STAR, YELLOW_BODY, -1), 0, 1, 10},
    {{2875, 4041}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 2, 20},
    {{8645, 4062}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 2, 65},
    {{ 860, 4065}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 1, 83},
    {{5958, 4083}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 2, 10},
    {{3038, 4083}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 3, 20},
    {{ 291, 4104}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 2, 77},
    {{6166, 4125}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 6, 10},
    {{9812, 4145}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 3, 62},
    {{8520, 4166}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 1, 65},
    {{9573, 4182}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 2, 62},
    {{ 500, 4187}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 3, 77},
    {{2145, 4208}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 3, 110},
    {{6208, 4229}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 7, 10},
    {{2812, 4250}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 7, 20},
    {{2937, 4306}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 4, 20},
    {{9416, 4395}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 1, 62},
    {{2875, 4479}, MAKE_STAR (GIANT_STAR, WHITE_BODY, -1), 0, 1, 20},
    {{ 250, 4583}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 5, 26},
    {{7250, 4583}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 4, 68},
    {{ 479, 4583}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 1, 26},
    {{5708, 4604}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 0, 104},
    {{ 479, 4645}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 2, 26},
    {{2895, 4687}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 6, 20},
    {{2708, 4708}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 5, 20},
    {{ 562, 4708}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 3, 26},
    {{ 416, 4717}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 4, 26},
    {{5094, 4931}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 4, 11},
    {{9000, 5000}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 2, 67},
    {{8958, 5000}, MAKE_STAR (GIANT_STAR, BLUE_BODY, -1), 0, 1, 67},
    {{5006, 5011}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 3, 11},
    {{7312, 5062}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 1, 68},
    {{3679, 5068}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 3, 17},
    {{9062, 5083}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 3, 67},
    {{7416, 5083}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), RAINBOW_DEFINED, 3, 68},
    {{5155, 5122}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 2, 11},
    {{3875, 5145}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 4, 17},
    {{4937, 5145}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 1, 11},
    {{2979, 5166}, MAKE_STAR (GIANT_STAR, ORANGE_BODY, -1), 0, 1, 15},
    {{3035, 5178}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 2, 15},
    {{3994, 5185}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 5, 17},
    {{3541, 5187}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 1, 17},
    {{5977, 5246}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 1, 102},
    {{3770, 5250}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 2, 17},
    {{1520, 5261}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 3, 55},
    {{1613, 5279}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 4, 55},
    {{7020, 5291}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 2, 68},
    {{1416, 5315}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 2, 55},
    {{2993, 5318}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 3, 15},
    {{1425, 5404}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 1, 55},
    {{1854, 5416}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 5, 55},
    {{3625, 5437}, MAKE_STAR (GIANT_STAR, GREEN_BODY, -1), 0, 1, 16},
    {{3416, 5437}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 2, 16},
    {{4000, 5437}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), ZOQFOT_DEFINED, 1, 18},
    {{6270, 5479}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 2, 102},
    {{3583, 5479}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 3, 16},
    {{4083, 5513}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 3, 18},
    {{2159, 5614}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 6, 55},
    {{3937, 5625}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 2, 18},
    {{6014, 5632}, MAKE_STAR (GIANT_STAR, BLUE_BODY, -1), 0, 1, 21},
    {{ 250, 5687}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 1, 25},
    {{3625, 5750}, MAKE_STAR (GIANT_STAR, RED_BODY, -1), 0, 2, 19},
    {{ 371, 5772}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 2, 25},
    {{6107, 5785}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 2, 21},
    {{9645, 5791}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), BURVIXESE_DEFINED, 0, 130},
    {{1545, 5818}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 5, 54},
    {{3750, 5833}, MAKE_STAR (GIANT_STAR, GREEN_BODY, -1), 0, 1, 19},
    {{6301, 5875}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 5, 21},
    {{1923, 5878}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 3, 54},
    {{4625, 5895}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 2, 131},
    {{ 152, 5900}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 3, 25},
    {{5437, 5916}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 4, 33},
    {{1714, 5926}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 4, 54},
    {{6200, 5935}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), SAMATRA_DEFINED, 4, 21},
    {{6429, 5958}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 7, 21},
    {{4729, 5958}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 4, 131},
    {{1978, 5968}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), TALKING_PET_DEFINED, 2, 54},
    {{ 395, 5979}, MAKE_STAR (GIANT_STAR, GREEN_BODY, -1), 0, 1, 22},
    {{ 563, 5980}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 5, 22},
    {{ 456, 5989}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 6, 22},
    {{4625, 6000}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 1, 131},
    {{6166, 6000}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 3, 21},
    {{6496, 6032}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 6, 21},
    {{2228, 6038}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 12, 54},
    {{4583, 6041}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 3, 131},
    {{1558, 6058}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 6, 54},
    {{1902, 6065}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 1, 54},
    {{2159, 6073}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 11, 54},
    {{ 365, 6093}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 2, 22},
    {{ 541, 6145}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 4, 22},
    {{2200, 6176}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 10, 54},
    {{ 729, 6208}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 3, 23},
    {{5250, 6229}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 3, 33},
    {{8166, 6250}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 3, 40},
    {{6215, 6255}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 8, 21},
    {{ 437, 6270}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 3, 22},
    {{5583, 6291}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 1, 33},
    {{1881, 6308}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 8, 54},
    {{1795, 6329}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 7, 54},
    {{2118, 6379}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 9, 54},
    {{ 750, 6458}, MAKE_STAR (GIANT_STAR, WHITE_BODY, -1), 0, 1, 23},
    {{3716, 6458}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 1, 30},
    {{1360, 6489}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 2, 56},
    {{7333, 6500}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 1, 40},
    {{3770, 6500}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 2, 30},
    {{4500, 6500}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 0, 37},
    {{ 187, 6520}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 4, 24},
    {{ 125, 6541}, MAKE_STAR (GIANT_STAR, RED_BODY, -1), 0, 1, 24},
    {{7812, 6562}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 2, 40},
    {{ 770, 6602}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 2, 23},
    {{5910, 6624}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 1, 29},
    {{ 208, 6625}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 3, 24},
    {{2604, 6645}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 1, 101},
    {{1578, 6668}, MAKE_STAR (SUPER_GIANT_STAR, GREEN_BODY, -1), MELNORME5_DEFINED, 1, 56},
    {{5479, 6687}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 2, 33},
    {{ 375, 6716}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 2, 24},
    {{ 312, 6728}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 5, 24},
    {{6020, 6729}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 2, 29},
    {{5062, 6750}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 10, 28},
    {{4208, 6854}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 1, 31},
    {{5145, 6875}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 9, 28},
    {{4291, 6937}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 2, 31},
    {{5145, 6958}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 8, 28},
    {{7208, 7000}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 3, 39},
    {{8625, 7000}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), RAINBOW_DEFINED, 1, 41},
    {{4955, 7034}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 2, 28},
    {{4895, 7041}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 3, 28},
    {{4971, 7104}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 1, 28},
    {{8666, 7104}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 2, 41},
    {{4854, 7125}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 4, 28},
    {{5083, 7145}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 7, 28},
    {{7360, 7184}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 4, 39},
    {{1020, 7187}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 3, 58},
    {{3875, 7187}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 1, 32},
    {{4879, 7201}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 5, 28},
    {{4958, 7229}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 6, 28},
    {{7125, 7250}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 2, 39},
    {{7532, 7258}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 5, 39},
    {{2416, 7291}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 2, 101},
    {{3854, 7291}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 2, 32},
    {{9687, 7333}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 3, 44},
    {{ 395, 7458}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), RAINBOW_DEFINED, 2, 60},
    {{4895, 7458}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 3, 36},
    {{4645, 7479}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 4, 36},
    {{6940, 7514}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 11, 39},
    {{7443, 7538}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 6, 39},
    {{6479, 7541}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 0, 38},
    {{7208, 7541}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 1, 39},
    {{5791, 7583}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 3, 34},
    {{ 333, 7625}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 1, 60},
    {{5958, 7645}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 2, 34},
    {{1041, 7708}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 2, 58},
    {{5875, 7729}, MAKE_STAR (SUPER_GIANT_STAR, YELLOW_BODY, -1), MELNORME6_DEFINED, 1, 34},
    {{1125, 7791}, MAKE_STAR (GIANT_STAR, BLUE_BODY, -1), 0, 1, 58},
    {{4979, 7791}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 2, 36},
    {{4958, 7791}, MAKE_STAR (GIANT_STAR, WHITE_BODY, -1), 0, 1, 36},
    {{6889, 7803}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 10, 39},
    {{7200, 7849}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 8, 39},
    {{7395, 7854}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 7, 39},
    {{9437, 7854}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 2, 44},
    {{2836, 7857}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), RAINBOW_DEFINED, 5, 53},
    {{5375, 7875}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 1, 35},
    {{6187, 7875}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 7, 35},
    {{6041, 7916}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 5, 35},
    {{5979, 7979}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 4, 35},
    {{7083, 7993}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 9, 39},
    {{3270, 8000}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 8, 53},
    {{6104, 8000}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 6, 35},
    {{ 687, 8000}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 2, 59},
    {{ 562, 8000}, MAKE_STAR (GIANT_STAR, GREEN_BODY, -1), URQUAN_WRECK_DEFINED, 1, 59},
    {{5645, 8020}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 2, 35},
    {{1395, 8041}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 4, 58},
    {{8229, 8041}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 2, 43},
    {{2518, 8056}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 3, 53},
    {{5875, 8062}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 3, 35},
    {{8416, 8083}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 1, 43},
    {{9000, 8229}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 1, 44},
    {{3562, 8250}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 9, 53},
    {{5437, 8270}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), RAINBOW_DEFINED, 5, 48},
    {{1520, 8333}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 5, 58},
    {{2771, 8351}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 1, 53},
    {{2535, 8358}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), THRADD_DEFINED, 4, 53},
    {{3151, 8390}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 7, 53},
    {{2362, 8395}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 11, 53},
    {{2822, 8395}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 2, 53},
    {{5500, 8395}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 2, 48},
    {{2536, 8504}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 4, 2},
    {{2582, 8507}, MAKE_STAR (SUPER_GIANT_STAR, YELLOW_BODY, -1), MELNORME7_DEFINED, 1, 2},
    {{8625, 8562}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 6, 3},
    {{4375, 8562}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 0, 50},
    {{2593, 8569}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 2, 2},
    {{2562, 8572}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 3, 2},
    {{8492, 8578}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 7, 3},
    {{1125, 8583}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 6, 58},
    {{8073, 8588}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 1, 46},
    {{8560, 8638}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 1, 3},
    {{8750, 8645}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 5, 3},
    {{5562, 8645}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 1, 48},
    {{2588, 8653}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 14, 53},
    {{2458, 8666}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 10, 53},
    {{7666, 8666}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), RAINBOW_DEFINED, 2, 46},
    {{2776, 8673}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), AQUA_HELIX_DEFINED, 6, 53},
    {{8630, 8693}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), UTWIG_DEFINED, 2, 3},
    {{2310, 8702}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 12, 53},
    {{ 437, 8770}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 1, 57},
    {{8534, 8797}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), RAINBOW_DEFINED, 3, 3},
    {{8588, 8812}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 4, 3},
    {{7187, 8812}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 3, 46},
    {{5475, 8823}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 3, 48},
    {{3050, 8833}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 4, 1},
    {{2831, 8854}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 3, 1},
    {{2300, 8861}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 13, 53},
    {{ 479, 8875}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 2, 57},
    {{2706, 8910}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 2, 1},
    {{ 333, 8916}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 3, 57},
    {{2535, 8917}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 5, 1},
    {{8322, 8934}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 1, 45},
    {{8249, 8958}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 4, 45},
    {{8375, 8958}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 2, 45},
    {{5645, 8979}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 4, 48},
    {{2687, 9000}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 1, 1},
    {{8375, 9041}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 3, 45},
    {{9960, 9042}, MAKE_STAR (GIANT_STAR, WHITE_BODY, -1), RAINBOW_DEFINED, 0, 42},
    {{7354, 9062}, MAKE_STAR (DWARF_STAR, BLUE_BODY, -1), 0, 1, 47},
    {{7833, 9083}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 6, 47},
    {{2581, 9105}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 6, 1},
    {{7545, 9107}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 3, 47},
    {{7414, 9124}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), SUPOX_DEFINED, 2, 47},
    {{8500, 9125}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 5, 45},
    {{ 104, 9125}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 5, 27},
    {{7889, 9181}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 7, 47},
    {{7791, 9187}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 4, 47},
    {{7791, 9229}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 5, 47},
    {{4812, 9270}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 0, 51},
    {{8500, 9372}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), BOMB_DEFINED, 6, 45},
    {{7255, 9374}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 11, 45},
    {{8458, 9393}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 7, 45},
    {{1000, 9395}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 4, 27},
    {{5711, 9475}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 2, 49},
    {{  62, 9479}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 3, 27},
    {{5989, 9496}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 7, 49},
    {{8000, 9505}, MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 9, 45},
    {{5329, 9538}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 5, 49},
    {{2916, 9541}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 2, 103},
    {{8296, 9548}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 8, 45},
    {{5600, 9552}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 1, 49},
    {{7664, 9589}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 10, 45},
    {{6125, 9604}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 8, 49},
    {{9144, 9686}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 4, 4},
    {{5781, 9711}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 3, 49},
    {{5229, 9729}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 6, 49},
    {{9120, 9741}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 3, 4},
    {{9186, 9741}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 2, 4},
    {{9159, 9745}, MAKE_STAR (SUPER_GIANT_STAR, BLUE_BODY, -1), MELNORME8_DEFINED, 1, 4},
    {{ 333, 9750}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 0, 0},
    {{9147, 9790}, MAKE_STAR (DWARF_STAR, ORANGE_BODY, -1), 0, 5, 4},
    {{5704, 9795}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), VUX_BEAST_DEFINED, 4, 49},
    {{ 333, 9812}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), SLYLANDRO_DEFINED, 2, 27},
    {{1020, 9937}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 7, 27},
    {{  83, 9979}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 6, 27},
    {{1937, 9979}, MAKE_STAR (DWARF_STAR, RED_BODY, -1), 0, 1, 103},
    {{4395, 9979}, MAKE_STAR (DWARF_STAR, GREEN_BODY, -1), 0, 0, 52},

    {{MAX_X_UNIVERSE << 1, MAX_Y_UNIVERSE << 1}, 0, 0, 0, 0},

#define VORTEX_SCALE	20
    {{(-12* VORTEX_SCALE) + 5000, (-21 * VORTEX_SCALE) + 5000},
	    MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 0, 132},
    {{( 1 * VORTEX_SCALE) + 5000, (-20 * VORTEX_SCALE) + 5000},
	    MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 0, 132},
    {{(-16 * VORTEX_SCALE) + 5000, (-18 * VORTEX_SCALE) + 5000},
	    MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 0, 132},
    {{( 8 * VORTEX_SCALE) + 5000, (-17 * VORTEX_SCALE) + 5000},
	    MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 0, 132},
    {{( 3 * VORTEX_SCALE) + 5000, (-13 * VORTEX_SCALE) + 5000},
	    MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 0, 132},
    {{(-21 * VORTEX_SCALE) + 5000, (-4 * VORTEX_SCALE) + 5000},
	    MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 0, 132},
    {{(-4 * VORTEX_SCALE) + 5000, (-4 * VORTEX_SCALE) + 5000},
	    MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 0, 132},
    {{(-12 * VORTEX_SCALE) + 5000, (-2 * VORTEX_SCALE) + 5000},
	    MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 0, 132},
    {{(-26 * VORTEX_SCALE) + 5000, (2 * VORTEX_SCALE) + 5000},
	    MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 0, 132},
    {{(-17 * VORTEX_SCALE) + 5000, (7 * VORTEX_SCALE) + 5000},
	    MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 0, 132},
    {{(10 * VORTEX_SCALE) + 5000, (7 * VORTEX_SCALE) + 5000},
	    MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 0, 132},
    {{(15 * VORTEX_SCALE) + 5000, (14 * VORTEX_SCALE) + 5000},
	    MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 0, 132},
    {{(22 * VORTEX_SCALE) + 5000, (16 * VORTEX_SCALE) + 5000},
	    MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 0, 132},
    {{(-6 * VORTEX_SCALE) + 5000, (19 * VORTEX_SCALE) + 5000},
	    MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 0, 132},
    {{(10 * VORTEX_SCALE) + 5000, (20 * VORTEX_SCALE) + 5000},
	    MAKE_STAR (DWARF_STAR, WHITE_BODY, -1), 0, 0, 132},

    {{6134, 5900}, MAKE_STAR (DWARF_STAR, YELLOW_BODY, -1), 0, 0, 132},

    {{MAX_X_UNIVERSE << 1, MAX_Y_UNIVERSE << 1}, 0, 0, 0, 0},
};

CONST BYTE	element_array[NUMBER_OF_ELEMENTS] =
{
    COMMON,	/* HYDROGEN */
    COMMON,	/* HELIUM */
    COMMON,	/* LITHIUM */
    BASE_METAL,	/* BERYLLIUM */
    BASE_METAL,	/* BORON */
    COMMON,	/* CARBON */
    COMMON,	/* NITROGEN */
    CORROSIVE,	/* OXYGEN */
    CORROSIVE,	/* FLUORINE */
    NOBLE,	/* NEON */
    BASE_METAL,	/* SODIUM */
    BASE_METAL,	/* MAGNESIUM */
    BASE_METAL,	/* ALUMINUM */
    COMMON,	/* SILICON */
    COMMON,	/* PHOSPHORUS */
    CORROSIVE,	/* SULFUR */
    CORROSIVE,	/* CHLORINE */
    NOBLE,	/* ARGON */
    BASE_METAL,	/* POTASSIUM */
    BASE_METAL,	/* CALCIUM */
    BASE_METAL,	/* SCANDIUM */
    BASE_METAL,	/* TITANIUM */
    BASE_METAL,	/* VANADIUM */
    BASE_METAL,	/* CHROMIUM */
    BASE_METAL,	/* MANGANESE */
    BASE_METAL,	/* IRON */
    BASE_METAL,	/* COBALT */
    BASE_METAL,	/* NICKEL */
    BASE_METAL,	/* COPPER */
    BASE_METAL,	/* ZINC */
    BASE_METAL,	/* GALLIUM */
    BASE_METAL,	/* GERMANIUM */
    COMMON,	/* ARSENIC */
    COMMON,	/* SELENIUM */
    CORROSIVE,	/* BROMINE */
    NOBLE,	/* KRYPTON */
    BASE_METAL,	/* RUBIDIUM */
    BASE_METAL,	/* STRONTIUM */
    BASE_METAL,	/* YTTRIUM */
    BASE_METAL,	/* ZIRCONIUM */
    BASE_METAL,	/* NIOBIUM */
    BASE_METAL,	/* MOLYBDENUM */
    RADIOACTIVE,	/* TECHNETIUM */
    BASE_METAL,	/* RUTHENIUM */
    BASE_METAL,	/* RHODIUM */
    PRECIOUS,	/* PALLADIUM */
    PRECIOUS,	/* SILVER */
    BASE_METAL,	/* CADMIUM */
    BASE_METAL,	/* INDIUM */
    BASE_METAL,	/* TIN */
    BASE_METAL,	/* ANTIMONY */
    BASE_METAL,	/* TELLURIUM */
    CORROSIVE,	/* IODINE */
    NOBLE,	/* XENON */
    BASE_METAL,	/* CESIUM */
    BASE_METAL,	/* BARIUM */
    RARE_EARTH,	/* LANTHANUM */
    RARE_EARTH,	/* CERIUM */
    RARE_EARTH,	/* PRASEODYMIUM */
    RARE_EARTH,	/* NEODYMIUM */
    RARE_EARTH,	/* PROMETHIUM */
    RARE_EARTH,	/* SAMARIUM */
    RARE_EARTH,	/* EUROPIUM */
    RARE_EARTH,	/* GADOLINIUM */
    RARE_EARTH,	/* TERBIUM */
    RARE_EARTH,	/* DYPROSIUM */
    RARE_EARTH,	/* HOLMIUM */
    RARE_EARTH,	/* ERBIUM */
    RARE_EARTH,	/* THULIUM */
    RARE_EARTH,	/* YTTERBIUM */
    RARE_EARTH,	/* LUTETIUM */
    BASE_METAL,	/* HAFNIUM */
    BASE_METAL,	/* TANTALUM */
    BASE_METAL,	/* TUNGSTEN */
    BASE_METAL,	/* RHENIUM */
    BASE_METAL,	/* OSMIUM */
    PRECIOUS,	/* IRIDIUM */
    PRECIOUS,	/* PLATINUM */
    PRECIOUS,	/* GOLD */
    BASE_METAL,	/* MERCURY */
    BASE_METAL,	/* THALLIUM */
    BASE_METAL,	/* LEAD */
    BASE_METAL,	/* BISMUTH */
    RADIOACTIVE,	/* POLONIUM */
    RADIOACTIVE,	/* ASTATINE */
    NOBLE,	/* RADON */
    RADIOACTIVE,	/* FRANCIUM */
    RADIOACTIVE,	/* RADIUM */
    RADIOACTIVE,	/* ACTINIUM */
    RADIOACTIVE,	/* THORIUM */
    RADIOACTIVE,	/* PROTACTINIUM */
    RADIOACTIVE,	/* URANIUM */
    RADIOACTIVE,	/* NEPTUNIUM */
    RADIOACTIVE,	/* PLUTONIUM */

    COMMON,	/* OZONE */
    COMMON,	/* FREE RADICALS */
    COMMON,	/* CARBON DIOXIDE */
    COMMON,	/* CARBON MONOXIDE */
    COMMON,	/* AMMONIA */
    COMMON,	/* METHANE */
    COMMON,	/* SULFURIC ACID */
    COMMON,	/* HYDROCHLORIC ACID */
    COMMON,	/* HYDROCYANIC ACID */
    COMMON,	/* FORMIC ACID */
    COMMON,	/* PHOSPHORIC ACID */
    COMMON,	/* FORMALDEHYDE */
    COMMON,	/* CYANOACETYLENE */
    COMMON,	/* METHANOL */
    COMMON,	/* ETHANOL */
    COMMON,	/* SILICON MONOXIDE */
    COMMON,	/* TITANIUM OXIDE */
    COMMON,	/* ZIRCONIUM OXIDE */
    COMMON,	/* WATER */
    COMMON,	/* SILICON COMPOUNDS */
    COMMON,	/* METAL OXIDES */
    EXOTIC,	/* QUANTUM BLACK HOLES */
    EXOTIC,	/* NEUTRONIUM */
    EXOTIC,	/* MAGNETIC MONOPOLES */
    EXOTIC,	/* DEGENERATE MATTER */
    EXOTIC,	/* SUPER FLUIDS */
    EXOTIC,	/* AGUUTI NODULES */
    COMMON,	/* IRON COMPOUNDS */
    COMMON,	/* ALUMINUM COMPOUNDS */
    COMMON,	/* NITROUS OXIDE */
    COMMON,	/* RADIOACTIVES */
    COMMON,	/* HYDROCARBONS */
    COMMON,	/* CARBON COMPOUNDS */
    EXOTIC,	/* ANTIMATTER */
    EXOTIC,	/* CHARON DUST */
    EXOTIC,	/* REISBURG HELICES */
    EXOTIC,	/* TZO CRYSTALS */
    COMMON,	/* CALCIUM COMPOUNDS */
    COMMON,	/* NITRIC ACID */
};

/*------------------------------ Global Data ------------------------------ */
#define NO_TECTONICS		0
#define LOW_TECTONICS		40
#define MED_TECTONICS		80
#define HIGH_TECTONICS		140
#define SUPER_TECTONICS		200

#define NO_DEPOSIT		0

#define TRACE_USEFUL		MINERAL_DEPOSIT (FEW, LIGHT)
#define LIGHT_USEFUL		MINERAL_DEPOSIT (MODERATE, LIGHT)
#define MEDIUM_USEFUL		MINERAL_DEPOSIT (MODERATE, MEDIUM)
#define HEAVY_USEFUL		MINERAL_DEPOSIT (MODERATE, HEAVY)
#define HUGE_USEFUL		MINERAL_DEPOSIT (NUMEROUS, HEAVY)

CONST PlanetFrame	planet_array[NUMBER_OF_PLANET_TYPES] =
{
    {		/* OOLITE_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + CRATERED_ALGO, 
		VIOLET_BODY),	/* Color and type/size of planet			*/
	LOW_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (HIGH_DENSITY, HEAVY),			/* Atmosphere and density */
	{
	    {HOLMIUM, MEDIUM_USEFUL},
	    {ERBIUM, MEDIUM_USEFUL},
	    {THULIUM, MEDIUM_USEFUL},
	    {YTTERBIUM, MEDIUM_USEFUL},
	    {LUTETIUM, MEDIUM_USEFUL},
	    {PALLADIUM, MEDIUM_USEFUL},
	    {SILVER, MEDIUM_USEFUL},
	    {IRIDIUM, MEDIUM_USEFUL},
	},
	OOLITE_COLOR_TAB,
	OOLITE_XLAT_TAB,
	230, 2, 200, 150,
    },
    {		/* YTTRIC_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + CRATERED_ALGO, 
		VIOLET_BODY),	/* Color and type/size of planet			*/
	LOW_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (HIGH_DENSITY, MEDIUM),			/* Atmosphere and density */
	{
	    {YTTERBIUM, HUGE_USEFUL},
	    {YTTRIUM, HUGE_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	YTTRIC_COLOR_TAB,
	YTTRIC_XLAT_TAB,
	250, 2, 80, 200,
    },
    {		/* QUASI_DEGENERATE_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + TOPO_ALGO, 
		GREEN_BODY),	/* Color and type/size of planet			*/
	MED_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (HIGH_DENSITY, NOTHING),		/* Atmosphere and density */
	{
	    {DEGENERATE_MATTER, MEDIUM_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	QUASI_DEGENERATE_COLOR_TAB,
	QUASI_DEGENERATE_XLAT_TAB,
	500, 1, 0, 160,
    },
    {		/* LANTHANIDE_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + CRATERED_ALGO, 
		YELLOW_BODY),	/* Color and type/size of planet			*/
	HIGH_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (HIGH_DENSITY, LIGHT),			/* Atmosphere and density */
	{
	    {LANTHANUM, MEDIUM_USEFUL},
	    {CERIUM, MEDIUM_USEFUL},
	    {PRASEODYMIUM, MEDIUM_USEFUL},
	    {NEODYMIUM, MEDIUM_USEFUL},
	    {PROMETHIUM, MEDIUM_USEFUL},
	    {SAMARIUM, MEDIUM_USEFUL},
	    {GADOLINIUM, MEDIUM_USEFUL},
	    {TERBIUM, MEDIUM_USEFUL},
	},
	LANTHANIDE_COLOR_TAB,
	YTTRIC_XLAT_TAB,
	250, 2, 80, 200,
    },
    {		/* TREASURE_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + TOPO_ALGO, 
		YELLOW_BODY),	/* Color and type/size of planet			*/
	NO_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (SUPER_DENSITY, LIGHT),			/* Atmosphere and density */
	{
	    {PALLADIUM, HEAVY_USEFUL},
	    {SILVER, HEAVY_USEFUL},
	    {SILVER, HEAVY_USEFUL},
	    {IRIDIUM, HEAVY_USEFUL},
	    {GOLD, HEAVY_USEFUL},
	    {PLATINUM, HEAVY_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	TREASURE_COLOR_TAB,
	QUASI_DEGENERATE_XLAT_TAB,
	500, 1, 0, 160,
    },
    {		/* UREA_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + CRATERED_ALGO, 
		YELLOW_BODY),	/* Color and type/size of planet			*/
	HIGH_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (LOW_DENSITY, HEAVY),			/* Atmosphere and density */
	{
	    {AMMONIA, LIGHT_USEFUL},
	    {FORMALDEHYDE, MEDIUM_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	UREA_COLOR_TAB,
	UREA_XLAT_TAB,
	230, 2, 200, 150,
    },
    {		/* METAL_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + CRATERED_ALGO, 
		ORANGE_BODY),	/* Color and type/size of planet			*/
	LOW_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (HIGH_DENSITY, HEAVY),			/* Atmosphere and density */
	{
	    {IRON, HUGE_USEFUL},
	    {NICKEL, HEAVY_USEFUL},
	    {VANADIUM, MEDIUM_USEFUL},
	    {SILVER, MEDIUM_USEFUL},
	    {URANIUM, HEAVY_USEFUL},
	    {SULFUR, MEDIUM_USEFUL},
	    {COPPER, HEAVY_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	},
	METAL_COLOR_TAB,
	METAL_XLAT_TAB,
	230, 2, 200, 150,
    },
    {		/* RADIOACTIVE_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + CRATERED_ALGO, 
		ORANGE_BODY),	/* Color and type/size of planet			*/
	NO_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (SUPER_DENSITY, LIGHT),			/* Atmosphere and density */
	{
	    {ASTATINE, MEDIUM_USEFUL},
	    {FRANCIUM, MEDIUM_USEFUL},
	    {RADIUM, MEDIUM_USEFUL},
	    {ACTINIUM, MEDIUM_USEFUL},
	    {THORIUM, MEDIUM_USEFUL},
	    {PROTACTINIUM, MEDIUM_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	RADIOACTIVE_COLOR_TAB,
	YTTRIC_XLAT_TAB,
	250, 2, 80, 200,
    },
    {		/* OPALESCENT_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + CRATERED_ALGO, 
		CYAN_BODY),	/* Color and type/size of planet			*/
	LOW_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (LOW_DENSITY, NOTHING),		/* Atmosphere and density */
	{
	    {SAMARIUM, LIGHT_USEFUL},
	    {GADOLINIUM, LIGHT_USEFUL},
	    {ARGON, LIGHT_USEFUL},
	    {LITHIUM, LIGHT_USEFUL},
	    {SILICON, LIGHT_USEFUL},
	    {ARSENIC, LIGHT_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	OPALESCENT_COLOR_TAB,
	OPALESCENT_XLAT_TAB,
	400, 1, 100, 190,
    },
    {		/* CYANIC_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + TOPO_ALGO, 
		GREEN_BODY),	/* Color and type/size of planet			*/
	NO_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (LIGHT_DENSITY, LIGHT),			/* Atmosphere and density */
	{
	    {CYANOACETYLENE, HUGE_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	CYANIC_COLOR_TAB,
	QUASI_DEGENERATE_XLAT_TAB,
	500, 1, 0, 160,
    },
    {		/* ACID_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + CRATERED_ALGO, 
		GREEN_BODY),	/* Color and type/size of planet			*/
	LOW_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (NORMAL_DENSITY, LIGHT),			/* Atmosphere and density */
	{
	    {SULFURIC_ACID, HEAVY_USEFUL},
	    {HYDROCHLORIC_ACID, HEAVY_USEFUL},
	    {FORMIC_ACID, HEAVY_USEFUL},
	    {HYDROCYANIC_ACID, HEAVY_USEFUL},
	    {PHOSPHORIC_ACID, HEAVY_USEFUL},
	    {NITRIC_ACID, HEAVY_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	ACID_COLOR_TAB,
	YTTRIC_XLAT_TAB,
	250, 2, 80, 200,
    },
    {		/* ALKALI_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + CRATERED_ALGO, 
		GREEN_BODY),	/* Color and type/size of planet			*/
	LOW_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (NORMAL_DENSITY, MEDIUM),			/* Atmosphere and density */
	{
	    {CALCIUM, MEDIUM_USEFUL},
	    {BARIUM, MEDIUM_USEFUL},
	    {STRONTIUM, MEDIUM_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	ALKALI_COLOR_TAB,
	YTTRIC_XLAT_TAB,
	250, 2, 80, 200,
    },
    {		/* HALIDE_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + CRATERED_ALGO, 
		GREEN_BODY),	/* Color and type/size of planet			*/
	NO_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (LOW_DENSITY, LIGHT),			/* Atmosphere and density */
	{
	    {FLUORINE, MEDIUM_USEFUL},
	    {BROMINE, MEDIUM_USEFUL},
	    {BROMINE, MEDIUM_USEFUL},
	    {ASTATINE, MEDIUM_USEFUL},
	    {IODINE, MEDIUM_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	HALIDE_COLOR_TAB,
	YTTRIC_XLAT_TAB,
	250, 2, 80, 200,
    },
    {		/* GREEN_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + CRATERED_ALGO,
		GREEN_BODY),	/* Color and type/size of planet			*/
	MED_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (NORMAL_DENSITY, HEAVY),			/* Atmosphere and density */
	{
	    {PRASEODYMIUM, HEAVY_USEFUL},
	    {NEODYMIUM, HEAVY_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	GREEN_COLOR_TAB,
	UREA_XLAT_TAB,
	230, 2, 200, 150,
    },
    {		/* COPPER_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + TOPO_ALGO,
		GREEN_BODY),	/* Color and type/size of planet			*/
	NO_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (HIGH_DENSITY, MEDIUM),			/* Atmosphere and density */
	{
	    {COPPER, HUGE_USEFUL},
	    {COPPER, HUGE_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	COPPER_COLOR_TAB,
	QUASI_DEGENERATE_XLAT_TAB,
	500, 1, 0, 160,
    },
    {		/* CARBIDE_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + CRATERED_ALGO,
		RED_BODY),	/* Color and type/size of planet			*/
	LOW_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (NORMAL_DENSITY, LIGHT),			/* Atmosphere and density */
	{
	    {CARBON, HEAVY_USEFUL},
	    {CARBON, HEAVY_USEFUL},
	    {CARBON, HEAVY_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	CARBIDE_COLOR_TAB,
	OPALESCENT_XLAT_TAB,
	400, 1, 100, 190,
    },
    {		/* ULTRAMARINE_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + CRATERED_ALGO,
		BLUE_BODY),	/* Color and type/size of planet			*/
	NO_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (LIGHT_DENSITY, HEAVY),			/* Atmosphere and density */
	{
	    {KRYPTON, MEDIUM_USEFUL},
	    {COBALT, MEDIUM_USEFUL},
	    {HOLMIUM, MEDIUM_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	ULTRAMARINE_COLOR_TAB,
	UREA_XLAT_TAB,
	200, 2, 100, 100,
    },
    {		/* NOBLE_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + CRATERED_ALGO,
		BLUE_BODY),	/* Color and type/size of planet			*/
	NO_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (LIGHT_DENSITY, LIGHT),			/* Atmosphere and density */
	{
	    {NEON, LIGHT_USEFUL},
	    {RADON, LIGHT_USEFUL},
	    {ARGON, LIGHT_USEFUL},
	    {KRYPTON, LIGHT_USEFUL},
	    {XENON, LIGHT_USEFUL},
	    {HELIUM, LIGHT_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	NOBLE_COLOR_TAB,
	YTTRIC_XLAT_TAB,
	250, 2, 80, 200,
    },
    {		/* AZURE_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + CRATERED_ALGO,
		BLUE_BODY),	/* Color and type/size of planet			*/
	MED_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (LOW_DENSITY, MEDIUM),			/* Atmosphere and density */
	{
	    {INDIUM, LIGHT_USEFUL},
	    {MOLYBDENUM, LIGHT_USEFUL},
	    {VANADIUM, LIGHT_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	AZURE_COLOR_TAB,
	UREA_XLAT_TAB,
	230, 2, 200, 150,
    },
    {		/* CHONDRITE_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + CRATERED_ALGO,
		PURPLE_BODY),	/* Color and type/size of planet			*/
	HIGH_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (NORMAL_DENSITY, NOTHING),		/* Atmosphere and density */
	{
	    {ETHANOL, HEAVY_USEFUL},
	    {FREE_RADICALS, HEAVY_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	CHONDRITE_COLOR_TAB,
	CHONDRITE_XLAT_TAB,
	500, 1, 100, 190,
    },
    {		/* PURPLE_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + CRATERED_ALGO,
		PURPLE_BODY),	/* Color and type/size of planet			*/
	LOW_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (LOW_DENSITY, LIGHT),			/* Atmosphere and density */
	{
	    {RHENIUM, MEDIUM_USEFUL},
	    {CADMIUM, MEDIUM_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	PURPLE_COLOR_TAB,
	UREA_XLAT_TAB,
	230, 2, 200, 150,
    },
    {		/* SUPER_DENSE_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + TOPO_ALGO,
		PURPLE_BODY),	/* Color and type/size of planet			*/
	HIGH_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (SUPER_DENSITY, HEAVY),			/* Atmosphere and density */
	{
	    {LEAD, MEDIUM_USEFUL},
	    {OSMIUM, MEDIUM_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	SUPER_DENSE_COLOR_TAB,
	QUASI_DEGENERATE_XLAT_TAB,
	500, 1, 0, 160,
    },
    {		/* PELLUCID_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + CRATERED_ALGO,
		PURPLE_BODY),	/* Color and type/size of planet			*/
	NO_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (LOW_DENSITY, LIGHT),			/* Atmosphere and density */
	{
	    {TZO_CRYSTALS, TRACE_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	PELLUCID_COLOR_TAB,
	OPALESCENT_XLAT_TAB,
	400, 1, 100, 190,
    },
    {		/* DUST_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + CRATERED_ALGO,
		RED_BODY),	/* Color and type/size of planet			*/
	LOW_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (LOW_DENSITY, MEDIUM),			/* Atmosphere and density */
	{
	    {BISMUTH, LIGHT_USEFUL},
	    {ALUMINUM, LIGHT_USEFUL},
	    {POTASSIUM, LIGHT_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	DUST_COLOR_TAB,
	YTTRIC_XLAT_TAB,
	250, 2, 80, 200,
    },
    {		/* CRIMSON_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + CRATERED_ALGO,
		RED_BODY),	/* Color and type/size of planet			*/
	NO_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (NORMAL_DENSITY, HEAVY),			/* Atmosphere and density */
	{
	    {BARIUM, LIGHT_USEFUL},
	    {BORON, LIGHT_USEFUL},
	    {BERYLLIUM, LIGHT_USEFUL},
	    {BISMUTH, LIGHT_USEFUL},
	    {BROMINE, LIGHT_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	MAROON_COLOR_TAB,
	UREA_XLAT_TAB,
	230, 2, 200, 150,
    },
    {		/* CIMMERIAN_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + TOPO_ALGO,
		RED_BODY),	/* Color and type/size of planet			*/
	LOW_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (HIGH_DENSITY, NOTHING),		/* Atmosphere and density */
	{
	    {METHANE, MEDIUM_USEFUL},
	    {AMMONIA, MEDIUM_USEFUL},
	    {METHANOL, MEDIUM_USEFUL},
	    {LITHIUM, MEDIUM_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	CIMMERIAN_COLOR_TAB,
	QUASI_DEGENERATE_XLAT_TAB,
	500, 1, 0, 160,
    },
    {		/* INFRARED_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + CRATERED_ALGO,
		RED_BODY),	/* Color and type/size of planet			*/
	LOW_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (NORMAL_DENSITY, HEAVY),		/* Atmosphere and density */
	{
	    {MERCURY, HEAVY_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	INFRARED_COLOR_TAB,
	OPALESCENT_XLAT_TAB,
	400, 1, 100, 190,
    },
    {		/* SELENIC_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + CRATERED_ALGO,
		WHITE_BODY),	/* Color and type/size of planet			*/
	NO_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (LOW_DENSITY, LIGHT),			/* Atmosphere and density */
	{
	    {IRON, LIGHT_USEFUL},
	    {ALUMINUM, LIGHT_USEFUL},
	    {CALCIUM, MEDIUM_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	SELENIC_COLOR_TAB,
	UREA_XLAT_TAB,
	230, 2, 200, 150,
    },
    {		/* AURIC_WORLD */
	MAKE_BYTE (SMALL_ROCKY_WORLD + TOPO_ALGO,
		YELLOW_BODY),	/* Color and type/size of planet			*/
	NO_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (HIGH_DENSITY, MEDIUM),			/* Atmosphere and density */
	{
	    {GOLD, HUGE_USEFUL},
	    {GOLD, HUGE_USEFUL},
	    {GOLD, HUGE_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	AURIC_COLOR_TAB,
	QUASI_DEGENERATE_XLAT_TAB,
	500, 1, 0, 160,
    },


    {		/* FLUORESCENT_WORLD */
	MAKE_BYTE (LARGE_ROCKY_WORLD + CRATERED_ALGO,
		VIOLET_BODY),	/* Color and type/size of planet			*/
	LOW_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (HIGH_DENSITY, LIGHT),			/* Atmosphere and density */
	{
	    {TECHNETIUM, HUGE_USEFUL},
	    {NEON, HUGE_USEFUL},
	    {RADON, LIGHT_USEFUL},
	    {POTASSIUM, LIGHT_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	FLUORESCENT_COLOR_TAB,
	OPALESCENT_XLAT_TAB,
	400, 1, 100, 190,
    },
    {		/* ULTRAVIOLET_WORLD */
	MAKE_BYTE (LARGE_ROCKY_WORLD + CRATERED_ALGO,
		VIOLET_BODY),	/* Color and type/size of planet			*/
	MED_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (HIGH_DENSITY, HEAVY),			/* Atmosphere and density */
	{
	    {POLONIUM, HUGE_USEFUL},
	    {GOLD, HUGE_USEFUL},
	    {PHOSPHORUS, HUGE_USEFUL},
	    {SCANDIUM, MEDIUM_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	ULTRAVIOLET_COLOR_TAB,
	YTTRIC_XLAT_TAB,
	250, 2, 80, 200,
    },
    {		/* PLUTONIC_WORLD */
	MAKE_BYTE (LARGE_ROCKY_WORLD + CRATERED_ALGO,
		YELLOW_BODY),	/* Color and type/size of planet			*/
	LOW_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (HIGH_DENSITY, HEAVY),			/* Atmosphere and density */
	{
	    {BERYLLIUM, HUGE_USEFUL},
	    {BORON, HUGE_USEFUL},
	    {LANTHANUM, MEDIUM_USEFUL},
	    {ASTATINE, MEDIUM_USEFUL},
	    {FRANCIUM, MEDIUM_USEFUL},
	    {TITANIUM, LIGHT_USEFUL},
	    {CERIUM, MEDIUM_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	},
	PLUTONIC_COLOR_TAB,
	YTTRIC_XLAT_TAB,
	250, 2, 80, 200,
    },
    {		/* RAINBOW_WORLD */
	MAKE_BYTE (LARGE_ROCKY_WORLD + TOPO_ALGO,
		YELLOW_BODY),	/* Color and type/size of planet			*/
	NO_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (LOW_DENSITY, LIGHT),			/* Atmosphere and density */
	{
	    {ACTINIUM, HEAVY_USEFUL},
	    {THORIUM, HEAVY_USEFUL},
	    {PROTACTINIUM, HEAVY_USEFUL},
	    {NEPTUNIUM, HEAVY_USEFUL},
	    {PLUTONIUM, HEAVY_USEFUL},
	    {OZONE, HUGE_USEFUL},
	    {OZONE, HUGE_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	},
	RAINBOW_COLOR_TAB,
	RAINBOW_XLAT_TAB,
	500, 1, 20, 100,
    },
    {		/* CRACKED_WORLD */
	MAKE_BYTE (LARGE_ROCKY_WORLD + CRATERED_ALGO,
		ORANGE_BODY),	/* Color and type/size of planet			*/
	SUPER_TECTONICS,	/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (NORMAL_DENSITY, NOTHING),			/* Atmosphere and density */
	{
	    {PALLADIUM, HUGE_USEFUL},
	    {IRIDIUM, HUGE_USEFUL},
	    {TECHNETIUM, HUGE_USEFUL},
	    {POLONIUM, HUGE_USEFUL},
	    {SODIUM, HUGE_USEFUL},
	    {MANGANESE, HUGE_USEFUL},
	    {CHROMIUM, HUGE_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	},
	CRACKED_COLOR_TAB,
	CRACKED_XLAT_TAB,
	500, 1, 0, 185,
    },
    {		/* SAPPHIRE_WORLD */
	MAKE_BYTE (LARGE_ROCKY_WORLD + TOPO_ALGO,
		CYAN_BODY),	/* Color and type/size of planet			*/
	NO_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (LOW_DENSITY, NOTHING),		/* Atmosphere and density */
	{
	    {REISBURG_HELICES, HUGE_USEFUL},
	    {RT_SUPER_FLUID, MEDIUM_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	SAPPHIRE_COLOR_TAB,
	SAPPHIRE_XLAT_TAB,
	80, 1, 0, 128,
    },
    {		/* ORGANIC_WORLD */
	MAKE_BYTE (LARGE_ROCKY_WORLD + TOPO_ALGO,
		CYAN_BODY),	/* Color and type/size of planet			*/
	MED_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (NORMAL_DENSITY, LIGHT),			/* Atmosphere and density */
	{
	    {FREE_RADICALS, HEAVY_USEFUL},
	    {FORMALDEHYDE, HEAVY_USEFUL},
	    {CARBON, HEAVY_USEFUL},
	    {CARBON_DIOXIDE, HEAVY_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	ORGANIC_COLOR_TAB,
	QUASI_DEGENERATE_XLAT_TAB,
	500, 1, 0, 160,
    },
    {		/* XENOLITHIC_WORLD */
	MAKE_BYTE (LARGE_ROCKY_WORLD + CRATERED_ALGO,
		CYAN_BODY),	/* Color and type/size of planet			*/
	LOW_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (NORMAL_DENSITY, MEDIUM),			/* Atmosphere and density */
	{
	    {ALUMINUM, HUGE_USEFUL},
	    {PLATINUM, LIGHT_USEFUL},
	    {GERMANIUM, TRACE_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	XENOLITHIC_COLOR_TAB,
	OPALESCENT_XLAT_TAB,
	400, 1, 100, 190,
    },
    {		/* REDUX_WORLD */
	MAKE_BYTE (LARGE_ROCKY_WORLD + CRATERED_ALGO,
		GREEN_BODY),	/* Color and type/size of planet			*/
	LOW_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (NORMAL_DENSITY, LIGHT),			/* Atmosphere and density */
	{
	    {BROMINE, MEDIUM_USEFUL},
	    {OXYGEN, MEDIUM_USEFUL},
	    {FLUORINE, MEDIUM_USEFUL},
	    {SULFUR, MEDIUM_USEFUL},
	    {CHLORINE, MEDIUM_USEFUL},
	    {IODINE, MEDIUM_USEFUL},
	    {ZIRCONIUM, LIGHT_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	},
	REDUX_COLOR_TAB,
	REDUX_XLAT_TAB,
	500, 1, 0, 190,
    },
    {		/* PRIMORDIAL_WORLD */
	MAKE_BYTE (LARGE_ROCKY_WORLD + CRATERED_ALGO,
		GREEN_BODY),	/* Color and type/size of planet			*/
	SUPER_TECTONICS,	/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (NORMAL_DENSITY, HEAVY),			/* Atmosphere and density */
	{
	    {CESIUM, MEDIUM_USEFUL},
	    {BARIUM, MEDIUM_USEFUL},
	    {RUBIDIUM, MEDIUM_USEFUL},
	    {METHANE, HUGE_USEFUL},
	    {AMMONIA, HUGE_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	PRIMORDIAL_COLOR_TAB,
	YTTRIC_XLAT_TAB,
	250, 2, 10, 200,
    },
    {		/* EMERALD_WORLD */
	MAKE_BYTE (LARGE_ROCKY_WORLD + TOPO_ALGO,
		GREEN_BODY),	/* Color and type/size of planet			*/
	NO_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (LOW_DENSITY, NOTHING),		/* Atmosphere and density */
	{
	    {AGUUTI_NODULES, HUGE_USEFUL},
	    {ANTIMATTER, MEDIUM_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	EMERALD_COLOR_TAB,
	SAPPHIRE_XLAT_TAB,
	80, 1, 0, 128,
    },
    {		/* CHLORINE_WORLD */
	MAKE_BYTE (LARGE_ROCKY_WORLD + CRATERED_ALGO,
		GREEN_BODY),	/* Color and type/size of planet			*/
	HIGH_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (NORMAL_DENSITY, LIGHT),			/* Atmosphere and density */
	{
	    {CHLORINE, HEAVY_USEFUL},
	    {CHLORINE, HEAVY_USEFUL},
	    {HYDROCHLORIC_ACID, HEAVY_USEFUL},
	    {HYDROCHLORIC_ACID, HEAVY_USEFUL},
	    {ZINC, LIGHT_USEFUL},
	    {GALLIUM, TRACE_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	CHLORINE_COLOR_TAB,
	CHLORINE_XLAT_TAB,
	500, 1, 0, 190,
    },
    {		/* MAGNETIC_WORLD */
	MAKE_BYTE (LARGE_ROCKY_WORLD + CRATERED_ALGO,
		GREEN_BODY),	/* Color and type/size of planet			*/
	HIGH_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (HIGH_DENSITY, NOTHING),		/* Atmosphere and density */
	{
	    {ZINC, HEAVY_USEFUL},
	    {NICKEL, MEDIUM_USEFUL},
	    {MAGNETIC_MONOPOLES, TRACE_USEFUL},
	    {NIOBIUM, LIGHT_USEFUL},
	    {IRON, HEAVY_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	MAGNETIC_COLOR_TAB,
	OPALESCENT_XLAT_TAB,
	400, 1, 100, 190,
    },
    {		/* WATER_WORLD */
	MAKE_BYTE (LARGE_ROCKY_WORLD + CRATERED_ALGO,
		BLUE_BODY),	/* Color and type/size of planet			*/
	LOW_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (NORMAL_DENSITY, MEDIUM),			/* Atmosphere and density */
	{
	    {IRON, LIGHT_USEFUL},
	    {ALUMINUM, LIGHT_USEFUL},
	    {TIN, LIGHT_USEFUL},
	    {LEAD, LIGHT_USEFUL},
	    {URANIUM, TRACE_USEFUL},
	    {MOLYBDENUM, TRACE_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	WATER_COLOR_TAB,
	CHLORINE_XLAT_TAB,
	500, 1, 0, 190,
    },
    {		/* TELLURIC_WORLD */
	MAKE_BYTE (LARGE_ROCKY_WORLD + CRATERED_ALGO,
		BLUE_BODY),	/* Color and type/size of planet			*/
	LOW_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (NORMAL_DENSITY, HEAVY),			/* Atmosphere and density */
	{
	    {IRIDIUM, MEDIUM_USEFUL},
	    {RUTHENIUM, MEDIUM_USEFUL},
	    {THALLIUM, MEDIUM_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	TELLURIC_COLOR_TAB,
	YTTRIC_XLAT_TAB,
	250, 2, 80, 200,
    },
    {		/* HYDROCARBON_WORLD */
	MAKE_BYTE (LARGE_ROCKY_WORLD + TOPO_ALGO,
		BLUE_BODY),	/* Color and type/size of planet			*/
	LOW_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (NORMAL_DENSITY, LIGHT),			/* Atmosphere and density */
	{
	    {HYDROCARBONS, HUGE_USEFUL},
	    {BISMUTH, LIGHT_USEFUL},
	    {TANTALUM, TRACE_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	HYDROCARBON_COLOR_TAB,
	QUASI_DEGENERATE_XLAT_TAB,
	500, 1, 0, 160,
    },
    {		/* IODINE_WORLD */
	MAKE_BYTE (LARGE_ROCKY_WORLD + CRATERED_ALGO,
		GREEN_BODY),	/* Color and type/size of planet			*/
	MED_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (LOW_DENSITY, MEDIUM),			/* Atmosphere and density */
	{
	    {IODINE, HEAVY_USEFUL},
	    {MAGNESIUM, LIGHT_USEFUL},
	    {TUNGSTEN, TRACE_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	IODINE_COLOR_TAB,
	UREA_XLAT_TAB,
	230, 2, 200, 150,
    },
    {		/* VINYLOGOUS_WORLD */
	MAKE_BYTE (LARGE_ROCKY_WORLD + CRATERED_ALGO,
		PURPLE_BODY),	/* Color and type/size of planet			*/
	LOW_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (NORMAL_DENSITY, NOTHING),		/* Atmosphere and density */
	{
	    {TITANIUM, LIGHT_USEFUL},
	    {ARSENIC, LIGHT_USEFUL},
	    {POTASSIUM, LIGHT_USEFUL},
	    {RHENIUM, LIGHT_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	VINYLOGOUS_COLOR_TAB,
	OPALESCENT_XLAT_TAB,
	400, 1, 100, 190,
    },
    {		/* RUBY_WORLD */
	MAKE_BYTE (LARGE_ROCKY_WORLD + TOPO_ALGO,
		RED_BODY),	/* Color and type/size of planet			*/
	NO_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (LOW_DENSITY, LIGHT),			/* Atmosphere and density */
	{
	    {TZO_CRYSTALS, HUGE_USEFUL},
	    {NEUTRONIUM, MEDIUM_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	RUBY_COLOR_TAB,
	SAPPHIRE_XLAT_TAB,
	80, 1, 0, 128,
    },
    {		/* MAGMA_WORLD */
	MAKE_BYTE (LARGE_ROCKY_WORLD + TOPO_ALGO,
		RED_BODY),	/* Color and type/size of planet			*/
	SUPER_TECTONICS,	/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (HIGH_DENSITY, LIGHT),			/* Atmosphere and density */
	{
	    {LEAD, LIGHT_USEFUL},
	    {NICKEL, LIGHT_USEFUL},
	    {IRON, LIGHT_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	MAGMA_COLOR_TAB,
	QUASI_DEGENERATE_XLAT_TAB,
	500, 1, 0, 160,
    },
    {		/* MAROON_WORLD */
	MAKE_BYTE (LARGE_ROCKY_WORLD + CRATERED_ALGO,
		RED_BODY),	/* Color and type/size of planet			*/
	NO_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (LOW_DENSITY, MEDIUM),			/* Atmosphere and density */
	{
	    {CESIUM, MEDIUM_USEFUL},
	    {SILICON, MEDIUM_USEFUL},
	    {PHOSPHORUS, MEDIUM_USEFUL},
	    {RHODIUM, MEDIUM_USEFUL},
	    {CADMIUM, MEDIUM_USEFUL},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	CRIMSON_COLOR_TAB,
	UREA_XLAT_TAB,
	230, 2, 200, 150,
    },

    {	      
	MAKE_BYTE (GAS_GIANT + GAS_GIANT_ALGO,
		BLUE_BODY),	/* Color and type/size of planet		*/
	NO_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (GAS_DENSITY, HEAVY),			/* Atmosphere and density */
	{
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	BLU_GAS_COLOR_TAB,
	GAS_XLAT_TAB,
	10, 2, 8, 29,
    },
    {
	MAKE_BYTE (GAS_GIANT + GAS_GIANT_ALGO,
		CYAN_BODY),	/* Color and type/size of planet		*/
	NO_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (GAS_DENSITY, HEAVY),			/* Atmosphere and density */
	{
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	CYA_GAS_COLOR_TAB,
	GAS_XLAT_TAB,
	10, 2, 8, 29,
    },
    {	     
	MAKE_BYTE (GAS_GIANT + GAS_GIANT_ALGO,
		GREEN_BODY),	/* Color and type/size of planet		*/
	NO_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (GAS_DENSITY, HEAVY),			/* Atmosphere and density */
	{
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	GRN_GAS_COLOR_TAB,
	GAS_XLAT_TAB,
	10, 2, 8, 29,
    },
    {
	MAKE_BYTE (GAS_GIANT + GAS_GIANT_ALGO,
		GRAY_BODY),	/* Color and type/size of planet		*/
	NO_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (GAS_DENSITY, HEAVY),			/* Atmosphere and density */
	{
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	GRY_GAS_COLOR_TAB,
	GAS_XLAT_TAB,
	10, 2, 8, 29,
    },
    {
	MAKE_BYTE (GAS_GIANT + GAS_GIANT_ALGO,
		ORANGE_BODY),	/* Color and type/size of planet		*/
	NO_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (GAS_DENSITY, HEAVY),			/* Atmosphere and density */
	{
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	ORA_GAS_COLOR_TAB,
	GAS_XLAT_TAB,
	10, 2, 8, 29,
    },
    {	
	MAKE_BYTE (GAS_GIANT + GAS_GIANT_ALGO,
		PURPLE_BODY),	/* Color and type/size of planet		*/
	NO_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (GAS_DENSITY, HEAVY),			/* Atmosphere and density */
	{
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	PUR_GAS_COLOR_TAB,
	GAS_XLAT_TAB,
	10, 2, 8, 29,
    },
    {
	MAKE_BYTE (GAS_GIANT + GAS_GIANT_ALGO,
		RED_BODY),	/* Color and type/size of planet		*/
	NO_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (GAS_DENSITY, HEAVY),			/* Atmosphere and density */
	{
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	RED_GAS_COLOR_TAB,
	GAS_XLAT_TAB,
	10, 2, 8, 29,
    },
    {
	MAKE_BYTE (GAS_GIANT + GAS_GIANT_ALGO,
		VIOLET_BODY),	/* Color and type/size of planet		*/
	NO_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (GAS_DENSITY, HEAVY),			/* Atmosphere and density */
	{
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	VIO_GAS_COLOR_TAB,
	GAS_XLAT_TAB,
	10, 2, 8, 29,
    },
    {		/* A Jupiter-like World */
	MAKE_BYTE (GAS_GIANT + GAS_GIANT_ALGO,
		YELLOW_BODY),	/* Color and type/size of planet		*/
	NO_TECTONICS,		/* Tectonics - Scaled with Earth at 82   		*/
	MAKE_BYTE (GAS_DENSITY, HEAVY),			/* Atmosphere and density */
	{
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	    {NOTHING, NO_DEPOSIT},
	},
	YEL_GAS_COLOR_TAB,
	GAS_XLAT_TAB,
	10, 2, 8, 29,
    },
};

