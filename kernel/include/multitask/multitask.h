#pragma once
#ifndef MULTITASK_H
#define MULTITASK_H

#include <stdint.h>
#include <stddef.h>

#include "memory/pageFrame.h"

namespace Multitask
{
    enum class TaskType
    {
        KERNEL,
        USER
    };

    class Task
    {
        public:
            Task(char const* sName, const TaskType type, const uint32_t entrypoint);

            void SwitchToTask();
            void SwitchFromTask();
            void SetEntrypoint(const uint32_t entrypoint);

        private:
            char m_sName[32];
            uint32_t* m_pStack;
            uint32_t m_Entrypoint;
            
        public:
            TaskType m_Type;
            Memory::PageFrame m_PageFrame; // Technically kernel tasks don't need one but hey-ho
    };

    void Init();
    int CreateTask(char const* sName);
    int CreateTask(char const* sName, const TaskType type, void (*entrypoint)());

    extern "C"
    {
        // Variables for assembly
        extern uint32_t* pSavedTaskStack;
        extern uint32_t* pNewTaskStack;
        extern uint32_t kernelCR3;
        extern bool bPrivilegeChange;

        void OnPIT();
        void IRQ0();
    }
}

#endif