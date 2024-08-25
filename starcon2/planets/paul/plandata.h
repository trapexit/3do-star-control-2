#ifndef	_PLANDATA_H
#define	_PLANDATA_H

/*------------------------------ Type Defines ----------------------------- */
#define	NUMBER_OF_ORBITS	16
#define	VACANT			0xFF

enum
{
    SMALL_ROCKY_WORLD = 0,
    LARGE_ROCKY_WORLD,
    GAS_GIANT,

    NUM_PLANET_TYPES
};

enum
{
    DWARF_STAR = 0,
    GIANT_STAR,
    SUPER_GIANT_STAR,

    NUM_STAR_TYPES
};

enum
{
    BLUE_BODY = 0,
    GREEN_BODY,
    ORANGE_BODY,
    RED_BODY,
    WHITE_BODY,
    GRAY_BODY = WHITE_BODY,
    YELLOW_BODY,

    NUM_STAR_COLORS,

    CYAN_BODY = NUM_STAR_COLORS,
    PURPLE_BODY,
    VIOLET_BODY
};

enum
{
    OWNER_NOBODY = 0,
    OWNER_NEUTRAL,
    OWNER_HIERARCHY,

    OWNER_PLAYER = (1 << 2)
};

#define STAR_OWNER_SHIFT	0
#define STAR_TYPE_SHIFT		3	/* STAR_OWNER_SHIFT + 3 */
#define STAR_COLOR_SHIFT	5	/* STAR_TYPE_SHIFT + 2 */
#define STAR_COLOR_MASK		(BYTE)(0xFF << STAR_COLOR_SHIFT)
#define STAR_TYPE_MASK		(BYTE)((0xFF << STAR_TYPE_SHIFT) \
					& ~STAR_COLOR_MASK)
#define STAR_OWNER_MASK		(BYTE)((0xFF << STAR_OWNER_SHIFT) \
					& ~(STAR_COLOR_MASK \
					| STAR_TYPE_MASK))
#define STAR_UNKNOWN_MASK	(STAR_OWNER_MASK & ~OWNER_PLAYER)

#define MAKE_STAR(t,c,o)	\
	(BYTE)((((BYTE)(t) << STAR_TYPE_SHIFT) & STAR_TYPE_MASK) \
	| (((BYTE)(c) << STAR_COLOR_SHIFT) & STAR_COLOR_MASK) \
	| (((BYTE)(o) << STAR_OWNER_SHIFT) & STAR_OWNER_MASK))
#define STAR_TYPE(f)	(BYTE)(((f) & STAR_TYPE_MASK) >> STAR_TYPE_SHIFT)
#define STAR_COLOR(f)	(BYTE)(((f) & STAR_COLOR_MASK) >> STAR_COLOR_SHIFT)
#define STAR_OWNER(f)	(BYTE)(((f) & STAR_OWNER_MASK) >> STAR_OWNER_SHIFT)
#define STAR_UNKNOWN(f)	(BOOLEAN)((STAR_OWNER(f) \
				& STAR_UNKNOWN_MASK) == STAR_UNKNOWN_MASK)

#define PLAN_SIZE_MASK		0x03

#define TOPO_ALGO		(0 << 2)
#define CRATERED_ALGO		(1 << 2)
#define GAS_GIANT_ALGO		(2 << 2)
#define PLAN_ALGO_MASK		0x0C

#define	PLANSIZE(type) 	((BYTE)((type) & PLAN_SIZE_MASK))
#define	PLANALGO(type) 	((BYTE)((type) & PLAN_ALGO_MASK))
#define PLANCOLOR(type)	HINIBBLE (type)

#define THIN_ATMOSPHERE		10
#define NORMAL_ATMOSPHERE	75
#define THICK_ATMOSPHERE	200
#define SUPER_THICK_ATMOSPHERE	2500
#define	GAS_GIANT_ATMOSPHERE	0xFFFF

enum	
{
    FIRST_ROCKY_WORLD = 0,
    FIRST_SMALL_ROCKY_WORLD = FIRST_ROCKY_WORLD,

    OOLITE_WORLD = FIRST_SMALL_ROCKY_WORLD,
    YTTRIC_WORLD,
    QUASI_DEGENERATE_WORLD,
    LANTHANIDE_WORLD,
    TREASURE_WORLD,
    UREA_WORLD,
    METAL_WORLD,
    RADIOACTIVE_WORLD,
    OPALESCENT_WORLD,
    CYANIC_WORLD,
    ACID_WORLD,
    ALKALI_WORLD,
    HALIDE_WORLD,
    GREEN_WORLD,
    COPPER_WORLD,
    CARBIDE_WORLD,
    ULTRAMARINE_WORLD,
    NOBLE_WORLD,
    AZURE_WORLD,
    CHONDRITE_WORLD,
    PURPLE_WORLD,
    SUPER_DENSE_WORLD,
    PELLUCID_WORLD,
    DUST_WORLD,
    CRIMSON_WORLD,
    CIMMERIAN_WORLD,
    INFRARED_WORLD,
    SELENIC_WORLD,
    AURIC_WORLD,
    LAST_SMALL_ROCKY_WORLD = AURIC_WORLD,

    FIRST_LARGE_ROCKY_WORLD,
    FLUORESCENT_WORLD = FIRST_LARGE_ROCKY_WORLD,
    ULTRAVIOLET_WORLD,
    PLUTONIC_WORLD,
    RAINBOW_WORLD,
    CRACKED_WORLD,
    SAPPHIRE_WORLD,
    ORGANIC_WORLD,
    XENOLITHIC_WORLD,
    REDUX_WORLD,
    PRIMORDIAL_WORLD,
    EMERALD_WORLD,
    CHLORINE_WORLD,
    MAGNETIC_WORLD,
    WATER_WORLD,
    TELLURIC_WORLD,
    HYDROCARBON_WORLD,
    IODINE_WORLD,
    VINYLOGOUS_WORLD,
    RUBY_WORLD,
    MAGMA_WORLD,
    MAROON_WORLD,
    LAST_LARGE_ROCKY_WORLD = MAROON_WORLD,

    FIRST_GAS_GIANT,
    BLU_GAS_GIANT = FIRST_GAS_GIANT,	/* Gas Giants */
    CYA_GAS_GIANT,
    GRN_GAS_GIANT,
    GRY_GAS_GIANT,
    ORA_GAS_GIANT,
    PUR_GAS_GIANT,
    RED_GAS_GIANT,
    VIO_GAS_GIANT,
    YEL_GAS_GIANT,
    LAST_GAS_GIANT = YEL_GAS_GIANT,

    NUMBER_OF_PLANET_TYPES
};

#define NUMBER_OF_SMALL_ROCKY_WORLDS	(LAST_SMALL_ROCKY_WORLD - FIRST_SMALL_ROCKY_WORLD + 1)
#define NUMBER_OF_LARGE_ROCKY_WORLDS	(LAST_LARGE_ROCKY_WORLD - FIRST_LARGE_ROCKY_WORLD + 1)
#define NUMBER_OF_ROCKY_WORLDS		(NUMBER_OF_SMALL_ROCKY_WORLDS + NUMBER_OF_LARGE_ROCKY_WORLDS)
#define NUMBER_OF_GAS_GIANTS		(LAST_GAS_GIANT - FIRST_GAS_GIANT + 1)

typedef struct
{
    CONST SIZE	level_tab[3];
    CONST BYTE	xlat_tab[256];
} XLAT_DESC;
typedef CONST XLAT_DESC	*PXLAT_DESC;
typedef CONST XLAT_DESC	near *NPXLAT_DESC;
typedef CONST XLAT_DESC	far *LPXLAT_DESC;

typedef struct
{
    BYTE	ElementType;
    BYTE	Density;
} ElementEntry;

typedef struct
{
    BYTE		Type;	/*   HiNybble is interplanetary color				*/
				/* 		 \   	                    			*/
				/* 		  \  Planet size    map algo			*/
				/* 		   \	      \	      \				*/
				/* 		    \	       \       \      			*/
				/* 		    0000 	00 	00			*/
    BYTE		BaseTectonics;	/* Scaled with Earth at 100 */
    BYTE		AtmoAndDensity;
#define NUM_USEFUL_ELEMENTS	8
    ElementEntry	UsefulElements[NUM_USEFUL_ELEMENTS];

    RESOURCE		CMapInstance, XlatTabInstance;
    COUNT		num_faults;
    SIZE		fault_depth;
    COUNT		num_blemishes;
    SIZE		base_elevation;
} PlanetFrame;
typedef CONST PlanetFrame	*PPLANDATA;
typedef CONST PlanetFrame	near *NPPLANDATA;
typedef CONST PlanetFrame	far *LPPLANDATA;

typedef CONST ElementEntry	*PELEMENT_ENTRY;
typedef CONST ElementEntry	near *NPELEMENT_ENTRY;
typedef CONST ElementEntry	far *LPELEMENT_ENTRY;

#define XLAT_DESCPTR		LPXLAT_DESC
#define PLANDATAPTR		LPPLANDATA
#define ELEMENT_ENTRYPTR	LPELEMENT_ENTRY

typedef struct
{
    SIZE	AxialTilt;
    UWORD	Tectonics;
    UWORD	Weather;
    UWORD	PlanetDensity;
    UWORD	PlanetRadius;
    UWORD	SurfaceGravity;
    SIZE	SurfaceTemperature;
    UWORD	RotationPeriod;
    UWORD	AtmoDensity;
    SIZE	LifeChance;
    UWORD	PlanetToSunDist;

    PLANDATAPTR	PlanDataPtr;

    DWORD	ScanSeed[NUM_SCAN_TYPES];
    DWORD	ScanRetrieveMask[NUM_SCAN_TYPES];
    POINT	CurPt;
    COUNT	CurDensity;
    COUNT	CurType;

    STRING	DiscoveryString;
    FONT	LanderFont;
} PLANET_INFO;
typedef PLANET_INFO		*PPLANET_INFO;
typedef PLANET_INFO		near *NPPLANET_INFO;
typedef PLANET_INFO		far *LPPLANET_INFO;

enum
{
    GAS_DENSITY,
    LIGHT_DENSITY,
    LOW_DENSITY,
    NORMAL_DENSITY,
    HIGH_DENSITY,
    SUPER_DENSITY
};

#define CalcGravity(d,r)	(UWORD)((DWORD)(d) * (r) / 100)
#define CalcFromBase(b,v)	((UWORD)(b) + ((UWORD)random () % (v)))

#define PLANET_INFOPTR		PPLANET_INFO

#define EARTH_ATMOSPHERE	50

#define COLD_THRESHOLD	-40
#define HOT_THRESHOLD	100

/*------------------------------ Global Data ------------------------------ */

extern CONST PlanetFrame	far *PlanData; 

#endif /* _PLANDATA_H */

