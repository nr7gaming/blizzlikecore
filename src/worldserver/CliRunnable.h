/*
 * BlizzLikeCore Copyright (C) 2013  BlizzLikeGroup
 * Integrated Files: CREDITS.md and LICENSE.md
 */

#ifndef __CLIRUNNABLE_H
#define __CLIRUNNABLE_H

// Command Line Interface handling thread
class CliRunnable : public ACE_Based::Runnable
{
    public:
        void run();
};
#endif

