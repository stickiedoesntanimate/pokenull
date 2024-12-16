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

// This file's functions.
static u16 PickRandomSpecies(void);
static u8 GetWonderTradeOT(u8 *name);

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

static const u16 sCommonMons[] = {
    SPECIES_VENIPEDE,
    SPECIES_NINCADA,
    SPECIES_PANSAGE,
    SPECIES_PANPOUR,
    SPECIES_PANSEAR,
    SPECIES_FLETCHLING,
    SPECIES_PIKIPEK,
    SPECIES_ZIGZAGOON,
    SPECIES_ZIGZAGOON_GALAR,
    SPECIES_HOUNDOUR,
    SPECIES_ROCKRUFF,
    SPECIES_FOMANTIS,
    SPECIES_FERROSEED,
    SPECIES_PARAS,
    SPECIES_PARASECT,
    SPECIES_MURKROW,
    SPECIES_MORELULL,
    SPECIES_IMPIDIMP,
    SPECIES_HATENNA,
    SPECIES_VULPIX,
    SPECIES_VULPIX_ALOLA,
    SPECIES_GROWLITHE,
    SPECIES_SKIDDO,
    SPECIES_SALANDIT,
    SPECIES_CHARCADET,
    SPECIES_SANDSHREW,
    SPECIES_SANDSHREW_ALOLA,
    SPECIES_WIMPOD,
    SPECIES_GLIGAR,
    SPECIES_ARON,
    SPECIES_CARBINK,
    SPECIES_CHINGLING,
    SPECIES_TRUBBISH,
    SPECIES_TINKATINK,
    SPECIES_SLUGMA,
    SPECIES_FOONGUS,
    SPECIES_CUBONE,
    SPECIES_DWEBBLE,
    SPECIES_SANDYGAST,
    SPECIES_WOOPER,
    SPECIES_WOOPER_PALDEA,
    SPECIES_SHELLOS_EAST,
    SPECIES_TOGEPI,
    SPECIES_WINGULL,
    SPECIES_EKANS,
    SPECIES_SHROOMISH,
    SPECIES_PAWMI,
    SPECIES_SLOWPOKE,
    SPECIES_BUDEW,
    SPECIES_PETILIL,
    SPECIES_CUTIEFLY,
    SPECIES_JOLTIK,
    SPECIES_SNOM,
    SPECIES_PUMPKABOO,
    SPECIES_LITWICK,
    SPECIES_MILCERY,
    SPECIES_DRIFLOON,
    SPECIES_SHUPPET,
    SPECIES_KOFFING,
    SPECIES_RALTS,
    SPECIES_YAMASK,
    SPECIES_YAMASK_GALAR,
    SPECIES_SOLOSIS,
    SPECIES_INKAY,
    SPECIES_RIOLU,
    SPECIES_TOXEL,
    SPECIES_CARVANHA,
    SPECIES_FEEBAS,
    SPECIES_APPLIN,
    SPECIES_PORYGON,
    SPECIES_SHINX,
    SPECIES_TADBULB,
    SPECIES_MANKEY,
    SPECIES_AIPOM,
    SPECIES_CHINCHOU,
};

static const u16 sUncommonMons[] = {
    SPECIES_WHIRLIPEDE,
    SPECIES_NINJASK,
    SPECIES_SIMISAGE,
    SPECIES_SIMIPOUR,
    SPECIES_SIMISEAR,
    SPECIES_FLETCHINDER,
    SPECIES_TRUMBEAK,
    SPECIES_DUNSPARCE,
    SPECIES_TANDEMAUS,
    SPECIES_LINOONE,
    SPECIES_LINOONE_GALAR,
    SPECIES_HOUNDOOM,
    SPECIES_LYCANROC,
    SPECIES_LYCANROC_MIDNIGHT,
    SPECIES_LURANTIS,
    SPECIES_FERROTHORN,
    SPECIES_SHIINOTIC,
    SPECIES_MORGREM,
    SPECIES_HATTREM,
    SPECIES_DEDENNE,
    SPECIES_NINETALES,
    SPECIES_ARCANINE,
    SPECIES_GROWLITHE_HISUI,
    SPECIES_GOGOAT,
    SPECIES_ARMAROUGE,
    SPECIES_CERULEDGE,
    SPECIES_SANDSLASH,
    SPECIES_SMEARGLE,
    SPECIES_LAIRON,
    SPECIES_SABLEYE,
    SPECIES_MAWILE,
    SPECIES_CHIMECHO,
    SPECIES_GARBODOR,
    SPECIES_KLINK,
    SPECIES_KLANG,
    SPECIES_VAROOM,
    SPECIES_TINKATUFF,
    SPECIES_NUMEL,
    SPECIES_TRAPINCH,
    SPECIES_MAGCARGO,
    SPECIES_AMOONGUSS,
    SPECIES_MAROWAK,
    SPECIES_LARVESTA,
    SPECIES_PALOSSAND,
    SPECIES_QUAGSIRE,
    SPECIES_CLODSIRE,
    SPECIES_SHELLOS_WEST,
    SPECIES_QWILFISH,
    SPECIES_QWILFISH_HISUI,
    SPECIES_FALINKS,
    SPECIES_HAWLUCHA,
    SPECIES_DRILBUR,
    SPECIES_SCRAGGY,
    SPECIES_SWIRLIX,
    SPECIES_TOGETIC,
    SPECIES_PELIPPER,
    SPECIES_SWABLU,
    SPECIES_ARBOK,
    SPECIES_CAPSAKID,
    SPECIES_BRELOOM,
    SPECIES_PAWMO,
    SPECIES_ZANGOOSE,
    SPECIES_SEVIPER,
    SPECIES_SLOWPOKE_GALAR,
    SPECIES_SLOWBRO,
    SPECIES_ROSELIA,
    SPECIES_COTTONEE,
    SPECIES_DURANT,
    SPECIES_HEATMOR,
    SPECIES_SCYTHER,
    SPECIES_BALTOY,
    SPECIES_GALVANTULA,
    SPECIES_FROSMOTH,
    SPECIES_SNORUNT,
    SPECIES_GOURGEIST,
    SPECIES_POLTCHAGEIST,
    SPECIES_LAMPENT,
    SPECIES_KLEFKI,
    SPECIES_MIMIKYU,
    SPECIES_ALCREMIE_STRAWBERRY_RUBY_CREAM,
    //SPECIES_ALCREMIE_STRAWBERRY_MATCHA_CREAM,
    //SPECIES_ALCREMIE_STRAWBERRY_MINT_CREAM,
    //SPECIES_ALCREMIE_STRAWBERRY_LEMON_CREAM,
    //SPECIES_ALCREMIE_STRAWBERRY_SALTED_CREAM,
    //SPECIES_ALCREMIE_STRAWBERRY_RUBY_SWIRL,
    //SPECIES_ALCREMIE_STRAWBERRY_CARAMEL_SWIRL,
    //SPECIES_ALCREMIE_STRAWBERRY_RAINBOW_SWIRL,
    SPECIES_TOEDSCOOL,
    SPECIES_DRIFBLIM,
    SPECIES_BANETTE,
    SPECIES_KIRLIA,
    SPECIES_MARACTUS,
    SPECIES_SIGILYPH,
    SPECIES_ZORUA,
    SPECIES_ZORUA_HISUI,
    SPECIES_DUOSION,
    SPECIES_ABSOL,
    SPECIES_MALAMAR,
    SPECIES_LUCARIO,
    SPECIES_SHARPEDO,
    SPECIES_HORSEA,
    SPECIES_CORPHISH,
    SPECIES_SNEASEL,
    SPECIES_SNEASEL_HISUI,
    SPECIES_BRUXISH,
    SPECIES_FLAPPLE,
    SPECIES_APPLETUN,
    SPECIES_PORYGON2,
    SPECIES_TROPIUS,
    SPECIES_LUXIO,
    SPECIES_STONJOURNER,
    SPECIES_EISCUE,
    SPECIES_BELLIBOLT,
    SPECIES_STUFFUL,
    SPECIES_AMBIPOM,
    SPECIES_LANTURN,
    SPECIES_EEVEE,
};

static const u16 sRareMons[] = {
    SPECIES_SCOLIPEDE,
    SPECIES_SHEDINJA,
    SPECIES_TALONFLAME,
    SPECIES_TOUCANNON,
    SPECIES_DUDUNSPARCE,
    SPECIES_MAUSHOLD,
    SPECIES_OBSTAGOON,
    SPECIES_ROCKRUFF_OWN_TEMPO,
    SPECIES_HONCHKROW,
    SPECIES_GRIMMSNARL,
    SPECIES_HATTERENE,
    SPECIES_NINETALES_ALOLA,
    SPECIES_ARCANINE_HISUI,
    SPECIES_SANDSLASH_ALOLA,
    SPECIES_GOLISOPOD,
    SPECIES_GLISCOR,
    SPECIES_KLINKLANG,
    SPECIES_REVAVROOM,
    SPECIES_TINKATON,
    SPECIES_CAMERUPT,
    SPECIES_VIBRAVA,
    SPECIES_CRUSTLE,
    SPECIES_GASTRODON_EAST,
    SPECIES_GASTRODON_WEST,
    SPECIES_SKRELP,
    SPECIES_DHELMISE,
    SPECIES_MAREANIE,
    SPECIES_EXCADRILL,
    SPECIES_SCRAFTY,
    SPECIES_SLURPUFF,
    SPECIES_TOGEKISS,
    SPECIES_SCOVILLAIN,
    SPECIES_PAWMOT,
    SPECIES_SLOWKING,
    SPECIES_SLOWBRO_GALAR,
    SPECIES_WHIMSICOTT,
    SPECIES_SCIZOR,
    SPECIES_LILLIGANT,
    SPECIES_CLAYDOL,
    SPECIES_RIBOMBEE,
    SPECIES_GLALIE,
    SPECIES_SINISTCHA,
    SPECIES_CHANDELURE,
    SPECIES_TOEDSCRUEL,
    SPECIES_WEEZING,
    SPECIES_COFAGRIGUS,
    SPECIES_ZOROARK,
    SPECIES_ZOROARK_HISUI,
    SPECIES_ROTOM,
    //SPECIES_ROTOM_HEAT,
    //SPECIES_ROTOM_WASH,
    //SPECIES_ROTOM_MOW,
    //SPECIES_ROTOM_FAN,
    //SPECIES_ROTOM_FROST,
    SPECIES_REUNICLUS,
    SPECIES_SEADRA,
    SPECIES_CRAWDAUNT,
    SPECIES_WAILMER,
    SPECIES_WEAVILE,
    SPECIES_SNEASLER,
    SPECIES_CETODDLE,
    SPECIES_DIPPLIN,
    SPECIES_LUXRAY,
    SPECIES_PRIMEAPE,
    SPECIES_BEWEAR,
    SPECIES_LAPRAS,
    SPECIES_DITTO,
    SPECIES_ANORITH,
    SPECIES_LILEEP,
    SPECIES_TIRTOUGA,
    SPECIES_ARCHEN,
    SPECIES_TYRUNT,
    SPECIES_AMAURA,
};

static const u16 sSuperRareMons[] = {
    SPECIES_DUDUNSPARCE_THREE_SEGMENT,
    SPECIES_MAUSHOLD_THREE,
    SPECIES_LYCANROC_DUSK,
    SPECIES_SALAZZLE,
    SPECIES_AGGRON,
    SPECIES_FLYGON,
    SPECIES_VOLCARONA,
    SPECIES_DRAGALGE,
    SPECIES_OVERQWIL,
    SPECIES_TOXAPEX,
    SPECIES_ALTARIA,
    SPECIES_SLOWKING_GALAR,
    SPECIES_ROSERADE,
    SPECIES_KLEAVOR,
    SPECIES_FROSLASS,
    SPECIES_WEEZING_GALAR,
    SPECIES_GARDEVOIR,
    SPECIES_GALLADE,
    SPECIES_RUNERIGUS,
    SPECIES_KINGDRA,
    SPECIES_MILOTIC,
    SPECIES_WAILORD,
    SPECIES_CETITAN,
    SPECIES_HYDRAPPLE,
    SPECIES_PORYGON_Z,
    SPECIES_ANNIHILAPE,
    SPECIES_SPIRITOMB,
    SPECIES_VAPOREON,
    SPECIES_FLAREON,
    SPECIES_JOLTEON,
    SPECIES_ESPEON,
    SPECIES_UMBREON,
    SPECIES_LEAFEON,
    SPECIES_GLACEON,
    SPECIES_SYLVEON,
    SPECIES_LARVITAR,
    SPECIES_GOOMY,
    SPECIES_FRIGIBAX,
    SPECIES_BULBASAUR,//Starters
    SPECIES_CHARMANDER,
    SPECIES_SQUIRTLE,
    SPECIES_CHIKORITA,
    SPECIES_CYNDAQUIL,
    SPECIES_TOTODILE,
    SPECIES_TREECKO,
    SPECIES_TORCHIC,
    SPECIES_MUDKIP,
    SPECIES_PIPLUP,
    SPECIES_CHIMCHAR,
    SPECIES_TURTWIG,
    SPECIES_SNIVY,
    SPECIES_OSHAWOTT,
    SPECIES_TEPIG,
    SPECIES_FENNEKIN,
    SPECIES_FROAKIE,
    SPECIES_CHESPIN,
    SPECIES_ROWLET,
    SPECIES_LITTEN,
    SPECIES_POPPLIO,
    SPECIES_SCORBUNNY,
    SPECIES_SOBBLE,
    SPECIES_GROOKEY,
    SPECIES_SPRIGATITO,
    SPECIES_QUAXLY,
    SPECIES_FUECOCO,
};

static u16 PickRandomSpecies(void)
{
    u8 rarermonchance = (Random() % 100);
    u16 species = 1;
    if (rarermonchance >= 65 && FlagGet(FLAG_BADGE02_GET) == TRUE)//35 for uncommon, 65 for common, -10 uncommon for rare, -10 common for superrare
    {
        if (rarermonchance >= 75 && FlagGet(FLAG_BADGE05_GET) == TRUE)//and gym 5
        {
            if (rarermonchance <= 10 && FlagGet(FLAG_BADGE07_GET) == TRUE)//and gym 7
            {
            species = sSuperRareMons[Random() % NELEMS(sSuperRareMons)];
            }
            else
            {
            species = sRareMons[Random() % NELEMS(sRareMons)];
            }
        }
        else
        {
        species = sUncommonMons[Random() % NELEMS(sUncommonMons)];
        }
    }
    else
    {
        species = sCommonMons[Random() % NELEMS(sCommonMons)];
    }
    if (species == SPECIES_ROTOM)
    {
        rarermonchance = (Random() % 5);
        if (rarermonchance != 0)
        {
            species = 1064 + rarermonchance;
        }
    }
    if (species == SPECIES_ALCREMIE_STRAWBERRY_RUBY_CREAM)
    {
        species += (Random() % 7);
    }
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
    else// female OT selected
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
    u8 playerMonLevel = 5;

    playerMonLevel += (Random() % 5);
    if (FlagGet(FLAG_BADGE01_GET) == TRUE)
    {
        playerMonLevel += 7;
        playerMonLevel += (Random() % 2);
        playerMonLevel -= 2;
    };
    if (FlagGet(FLAG_BADGE02_GET) == TRUE)
    {
        playerMonLevel += 3;
    };
    if (FlagGet(FLAG_BADGE03_GET) == TRUE)
    {
        playerMonLevel += 5;
    };
    if (FlagGet(FLAG_BADGE04_GET) == TRUE)
    {
        playerMonLevel += 5;
    };
    if (FlagGet(FLAG_BADGE05_GET) == TRUE)
    {
        playerMonLevel += 5;
    };
    if (FlagGet(FLAG_BADGE06_GET) == TRUE)
    {
        playerMonLevel += 4;
        playerMonLevel += (Random() % 2);
        playerMonLevel -= 2;
    };
    if (FlagGet(FLAG_BADGE07_GET) == TRUE)
    {
        playerMonLevel += 8;
        playerMonLevel += (Random() % 4);
        playerMonLevel -= 4;
    };
    if (FlagGet(FLAG_BADGE08_GET) == TRUE)
    {
        playerMonLevel += 3;
        playerMonLevel += (Random() % 2);
        playerMonLevel -= 2;
    };

    u16 newHeldItem = ITEM_NONE;
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
    // 10% chance of giving the incoming Pokémon their HA, if they have one
    if (gSpeciesInfo[wonderTradeSpecies].abilities[2] != ABILITY_NONE && (Random() % 99) < 50)
    {
        if ((Random() % 99) < 10)
        {
            abilityNum = 2;
            SetMonData(&gEnemyParty[0], MON_DATA_ABILITY_NUM, &abilityNum);
        }
    }


    SetMonData(&gEnemyParty[0], MON_DATA_HELD_ITEM, &newHeldItem);
    SetMonData(&gEnemyParty[0], MON_DATA_HP_IV, &wonderTrade->ivs[0]);
    SetMonData(&gEnemyParty[0], MON_DATA_ATK_IV, &wonderTrade->ivs[1]);
    SetMonData(&gEnemyParty[0], MON_DATA_DEF_IV, &wonderTrade->ivs[2]);
    SetMonData(&gEnemyParty[0], MON_DATA_SPEED_IV, &wonderTrade->ivs[3]);
    SetMonData(&gEnemyParty[0], MON_DATA_SPATK_IV, &wonderTrade->ivs[4]);
    SetMonData(&gEnemyParty[0], MON_DATA_SPDEF_IV, &wonderTrade->ivs[5]);
    StringCopy(monName, GetSpeciesName(wonderTradeSpecies));
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