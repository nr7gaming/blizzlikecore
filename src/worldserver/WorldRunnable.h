/*
 * This file is part of the BlizzLikeCore Project.
 * See CREDITS and LICENSE files for Copyright information.
 */

#ifndef __WORLDRUNNABLE_H
#define __WORLDRUNNABLE_H

// Heartbeat thread for the World
class WorldRunnable : public ACE_Based::Runnable
{
    public:
        void run();
};
#endif

