/**
 * =============================================================================
 * CS2Fixes
 * Copyright (C) 2023-2026 Source2ZE
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

class CKHookBase
{
  public:
    virtual void Configure() = 0;
};

extern std::vector<CKHookBase*>& GetKHookList();
extern bool g_bRequiredInitLoaded;
void InitKHooks();

template <typename CLASS, typename RETURN, typename... ARGS> class CKHookVirtual : public CKHookBase
{
  public:
    using fnCallback = KHook::Return<RETURN> (*)(CLASS*, ARGS...);

    CKHookVirtual(RETURN (CLASS::*pFunction)(ARGS...), CLASS*& pInstance, fnCallback cbPre, fnCallback cbPost)
        : m_hook(pFunction, cbPre, cbPost), m_pInstance(pInstance)
    {
        GetKHookList().push_back(this);
    }

    ~CKHookVirtual()
    {
        if (m_hook.IsActive()) m_hook.Remove(m_pInstance);
    }

    virtual void Configure() override
    {
        if (!m_pInstance)
        {
            g_bRequiredInitLoaded = false;
            return;
        }

        m_hook.Add(m_pInstance);
    }

    RETURN CallOriginal(CLASS* this_ptr, ARGS... args) { return m_hook.CallOriginal(this_ptr, args...); }

  private:
    KHook::Virtual<CLASS, RETURN, ARGS...> m_hook;
    CLASS*& m_pInstance;
};

template <typename CLASS, typename RETURN, typename... ARGS>
auto MakeKHookVirtual(RETURN (CLASS::*pFunction)(ARGS...), CLASS*& pInstance, KHook::Return<RETURN> (*cbPre)(CLASS*, ARGS...), std::nullptr_t cbPost)
{
    return std::make_unique<CKHookVirtual<CLASS, RETURN, ARGS...>>(pFunction, pInstance, cbPre, nullptr);
}

template <typename CLASS, typename RETURN, typename... ARGS>
auto MakeKHookVirtual(RETURN (CLASS::*pFunction)(ARGS...), CLASS*& pInstance, std::nullptr_t cbPre, KHook::Return<RETURN> (*cbPost)(CLASS*, ARGS...))
{
    return std::make_unique<CKHookVirtual<CLASS, RETURN, ARGS...>>(pFunction, pInstance, nullptr, cbPost);
}

template <typename CLASS, typename RETURN, typename... ARGS>
auto MakeKHookVirtual(RETURN (CLASS::*pFunction)(ARGS...),
                      CLASS*& pInstance,
                      KHook::Return<RETURN> (*cbPre)(CLASS*, ARGS...),
                      KHook::Return<RETURN> (*cbPost)(CLASS*, ARGS...))
{
    return std::make_unique<CKHookVirtual<CLASS, RETURN, ARGS...>>(pFunction, pInstance, cbPre, cbPost);
}

#define KHOOK_VIRTUAL(name, function, instance, pre, post) static auto hook##name = MakeKHookVirtual(function, instance, pre, post)
