/*
 * This file is part of the BlizzLikeCore Project.
 * See CREDITS and LICENSE files for Copyright information.
 */

/* ScriptData
Name: Instance_Arcatraz
Complete(%): 80
Comment: Mainly Harbringer Skyriss event
Category: Tempest Keep, The Arcatraz
EndScriptData */

#include "ScriptPCH.h"
#include "arcatraz.h"

#define ENCOUNTERS 9

#define CONTAINMENT_CORE_SECURITY_FIELD_ALPHA 184318        //door opened when Wrath-Scryer Soccothrates dies
#define CONTAINMENT_CORE_SECURITY_FIELD_BETA  184319        //door opened when Dalliah the Doomsayer dies
#define POD_ALPHA   183961                                  //pod first boss wave
#define POD_BETA    183963                                  //pod second boss wave
#define POD_DELTA   183964                                  //pod third boss wave
#define POD_GAMMA   183962                                  //pod fourth boss wave
#define POD_OMEGA   183965                                  //pod fifth boss wave
#define WARDENS_SHIELD  184802                              // warden shield
#define SEAL_SPHERE 184802                                  //shield 'protecting' mellichar

#define MELLICHAR   20904                                   //skyriss will kill this unit


/* Arcatraz encounters:
1 - Zereketh the Unbound event
2 - Dalliah the Doomsayer event
3 - Wrath-Scryer Soccothrates event
4 - Harbinger Skyriss event, 5 sub-events
*/

struct instance_arcatraz : public ScriptedInstance
{
    instance_arcatraz(Map* map) : ScriptedInstance(map) {Initialize();};

    uint32 Encounter[ENCOUNTERS];

    GameObject* Containment_Core_Security_Field_Alpha;
    GameObject* Containment_Core_Security_Field_Beta;
    GameObject* Pod_Alpha;
    GameObject* Pod_Gamma;
    GameObject* Pod_Beta;
    GameObject* Pod_Delta;
    GameObject* Pod_Omega;
    GameObject* Wardens_Shield;

    uint64 GoSphereGUID;
    uint64 MellicharGUID;

    void Initialize()
    {
        Containment_Core_Security_Field_Alpha = NULL;
        Containment_Core_Security_Field_Beta  = NULL;
        Pod_Alpha = NULL;
        Pod_Beta  = NULL;
        Pod_Delta = NULL;
        Pod_Gamma = NULL;
        Pod_Omega = NULL;
        Wardens_Shield = NULL;

        GoSphereGUID = 0;
        MellicharGUID = 0;

        for (uint8 i = 0; i < ENCOUNTERS; i++)
            Encounter[i] = NOT_STARTED;
    }

    bool IsEncounterInProgress() const
    {
        for (uint8 i = 0; i < ENCOUNTERS; i++)
            if (Encounter[i] == IN_PROGRESS)
                return true;

        return false;
    }

    void OnGameObjectCreate(GameObject* pGo, bool /*add*/)
    {
        switch (pGo->GetEntry())
        {
            case CONTAINMENT_CORE_SECURITY_FIELD_ALPHA: Containment_Core_Security_Field_Alpha = pGo; break;
            case CONTAINMENT_CORE_SECURITY_FIELD_BETA:  Containment_Core_Security_Field_Beta =  pGo; break;
            case POD_ALPHA: Pod_Alpha = pGo; break;
            case POD_BETA:  Pod_Beta =  pGo; break;
            case POD_DELTA: Pod_Delta = pGo; break;
            case POD_GAMMA: Pod_Gamma = pGo; break;
            case POD_OMEGA: Pod_Omega = pGo; break;
            case SEAL_SPHERE: GoSphereGUID = pGo->GetGUID(); break;
            //case WARDENS_SHIELD: Wardens_Shield = pGo; break;
        }
    }

    void OnCreatureCreate(Creature* pCreature, bool /*add*/)
    {
        if (pCreature->GetEntry() == MELLICHAR)
            MellicharGUID = pCreature->GetGUID();
    }

    void SetData(uint32 type, uint32 data)
    {
        switch (type)
        {
            case TYPE_ZEREKETH:
                Encounter[0] = data;
                break;

            case TYPE_DALLIAH:
                if (data == DONE)
                    if (Containment_Core_Security_Field_Beta)
                        Containment_Core_Security_Field_Beta->UseDoorOrButton();
                Encounter[1] = data;
                break;

            case TYPE_SOCCOTHRATES:
                if (data == DONE)
                    if (Containment_Core_Security_Field_Alpha)
                        Containment_Core_Security_Field_Alpha->UseDoorOrButton();
                Encounter[2] = data;
                break;

            case TYPE_HARBINGERSKYRISS:
                if (data == NOT_STARTED || data == FAIL)
                {
                    Encounter[4] = NOT_STARTED;
                    Encounter[5] = NOT_STARTED;
                    Encounter[6] = NOT_STARTED;
                    Encounter[7] = NOT_STARTED;
                    Encounter[8] = NOT_STARTED;
                }
                Encounter[3] = data;
                break;

            case TYPE_WARDEN_1:
                if (data == IN_PROGRESS)
                    if (Pod_Alpha)
                        Pod_Alpha->UseDoorOrButton();
                Encounter[4] = data;
                break;

            case TYPE_WARDEN_2:
                if (data == IN_PROGRESS)
                    if (Pod_Beta)
                        Pod_Beta->UseDoorOrButton();
                Encounter[5] = data;
                break;

            case TYPE_WARDEN_3:
                if (data == IN_PROGRESS)
                    if (Pod_Delta)
                        Pod_Delta->UseDoorOrButton();
                Encounter[6] = data;
                break;

            case TYPE_WARDEN_4:
                if (data == IN_PROGRESS)
                    if (Pod_Gamma)
                        Pod_Gamma->UseDoorOrButton();
                Encounter[7] = data;
                break;

            case TYPE_WARDEN_5:
                if (data == IN_PROGRESS)
                    if (Pod_Omega)
                        Pod_Omega->UseDoorOrButton();
                Encounter[8] = data;
                break;

            case TYPE_SHIELD_OPEN:
                if (data == IN_PROGRESS)
                    if (Wardens_Shield)
                        Wardens_Shield->UseDoorOrButton();
                break;
        }
    }

    uint32 GetData(uint32 type)
    {
         switch (type)
        {
            case TYPE_HARBINGERSKYRISS:
                return Encounter[3];
            case TYPE_WARDEN_1:
                return Encounter[4];
            case TYPE_WARDEN_2:
                return Encounter[5];
            case TYPE_WARDEN_3:
                return Encounter[6];
            case TYPE_WARDEN_4:
                return Encounter[7];
            case TYPE_WARDEN_5:
                return Encounter[8];
        }
        return 0;
    }

    uint64 GetData64(uint32 data)
    {
        switch (data)
        {
            case DATA_MELLICHAR:
                return MellicharGUID;
            case DATA_SPHERE_SHIELD:
                return GoSphereGUID;
        }
        return 0;
    }
};

InstanceData* GetInstanceData_instance_arcatraz(Map* map)
{
    return new instance_arcatraz(map);
}

void AddSC_instance_arcatraz()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "instance_arcatraz";
    newscript->GetInstanceData = &GetInstanceData_instance_arcatraz;
    newscript->RegisterSelf();
}

