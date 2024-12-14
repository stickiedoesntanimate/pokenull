#include "global.h"
#include "pokemon.h"
#include "strings.h"
#include "random.h"
#include "text.h"
#include "event_data.h"
#include "region_map.h"
#include "constants/species.h"
#include "constants/items.h"
#include "constants/abilities.h"
#include "data/text/wonder_trade_OT_names.h"
#include "constants/region_map_sections.h"
#include "item.h"
#include "constants/item.h"
#include "constants/hold_effects.h"
#include "mail.h"
#include "constants/pokemon.h"
#include "party_menu.h"
#include "field_weather.h"
#include "constants/weather.h"
#include "battle.h" // A workaround to include the expansion's constants/form_change_types.h without breaking Pret/Pokeemerald compatibility.
#include "string_util.h"
#include "daycare.h"
#include "wonder_trade.h"

//extern struct Evolution gEvolutionTable[][EVOS_PER_MON];



// This file's functions.
static u16 PickRandomSpecies(void);
static u8 GetWonderTradeOT(u8 *name);
static u16 GetWonderTradeEvolutionTargetSpecies(struct Pokemon *mon);
//static u32 GetEvolutionSpecies(u16 speciesId);
//static bool32 IsSpeciesFamilyMegaEvolutionCompatible(u16 species, u16 heldStone);

struct WonderTrade {
    u8 nickname[POKEMON_NAME_LENGTH + 1];
    u16 species;
    u8 ivs[NUM_STATS];
    u8 abilityNum;
    u32 otId;
    u8 conditions[CONTEST_CATEGORIES_COUNT];
    u32 personality;
    u16 heldItem;
    u8 mailNum;
    u8 otName[TRAINER_NAME_LENGTH + 1];
    u8 otGender;
    u8 sheen;
    u16 requestedSpecies;
};

#ifndef RHH_EXPANSION
// This is a list of items that are not used in vanilla.
// Feel free to delete it and remove the check that uses it,
// if you use the pokeemerald-expansion.
static const u16 sIsInvalidItem[] = {
    [ITEM_034]            = TRUE,
    [ITEM_035]            = TRUE,
    [ITEM_036]            = TRUE,
    [ITEM_037]            = TRUE,
    [ITEM_038]            = TRUE,
    [ITEM_039]            = TRUE,
    [ITEM_03A]            = TRUE,
    [ITEM_03B]            = TRUE,
    [ITEM_03C]            = TRUE,
    [ITEM_03D]            = TRUE,
    [ITEM_03E]            = TRUE,
    [ITEM_048]            = TRUE,
    [ITEM_052]            = TRUE,
    [ITEM_057]            = TRUE,
    [ITEM_058]            = TRUE,
    [ITEM_059]            = TRUE,
    [ITEM_05A]            = TRUE,
    [ITEM_05B]            = TRUE,
    [ITEM_05C]            = TRUE,
    [ITEM_063]            = TRUE,
    [ITEM_064]            = TRUE,
    [ITEM_065]            = TRUE,
    [ITEM_066]            = TRUE,
    [ITEM_069]            = TRUE,
    [ITEM_070]            = TRUE,
    [ITEM_071]            = TRUE,
    [ITEM_072]            = TRUE,
    [ITEM_073]            = TRUE,
    [ITEM_074]            = TRUE,
    [ITEM_075]            = TRUE,
    [ITEM_076]            = TRUE,
    [ITEM_077]            = TRUE,
    [ITEM_078]            = TRUE,
    [ITEM_UNUSED_BERRY_1] = TRUE,
    [ITEM_UNUSED_BERRY_2] = TRUE,
    [ITEM_UNUSED_BERRY_3] = TRUE,
    [ITEM_0E2]            = TRUE,
    [ITEM_0E3]            = TRUE,
    [ITEM_0E4]            = TRUE,
    [ITEM_0E5]            = TRUE,
    [ITEM_0E6]            = TRUE,
    [ITEM_0E7]            = TRUE,
    [ITEM_0E8]            = TRUE,
    [ITEM_0E9]            = TRUE,
    [ITEM_0EA]            = TRUE,
    [ITEM_0EB]            = TRUE,
    [ITEM_0EC]            = TRUE,
    [ITEM_0ED]            = TRUE,
    [ITEM_0EE]            = TRUE,
    [ITEM_0EF]            = TRUE,
    [ITEM_0F0]            = TRUE,
    [ITEM_0F1]            = TRUE,
    [ITEM_0F2]            = TRUE,
    [ITEM_0F3]            = TRUE,
    [ITEM_0F4]            = TRUE,
    [ITEM_0F5]            = TRUE,
    [ITEM_0F6]            = TRUE,
    [ITEM_0F7]            = TRUE,
    [ITEM_0F8]            = TRUE,
    [ITEM_0F9]            = TRUE,
    [ITEM_0FA]            = TRUE,
    [ITEM_0FB]            = TRUE,
    [ITEM_0FC]            = TRUE,
    [ITEM_0FD]            = TRUE,
    [ITEM_10B]            = TRUE,
    [ITEM_15B]            = TRUE,
    [ITEM_15C]            = TRUE,
};
#endif

static const u16 sIsValidSpecies[] = {
    SPECIES_BULBASAUR,
    SPECIES_CHARMANDER,
    SPECIES_SQUIRTLE,
    SPECIES_CATERPIE,
    SPECIES_WEEDLE,
    SPECIES_PIDGEY,
    SPECIES_RATTATA,
    SPECIES_SPEAROW,
    SPECIES_EKANS,
    SPECIES_SANDSHREW,
    SPECIES_NIDORAN_F,
    SPECIES_NIDORAN_M,
    SPECIES_VULPIX,
    SPECIES_ZUBAT,
    SPECIES_ODDISH,
    SPECIES_PARAS,
    SPECIES_VENONAT,
    SPECIES_DIGLETT,
    SPECIES_MEOWTH,
    SPECIES_PSYDUCK,
    SPECIES_MANKEY,
    SPECIES_GROWLITHE,
    SPECIES_POLIWAG,
    SPECIES_ABRA,
    SPECIES_MACHOP,
    SPECIES_BELLSPROUT,
    SPECIES_TENTACOOL,
    SPECIES_GEODUDE,
    SPECIES_PONYTA,
    SPECIES_SLOWPOKE,
    SPECIES_MAGNEMITE,
    SPECIES_FARFETCHD,
    SPECIES_DODUO,
    SPECIES_SEEL,
    SPECIES_GRIMER,
    SPECIES_SHELLDER,
    SPECIES_GASTLY,
    SPECIES_ONIX,
    SPECIES_DROWZEE,
    SPECIES_KRABBY,
    SPECIES_VOLTORB,
    SPECIES_EXEGGCUTE,
    SPECIES_CUBONE,
    SPECIES_LICKITUNG,
    SPECIES_KOFFING,
    SPECIES_RHYHORN,
    SPECIES_CHANSEY,
    SPECIES_TANGELA,
    SPECIES_KANGASKHAN,
    SPECIES_HORSEA,
    SPECIES_GOLDEEN,
    SPECIES_STARYU,
#ifndef RHH_EXPANSION
    SPECIES_MR_MIME,
#endif
    SPECIES_SCYTHER,
    SPECIES_PINSIR,
    SPECIES_TAUROS,
    SPECIES_MAGIKARP,
    SPECIES_LAPRAS,
    SPECIES_DITTO,
    SPECIES_EEVEE,
    SPECIES_PORYGON,
    SPECIES_OMANYTE,
    SPECIES_KABUTO,
    SPECIES_AERODACTYL,
#ifndef RHH_EXPANSION
    SPECIES_SNORLAX,
#endif
    SPECIES_DRATINI,
    SPECIES_CHIKORITA,
    SPECIES_TOTODILE,
    SPECIES_SENTRET,
    SPECIES_HOOTHOOT,
    SPECIES_LEDYBA,
    SPECIES_SPINARAK,
    SPECIES_CHINCHOU,
    SPECIES_PICHU,
    SPECIES_CLEFFA,
    SPECIES_IGGLYBUFF,
    SPECIES_TOGEPI,
    SPECIES_NATU,
    SPECIES_MAREEP,
    SPECIES_SUDOWOODO,
    SPECIES_HOPPIP,
    SPECIES_AIPOM,
    SPECIES_SUNKERN,
    SPECIES_YANMA,
    SPECIES_WOOPER,
    SPECIES_MURKROW,
    SPECIES_MISDREAVUS,
    SPECIES_UNOWN,
    SPECIES_GIRAFARIG,
    SPECIES_PINECO,
    SPECIES_DUNSPARCE,
    SPECIES_GLIGAR,
    SPECIES_SNUBBULL,
    SPECIES_QWILFISH,
    SPECIES_SHUCKLE,
    SPECIES_HERACROSS,
    SPECIES_SNEASEL,
    SPECIES_TEDDIURSA,
    SPECIES_SLUGMA,
    SPECIES_SWINUB,
    SPECIES_CORSOLA,
    SPECIES_REMORAID,
    SPECIES_DELIBIRD,
    SPECIES_MANTINE,
    SPECIES_SKARMORY,
    SPECIES_HOUNDOUR,
    SPECIES_PHANPY,
    SPECIES_STANTLER,
    SPECIES_SMEARGLE,
    SPECIES_TYROGUE,
    SPECIES_SMOOCHUM,
    SPECIES_ELEKID,
    SPECIES_MAGBY,
    SPECIES_MILTANK,
    SPECIES_LARVITAR,
    SPECIES_TREECKO,
    SPECIES_TORCHIC,
    SPECIES_MUDKIP,
    SPECIES_POOCHYENA,
    SPECIES_ZIGZAGOON,
    SPECIES_WURMPLE,
    SPECIES_LOTAD,
    SPECIES_SEEDOT,
    SPECIES_NINCADA,
    SPECIES_TAILLOW,
    SPECIES_SHROOMISH,
    SPECIES_SPINDA,
    SPECIES_WINGULL,
    SPECIES_SURSKIT,
    SPECIES_WAILMER,
    SPECIES_SKITTY,
    SPECIES_KECLEON,
    SPECIES_BALTOY,
    SPECIES_NOSEPASS,
    SPECIES_TORKOAL,
    SPECIES_SABLEYE,
    SPECIES_BARBOACH,
    SPECIES_LUVDISC,
    SPECIES_CORPHISH,
    SPECIES_FEEBAS,
    SPECIES_CARVANHA,
    SPECIES_TRAPINCH,
    SPECIES_MAKUHITA,
    SPECIES_ELECTRIKE,
    SPECIES_NUMEL,
    SPECIES_SPHEAL,
    SPECIES_CACNEA,
    SPECIES_SNORUNT,
    SPECIES_LUNATONE,
    SPECIES_SOLROCK,
    SPECIES_AZURILL,
    SPECIES_SPOINK,
    SPECIES_PLUSLE,
    SPECIES_MINUN,
    SPECIES_MAWILE,
    SPECIES_MEDITITE,
    SPECIES_SWABLU,
    SPECIES_WYNAUT,
    SPECIES_DUSKULL,
    SPECIES_ROSELIA,
    SPECIES_SLAKOTH,
    SPECIES_GULPIN,
    SPECIES_TROPIUS,
    SPECIES_WHISMUR,
    SPECIES_CLAMPERL,
    SPECIES_ABSOL,
    SPECIES_SHUPPET,
    SPECIES_SEVIPER,
    SPECIES_ZANGOOSE,
    SPECIES_RELICANTH,
    SPECIES_ARON,
    SPECIES_CASTFORM,
    SPECIES_VOLBEAT,
    SPECIES_ILLUMISE,
    SPECIES_LILEEP,
    SPECIES_ANORITH,
    SPECIES_RALTS,
    SPECIES_BAGON,
    SPECIES_BELDUM,
    SPECIES_CHIMECHO,
#ifdef RHH_EXPANSION
    SPECIES_TURTWIG,
    SPECIES_CHIMCHAR,
    SPECIES_PIPLUP,
    SPECIES_STARLY,
    SPECIES_BIDOOF,
    SPECIES_KRICKETOT,
    SPECIES_SHINX,
    SPECIES_BUDEW,
    SPECIES_CRANIDOS,
    SPECIES_SHIELDON,
    SPECIES_BURMY,
    SPECIES_COMBEE,
    SPECIES_PACHIRISU,
    SPECIES_BUIZEL,
    SPECIES_CHERUBI,
    SPECIES_SHELLOS,
    SPECIES_DRIFLOON,
    SPECIES_BUNEARY,
    SPECIES_GLAMEOW,
    SPECIES_CHINGLING,
    SPECIES_STUNKY,
    SPECIES_BRONZOR,
    SPECIES_BONSLY,
    SPECIES_MIME_JR,
    SPECIES_HAPPINY,
    SPECIES_CHATOT,
    SPECIES_SPIRITOMB,
    SPECIES_GIBLE,
    SPECIES_MUNCHLAX,
    SPECIES_RIOLU,
    SPECIES_HIPPOPOTAS,
    SPECIES_SKORUPI,
    SPECIES_CROAGUNK,
    SPECIES_CARNIVINE,
    SPECIES_FINNEON,
    SPECIES_MANTYKE,
    SPECIES_SNOVER,
    SPECIES_ROTOM,
    SPECIES_SNIVY,
    SPECIES_TEPIG,
    SPECIES_OSHAWOTT,
    SPECIES_PATRAT,
    SPECIES_LILLIPUP,
    SPECIES_PURRLOIN,
    SPECIES_PANSAGE,
    SPECIES_PANSEAR,
    SPECIES_PANPOUR,
    SPECIES_MUNNA,
    SPECIES_PIDOVE,
    SPECIES_BLITZLE,
    SPECIES_ROGGENROLA,
    SPECIES_WOOBAT,
    SPECIES_DRILBUR,
    SPECIES_AUDINO,
    SPECIES_TIMBURR,
    SPECIES_TYMPOLE,
    SPECIES_THROH,
    SPECIES_SAWK,
    SPECIES_SEWADDLE,
    SPECIES_VENIPEDE,
    SPECIES_COTTONEE,
    SPECIES_PETILIL,
    SPECIES_BASCULIN,
    SPECIES_SANDILE,
    SPECIES_DARUMAKA,
    SPECIES_MARACTUS,
    SPECIES_DWEBBLE,
    SPECIES_SCRAGGY,
    SPECIES_SIGILYPH,
    SPECIES_YAMASK,
    SPECIES_TIRTOUGA,
    SPECIES_ARCHEN,
    SPECIES_TRUBBISH,
    SPECIES_ZORUA,
    SPECIES_MINCCINO,
    SPECIES_GOTHITA,
    SPECIES_SOLOSIS,
    SPECIES_DUCKLETT,
    SPECIES_VANILLITE,
    SPECIES_DEERLING,
    SPECIES_EMOLGA,
    SPECIES_KARRABLAST,
    SPECIES_FOONGUS,
    SPECIES_FRILLISH,
    SPECIES_ALOMOMOLA,
    SPECIES_JOLTIK,
    SPECIES_FERROSEED,
    SPECIES_KLINK,
    SPECIES_TYNAMO,
    SPECIES_ELGYEM,
    SPECIES_LITWICK,
    SPECIES_AXEW,
    SPECIES_CUBCHOO,
    SPECIES_CRYOGONAL,
    SPECIES_SHELMET,
    SPECIES_STUNFISK,
    SPECIES_MIENFOO,
    SPECIES_DRUDDIGON,
    SPECIES_GOLETT,
    SPECIES_PAWNIARD,
    SPECIES_BOUFFALANT,
    SPECIES_RUFFLET,
    SPECIES_VULLABY,
    SPECIES_HEATMOR,
    SPECIES_DURANT,
    SPECIES_DEINO,
    SPECIES_LARVESTA,
    SPECIES_CHESPIN,
    SPECIES_FENNEKIN,
    SPECIES_FROAKIE,
    SPECIES_BUNNELBY,
    SPECIES_FLETCHLING,
    SPECIES_SCATTERBUG,
    SPECIES_LITLEO,
    SPECIES_FLABEBE,
    SPECIES_SKIDDO,
    SPECIES_PANCHAM,
    SPECIES_FURFROU,
    SPECIES_ESPURR,
    SPECIES_HONEDGE,
    SPECIES_SPRITZEE,
    SPECIES_SWIRLIX,
    SPECIES_INKAY,
    SPECIES_BINACLE,
    SPECIES_SKRELP,
    SPECIES_CLAUNCHER,
    SPECIES_HELIOPTILE,
    SPECIES_TYRUNT,
    SPECIES_AMAURA,
    SPECIES_HAWLUCHA,
    SPECIES_DEDENNE,
    SPECIES_CARBINK,
    SPECIES_GOOMY,
    SPECIES_KLEFKI,
    SPECIES_PHANTUMP,
    SPECIES_PUMPKABOO,
    SPECIES_BERGMITE,
    SPECIES_NOIBAT,
    SPECIES_ROWLET,
    SPECIES_LITTEN,
    SPECIES_POPPLIO,
    SPECIES_PIKIPEK,
    SPECIES_YUNGOOS,
    SPECIES_GRUBBIN,
    SPECIES_CRABRAWLER,
    SPECIES_ORICORIO,
    SPECIES_CUTIEFLY,
    SPECIES_ROCKRUFF,
    SPECIES_WISHIWASHI,
    SPECIES_MAREANIE,
    SPECIES_MUDBRAY,
    SPECIES_DEWPIDER,
    SPECIES_FOMANTIS,
    SPECIES_MORELULL,
    SPECIES_SALANDIT,
    SPECIES_STUFFUL,
    SPECIES_BOUNSWEET,
    SPECIES_COMFEY,
    SPECIES_PASSIMIAN,
    SPECIES_WIMPOD,
    SPECIES_SANDYGAST,
    SPECIES_PYUKUMUKU,
    SPECIES_MINIOR,
    SPECIES_KOMALA,
    SPECIES_TURTONATOR,
    SPECIES_TOGEDEMARU,
    SPECIES_MIMIKYU,
    SPECIES_BRUXISH,
    SPECIES_DRAMPA,
    SPECIES_DHELMISE,
    SPECIES_JANGMO_O,
    SPECIES_MELTAN,
    SPECIES_GROOKEY,
    SPECIES_SCORBUNNY,
    SPECIES_SOBBLE,
    SPECIES_SKWOVET,
    SPECIES_ROOKIDEE,
    SPECIES_BLIPBUG,
    SPECIES_NICKIT,
    SPECIES_GOSSIFLEUR,
    SPECIES_WOOLOO,
    SPECIES_CHEWTLE,
    SPECIES_YAMPER,
    SPECIES_ROLYCOLY,
    SPECIES_APPLIN,
    SPECIES_SILICOBRA,
    SPECIES_CRAMORANT,
    SPECIES_ARROKUDA,
    SPECIES_TOXEL,
    SPECIES_SIZZLIPEDE,
    SPECIES_CLOBBOPUS,
    SPECIES_SINISTEA,
    SPECIES_HATENNA,
    SPECIES_IMPIDIMP,
    SPECIES_MILCERY,
    SPECIES_FALINKS,
    SPECIES_PINCURCHIN,
    SPECIES_SNOM,
    SPECIES_EISCUE,
    SPECIES_INDEEDEE,
    SPECIES_MORPEKO,
    SPECIES_CUFANT,
    SPECIES_DRACOZOLT,
    SPECIES_ARCTOZOLT,
    SPECIES_DRACOVISH,
    SPECIES_ARCTOVISH,
    SPECIES_DURALUDON,
    SPECIES_DREEPY,
    SPECIES_RATTATA_ALOLA,
    SPECIES_SANDSHREW_ALOLA,
    SPECIES_VULPIX_ALOLA,
    SPECIES_DIGLETT_ALOLA,
    SPECIES_MEOWTH_ALOLA,
    SPECIES_GEODUDE_ALOLA,
    SPECIES_GRIMER_ALOLA,
    SPECIES_MEOWTH_GALAR,
    SPECIES_PONYTA_GALAR,
    SPECIES_SLOWPOKE_GALAR,
    SPECIES_FARFETCHD_GALAR,
    SPECIES_MR_MIME_GALAR,
    SPECIES_CORSOLA_GALAR,
    SPECIES_ZIGZAGOON_GALAR,
    SPECIES_DARUMAKA_GALAR,
    SPECIES_YAMASK_GALAR,
    SPECIES_STUNFISK_GALAR,
    SPECIES_GROWLITHE_HISUI,
    SPECIES_VOLTORB_HISUI,
    SPECIES_QWILFISH_HISUI,
    SPECIES_SNEASEL_HISUI,
    SPECIES_ZORUA_HISUI,
    //SPECIES_BURMY_SANDY_CLOAK,
    //SPECIES_BURMY_TRASH_CLOAK,
    SPECIES_SHELLOS_EAST,
    //SPECIES_BASCULIN_BLUE_STRIPED,
    //SPECIES_BASCULIN_WHITE_STRIPED,
    //SPECIES_DEERLING_SUMMER,
    //SPECIES_DEERLING_AUTUMN,
    //SPECIES_DEERLING_WINTER,
    //SPECIES_FLABEBE_YELLOW_FLOWER,
    //SPECIES_FLABEBE_ORANGE_FLOWER,
    //SPECIES_FLABEBE_BLUE_FLOWER,
    //SPECIES_FLABEBE_WHITE_FLOWER,
    //SPECIES_MEOWSTIC_FEMALE,
    SPECIES_PUMPKABOO_SMALL,
    SPECIES_PUMPKABOO_LARGE,
    SPECIES_PUMPKABOO_SUPER,
    SPECIES_ORICORIO_POM_POM,
    SPECIES_ORICORIO_PAU,
    SPECIES_ORICORIO_SENSU,
    SPECIES_ROCKRUFF_OWN_TEMPO,
    SPECIES_MINIOR_METEOR_ORANGE,
    SPECIES_MINIOR_METEOR_YELLOW,
    SPECIES_MINIOR_METEOR_GREEN,
    SPECIES_MINIOR_METEOR_BLUE,
    SPECIES_MINIOR_METEOR_INDIGO,
    SPECIES_MINIOR_METEOR_VIOLET,
    SPECIES_SINISTEA_ANTIQUE,
    //SPECIES_INDEEDEE_FEMALE,
    SPECIES_SPRIGATITO,
    SPECIES_FUECOCO,
    SPECIES_QUAXLY,
    SPECIES_LECHONK,
    SPECIES_TAROUNTULA,
    SPECIES_NYMBLE,
    SPECIES_PAWMI,
    SPECIES_TANDEMAUS,
    SPECIES_FIDOUGH,
    SPECIES_SMOLIV,
    //SPECIES_SQUAWKABILLY_GREEN_PLUMAGE,
    //SPECIES_SQUAWKABILLY_BLUE_PLUMAGE,
    //SPECIES_SQUAWKABILLY_YELLOW_PLUMAGE,
    //SPECIES_SQUAWKABILLY_WHITE_PLUMAGE,
    SPECIES_NACLI,
    SPECIES_CHARCADET,
    SPECIES_TADBULB,
    SPECIES_WATTREL,
    SPECIES_MASCHIFF,
    SPECIES_SHROODLE,
    SPECIES_BRAMBLIN,
    SPECIES_TOEDSCOOL,
    SPECIES_KLAWF,
    SPECIES_CAPSAKID,
    SPECIES_RELLOR,
    SPECIES_FLITTLE,
    SPECIES_TINKATINK,
    SPECIES_WIGLETT,
    SPECIES_BOMBIRDIER,
    SPECIES_FINIZEN,
    SPECIES_VAROOM,
    SPECIES_CYCLIZAR,
    SPECIES_ORTHWORM,
    SPECIES_GLIMMET,
    SPECIES_GREAVARD,
    SPECIES_FLAMIGO,
    SPECIES_CETODDLE,
    SPECIES_VELUZA,
    SPECIES_DONDOZO,
    SPECIES_TATSUGIRI_CURLY,
    SPECIES_TATSUGIRI_DROOPY,
    SPECIES_TATSUGIRI_STRETCHY,
    SPECIES_FRIGIBAX,
    SPECIES_GIMMIGHOUL_CHEST,
    SPECIES_GIMMIGHOUL_ROAMING,
    SPECIES_POLTCHAGEIST_COUNTERFEIT,
    SPECIES_POLTCHAGEIST_ARTISAN,
#endif
};

static u16 PickRandomSpecies(void) // picks only base forms
{
    u16 species = sIsValidSpecies[Random() % NELEMS(sIsValidSpecies)];
    return species;
}

static u8 GetWonderTradeOT(u8 *name)
{
    u8 randGender = (Random() % 2); // 0 for male, 1 for female
    u8 numOTNames = 250;
    u8 selectedName = Random() %numOTNames;
    u8 i;
    if (randGender == MALE) // male OT selected
    {
        randGender = 0;
        for (i = 0; i < 8; ++i)
        {
            name[i] = maleWTNames[selectedName][i];
        }
        name[8] = EOS;
    }
    else                    // female OT selected
    {
        randGender = 0xFF;
        for (i = 0; i < 8; ++i)
        {
            name[i] = femaleWTNames[selectedName][i];
        }
        name[8] = EOS;
    }
    return randGender;
}

void CreateWonderTradePokemon(void)
{
    u16 wonderTradeSpecies = PickRandomSpecies();
    u8 playerMonLevel = GetMonData(&gPlayerParty[gSpecialVar_0x8004], MON_DATA_LEVEL);
    u8 chanceToEvolve = Random() % 99;
    u16 newHeldItem = ITEM_NONE;
    //u16 playerMonHeldItem = GetMonData(&gPlayerParty[gSpecialVar_0x8004], MON_DATA_HELD_ITEM);
    u32 i;
    u8 abilityNum;
    u8 monName[POKEMON_NAME_LENGTH + 1];
    u8 otName[PLAYER_NAME_LENGTH + 1];
    u8 genderOT = GetWonderTradeOT(otName);
    u8 metLocation = METLOC_IN_GAME_TRADE;
    struct WonderTrade sWonderTrades[] =
    {
        [0] = {
            .nickname = _(""),
            .species = wonderTradeSpecies,
            .ivs = {(Random() % 32), (Random() % 32), (Random() % 32), (Random() % 32), (Random() % 32), (Random() % 32)},
            .abilityNum = (Random() % 2), // Can't use NUM_NORMAL_ABILITY_SLOTS because it's not present in Pret/Pokeemerald.
            .otId = 0, // Handled by CreateMon->CreateBoxMon.
            .conditions = {0, 0, 0, 0, 0},
            .personality = 0, // Handled by CreateMon->CreateBoxMon.
            .heldItem = newHeldItem,
            .mailNum = -1,
            .otName = _("ERROR"),
            .otGender = FEMALE, // Decided by the personality generated in CreateMon->CreateBoxMon.
            .sheen = 0,
            .requestedSpecies = GetMonData(&gPlayerParty[gSpecialVar_0x8004], MON_DATA_SPECIES)
        },
    };
    struct WonderTrade *wonderTrade = &sWonderTrades[0];

    // Creates the base of a Pokémon in the first slot of a nonexistent enemy's party.
    CreateMon(&gEnemyParty[0], wonderTradeSpecies, playerMonLevel, USE_RANDOM_IVS, FALSE, 0, OT_ID_PRESET, ((Random() << 16) | Random()));

    // 10% chance of having the generated Wonder Traded 'mon carry an item.
    //if (playerMonHeldItem == ITEM_NONE)
    //{
        for (i = 0; i < 5; i++)
        {
            const struct Evolution *evolutions = GetSpeciesEvolutions(i);
            if (evolutions[i].method == EVO_TRADE_ITEM && Random() % 100 < 50)
            {
                // 30% chance for the in coming Pokémon to hold the item they need to evolve if they need one
                if (Random() % 100 <= 29)
                {
                    newHeldItem = evolutions[i].param;
                    SetMonData(&gEnemyParty[0], MON_DATA_HELD_ITEM, &newHeldItem);
                }
            }
        }
    //}

    if (chanceToEvolve > 69) // 30% to evolve into the highest stage.
    {
        // 1st pass.
        wonderTradeSpecies = GetWonderTradeEvolutionTargetSpecies(&gEnemyParty[0]);
        SetMonData(&gEnemyParty[0], MON_DATA_SPECIES, &wonderTradeSpecies);
        // 2nd pass.
        wonderTradeSpecies = GetWonderTradeEvolutionTargetSpecies(&gEnemyParty[0]);
        SetMonData(&gEnemyParty[0], MON_DATA_SPECIES, &wonderTradeSpecies);
    }
    else if (chanceToEvolve >= 19 && chanceToEvolve <= 69) // 50% to evolve once.
    {
        wonderTradeSpecies = GetWonderTradeEvolutionTargetSpecies(&gEnemyParty[0]);
        SetMonData(&gEnemyParty[0], MON_DATA_SPECIES, &wonderTradeSpecies);
    }

#ifdef RHH_EXPANSION
    // 10% chance of giving the incoming Pokémon their HA, if they have one
    if (gSpeciesInfo[wonderTradeSpecies].abilities[2] != ABILITY_NONE && (Random() % 99) < 50)
    {
        if ((Random() % 99) < 10)
        {
            abilityNum = 2;
            SetMonData(&gEnemyParty[0], MON_DATA_ABILITY_NUM, &abilityNum);
        }
    }
#endif

    SetMonData(&gEnemyParty[0], MON_DATA_HELD_ITEM, &newHeldItem);
    SetMonData(&gEnemyParty[0], MON_DATA_HP_IV, &wonderTrade->ivs[0]);
    SetMonData(&gEnemyParty[0], MON_DATA_ATK_IV, &wonderTrade->ivs[1]);
    SetMonData(&gEnemyParty[0], MON_DATA_DEF_IV, &wonderTrade->ivs[2]);
    SetMonData(&gEnemyParty[0], MON_DATA_SPEED_IV, &wonderTrade->ivs[3]);
    SetMonData(&gEnemyParty[0], MON_DATA_SPATK_IV, &wonderTrade->ivs[4]);
    SetMonData(&gEnemyParty[0], MON_DATA_SPDEF_IV, &wonderTrade->ivs[5]);
#ifdef RHH_EXPANSION
    StringCopy(monName, GetSpeciesName(wonderTradeSpecies));
#else
    GetSpeciesName(monName, wonderTradeSpecies);
#endif
    SetMonData(&gEnemyParty[0], MON_DATA_NICKNAME, monName);
    SetMonData(&gEnemyParty[0], MON_DATA_OT_NAME, otName);
    SetMonData(&gEnemyParty[0], MON_DATA_BEAUTY, &wonderTrade->conditions[1]);
    SetMonData(&gEnemyParty[0], MON_DATA_CUTE, &wonderTrade->conditions[2]);
    SetMonData(&gEnemyParty[0], MON_DATA_COOL, &wonderTrade->conditions[0]);
    SetMonData(&gEnemyParty[0], MON_DATA_SMART, &wonderTrade->conditions[3]);
    SetMonData(&gEnemyParty[0], MON_DATA_TOUGH, &wonderTrade->conditions[4]);
    SetMonData(&gEnemyParty[0], MON_DATA_SHEEN, &wonderTrade->sheen);
    SetMonData(&gEnemyParty[0], MON_DATA_OT_GENDER, &genderOT);
    SetMonData(&gEnemyParty[0], MON_DATA_MET_LOCATION, &metLocation);
    CalculateMonStats(&gEnemyParty[0]);
}

static u16 GetWonderTradeEvolutionTargetSpecies(struct Pokemon *mon)
{
    int i, j;
    u16 targetSpecies = 0;
    u16 partnerSpecies = GetMonData(mon, MON_DATA_SPECIES);
    u32 partnerPersonality = GetMonData(mon, MON_DATA_PERSONALITY);
    u16 upperPersonality = partnerPersonality >> 16;
    u8 partnerLevel = GetMonData(mon, MON_DATA_LEVEL);
    u16 rndEeveevolutionChance = Random() % 8;
    u16 rndAlcrememeEvoChance = Random() % 9;
    u16 currentMap = ((gSaveBlock1Ptr->location.mapGroup) << 8 | gSaveBlock1Ptr->location.mapNum);
    u16 partnerHeldItem = GetMonData(mon, MON_DATA_HELD_ITEM);
    u16 playerSpecies = GetMonData(&gPlayerParty[gSpecialVar_0x8004], MON_DATA_SPECIES);
    u16 partnerHoldEffect = ItemId_GetHoldEffect(partnerHeldItem);

    // partnerSpecies-specific exceptions.
    if (partnerSpecies == SPECIES_NINCADA && partnerLevel >= 20)
    {
        if ((Random() % 2) == 0)
            targetSpecies = SPECIES_NINJASK;
        else
            targetSpecies = SPECIES_SHEDINJA;
        return targetSpecies;
    }
    else if (partnerSpecies == SPECIES_EEVEE && partnerLevel >= 25)
    {
        if (rndEeveevolutionChance == 0)
            targetSpecies = SPECIES_VAPOREON;
        else if (rndEeveevolutionChance == 1)
            targetSpecies = SPECIES_JOLTEON;
        else if (rndEeveevolutionChance == 2)
            targetSpecies = SPECIES_FLAREON;
        else if (rndEeveevolutionChance == 3)
            targetSpecies = SPECIES_ESPEON;
        else if (rndEeveevolutionChance == 4)
            targetSpecies = SPECIES_UMBREON;
        else if (rndEeveevolutionChance == 5)
            targetSpecies = SPECIES_LEAFEON;
        else if (rndEeveevolutionChance == 6)
            targetSpecies = SPECIES_GLACEON;
        else if (rndEeveevolutionChance == 7)
            targetSpecies = SPECIES_SYLVEON;
        return targetSpecies;
    }
    else if (partnerSpecies == SPECIES_MILCERY && partnerLevel >= 25)
    {
        if (rndAlcrememeEvoChance == 0)
            targetSpecies = SPECIES_ALCREMIE;
        else if (rndAlcrememeEvoChance == 1)
            targetSpecies = SPECIES_ALCREMIE_RUBY_CREAM;
        else if (rndAlcrememeEvoChance == 2)
            targetSpecies = SPECIES_ALCREMIE_MATCHA_CREAM;
        else if (rndAlcrememeEvoChance == 3)
            targetSpecies = SPECIES_ALCREMIE_MINT_CREAM;
        else if (rndAlcrememeEvoChance == 4)
            targetSpecies = SPECIES_ALCREMIE_LEMON_CREAM;
        else if (rndAlcrememeEvoChance == 5)
            targetSpecies = SPECIES_ALCREMIE_SALTED_CREAM;
        else if (rndAlcrememeEvoChance == 6)
            targetSpecies = SPECIES_ALCREMIE_RUBY_SWIRL;
        else if (rndAlcrememeEvoChance == 7)
            targetSpecies = SPECIES_ALCREMIE_CARAMEL_SWIRL;
        else if (rndAlcrememeEvoChance == 8)
            targetSpecies = SPECIES_ALCREMIE_RAINBOW_SWIRL;
        return targetSpecies;
    }

    for (i = 0; i < 5; i++)
    {
        const struct Evolution *evolutions = GetSpeciesEvolutions(i);
        switch (evolutions[i].method)
        {
        case EVO_FRIENDSHIP:
            if ((partnerSpecies == SPECIES_PICHU || partnerSpecies == SPECIES_CLEFFA || partnerSpecies == SPECIES_IGGLYBUFF
              || partnerSpecies == SPECIES_TOGEPI || partnerSpecies == SPECIES_AZURILL || partnerSpecies == SPECIES_BUDEW
              || partnerSpecies == SPECIES_BUNEARY || partnerSpecies == SPECIES_SWOOBAT || partnerSpecies == SPECIES_SWADLOON)
              && partnerLevel >= 16)
                targetSpecies = evolutions[i].targetSpecies;
            else if (partnerSpecies == SPECIES_MEOWTH_ALOLA && partnerLevel >= 28)
                targetSpecies = evolutions[i].targetSpecies;
            else if ((partnerSpecies == SPECIES_GOLBAT || partnerSpecies == SPECIES_CHANSEY
                   || partnerSpecies == SPECIES_MUNCHLAX) && partnerLevel >= 35)
                targetSpecies = evolutions[i].targetSpecies;
            break;
        case EVO_LEVEL:
            if (partnerSpecies == SPECIES_SLOWPOKE && partnerLevel >= 37)
            {
                if ((Random() % 2) == 0)
                    targetSpecies = SPECIES_SLOWBRO;
                else
                    targetSpecies = SPECIES_SLOWKING;
            }
            else if (partnerSpecies == SPECIES_TOXEL && partnerLevel >= 30)
            {
                if (GetNature(mon) == NATURE_HARDY || GetNature(mon) == NATURE_BRAVE
                 || GetNature(mon) == NATURE_ADAMANT || GetNature(mon) == NATURE_NAUGHTY
                 || GetNature(mon) == NATURE_DOCILE || GetNature(mon) == NATURE_IMPISH
                 || GetNature(mon) == NATURE_LAX || GetNature(mon) == NATURE_HASTY
                 || GetNature(mon) == NATURE_JOLLY || GetNature(mon) == NATURE_NAIVE
                 || GetNature(mon) == NATURE_RASH || GetNature(mon) == NATURE_SASSY
                 || GetNature(mon) == NATURE_QUIRKY)
                    targetSpecies = SPECIES_TOXTRICITY;
                else
                    targetSpecies = SPECIES_TOXTRICITY_LOW_KEY;
            }
            else if (evolutions[i].param <= partnerLevel)
            {
                targetSpecies = evolutions[i].targetSpecies;
            }
            break;
        case EVO_LEVEL_ATK_GT_DEF:
            if (evolutions[i].param <= partnerLevel)
            {
                if (GetMonData(mon, MON_DATA_ATK, 0) > GetMonData(mon, MON_DATA_DEF, 0))
                    targetSpecies = evolutions[i].targetSpecies;
            }
            break;
        case EVO_LEVEL_ATK_EQ_DEF:
            if (evolutions[i].param <= partnerLevel)
            {
                if (GetMonData(mon, MON_DATA_ATK, 0) == GetMonData(mon, MON_DATA_DEF, 0))
                    targetSpecies = evolutions[i].targetSpecies;
            }
            break;
        case EVO_LEVEL_ATK_LT_DEF:
            if (evolutions[i].param <= partnerLevel)
            {
                if (GetMonData(mon, MON_DATA_ATK, 0) < GetMonData(mon, MON_DATA_DEF, 0))
                    targetSpecies = evolutions[i].targetSpecies;
            }
            break;
        case EVO_LEVEL_SILCOON:
            if (evolutions[i].param <= partnerLevel && (upperPersonality % 10) <= 4)
                targetSpecies = evolutions[i].targetSpecies;
            break;
        case EVO_LEVEL_CASCOON:
            if (evolutions[i].param <= partnerLevel && (upperPersonality % 10) > 4)
                targetSpecies = evolutions[i].targetSpecies;
            break;
        case EVO_BEAUTY:
            if (partnerLevel >= 30)
                targetSpecies = evolutions[i].targetSpecies;
            break;
        case EVO_ITEM:
            if (partnerSpecies == SPECIES_GLOOM && partnerLevel >= 36)
            {
                if ((Random() % 2) == 0)
                    targetSpecies = SPECIES_VILEPLUME;
                else
                    targetSpecies = SPECIES_BELLOSSOM;
            }
            else if (partnerSpecies == SPECIES_WEEPINBELL && partnerLevel >= 36)
            {
                targetSpecies = evolutions[i].targetSpecies;
            }
            else if ((partnerSpecies == SPECIES_VULPIX || partnerSpecies == SPECIES_GROWLITHE) && partnerLevel >= 32)
            {
                targetSpecies = evolutions[i].targetSpecies;
            }
            else if ((partnerSpecies == SPECIES_SHELLDER || partnerSpecies == SPECIES_STARYU) && partnerLevel >= 43)
            {
                targetSpecies = evolutions[i].targetSpecies;
            }
            else if ((partnerSpecies == SPECIES_NIDORINA || partnerSpecies == SPECIES_NIDORINO || partnerSpecies == SPECIES_EXEGGCUTE) && partnerLevel >= 26)
            {
                targetSpecies = evolutions[i].targetSpecies;
            }
            else if ((partnerSpecies == SPECIES_JIGGLYPUFF || partnerSpecies == SPECIES_CLEFAIRY || partnerSpecies == SPECIES_SKITTY) && partnerLevel >= 38)
            {
                targetSpecies = evolutions[i].targetSpecies;
            }
            else if ((partnerSpecies == SPECIES_LOMBRE || partnerSpecies == SPECIES_NUZLEAF) && partnerLevel >= 38)
            {
                targetSpecies = evolutions[i].targetSpecies;
            }
            else if (partnerSpecies == SPECIES_POLIWHIRL && partnerLevel >= 44)
            {
                if ((Random() % 2) == 0)
                    targetSpecies = SPECIES_POLIWRATH;
                else
                    targetSpecies = SPECIES_POLITOED;
            }
            else if (partnerSpecies == SPECIES_PIKACHU && partnerLevel >= 27)
            {
                targetSpecies = evolutions[i].targetSpecies;
            }
            else if (partnerSpecies == SPECIES_SUNKERN && partnerLevel >= 15)
            {
                targetSpecies = evolutions[i].targetSpecies;
            }
            else if ((partnerSpecies == SPECIES_MURKROW || partnerSpecies == SPECIES_MISDREAVUS
                   || partnerSpecies == SPECIES_MUNNA || partnerSpecies == SPECIES_DOUBLADE
                   || partnerSpecies == SPECIES_SINISTEA || partnerSpecies == SPECIES_SANDSHREW_ALOLA
                   || partnerSpecies == SPECIES_VULPIX_ALOLA || partnerSpecies == SPECIES_SINISTEA_ANTIQUE)
                   && partnerLevel >= 25)
            {
                targetSpecies = evolutions[i].targetSpecies;
            }
            else if ((partnerSpecies == SPECIES_PANSAGE || partnerSpecies == SPECIES_PANSEAR
                   || partnerSpecies == SPECIES_PANPOUR || partnerSpecies == SPECIES_COTTONEE
                   || partnerSpecies == SPECIES_PETILIL || partnerSpecies == SPECIES_MINCCINO
                   ) && partnerLevel >= 15)
            {
                targetSpecies = evolutions[i].targetSpecies;
            }
            else if ((partnerSpecies == SPECIES_EELEKTRIK || partnerSpecies == SPECIES_LAMPENT
                   || partnerSpecies == SPECIES_HELIOPTILE || partnerSpecies == SPECIES_CHARJABUG
                   || partnerSpecies == SPECIES_DARUMAKA_GALAR) && partnerLevel >= 35)
            {
                targetSpecies = evolutions[i].targetSpecies;
            }
            else if (partnerSpecies == SPECIES_APPLIN && partnerLevel >= 30)
            {
                if ((Random() % 2) == 0)
                    targetSpecies = SPECIES_FLAPPLE;
                else
                    targetSpecies = SPECIES_APPLETUN;
            }
            else if (partnerSpecies == SPECIES_SLOWPOKE_GALAR && partnerLevel >= 37)
            {
                if ((Random() % 2) == 0)
                    targetSpecies = SPECIES_SLOWBRO_GALAR;
                else
                    targetSpecies = SPECIES_SLOWKING_GALAR;
            }
            break;
        case EVO_ITEM_FEMALE:
            if (GetMonGender(mon) == MON_FEMALE && partnerSpecies == SPECIES_SNORUNT && partnerLevel >= 35)
                targetSpecies = evolutions[i].targetSpecies;
            break;
        case EVO_ITEM_MALE:
            if (GetMonGender(mon) == MON_MALE && partnerSpecies == SPECIES_KIRLIA && partnerLevel >= 35)
                targetSpecies = evolutions[i].targetSpecies;
            break;
        case EVO_MOVE:
            if (MonKnowsMove(mon, evolutions[i].param))
                targetSpecies = evolutions[i].targetSpecies;
            break;
        case EVO_FRIENDSHIP_MOVE_TYPE:
            for (j = 0; j < MAX_MON_MOVES; j++)
            {
                if (gMovesInfo[GetMonData(mon, MON_DATA_MOVE1 + j, NULL)].type == evolutions[i].param)
                {
                    targetSpecies = evolutions[i].targetSpecies;
                    break;
                }
            }
            break;
        case EVO_LEVEL_DARK_TYPE_MON_IN_PARTY:
            if (evolutions[i].param <= partnerLevel)
            {
                for (j = 0; j < PARTY_SIZE; j++)
                {
                    u16 currSpecies = GetMonData(&gPlayerParty[j], MON_DATA_SPECIES, NULL);
                    if (gSpeciesInfo[currSpecies].types[0] == TYPE_DARK
                     || gSpeciesInfo[currSpecies].types[1] == TYPE_DARK)
                    {
                        targetSpecies = evolutions[i].targetSpecies;
                        break;
                    }
                }
            }
            break;
        case EVO_LEVEL_RAIN:
            j = GetCurrentWeather();
            if (j == WEATHER_RAIN || j == WEATHER_RAIN_THUNDERSTORM || j == WEATHER_DOWNPOUR)
                targetSpecies = evolutions[i].targetSpecies;
            break;
        case EVO_MAPSEC:
            if (gMapHeader.regionMapSectionId == evolutions[i].param)
                targetSpecies = evolutions[i].targetSpecies;
            break;
        case EVO_SPECIFIC_MAP:
            if (currentMap == evolutions[i].param)
                targetSpecies = evolutions[i].targetSpecies;
            break;
        case EVO_SPECIFIC_MON_IN_PARTY:
            for (j = 0; j < PARTY_SIZE; j++)
            {
                if (GetMonData(&gPlayerParty[j], MON_DATA_SPECIES) == evolutions[i].param)
                    targetSpecies = evolutions[i].targetSpecies;
            }
            break;
        case EVO_TRADE:
            targetSpecies = evolutions[i].targetSpecies;
            break;
        case EVO_TRADE_ITEM:
            if (evolutions[i].param == partnerHeldItem)
            {
                partnerHeldItem = ITEM_NONE;
                SetMonData(mon, MON_DATA_HELD_ITEM, &partnerHeldItem);
                targetSpecies = evolutions[i].targetSpecies;
            }
            break;
        case EVO_TRADE_SPECIFIC_MON:
            if (evolutions[i].param == playerSpecies && partnerHoldEffect != HOLD_EFFECT_PREVENT_EVOLVE)
                targetSpecies = evolutions[i].targetSpecies;
            break;
        case EVO_LEVEL_FOG:
            j = GetCurrentWeather();
            if (evolutions[i].param <= partnerLevel && (j == WEATHER_FOG_HORIZONTAL || j == WEATHER_FOG_DIAGONAL))
                targetSpecies = evolutions[i].targetSpecies;
            break;
        case EVO_LEVEL_NATURE_AMPED:
            if (evolutions[i].param <= partnerLevel)
            {
                u8 nature = GetNature(mon);
                switch (nature)
                {
                case NATURE_HARDY:
                case NATURE_BRAVE:
                case NATURE_ADAMANT:
                case NATURE_NAUGHTY:
                case NATURE_DOCILE:
                case NATURE_IMPISH:
                case NATURE_LAX:
                case NATURE_HASTY:
                case NATURE_JOLLY:
                case NATURE_NAIVE:
                case NATURE_RASH:
                case NATURE_SASSY:
                case NATURE_QUIRKY:
                    targetSpecies = evolutions[i].targetSpecies;
                    break;
                }
            }
            break;
        case EVO_LEVEL_NATURE_LOW_KEY:
            if (evolutions[i].param <= partnerLevel)
            {
                u8 nature = GetNature(mon);
                switch (nature)
                {
                case NATURE_LONELY:
                case NATURE_BOLD:
                case NATURE_RELAXED:
                case NATURE_TIMID:
                case NATURE_SERIOUS:
                case NATURE_MODEST:
                case NATURE_MILD:
                case NATURE_QUIET:
                case NATURE_BASHFUL:
                case NATURE_CALM:
                case NATURE_GENTLE:
                case NATURE_CAREFUL:
                    targetSpecies = evolutions[i].targetSpecies;
                    break;
                }
            }
            break;
        case EVO_ITEM_HOLD:
            if (partnerHeldItem == evolutions[i].param)
            {
                partnerHeldItem = 0;
                SetMonData(mon, MON_DATA_HELD_ITEM, &partnerHeldItem);
                targetSpecies = evolutions[i].targetSpecies;
            }
            break;
        }
    }

    return (targetSpecies == 0) ? partnerSpecies : targetSpecies;
}