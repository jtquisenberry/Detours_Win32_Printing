#if !defined(AFX_MAPEX_H__A8717FF9_DA8B_46dd_A7E3_36C4547A8C80__INCLUDED_)
#define AFX_MAPEX_H__A8717FF9_DA8B_46dd_A7E3_36C4547A8C80__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
class CMapEx : public CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>
{
public:
    void Cleanup( void )
    {
        POSITION pos = GetStartPosition();

        while (pos != NULL)
        {
            KEY Key;
            ARG_VALUE Value;
            GetNextAssoc(pos, Key, Value);

            delete Value;
        }

        RemoveAll();
    }
};

#endif