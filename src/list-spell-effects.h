/*
 * File: list-spell-effects.h
 * Purpose: List of side effects for monster spells.
 *
 * Fields:
 * - numerical index (RSE_this)
 * - which RSF_ attack method has this effect
 * - which GF_ type has this side effect
 * - is this effect temporary?
 * - flag for the effect
 * - basic duration (or impact if permanent)
 * - damage-dependent duration (or impact)
 * - chance of this effect arising from this attack (0 means always)
 * - does the player get a save against this effect? (after any save against
 *   the original attack)
 * - what object flag resists this effect? (in addition to gf_ptr->resist etc.)
 * - power rating of this side effect encoded as a random_value struct:
 * 		base = flat power rating
 * 		dice = percentage adjustment to damage output
 *		sides = rlev-dependent power
 *		m_bonus: +ve means power is proportional to rlev, negative means inverse
 */

/* index method			gf			timed	effect			base			dam		 		chance	save 	res_flag 		power */
RSE(0,	0,				0,			FALSE,	0,				RV(0,0,0,0),	RV(0,0,0,0),	0,		FALSE, 	0,				RV(0,0,0,0))
RSE(1,	0,				GF_POIS,	TRUE,	TMD_POISONED,	RV(10,0,0,0),	RV(0,1,100,0),	0,		FALSE,	OF_RES_POIS,	RV(0,125,100,1))
RSE(2,	0,				GF_LIGHT,	TRUE,	TMD_BLIND,		RV(2,1,5,0),	RV(0,0,0,0),	0,		FALSE,	OF_PROT_BLIND,	RV(10,0,0,0))
RSE(3,	0,				GF_DARK,	TRUE,	TMD_BLIND,		RV(2,1,5,0),	RV(0,0,0,0),	0,		FALSE,	OF_PROT_BLIND,	RV(10,0,0,0))
RSE(4,	0,				GF_SOUND,	TRUE,	TMD_STUN,		RV(5,0,0,0),	RV(0,1,33,35),	0,		FALSE,	OF_PROT_STUN,	RV(20,0,0,0))
RSE(5,	0,				GF_SHARD,	TRUE,	TMD_CUT,		RV(0,0,0,0),	RV(0,1,100,0),	0,		FALSE,	0,				RV(5,125,0,0))
RSE(6,	0,				GF_CHAOS,	TRUE,	TMD_IMAGE,		RV(0,1,10,0),	RV(0,0,0,0),	0,		FALSE,	OF_RES_CHAOS,	RV(0,0,2000,-1))
RSE(7,	0,				GF_CHAOS,	TRUE,	TMD_CONFUSED,	RV(9,1,20,0),	RV(0,0,0,0),	0,		FALSE,	OF_PROT_CONF,	RV(0,0,0,0))
RSE(8,	0,				GF_INERTIA,	TRUE,	TMD_SLOW,		RV(3,1,4,0),	RV(0,0,0,0),	0,		FALSE,	0,				RV(30,0,0,0))
RSE(9,	0,				GF_GRAVITY,	TRUE,	TMD_STUN,		RV(5,0,0,0),	RV(0,1,33,35),	0,		FALSE,	OF_PROT_STUN,	RV(30,0,0,0))
RSE(10,	RSF_BRAIN_SMASH,GF_GRAVITY,	TRUE,	TMD_SLOW,		RV(3,1,4,0),	RV(0,0,0,0),	0,		FALSE,	0,				RV(0,0,0,0))
RSE(11,	0,				GF_FORCE,	TRUE,	TMD_STUN,		RV(0,1,20,0),	RV(0,0,0,0),	0,		FALSE,	OF_PROT_STUN,	RV(30,0,0,0))
RSE(12,	0,				GF_PLASMA,	TRUE,	TMD_STUN,		RV(5,0,0,0),	RV(0,1,75,35),	0,		FALSE,	OF_PROT_STUN,	RV(30,0,0,0))
RSE(13,	0,				GF_WATER,	TRUE,	TMD_CONFUSED,	RV(5,1,5,0),	RV(0,0,0,0),	0,		FALSE,	OF_PROT_CONF,	RV(20,0,0,0))
RSE(14,	0,				GF_WATER,	TRUE,	TMD_STUN,		RV(0,1,40,0),	RV(0,0,0,0),	0,		FALSE,	OF_PROT_STUN,	RV(0,0,0,0))
RSE(15,	0,				GF_ICE,		TRUE,	TMD_CUT,		RV(0,5,8,0),	RV(0,0,0,0),	0,		FALSE,	OF_RES_SHARD,	RV(0,0,0,0))
RSE(16,	0,				GF_ICE,		TRUE,	TMD_STUN,		RV(0,1,15,0),	RV(0,0,0,0),	0,		FALSE,	OF_PROT_STUN,	RV(0,0,0,0))
RSE(17,	RSF_CAUSE_4,	0,			TRUE,	TMD_CUT,		RV(0,10,10,0),	RV(0,0,0,0),	0,		FALSE,	0,				RV(0,0,0,0))
RSE(18,	RSF_MIND_BLAST,	0,			TRUE,	TMD_CONFUSED,	RV(3,1,4,0),	RV(0,0,0,0),	0,		FALSE,	OF_PROT_CONF,	RV(0,0,0,0))
RSE(19,	0,				GF_ACID,	FALSE,	S_INV_DAM,		RV(0,0,0,0),	RV(5,0,0,0),	0,		FALSE,	OF_IM_ACID,		RV(20,0,0,0))
RSE(20,	0,				GF_ELEC,	FALSE,	S_INV_DAM,		RV(0,0,0,0),	RV(5,0,0,0),	0,		FALSE,	OF_IM_ELEC,		RV(10,0,0,0))
RSE(21,	0,				GF_FIRE,	FALSE,	S_INV_DAM,		RV(0,0,0,0),	RV(5,0,0,0),	0,		FALSE,	OF_IM_FIRE,		RV(10,0,0,0))
RSE(22,	0,				GF_COLD,	FALSE,	S_INV_DAM,		RV(0,0,0,0),	RV(5,0,0,0),	0,		FALSE,	OF_IM_COLD,		RV(10,0,0,0))
RSE(23,	0,				GF_NEXUS,	FALSE,	S_TELEPORT,		RV(200,0,0,0),	RV(0,0,0,0),	3,		FALSE,	OF_RES_NEXUS,	RV(20,0,0,0))
RSE(24,	0,				GF_NEXUS,	FALSE,	S_TELE_TO,		RV(0,0,0,0),	RV(0,0,0,0),	2,		FALSE,	OF_RES_NEXUS,	RV(0,0,0,0))
RSE(25,	0,				GF_NEXUS,	FALSE,	S_TELE_LEV,		RV(1,0,0,0),	RV(0,0,0,0),	1,		TRUE,	OF_RES_NEXUS,	RV(0,0,0,0))
RSE(26,	0,				GF_NEXUS,	FALSE,	S_SWAP_STAT,	RV(1,0,0,0),	RV(0,0,0,0),	1,		TRUE,	OF_RES_NEXUS,	RV(0,0,0,0))
RSE(27,	0,				GF_NETHER,	FALSE,	S_DRAIN_LIFE,	RV(200,0,1,0),	RV(0,0,0,0),	0,		FALSE,	OF_HOLD_LIFE,	RV(0,0,2000,-1))
RSE(28,	0,				GF_CHAOS,	FALSE,	S_DRAIN_LIFE,	RV(5000,0,1,0),	RV(0,0,0,0),	0,		FALSE,	OF_HOLD_LIFE,	RV(0,0,0,0))
RSE(29,	0,				GF_DISEN,	FALSE,	S_DISEN,		RV(1,0,0,0),	RV(0,0,0,0),	0,		FALSE,	OF_RES_DISEN,	RV(50,0,0,0))
RSE(30,	0,				GF_GRAVITY,	FALSE,	S_TELEPORT,		RV(5,0,0,0),	RV(0,0,0,127),	0,		FALSE,	0,				RV(0,0,0,0))
RSE(31,	0,				GF_TIME,	FALSE,	S_DRAIN_LIFE,	RV(100,0,1,0),	RV(0,0,0,0),	5,		FALSE,	0,				RV(0,0,2000,-1))
RSE(32,	0,				GF_TIME,	FALSE,	S_DRAIN_STAT,	RV(2,0,0,0),	RV(0,0,0,0),	4,		FALSE,	0,				RV(0,0,0,0))
RSE(33,	0,				GF_TIME,	FALSE,	S_DRAIN_ALL,	RV(1,0,0,0),	RV(0,0,0,0),	1,		FALSE,	0,				RV(0,0,0,0))
RSE(34,	RSF_BRAIN_SMASH,0,			TRUE,	TMD_BLIND,		RV(7,1,8,0),	RV(0,0,0,0),	0,		FALSE,	OF_PROT_BLIND,	RV(0,0,0,0))
RSE(35,	RSF_BRAIN_SMASH,0,			TRUE,	TMD_CONFUSED,	RV(3,1,4,0),	RV(0,0,0,0),	0,		FALSE,	OF_PROT_CONF,	RV(0,0,0,0))
RSE(36,	RSF_BRAIN_SMASH,0,			TRUE,	TMD_PARALYZED,	RV(3,1,4,0),	RV(0,0,0,0),	0,		FALSE,	OF_FREE_ACT,	RV(0,0,0,0))
RSE(37,	0,				GF_ICE,		FALSE,	S_INV_DAM,		RV(0,0,0,0),	RV(5,0,0,0),	0,		FALSE,	OF_IM_COLD,		RV(0,0,0,0))
RSE(38,	0,				GF_MANA,	FALSE,	0,				RV(0,0,0,0),	RV(0,0,0,0),	0,		FALSE,	0,				RV(100,0,0,0))
RSE(39,	RSF_DRAIN_MANA,	0,			FALSE,	S_DRAIN_MANA,	RV(0,0,0,0),	RV(0,1,50,0),	0,		FALSE,	0,				RV(5,0,0,0))
RSE(40,	RSF_SCARE,		0,			TRUE,	TMD_AFRAID,		RV(3,1,4,0),	RV(0,0,0,0),	0,		FALSE,	OF_PROT_FEAR,	RV(5,0,0,0))
RSE(41,	RSF_CONF,		0,			TRUE,	TMD_CONFUSED,	RV(3,1,4,0),	RV(0,0,0,0),	0,		FALSE,	OF_PROT_CONF,	RV(10,0,0,0))
RSE(42,	RSF_BLIND,		0,			TRUE,	TMD_BLIND,		RV(11,1,4,0),	RV(0,0,0,0),	0,		FALSE,	OF_PROT_BLIND,	RV(10,0,0,0))
RSE(43,	RSF_SLOW,		0,			TRUE,	TMD_SLOW,		RV(3,1,4,0),	RV(0,0,0,0),	0,		FALSE,	OF_FREE_ACT,	RV(15,0,0,0))
RSE(44,	RSF_HOLD,		0,			TRUE,	TMD_PARALYZED,	RV(3,1,4,0),	RV(0,0,0,0),	0,		FALSE,	OF_FREE_ACT,	RV(25,0,0,0))
RSE(45,	RSF_HASTE,		0,			TRUE,	MON_TMD_FAST,	RV(50,0,0,0),	RV(0,0,0,0),	0,		FALSE,	0,				RV(70,0,0,0))
RSE(46, RSF_HEAL,		0,			FALSE,	S_HEAL,			RV(0,0,0,0),	RV(6,0,0,0),	0,		FALSE,	0,				RV(30,0,0,0))
RSE(47,	RSF_BLINK,		0,			FALSE,	S_TELE_SELF,	RV(10,0,0,0),	RV(0,0,0,0),	0,		FALSE,	0,				RV(15,0,0,0))
RSE(48,	RSF_TELE_TO,	0,			FALSE,	S_TELE_TO,		RV(0,0,0,0),	RV(0,0,0,0),	0,		FALSE,	0,				RV(25,0,0,0))
RSE(49,	RSF_TELE_AWAY,	0,			FALSE,	S_TELEPORT,		RV(100,0,0,0),	RV(0,0,0,0),	0,		FALSE,	0,				RV(25,0,0,0))
RSE(50,	RSF_TELE_LEVEL,	0,			FALSE,	S_TELE_LEV,		RV(1,0,0,0),	RV(0,0,0,0),	0,		FALSE,	OF_RES_NEXUS,	RV(40,0,0,0))
RSE(51,	RSF_TPORT,		0,			FALSE,	S_TELE_SELF,	RV((MAX_SIGHT_LGE * 2 + 5),0,0,0), RV(0,0,0,0), 0, FALSE, 0,		RV(15,0,0,0))
RSE(52,	RSF_DARKNESS,	0,			FALSE,	S_DARKEN,		RV(3,0,0,0),	RV(0,0,0,0),	0,		FALSE,	0,				RV(5,0,0,0))
RSE(53,	RSF_TRAPS,		0,			FALSE,	S_TRAPS,		RV(0,0,0,0),	RV(0,0,0,0),	0,		FALSE,	0,				RV(10,0,0,0))
RSE(54,	RSF_FORGET,		0,			TRUE,	TMD_AMNESIA,	RV(3,0,0,0),	RV(0,0,0,0),	0,		FALSE,	0,				RV(25,0,0,0))
RSE(55,	RSF_SHRIEK,		0,			FALSE,	S_AGGRAVATE,	RV(0,0,0,0),	RV(0,0,0,0),	0,		FALSE,	0,				RV(0,0,150,1))
RSE(56,	RSF_S_KIN,		0,			FALSE,	S_KIN,			RV(8,0,0,0),	RV(0,0,0,0),	0,		FALSE,	0,				RV(0,0,200,1))
RSE(57,	RSF_S_MONSTER,	0,			FALSE,	S_MONSTER,		RV(1,0,0,0),	RV(0,0,0,0),	0,		FALSE,	0,				RV(40,0,0,0))
RSE(58,	RSF_S_MONSTERS,	0,			FALSE,	S_MONSTERS,		RV(8,0,0,0),	RV(0,0,0,0),	0,		FALSE,	0,				RV(80,0,0,0))
RSE(59,	RSF_S_ANIMAL,	0,			FALSE,	S_ANIMAL,		RV(12,0,0,0),	RV(0,0,0,0),	0,		FALSE,	0,				RV(30,0,0,0))
RSE(60,	RSF_S_SPIDER,	0,			FALSE,	S_SPIDER,		RV(12,0,0,0),	RV(0,0,0,0),	0,		FALSE,	0,				RV(20,0,0,0))
RSE(61,	RSF_S_HOUND,	0,			FALSE,	S_HOUND,		RV(12,0,0,0),	RV(0,0,0,0),	0,		FALSE,	0,				RV(100,0,0,0))
RSE(62,	RSF_S_HYDRA,	0,			FALSE,	S_HYDRA,		RV(6,0,0,0),	RV(0,0,0,0),	0,		FALSE,	0,				RV(150,0,0,0))
RSE(63,	RSF_S_AINU,		0,			FALSE,	S_AINU,			RV(1,0,0,0),	RV(0,0,0,0),	0,		FALSE,	0,				RV(150,0,0,0))
RSE(64,	RSF_S_DEMON,	0,			FALSE,	S_DEMON,		RV(1,0,0,0),	RV(0,0,0,0),	0,		FALSE,	0,				RV(0,0,150,1))
RSE(65,	RSF_S_UNDEAD,	0,			FALSE,	S_UNDEAD,		RV(1,0,0,0),	RV(0,0,0,0),	0,		FALSE,	0,				RV(0,0,150,1))
RSE(66,	RSF_S_DRAGON,	0,			FALSE,	S_DRAGON,		RV(1,0,0,0),	RV(0,0,0,0),	0,		FALSE,	0,				RV(0,0,150,1))
RSE(67,	RSF_S_HI_DEMON,	0,			FALSE,	S_HI_DEMON,		RV(8,0,0,0),	RV(0,0,0,0),	0,		FALSE,	0,				RV(250,0,0,0))
RSE(68,	RSF_S_HI_UNDEAD,0,			FALSE,	S_HI_UNDEAD,	RV(8,0,0,0),	RV(0,0,0,0),	0,		FALSE,	0,				RV(400,0,0,0))
RSE(69,	RSF_S_HI_DRAGON,0,			FALSE,	S_HI_DRAGON,	RV(8,0,0,0),	RV(0,0,0,0),	0,		FALSE,	0,				RV(400,0,0,0))
RSE(70,	RSF_S_WRAITH,	0,			FALSE,	S_WRAITH,		RV(8,0,0,0),	RV(8,0,0,0),	0,		FALSE,	0,				RV(450,0,0,0))
RSE(71,	RSF_S_UNIQUE,	0,			FALSE,	S_UNIQUE,		RV(8,0,0,0),	RV(8,0,0,0),	0,		FALSE,	0,				RV(500,0,0,0))
RSE(MAX,0,				0,			FALSE,	0,				RV(0,0,0,0),	RV(0,0,0,0),	0,		FALSE, 	0,				RV(0,0,0,0))
