#include <Common/Common.h>
#include <iostream>
using namespace std;

class CConfig;
struct IConfigCallback
{
    virtual void OnConfigChanged(CSharedPtr<CConfig> spConfig) = 0;
};

// 主题类
class CConfig : public CEnableSharedFromThis<CConfig>
{
public:
    void Register(CWeakPtr<IConfigCallback> &wpCallback)
    {
        m_wpCallback = wpCallback;
    }

    void ChangeConfig()
    {
        CSharedPtr<IConfigCallback> wpCallback = m_wpCallback.Lock();
        if (wpCallback != NULL)
        {
            wpCallback->OnConfigChanged(SharedFromThis());
        }
    }

private:
    CWeakPtr<IConfigCallback> m_wpCallback;
};

// 观察者类
class CWork : public IConfigCallback, public CEnableSharedFromThis<CWork>
{
    virtual void OnConfigChanged(CSharedPtr<CConfig> spConfig)
    {
        cout << "Config changed. CWork noted" << endl;
    }

public:
    void Initialize(CSharedPtr<CConfig> &spConfig)
    {
        CSharedPtr<CWork> spWork = SharedFromThis();    // 2, 1

        // dynamic_pointer_cast<IConfigCallback>(CWork*);

        // 把 CSharedPtr<CWork> 转换为 CSharedPtr<IConfigCallback>
        CSharedPtr<IConfigCallback> sp = dynamic_pointer_cast<IConfigCallback>(spWork);    // 3, 1
        CWeakPtr<IConfigCallback> wp(sp);               // 3, 2
        spConfig->Register(wp);
    }
};

typedef CSharedPtr<CWork> CWorkPtr;

Int32 main()
{
    CWorkPtr spWork(new CWork());          // 执行完后，计数器为：1, 1
    CSharedPtr<CConfig> spConfig(new CConfig());    // 执行完后，计数器为：1, 0

    spWork->Initialize(spConfig);                   // 执行完后，计数器为：1, 2/1, 0
    spConfig->ChangeConfig();
    spConfig->ChangeConfig();
    //spWork = NULL;
    spConfig->ChangeConfig();
    
    return 0;
}
