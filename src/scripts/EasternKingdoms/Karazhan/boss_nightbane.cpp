/*
 * This file is part of the BlizzLikeCore Project.
 * See CREDITS and LICENSE files for Copyright information.
 */

/* ScriptData
Name: Boss_Nightbane
Complete(%): 95
Comment:  timers, maybe need to check tuning
Category: Karazhan
EndScriptData */

#include "ScriptPCH.h"
#include "karazhan.h"

#define YELL_AGGRO                  "What fools! I shall bring a quick end to your suffering!"
#define YELL_FLY_PHASE              "Miserable vermin. I shall exterminate you from the air!"
#define YELL_LAND_PHASE_1           "Enough! I shall land and crush you myself!"
#define YELL_LAND_PHASE_2           "Insects! Let me show you my strength up close!"
#define EMOTE_BREATH                "takes a deep breath."

enum Spells
{
    // phase 1
    SPELL_BELLOWING_ROAR        = 39427,
    SPELL_CHARRED_EARTH         = 30129,
    SPELL_DISTRACTING_ASH       = 30130,
    SPELL_SMOLDERING_BREATH     = 30210,
    SPELL_TAIL_SWEEP            = 25653,
    // phase 2
    SPELL_RAIN_OF_BONES         = 37098,
    SPELL_SMOKING_BLAST         = 37057,
    SPELL_FIREBALL_BARRAGE      = 30282,
    SPELL_SEARING_CINDERS       = 30127,
    SPELL_SUMMON_SKELETON       = 30170
};

float IntroWay[8][3] =
{
    {-11053.37f, -1794.48f, 149.00f},
    {-11141.07f, -1841.40f, 125.00f},
    {-11187.28f, -1890.23f, 125.00f},
    {-11189.20f, -1931.25f, 125.00f},
    {-11153.76f, -1948.93f, 125.00f},
    {-11128.73f, -1929.75f, 125.00f},
    {-11140.00f, -1915.00f, 122.00f},
    {-11163.00f, -1903.00f, 91.473f}
};

struct boss_nightbaneAI : public ScriptedAI
{
    boss_nightbaneAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
        Intro = true;
        isReseted = false;
    }

    ScriptedInstance* pInstance;

    uint32 Phase;

    bool RainBones;
    bool Skeletons;

    uint32 BellowingRoarTimer;
    uint32 CharredEarthTimer;
    uint32 DistractingAshTimer;
    uint32 SmolderingBreathTimer;
    uint32 TailSweepTimer;
    uint32 RainofBonesTimer;
    uint32 SmokingBlastTimer;
    uint32 FireballBarrageTimer;
    uint32 SearingCindersTimer;

    uint32 FlyCount;
    uint32 FlyTimer;

    bool Intro;
    bool Flying;
    bool Movement;
    bool isReseted;

    uint32 WaitTimer;
    uint32 MovePhase;

    void Reset()
    {
        if (pInstance)
        {
            bool isCorrectSpawned = true;

            if (pInstance->GetData(TYPE_NIGHTBANE) != DONE)
            {
                uint64 NightbaneGUID = 0;
                NightbaneGUID = pInstance->GetData64(DATA_NIGHTBANE);

                if (NightbaneGUID)
                    if (Creature* Nightbane = Creature::GetCreature((*me), NightbaneGUID))
                        isCorrectSpawned = false;
            }
            else
                isCorrectSpawned = false;

            if (!isCorrectSpawned)
            {
                (*me).GetMotionMaster()->Clear(false);
                isReseted = true;
                me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                me->RemoveCorpse();
            }
            else
            {
                pInstance->SetData64(DATA_NIGHTBANE, me->GetGUID());
            }

            if (!Intro)
            {
                (*me).GetMotionMaster()->Clear(false);
                isReseted = true;
                pInstance->SetData(TYPE_NIGHTBANE, NOT_STARTED);
                me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                me->RemoveCorpse();
                return;
            }
            else
            {

                BellowingRoarTimer = urand(20000, 30000);
                CharredEarthTimer = urand(10000, 15000);
                DistractingAshTimer = urand(10000, 12000);
                SmolderingBreathTimer = urand(9000, 13000);
                TailSweepTimer = urand(12000, 15000);
                RainofBonesTimer = 3000;
                SmokingBlastTimer = urand(10000, 12000);
                FireballBarrageTimer = 10000;
                SearingCindersTimer = 12000;
                WaitTimer = urand(1000, 1200);

                Phase =1;
                FlyCount = 0;
                MovePhase = 0;

                me->SetSpeed(MOVE_RUN, 2.0f);
                me->AddUnitMovementFlag(MOVEFLAG_FLYING | MOVEFLAG_FLYING2);
                me->setActive(true);

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                HandleTerraceDoors(true);
                Flying = false;
                Movement = false;
            }
        }
    }

    void HandleTerraceDoors(bool open)
    {
        if (pInstance)
        {
            pInstance->HandleGameObject(pInstance->GetData64(DATA_MASTERS_TERRACE_DOOR_1), open);
            pInstance->HandleGameObject(pInstance->GetData64(DATA_MASTERS_TERRACE_DOOR_2), open);
        }
    }

    void EnterCombat(Unit* /*who*/)
    {
        if (pInstance)
            pInstance->SetData(TYPE_NIGHTBANE, IN_PROGRESS);

        HandleTerraceDoors(false);
        me->MonsterYell(YELL_AGGRO, LANG_UNIVERSAL, 0);
    }

    void AttackStart(Unit* who)
    {
        if (!Intro && !Flying)
        {
            if (Phase == 1)
                ScriptedAI::AttackStart(who);
            else
                AttackStartNoMove(who);
        }
    }

    void JustDied(Unit* /*killer*/)
    {
        if (!isReseted)
            if (pInstance)
                pInstance->SetData(TYPE_NIGHTBANE, DONE);

        HandleTerraceDoors(true);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (!Intro && !Flying)
        {
            if (!me->getVictim() && me->canStartAttack(who))
            {
                if (Phase == 1)
                    ScriptedAI::AttackStart(who);
                else
                    AttackStartNoMove(who);
            }
        }
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (type != POINT_MOTION_TYPE)
                return;

        if (Intro)
        {
            if (id >= 8)
            {
                Intro = false;
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetHomePosition(IntroWay[7][0],IntroWay[7][1],IntroWay[7][2],0);
                Phase = 1;
                return;
            }

            WaitTimer = 1;
        }

        if (Flying)
        {
            if (id == 0)
            {
                DoResetThreat();
                DoStartNoMovement(me->getVictim());
                me->MonsterTextEmote(EMOTE_BREATH, 0, true);
                Skeletons = false;
                Flying = false;
                Phase = 2;
                return;
            }

            if (id == 3)
            {
                MovePhase = 4;
                WaitTimer = 1;
                return;
            }

            if (id == 8)
            {
                DoResetThreat();
                Flying = false;
                Phase = 1;
                Movement = true;
                return;
            }

            WaitTimer = 1;
        }
    }

    void JustSummoned(Creature* summoned)
    {
        summoned->AI()->AttackStart(me->getVictim());
    }

    void TakeOff()
    {
        me->MonsterYell(YELL_FLY_PHASE, LANG_UNIVERSAL, 0);

        me->InterruptSpell(CURRENT_GENERIC_SPELL);
        me->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
        me->AddUnitMovementFlag(MOVEFLAG_FLYING | MOVEFLAG_FLYING2);
        (*me).GetMotionMaster()->Clear(false);
        (*me).GetMotionMaster()->MovePoint(0,IntroWay[2][0],IntroWay[2][1],IntroWay[2][2]);

        Flying = true;

        FlyTimer = urand(20000, 40000);
        ++FlyCount;

        RainofBonesTimer = 3000;
        RainBones = false;
    }

    void UpdateAI(const uint32 diff)
    {
        if (WaitTimer)
        {
            if (WaitTimer <= diff)
            {
                if (Intro || Flying)
                {
                    if (MovePhase >= 7)
                    {
                        me->HandleEmoteCommand(EMOTE_ONESHOT_LAND);
                        me->GetMotionMaster()->MovePoint(8,IntroWay[7][0],IntroWay[7][1],IntroWay[7][2]);
                        me->RemoveUnitMovementFlag(MOVEFLAG_FLYING | MOVEFLAG_FLYING2);
                    }
                    else
                    {
                        if (MovePhase < 6)
                            me->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
                        me->GetMotionMaster()->MovePoint(MovePhase,IntroWay[MovePhase][0],IntroWay[MovePhase][1],IntroWay[MovePhase][2]);
                        ++MovePhase;
                    }
                }

                WaitTimer = 0;
            } else WaitTimer -= diff;
        }

        if (!UpdateVictim())
            return;

        if (Flying)
            return;

        //  Phase 1 "GROUND FIGHT"
        if (Phase == 1)
        {
            if (Movement)
            {
                DoStartMovement(me->getVictim());
                Movement = false;
            }

            if (BellowingRoarTimer <= diff)
            {
                DoCast(me->getVictim(), SPELL_BELLOWING_ROAR);
                BellowingRoarTimer = urand(20000, 30000);
            } else BellowingRoarTimer -= diff;

            if (SmolderingBreathTimer <= diff)
            {
                DoCast(me->getVictim(), SPELL_SMOLDERING_BREATH);
                SmolderingBreathTimer = urand(14000, 20000);
            } else SmolderingBreathTimer -= diff;

            if (CharredEarthTimer <= diff)
            {
                if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                    DoCast(pTarget, SPELL_CHARRED_EARTH);
                CharredEarthTimer = urand(25000, 35000);
            } else CharredEarthTimer -= diff;

            if (TailSweepTimer <= diff)
            {
                if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                    if (!me->HasInArc(M_PI, pTarget))
                        DoCast(pTarget, SPELL_TAIL_SWEEP);
                TailSweepTimer = urand(14000, 20000);
            } else TailSweepTimer -= diff;

            if (SearingCindersTimer <= diff)
            {
                if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                    DoCast(pTarget, SPELL_SEARING_CINDERS);
                SearingCindersTimer = urand(6000, 12000);
            } else SearingCindersTimer -= diff;

            uint32 Prozent;
            Prozent = (me->GetHealth()*100) / me->GetMaxHealth();

            if (Prozent < 75 && FlyCount == 0) // first take off 75%
                TakeOff();

            if (Prozent < 50 && FlyCount == 1) // secound take off 50%
                TakeOff();

            if (Prozent < 25 && FlyCount == 2) // third take off 25%
                TakeOff();

            DoMeleeAttackIfReady();
        }

        //Phase 2 "FLYING FIGHT"
        if (Phase == 2)
        {
            if (!RainBones)
            {
                if (RainofBonesTimer <= diff && !RainBones) // only once at the beginning of phase 2
                {

                    if (!Skeletons)
                    {
                        for (int i = 0; i < 5; i++)
                            DoCast(me->getVictim(), SPELL_SUMMON_SKELETON, true);
                        Skeletons = true;
                    }

                    DoCast(me->getVictim(), SPELL_RAIN_OF_BONES);
                    RainBones = true;
                    SmokingBlastTimer = urand(1000, 3000);
                } else RainofBonesTimer -= diff;

                if (DistractingAshTimer <= diff)
                {
                    if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                        DoCast(pTarget, SPELL_DISTRACTING_ASH);
                    DistractingAshTimer = urand(7000, 13000);
                } else DistractingAshTimer -= diff;
            }

            if (RainBones)
            {
                if (SmokingBlastTimer <= diff)
                 {
                    DoCast(me->getVictim(), SPELL_SMOKING_BLAST);
                    SmokingBlastTimer = urand(1000, 3000);
                 } else SmokingBlastTimer -= diff;
            }

            if (FireballBarrageTimer <= diff)
            {
                Map* map = me->GetMap();
                if (!map->IsDungeon()) return;
                Map::PlayerList const &PlayerList = map->GetPlayers();
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                {
                    if (Player* i_pl = i->getSource())
                        if (i_pl->isAlive() && !me->IsWithinDistInMap(i_pl,80))
                        {
                            DoCast(i_pl, SPELL_FIREBALL_BARRAGE);
                        }
                }
                FireballBarrageTimer = urand(3000, 6000);
            } else FireballBarrageTimer -= diff;

            if (FlyTimer <= diff) //landing
            {
                me->MonsterYell(urand(0, 1) ? YELL_LAND_PHASE_1 : YELL_LAND_PHASE_2, LANG_UNIVERSAL, 0);
                me->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
                me->GetMotionMaster()->Clear(false);
                me->GetMotionMaster()->MovePoint(3,IntroWay[3][0],IntroWay[3][1],IntroWay[3][2]);

                Flying = true;
            } else FlyTimer -= diff;
        }
    }
};

CreatureAI* GetAI_boss_nightbane(Creature* pCreature)
{
    return new boss_nightbaneAI (pCreature);
}

void AddSC_boss_nightbane()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_nightbane";
    newscript->GetAI = &GetAI_boss_nightbane;
    newscript->RegisterSelf();
}

