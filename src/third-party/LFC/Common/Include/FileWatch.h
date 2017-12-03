/*
 * CFileWatch: 文件监控类
 * 可以监视文件的创建、删除等操作
 */

#ifndef ___FILE__WATCH__20160509___
#define ___FILE__WATCH__20160509___

#include "Event.h"
#include "EventEngine.h"
#include "Singleton.h"
#include <map>
#include <string>

struct IFileWatchEvent
{
	virtual void OnFileWatch(const std::string &strBaseName, const std::string &strFileName, UInt32 uMask, Bool bIsDirectory) = 0;
    virtual void OnFullScan(const std::string &strBaseName, const std::string &strFileName, Bool bIsDirectory) = 0;
};

class CFileWatch : public CEvent, public IMessageEvent
{
public:
	enum EWatchEvent
	{
		/* Supported events suitable for MASK parameter of INOTIFY_ADD_WATCH.  */
		IN_ACCESS        = 0x00000001,     /* File was accessed.  */
		IN_MODIFY        = 0x00000002,     /* File was modified.  */
		IN_ATTRIB        = 0x00000004,     /* Metadata changed.  */
		IN_CLOSE_WRITE   = 0x00000008,     /* Writtable file was closed.  */
		IN_CLOSE_NOWRITE = 0x00000010,     /* Unwrittable file closed.  */
		IN_CLOSE         = (IN_CLOSE_WRITE | IN_CLOSE_NOWRITE), /* Close.  */
		IN_OPEN          = 0x00000020,     /* File was opened.  */
		IN_MOVED_FROM    = 0x00000040,     /* File was moved from X.  */
		IN_MOVED_TO      = 0x00000080,     /* File was moved to Y.  */
		IN_MOVE          = (IN_MOVED_FROM | IN_MOVED_TO), /* Moves.  */
		IN_CREATE        = 0x00000100,     /* Subfile was created.  */
		IN_DELETE        = 0x00000200,     /* Subfile was deleted.  */
		IN_DELETE_SELF   = 0x00000400,     /* Self was deleted.  */
		IN_MOVE_SELF     = 0x00000800,     /* Self was moved.  */

		/* Events sent by the kernel.  */
		IN_UNMOUNT       = 0x00002000,     /* Backing fs was unmounted.  */
		IN_Q_OVERFLOW    = 0x00004000,     /* Event queued overflowed.  */
		IN_IGNORED       = 0x00008000,     /* File was ignored.  */

		/* Special flags.  */
		IN_ONLYDIR       = 0x01000000,     /* Only watch the path if it is a
												   directory.  */
		IN_DONT_FOLLOW   = 0x02000000,     /* Do not follow a sym link.  */
		IN_MASK_ADD      = 0x20000000,     /* Add to the mask of an already
												   existing watch.  */
		IN_ISDIR         = 0x40000000,     /* Event occurred against dir.  */
		IN_ONESHOT       = 0x80000000,     /* Only send event once.  */

		/* All events which a program can wait on.  */
		IN_ALL_EVENTS    = (IN_ACCESS | IN_MODIFY | IN_ATTRIB | IN_CLOSE_WRITE \
                           | IN_CLOSE_NOWRITE | IN_OPEN | IN_MOVED_FROM        \
                           | IN_MOVED_TO | IN_CREATE | IN_DELETE               \
                           | IN_DELETE_SELF | IN_MOVE_SELF)
	};

public:
	CFileWatch(CEventEngine *pEngine);
	virtual ~CFileWatch();

public:
	Bool Initialize();
	void WholeScan();
	Bool RegisterWatch(const std::string &strFileName, UInt32 uMask, IFileWatchEvent *pWatcher);
	void UnregisterWatch(IFileWatchEvent *pWatcher);

protected:
    // CEvent
	virtual Int32 GetFD() const;
	virtual void  OnRead();
	virtual void  OnWrite();
	virtual void  OnError(Int32 nErrCode);

    // IMessageEvent
    virtual void OnMessageEvent(Int32 nMessageID, void *wParam, void *lParam);

protected:
    void DoRegisterWatch(const std::string &strFileName, UInt32 uMask, IFileWatchEvent *pWatcher);
    void DoUnregisterWatch(IFileWatchEvent *pWatcher);
    void DoWholeScan();

protected:
	struct FileWatchItem
	{
		IFileWatchEvent*					pWatcher;					// 观察者对象
		std::string							strWatchFileName;			// 被观察的文件/文件夹
	};

	Int32									m_nNotifyFD;
	std::map<Int32, FileWatchItem>			m_mapWatchFD2Watcher;
};

#endif
