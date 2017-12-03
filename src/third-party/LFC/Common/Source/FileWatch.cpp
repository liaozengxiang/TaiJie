#include "../Include/FileWatch.h"
#include "../Include/Assert.h"
#include "../Include/Macro.h"
#include "../Include/OS.h"
#include <unistd.h>
#include <sys/inotify.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

// 定义消息命令字
#define REGISTER_WATCH      1
#define UNREGISTER_WATCH    2
#define WHOLE_SCAN_FILE     3

CFileWatch::CFileWatch(CEventEngine *pEngine) : CEvent(pEngine)
{
    m_nNotifyFD = -1;
}

CFileWatch::~CFileWatch()
{
    Unregister();

    if (m_nNotifyFD != -1)
    {
        close(m_nNotifyFD);
        m_nNotifyFD = -1;
    }
}

Bool CFileWatch::Initialize()
{
    Bool bSuccess = False;

    do
    {
        Assert(m_nNotifyFD == -1);

        m_nNotifyFD = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
        if (m_nNotifyFD == -1)
        {
            break;
        }

        if (!RegisterRead())
        {
            break;
        }

        bSuccess = True;
    } while (0);

    return bSuccess;
}

void CFileWatch::WholeScan()
{
    m_lpEngine->PostMessage(WHOLE_SCAN_FILE, NULL, NULL, this);
}


struct RegisterWatchInfo
{
    std::string      strFileName;
    UInt32           uMask;
    IFileWatchEvent* pCallback;
};

Bool CFileWatch::RegisterWatch(const std::string &strFileName, UInt32 uMask, IFileWatchEvent *pWatcher)
{
    RegisterWatchInfo *pInfo = new RegisterWatchInfo();
    pInfo->strFileName = strFileName;
    pInfo->uMask = uMask;
    pInfo->pCallback = pWatcher;

    return m_lpEngine->PostMessage(REGISTER_WATCH, pInfo, NULL, this);
}

void CFileWatch::UnregisterWatch(IFileWatchEvent *pWatcher)
{
    m_lpEngine->PostMessage(UNREGISTER_WATCH, pWatcher, NULL, this);
}

Int32 CFileWatch::GetFD() const
{
    return m_nNotifyFD;
}

void CFileWatch::OnRead()
{
    static const Int32 nMaxSize = 4096;
    char szBuffer[nMaxSize];
    Int32 nReadOffset = 0;

    while (1)
    {
        Int32 nBufferSize = read(m_nNotifyFD, szBuffer + nReadOffset, nMaxSize - nReadOffset);
        if (nBufferSize <= 0)
        {
            break;
        }

        nBufferSize += nReadOffset;
        nReadOffset = 0;
        while (nReadOffset < nBufferSize)
        {
            struct inotify_event *pEvent = (struct inotify_event *)(szBuffer + nReadOffset);
            if (nBufferSize - nReadOffset < (Int32)sizeof(struct inotify_event))
            {
                break;
            }

            if (nBufferSize - nReadOffset < (Int32)(sizeof(struct inotify_event) + pEvent->len))
            {
                break;
            }

            std::map<Int32, FileWatchItem>::iterator it = m_mapWatchFD2Watcher.find(pEvent->wd);
            if (it != m_mapWatchFD2Watcher.end())
            {
                std::string strFileName;
                if (pEvent->len > 0)
                {
                    strFileName = pEvent->name;
                }
                it->second.pWatcher->OnFileWatch(it->second.strWatchFileName, strFileName, pEvent->mask, (pEvent->mask & IN_ISDIR) != 0);
            }

            nReadOffset += (sizeof(struct inotify_event) + pEvent->len);
        }

        if (nBufferSize - nReadOffset > 0 && nReadOffset != 0)
        {
            memmove(szBuffer, szBuffer + nReadOffset, nBufferSize - nReadOffset);
        }
        nReadOffset = nBufferSize - nReadOffset;
    }
}

void CFileWatch::OnWrite()
{
    Assert(False);
}

void CFileWatch::OnError(Int32 nErrCode)
{
    if (m_nNotifyFD != -1)
    {
        close(m_nNotifyFD);
        m_nNotifyFD = -1;
    }
}

void CFileWatch::OnMessageEvent(Int32 nMessageID, void *wParam, void *lParam)
{
    RegisterWatchInfo *pRegisterInfo;

    switch (nMessageID)
    {
    case REGISTER_WATCH:
        pRegisterInfo = (RegisterWatchInfo *)wParam;
        DoRegisterWatch(pRegisterInfo->strFileName, pRegisterInfo->uMask, pRegisterInfo->pCallback);

        delete pRegisterInfo;
        pRegisterInfo = NULL;
        break;

    case UNREGISTER_WATCH:
        DoUnregisterWatch((IFileWatchEvent *)wParam);
        break;

    case WHOLE_SCAN_FILE:
        DoWholeScan();
        break;

    default:
        Assert(False);
        break;
    }
}

void CFileWatch::DoRegisterWatch(const std::string &strFileName, UInt32 uMask, IFileWatchEvent *pWatcher)
{
    FileWatchItem item;
    item.pWatcher = pWatcher;
    item.strWatchFileName = strFileName;
    OS::NormalizeFileName(item.strWatchFileName);

    Int32 nWatchFD = inotify_add_watch(m_nNotifyFD, item.strWatchFileName.c_str(), uMask);
    if (nWatchFD != -1)
    {
        m_mapWatchFD2Watcher.insert(std::make_pair(nWatchFD, item));
    }
}

void CFileWatch::DoUnregisterWatch(IFileWatchEvent *pWatcher)
{
    std::map<Int32, FileWatchItem>::iterator it = m_mapWatchFD2Watcher.begin();
    for (; it != m_mapWatchFD2Watcher.end(); ++it)
    {
        if (it->second.pWatcher == pWatcher)
        {
            inotify_rm_watch(m_nNotifyFD, it->first);
            m_mapWatchFD2Watcher.erase(it);
            break;
        }
    }
}

void CFileWatch::DoWholeScan()
{
    std::map<Int32, FileWatchItem>::iterator it = m_mapWatchFD2Watcher.begin();
    for (; it != m_mapWatchFD2Watcher.end(); ++it)
    {
        DIR *pDir = opendir(it->second.strWatchFileName.c_str());
        if (NULL == pDir)
        {
            continue;
        }

        IFileWatchEvent *pCallback = it->second.pWatcher;
        std::string strBasePath = it->second.strWatchFileName;

        struct dirent entry;
        struct dirent *result;

        while (readdir_r(pDir, &entry, &result) == 0)
        {
            if (NULL == result)
            {
                break;
            }

            if (0 == strcmp(entry.d_name, ".") || 0 == strcmp(entry.d_name, ".."))
            {
                continue;
            }

            Bool bIsDir = False;
            if (entry.d_type != DT_UNKNOWN)
            {
                bIsDir = (entry.d_type == DT_DIR);
            }
            else
            {
                Char szFullPath[MAX_PATH];
                strcpy(szFullPath, strBasePath.c_str());
                strcat(szFullPath, entry.d_name);

                struct stat st;
                if (stat(szFullPath, &st) == -1)
                {
                    continue;
                }

                bIsDir = S_ISDIR(st.st_mode) ? True : False;
            }

            pCallback->OnFullScan(strBasePath, entry.d_name, bIsDir);
        }

        closedir(pDir);
    }
}
